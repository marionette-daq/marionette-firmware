/*! \file fetch_adc.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @defgroup fetch_adc Fetch ADC
  * @{
  */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"
#include "adc_lld.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_io.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"
#include "mpipe.h"

#define FETCH_ADC_SAMPLE_DEPTH  1

#define FETCH_ADC2_CH_COUNT     7
#define FETCH_ADC3_CH_COUNT     7

#if FETCH_ADC2_CH_COUNT > ADC_SAMPLE_SET_SIZE || FETCH_ADC3_CH_COUNT > ADC_SAMPLE_SET_SIZE
#error "sample set size not large enough for number of adc channels"
#endif

#define FETCH_ADC2_BUFFER_SIZE  (FETCH_ADC_SAMPLE_DEPTH * FETCH_ADC2_CH_COUNT)
#define FETCH_ADC3_BUFFER_SIZE  (FETCH_ADC_SAMPLE_DEPTH * FETCH_ADC3_CH_COUNT)

#ifndef FETCH_ADC_MEM_POOL_SIZE
#define FETCH_ADC_MEM_POOL_SIZE 128
#endif

#ifndef FETCH_ADC_DEFAULT_SAMPLE_RATE
#define FETCH_ADC_DEFAULT_SAMPLE_RATE  100
#endif

#ifndef FETCH_ADC_TIMER_FREQ
#define FETCH_ADC_TIMER_FREQ 1000000
#endif

#define ADC_CR2_EXTSEL_TIM2_TRGO (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1) // 0b0110
#define ADC_CR2_EXTSEL_TIM3_TRGO (ADC_CR2_EXTSEL_3)                    // 0b1000

#define ADC_SMPR1(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24))
#define ADC_SMPR2(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24) | (smp<<27))

static void fetch_adc_error_cb(ADCDriver * adcp, adcerror_t err);
static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);

#if 0
static fetch_command_t fetch_adc_commands[] = {
  /*  function                    command string    help string */
    { fetch_adc_help_cmd,         "help",           "Display ADC help" },
    { fetch_adc_single_cmd,       "single",         "Single sample from each channel\nUsage: single(<dev>)" },
    { fetch_adc_stream_start_cmd, "start",          "Start ADC streaming\nUsage: start(<dev>)" },
    { fetch_adc_stream_stop_cmd,  "stop",           "Stop ADC streaming\nUsage: stop(<dev>)" },
    { fetch_adc_status_cmd,       "status",         "Current ADC status" },
    { fetch_adc_config_cmd,       "config",         "Configure ADC driver\nUsage: config(<dev>, <sample rate>)" },
    { fetch_adc_timer_reset_cmd,  "timerreset",     NULL },
    { fetch_adc_reset_cmd,        "reset",          "Reset ADC drivers" },
    { NULL, NULL, NULL }
  };
#endif

static adcsample_t adc2_sample_buffer[FETCH_ADC2_BUFFER_SIZE];
static adcsample_t adc3_sample_buffer[FETCH_ADC3_BUFFER_SIZE];

static adc_sample_set_t adc_sample_set_buffer[FETCH_ADC_MEM_POOL_SIZE];
memory_pool_t adc_sample_set_pool;

static volatile uint16_t adc2_sequence_number = 0;
static volatile uint16_t adc3_sequence_number = 0;

static uint16_t adc2_timer_interval = 0;
static uint16_t adc3_timer_interval = 0;
static uint32_t adc2_sample_rate = 0;
static uint32_t adc3_sample_rate = 0;

typedef struct {
  bool error_dmafailure;
  bool error_overflow;
  bool mpipe_overflow;
  bool mcard_overflow;
  bool mem_alloc_null;
} adc_status_t;

volatile adc_status_t adc2_status;
volatile adc_status_t adc3_status;

static GPTConfig gpt2_cfg;
static GPTConfig gpt3_cfg;

/*! \brief ADC conversion group configuration
 */

