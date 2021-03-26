/*
fetch_tsar.c --- functin definitions for TSAR's Marionette firmware revisions
Author: Seth Rohrbach
Modified: Feb 27, 2021
----------------------
Description:



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
#include "fetch_spi.h"
#include "fetch_parser.h"
#include "fetch_adc.h"
#include "mpipe.h"


//Parameters:
#ifndef FETCH_ADC_SAMPLE_DEPTH
#define FETCH_ADC_SAMPLE_DEPTH  1

#define FETCH_ADC2_CH_COUNT     7
#define FETCH_ADC3_CH_COUNT     7
#endif

#if FETCH_ADC2_CH_COUNT > ADC_SAMPLE_SET_SIZE || FETCH_ADC3_CH_COUNT > ADC_SAMPLE_SET_SIZE
#error "sample set size not large enough for number of adc channels"
#endif

#ifndef FETCH_ADC2_BUFFER_SIZE
#define FETCH_ADC2_BUFFER_SIZE  (FETCH_ADC_SAMPLE_DEPTH * FETCH_ADC2_CH_COUNT)
#define FETCH_ADC3_BUFFER_SIZE  (FETCH_ADC_SAMPLE_DEPTH * FETCH_ADC3_CH_COUNT)
#endif

#ifndef FETCH_ADC_MEM_POOL_SIZE
#define FETCH_ADC_MEM_POOL_SIZE 128
#endif

#ifndef FETCH_ADC_DEFAULT_SAMPLE_RATE
#define FETCH_ADC_DEFAULT_SAMPLE_RATE  100
#endif

#ifndef FETCH_ADC_TIMER_FREQ
#define FETCH_ADC_TIMER_FREQ 1000000
#endif

#ifndef ADC_CR2_EXTSEL_TIM2_TRGO
#define ADC_CR2_EXTSEL_TIM2_TRGO (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1) // 0b0110
#define ADC_CR2_EXTSEL_TIM3_TRGO (ADC_CR2_EXTSEL_3)       // 0b1000
#endif


#ifndef ADC_SMPR1
#define ADC_SMPR1(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24))
#define ADC_SMPR2(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24) | (smp<<27))
#endif

/*
Supporting function aliases for the OS AL
*/


static void fetch_adc_error_cb(ADCDriver * adcp, adcerror_t err);
static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);

/*
OS Abstraction Layer Variables:
*/
static adcsample_t adc2_sample_buffer[FETCH_ADC2_BUFFER_SIZE];
static adcsample_t adc3_sample_buffer[FETCH_ADC3_BUFFER_SIZE];

static adc_sample_set_t adc_sample_set_buffer[FETCH_ADC_MEM_POOL_SIZE];
memory_pool_t adc_tsar_sample_pool;

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


volatile adc_status_t adc2_tsar_status;
volatile adc_status_t adc3_tsar_status;


static GPTConfig gpt2_cfg;
static GPTConfig gpt3_cfg;

/*

SPI Definitions & Aliases:

*/

#ifndef FETCH_MAX_SPI_BYTES
#error FETCH_MAX_SPI_BYTES not defined
#endif

#define SPI_DRIVER_COUNT 2


static SPIDriver * spi_drivers[SPI_DRIVER_COUNT] = { &SPID2, &SPID6 };
static SPIConfig  spi_configs[SPI_DRIVER_COUNT];

static SPIDriver * parse_spi_dev( char * str, uint32_t * dev )
{
  uint32_t dev_id = str[0] - '0';

  if( str[1] != '\0' )
  {
    return NULL;
  }

  if( str[0] < '0' || str[0] >= ('0' + SPI_DRIVER_COUNT) )
  {
    return NULL;
  }

  if( dev )
  {
    *dev = dev_id;
  }

  return spi_drivers[dev_id];
}


/*! \brief ADC conversion group configuration
-- Looks like this is an abstraction layer for the ADC channels provided by the OS --SMR
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

//ALL functions needed to support my driver need to go in here.
//These functions will be called on by the fetch_parser.rl
//The commands will also need to be translated to Ragel syntax and put in the fetch_commands.rl file

/*
fetch_tsar_help - when called, sends a simple help menu over serial, to be printed over stdout
*/

bool fetch_tsar_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
    FETCH_MAX_ARGS(chp, argc, 0);

    FETCH_HELP_BREAK(chp);
    FETCH_HELP_LEGEND(chp);
    FETCH_HELP_BREAK(chp);
    FETCH_HELP_TITLE(chp, "TSAR Commands Help");
    FETCH_HELP_BREAK(chp);
    FETCH_HELP_CMD(chp, "MORE TO FOLLOW! - A command will go here");
    FETCH_HELP_DES(chp, "A description will go here");
    FETCH_HELP_ARG(chp, "dev", "And the arguments go here");
    FETCH_HELP_BREAK(chp);

    return true;

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
      return &ADCD2;
    case 0:
      return &ADCD3;
    default:
      return NULL;
  }
}