static ADCConversionGroup adc2_conv_grp = {
	.circular        = true,
	.num_channels    = FETCH_ADC2_CH_COUNT,
	.end_cb          = fetch_adc_end_cb,
	.error_cb        = fetch_adc_error_cb,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_EXTEN_0 | ADC_CR2_EXTSEL_TIM2_TRGO, // rising edge of TIM2_TRGO
	.smpr1           = ADC_SMPR1(ADC_SAMPLE_480),
	.smpr2           = ADC_SMPR2(ADC_SAMPLE_480),
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC2_CH_COUNT),
	.sqr2            = ADC_SQR2_SQ7_N(15),
	.sqr3            = ADC_SQR3_SQ1_N(2) | ADC_SQR3_SQ2_N(6) | ADC_SQR3_SQ3_N(7) | ADC_SQR3_SQ4_N(11) | ADC_SQR3_SQ5_N(13) | ADC_SQR3_SQ6_N(14)
};

static ADCConversionGroup adc3_conv_grp = {
	.circular        = true,
	.num_channels    = FETCH_ADC3_CH_COUNT,
	.end_cb          = fetch_adc_end_cb,
	.error_cb        = fetch_adc_error_cb,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_EXTEN_0 | ADC_CR2_EXTSEL_TIM3_TRGO, // rising edge of TIM3_TRGO
	.smpr1           = ADC_SMPR1(ADC_SAMPLE_480),
	.smpr2           = ADC_SMPR2(ADC_SAMPLE_480),
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC3_CH_COUNT),
	.sqr2            = ADC_SQR2_SQ7_N(15),
	.sqr3            = ADC_SQR3_SQ1_N(5) | ADC_SQR3_SQ2_N(6) | ADC_SQR3_SQ3_N(7) | ADC_SQR3_SQ4_N(8) | ADC_SQR3_SQ5_N(9) | ADC_SQR3_SQ6_N(14)
};

static void fetch_adc_error_cb(ADCDriver * adcp, adcerror_t err)
{
  if( adcp == &ADCD2 )
  {
    switch(err)
    {
      case ADC_ERR_DMAFAILURE:
        adc2_status.error_dmafailure = true;
        break;
      case ADC_ERR_OVERFLOW:
        adc2_status.error_overflow = true;
        break;
    }
  }
  else if( adcp == &ADCD3 )
  {
    switch(err)
    {
      case ADC_ERR_DMAFAILURE:
        adc3_status.error_dmafailure = true;
        break;
      case ADC_ERR_OVERFLOW:
        adc3_status.error_overflow = true;
        break;
    }
  }
}

/*!
 * ADC end conversion callback
 */