/*
-- Error callback needed to support the ADC device abstraction from the OS
*/


static void fetch_adc_error_cb(ADCDriver * adcp, adcerror_t err)
{
  if( adcp == &ADCD2 )
  {
    switch(err)
    {
      case ADC_ERR_DMAFAILURE:
        adc2_tsar_status.error_dmafailure = true;
        break;
      case ADC_ERR_OVERFLOW:
        adc2_tsar_status.error_overflow = true;
        break;
    }
  }
  else if( adcp == &ADCD3 )
  {
    switch(err)
    {
      case ADC_ERR_DMAFAILURE:
        adc3_tsar_status.error_dmafailure = true;
        break;
      case ADC_ERR_OVERFLOW:
        adc3_tsar_status.error_overflow = true;
        break;
    }
  }
}

/*!
 * ADC end conversion callback
 -- Looks like needed to support the ADC device abstraction from the OS
 */
static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n)
{

	(void) buffer;
	(void) n;
  adc_sample_set_t *ssp;
  uint16_t seq_num;

  chSysLockFromISR();
  ssp = chPoolAllocI(&adc_tsar_sample_pool);
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
        adc2_tsar_status.mpipe_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#if 0
      if( chMBPostI(&mcard_adc2_mb, (msg_t)ssp) != MSG_OK )
      {
        adc2_tsar_status.mcard_overflow = true;
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
      adc2_tsar_status.mem_alloc_null = true;
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
        adc3_tsar_status.mpipe_overflow = true;
      }
      else
      {
        ssp->mem_ref_count++;
      }
#if 0
      if( chMBPostI(&mcard_adc3_mb, (msg_t)ssp) != MSG_OK )
      {
        adc3_tsar_status.mcard_overflow = true;
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
      adc3_tsar_status.mem_alloc_null = true;
    }
  }

  if( ssp != NULL && ssp->mem_ref_count == 0 )
  {
    // we were not able to enqueue it anywhere so lets free it imediately
    chSysLockFromISR();
    chPoolFreeI(&adc_tsar_sample_pool, ssp);
    chSysUnlockFromISR();
  }
}


/*

Fetch_tsar_sample_cmd --

Command to return the values needed by TSAR over serial in the designated package format
i.e. 3F99, val_0, val_1, ..., val_n, ~

TODO: Add SPI channels, confirm formatting

*/

bool fetch_tsar_pack_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
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



      adc2_conv_grp.circular = false;
      adcConvert( &ADCD2, &adc2_conv_grp, adc2_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      util_message_uint16_array(chp, "3F99,", adc2_sample_buffer, FETCH_ADC2_BUFFER_SIZE);
      adc3_conv_grp.circular = false;
	  adcConvert( &ADCD3, &adc3_conv_grp, adc3_sample_buffer, FETCH_ADC_SAMPLE_DEPTH);
      util_message_uint16_array(chp, "", adc3_sample_buffer, FETCH_ADC3_BUFFER_SIZE);

      /*
      SPI data goes here!
      */

      util_message_uint16_array(chp, "\r", NULL , NULL ); //Terminating character - need to check syntax

  return true;
}

/*

fetch_tsar_init -- initialization for the adc channels / hardware pins
TODO: Add SPI initialization stuff

*/

void fetch_tsar_init(void)
{
  adcStart(&ADCD2,NULL);
  adcStart(&ADCD3,NULL);

  chPoolObjectInit(&adc_tsar_sample_pool, sizeof(adc_sample_set_t), NULL);
  chPoolLoadArray(&adc_tsar_sample_pool, adc_sample_set_buffer, FETCH_ADC_MEM_POOL_SIZE);

  adc2_tsar_status.error_dmafailure = false;
  adc2_tsar_status.error_overflow = false;
  adc2_tsar_status.mpipe_overflow = false;
  adc2_tsar_status.mcard_overflow = false;
  adc2_tsar_status.mem_alloc_null = false;

  adc3_tsar_status.error_dmafailure = false;
  adc3_tsar_status.error_overflow = false;
  adc3_tsar_status.mpipe_overflow = false;
  adc3_tsar_status.mcard_overflow = false;
  adc3_tsar_status.mem_alloc_null = false;


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

/*

fetch_tsar_reset - reset the ADC channels used by TSAR

TODO: Add SPI channel reset stuff

*/

bool fetch_tsar_reset(BaseSequentialStream * chp)
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

/*
SPI related functions:
*/


/*
void fetch_spi_init(void)
{
  // put spi initialization stuff here
  // ...
}

bool fetch_spi_reset(BaseSequentialStream * chp)
{
  for( uint32_t i = 0; i < SPI_DRIVER_COUNT; i++ )
  {
    spiStop(spi_drivers[i]);
  }

  return true;
}
*/