static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n)
{

	(void) buffer;
	(void) n;
  adc_sample_set_t *ssp;
  uint16_t seq_num;

  chSysLockFromISR();
  ssp = chPoolAllocI(&adc_sample_set_pool);
  chSysUnlockFromISR();

  if( ssp != NULL )
  {
    memcpy(ssp->sample, buffer, sizeof(adcsample_t) * ADC_SAMPLE_SET_SIZE);
  }

  if( adcp == &ADCD2 )
  {
    adc2_sequence_number++;
    if( ssp != NULL )
    {
      ssp->sequence_number = adc2_sequence_number;
      chSysLockFromISR();
      if( chMBPostI(&mpipe_adc2_mb, (msg_t)ssp) != MSG_OK )
      {
        adc2_status.mpipe_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#if 0
      if( chMBPostI(&mcard_adc2_mb, (msg_t)ssp) != MSG_OK )
      {
        adc2_status.mcard_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#endif
      chSysUnlockFromISR();
    }
    else
    {
      adc2_status.mem_alloc_null = true;
    }
  }
  else //ADCD3
  {
    adc3_sequence_number++;
    if( ssp != NULL )
    {
      ssp->sequence_number = adc3_sequence_number;
      chSysLockFromISR();
      if( chMBPostI(&mpipe_adc3_mb, (msg_t)ssp) != MSG_OK )
      {
        adc3_status.mpipe_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#if 0
      if( chMBPostI(&mcard_adc3_mb, (msg_t)ssp) != MSG_OK )
      {
        adc3_status.mcard_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#endif
      chSysUnlockFromISR();
    }
    else
    {
      adc3_status.mem_alloc_null = true;
    }
  }

  if( ssp != NULL && ssp->mem_ref_count == 0 )
  {
    // we were not able to enqueue it anywhere so lets free it imediately
    chSysLockFromISR();
    chPoolFreeI(&adc_sample_set_pool, ssp);
    chSysUnlockFromISR();
  }
}

void fetch_adc_free_sample_set( adc_sample_set_t *ssp )
{
  if( ssp != NULL )
  {
    chSysLock();
    ssp->mem_ref_count--;
    if( ssp->mem_ref_count <= 0 )
    {
      chPoolFreeI(&adc_sample_set_pool, ssp);
    }
    chSysUnlock();
  }
}

static ADCDriver * parse_adc_dev( char * str, int32_t * dev )
{
  char * endptr;
  int32_t num = strtol(str, &endptr, 0);

  if(*endptr != '\0')
  {
    return NULL;
  }

  if( dev != NULL )
  {
    *dev = num;
  }

  switch( num )
  {
    case 1: // Marionette name
    case 2: // STM/Chibios name
      return &ADCD2;
    case 0:
    case 3:
      return &ADCD3;
    default:
      return NULL;
  }
}

/*! \brief display adc help
 */
bool fetch_adc_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  FETCH_HELP_BREAK(chp);
  FETCH_HELP_LEGEND(chp);
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_TITLE(chp, "ADC Help");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "single(<dev>)");
  FETCH_HELP_DES(chp, "Single sample from each channel");
  FETCH_HELP_ARG(chp, "dev", "0 | 1");
  FETCH_HELP_CMD(chp, "start(<dev>)");
  FETCH_HELP_DES(chp, "Start ADC sampling");
  FETCH_HELP_ARG(chp, "dev", "0 | 1");
  FETCH_HELP_CMD(chp, "stop(<dev>)");
  FETCH_HELP_DES(chp, "Stop ADC sampling");
  FETCH_HELP_ARG(chp, "dev", "0 | 1");
  FETCH_HELP_CMD(chp, "status()");
  FETCH_HELP_DES(chp, "Query current adc status");
  FETCH_HELP_CMD(chp, "config(<dev>, <sample rate>)");
  FETCH_HELP_DES(chp, "Configure adc device");
  FETCH_HELP_ARG(chp, "sample rate", "16 ... 1000000");
  FETCH_HELP_CMD(chp, "reset");
  FETCH_HELP_DES(chp, "Reset adc module");

	return true;
}


/*! \brief return sample data
 */
bool fetch_adc_single_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  int32_t dev;
  ADCDriver *adc_drv = parse_adc_dev(argv[0], &dev);

  if( adc_drv == NULL )
  {
    util_message_error(chp, "invalid adc device");
    false;
  }

  // TODO add ability to sample from streamming state by grabbing the current sample set instead of trigging a conversion

  if( adc_drv->state != ADC_READY )
  {
    util_message_error(chp, "ADC device not in ready state");
    return false;
  }

  switch(dev)
  {
    case 2:
      adc2_conv_grp.circular = false;
      adcConvert( &ADCD2, &adc2_conv_grp, adc2_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      util_message_uint16_array(chp, "samples", adc2_sample_buffer, FETCH_ADC2_BUFFER_SIZE);
      break;
    case 3:
      adc3_conv_grp.circular = false;
	    adcConvert( &ADCD3, &adc3_conv_grp, adc3_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      util_message_uint16_array(chp, "samples", adc3_sample_buffer, FETCH_ADC3_BUFFER_SIZE);
      break;
  }

  return true;
}


/*! \brief Start a conversion
 */
bool fetch_adc_stream_start_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  int32_t dev;
  ADCDriver *adc_drv = parse_adc_dev(argv[0], &dev);

  if( adc_drv == NULL )
  {
    util_message_error(chp, "invalid adc device");
    false;
  }

  if( adc_drv->state != ADC_READY )
  {
    util_message_error(chp, "ADC device not in ready state");
    return false;
  }

  switch(dev)
  {
    case 2:
      adc2_conv_grp.circular = true;
      adcStartConversion( &ADCD2, &adc2_conv_grp, adc2_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      break;
    case 3:
      adc3_conv_grp.circular = true;
	    adcStartConversion( &ADCD3, &adc3_conv_grp, adc3_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      break;
  }

	return true;
}

/*! \brief Stop the current conversion
 */
bool fetch_adc_stream_stop_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);
  
  ADCDriver *adc_drv = parse_adc_dev(argv[0], NULL);

  if( adc_drv == NULL )
  {
    util_message_error(chp, "invalid adc device");
    return false;
  }

  adcStopConversion(adc_drv);

	return true;
}


/*! \brief check if adc is done
 */
bool fetch_adc_status_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  adc_status_t status;

  chSysLock();
  status = adc2_status;
  adc2_status.error_dmafailure = false;
  adc2_status.error_overflow = false;
  adc2_status.mpipe_overflow = false;
  adc2_status.mcard_overflow = false;
  adc2_status.mem_alloc_null = false;
  chSysUnlock();

  util_message_bool(chp, "adc2_error_dmafailure", status.error_dmafailure);
  util_message_bool(chp, "adc2_error_overflow", status.error_overflow);
  util_message_bool(chp, "adc2_mpipe_overflow", status.mpipe_overflow);
  util_message_bool(chp, "adc2_mcard_overflow", status.mcard_overflow);
  util_message_bool(chp, "adc2_mem_alloc_null", status.mem_alloc_null);
  util_message_uint16(chp, "adc2_sequence_number", adc2_sequence_number);
  util_message_uint16(chp, "adc2_timer_count", gptGetCounterX(&GPTD2));
  util_message_uint16(chp, "gpt2_cfg.cr2", gpt2_cfg.cr2);

  chSysLock();
  status = adc3_status;
  adc3_status.error_dmafailure = false;
  adc3_status.error_overflow = false;
  adc3_status.mpipe_overflow = false;
  adc3_status.mcard_overflow = false;
  adc3_status.mem_alloc_null = false;
  chSysUnlock();

  util_message_bool(chp, "adc3_error_dmafailure", status.error_dmafailure);
  util_message_bool(chp, "adc3_error_overflow", status.error_overflow);
  util_message_bool(chp, "adc3_mpipe_overflow", status.mpipe_overflow);
  util_message_bool(chp, "adc3_mcard_overflow", status.mcard_overflow);
  util_message_bool(chp, "adc3_mem_alloc_null", status.mem_alloc_null);
  util_message_uint16(chp, "adc3_sequence_number", adc3_sequence_number);
  util_message_uint16(chp, "adc3_timer_count", gptGetCounterX(&GPTD3));
  util_message_uint16(chp, "gpt3_cfg.cr2", gpt3_cfg.cr2);

  return true;
}


/*! \brief Process an ADC configure command
 */
bool fetch_adc_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 2);

  char * endptr;
  int32_t dev;
  ADCDriver *adc_drv = parse_adc_dev(argv[0], &dev);

  if( adc_drv == NULL )
  {
    util_message_error(chp, "invalid adc device");
    return false;
  }

  uint32_t sample_rate = strtoul(argv[1], &endptr, 0);

  if( *endptr != '\0' || sample_rate <= (FETCH_ADC_TIMER_FREQ / 0xffff) || sample_rate > FETCH_ADC_TIMER_FREQ )
  {
    util_message_error(chp, "invalid sample rate");
    return false;
  }

  switch(dev)
  {
    case 2:
      adc2_timer_interval = FETCH_ADC_TIMER_FREQ / sample_rate;
      adc2_sample_rate = FETCH_ADC_TIMER_FREQ / adc2_timer_interval;

      gptStopTimer(&GPTD2);
      gptStartContinuous( &GPTD2, adc2_timer_interval);

      util_message_uint32(chp, "sample_rate", adc2_sample_rate);
      break;
    case 3:
      adc3_timer_interval = FETCH_ADC_TIMER_FREQ / sample_rate;
      adc3_sample_rate = FETCH_ADC_TIMER_FREQ / adc3_timer_interval;
      
      gptStopTimer(&GPTD3);
      gptStartContinuous( &GPTD3, adc3_timer_interval);
      
      util_message_uint32(chp, "sample_rate", adc3_sample_rate);
      break;
  }

  return true;
}

bool fetch_adc_timer_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  gptStopTimer(&GPTD2);
  gptStopTimer(&GPTD3);
  gptStartContinuous( &GPTD2, adc2_timer_interval);
  gptStartContinuous( &GPTD3, adc3_timer_interval);

  return true;
}

/*! \brief Reset adc1
 */
bool fetch_adc_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  return fetch_adc_reset(chp);
}

void fetch_adc_init(void)
{
  adcStart(&ADCD2,NULL);
  adcStart(&ADCD3,NULL);
 
  chPoolObjectInit(&adc_sample_set_pool, sizeof(adc_sample_set_t), NULL);
  chPoolLoadArray(&adc_sample_set_pool, adc_sample_set_buffer, FETCH_ADC_MEM_POOL_SIZE);
  
  adc2_status.error_dmafailure = false;
  adc2_status.error_overflow = false;
  adc2_status.mpipe_overflow = false;
  adc2_status.mcard_overflow = false;
  adc2_status.mem_alloc_null = false;

  adc3_status.error_dmafailure = false;
  adc3_status.error_overflow = false;
  adc3_status.mpipe_overflow = false;
  adc3_status.mcard_overflow = false;
  adc3_status.mem_alloc_null = false;


  memset(&gpt2_cfg, 0, sizeof(gpt2_cfg));

  gpt2_cfg.frequency = 1000000;
  gpt2_cfg.callback = NULL;
  gpt2_cfg.cr2 = TIM_CR2_MMS_1; // 0b010 = TRGO on update event

  gpt3_cfg = gpt2_cfg;

  gptStart(&GPTD2, &gpt2_cfg);
  gptStart(&GPTD3, &gpt3_cfg);

  adc2_timer_interval = FETCH_ADC_TIMER_FREQ / FETCH_ADC_DEFAULT_SAMPLE_RATE;
  adc2_sample_rate = FETCH_ADC_TIMER_FREQ / adc2_timer_interval;

  adc3_timer_interval = FETCH_ADC_TIMER_FREQ / FETCH_ADC_DEFAULT_SAMPLE_RATE;
  adc3_sample_rate = FETCH_ADC_TIMER_FREQ / adc3_timer_interval;

  gptStartContinuous( &GPTD2, adc2_timer_interval);
  gptStartContinuous( &GPTD3, adc3_timer_interval);
}


bool fetch_adc_reset(BaseSequentialStream * chp)
{
	adcStopConversion(&ADCD2);
  adcStopConversion(&ADCD3);
  gptStopTimer(&GPTD2);
  gptStopTimer(&GPTD3);
  
  adc2_timer_interval = FETCH_ADC_TIMER_FREQ / FETCH_ADC_DEFAULT_SAMPLE_RATE;
  adc2_sample_rate = FETCH_ADC_TIMER_FREQ / adc2_timer_interval;

  adc3_timer_interval = FETCH_ADC_TIMER_FREQ / FETCH_ADC_DEFAULT_SAMPLE_RATE;
  adc3_sample_rate = FETCH_ADC_TIMER_FREQ / adc3_timer_interval;

  gptStartContinuous( &GPTD2, adc2_timer_interval);
  gptStartContinuous( &GPTD3, adc3_timer_interval);

  return true;
}

/*! @} */


