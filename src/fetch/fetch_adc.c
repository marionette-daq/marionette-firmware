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

#include "mshell_state.h"

#include "io_manage.h"
#include "io_manage_defs.h"
#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"

#ifndef FETCH_DEFAULT_VREF_MV
#define FETCH_DEFAULT_VREF_MV         3300
#endif

#ifndef FETCH_ADC_BUFFER_SIZE
#define FETCH_ADC_BUFFER_SIZE         (1024 * 8)
#endif

#ifndef FETCH_ADC_MAX_CHANNELS
#define FETCH_ADC_MAX_CHANNELS        16
#endif

#define FETCH_ADC_MAX_SAMPLES(ch_cnt) (FETCH_ADC_BUFFER_SIZE / (ch_cnt))

#define ADC_ENABLE_CH(n) (1<<(n))

#define ADC_V_25    (0.76)   //!< From stm32f4 datasheet. Voltage at 25C
#define ADC_VSLOPE 	(2.5)    //!< units: mv/C

enum {
  ADC_CONFIG_DEV = 0,
  ADC_CONFIG_RES,
  ADC_CONFIG_CLK,
  ADC_CONFIG_VREF,
  ADC_CONFIG_COUNT,
  ADC_CONFIG_CHANNELS
};

static const char * adc_dev_tok[] = {"ADC1", "ADC2", "ADC3"};
static const char * adc_res_tok[] = {"RES12","RES10","RES8","RES6"};
static const char * adc_sample_tok[] = {"CLK3","CLK15","CLK28","CLK56","CLK84","CLK112","CLK144","CLK480"};
static const char * adc_ch_tok[] = {"CH0","CH1","CH2","CH3","CH4","CH5","CH6","CH7","CH8","CH9","CH10","CH11","CH12","CH13","CH14","CH15","SENSOR","VREFINT","VBAT"};

static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);
static bool fetch_adc_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_samples_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_start_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_stop_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_wait_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_status_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char adc_config_help_string[] = "Configure ADC driver\n" \
                      "Usage: config(<dev>,<resolution>,<sample clocks>,<vref>,<count>,<channels>,...)\n" \
                      "\tdev = ADC1 | ADC2 | ADC3\n" \
                      "\tresolution = RES12 | RES10 | RES8 | RES6\n" \
                      "\tsample clocks = CLK3 | CLK15 | CLK28 | CLK56 | CLK84 | CLK112 | CLK144 | CLK480\n" \
                      "\tvref = <millivolts>\n" \
                      "\tcount = <sample count>\n" \
                      "\tchannels = CH0 | CH1 | CH2 | CH3 | CH4 | CH5 | CH6 | CH7\n" \
                      "\t           CH8 | CH9 | CH10 | CH11 | CH12 | CH13 | CH14 | CH 15\n" \
                      "\t           SENSOR, VREFINT, VBAT\n";

static fetch_command_t fetch_adc_commands[] = {
  /*  function                command string      help string */
    { fetch_adc_help_cmd,     "help",             "Display ADC help" },
    { fetch_adc_samples_cmd,  "samples",          "Return ADC samples" },
    { fetch_adc_start_cmd,    "start",            "Start ADC sampling" },
    { fetch_adc_stop_cmd,     "stop",             "Stop ADC sampling" },
    { fetch_adc_wait_cmd,     "wait",             "Wait for ADC to finish\nUsage: wait(timeout)\n\ttimeout = <milliseconds>" },
    { fetch_adc_status_cmd,   "status",           "Current ADC status" },
    { fetch_adc_config_cmd,   "config",           adc_config_help_string },
    { fetch_adc_reset_cmd,    "reset",            "Reset ADC driver" },
    { NULL, NULL, NULL }
  };

static adcsample_t adc_sample_buffer[FETCH_ADC_BUFFER_SIZE];

static uint32_t adc_sample_depth = 1;

static uint32_t adc_enabled_channels = 0; // channel bitmask

static uint32_t adc_vref_mv = FETCH_DEFAULT_VREF_MV;

static ADCDriver * adc_drv = NULL;

static BinarySemaphore adc_data_ready_sem;

static bool adc_init_flag = true;

static systime_t adc_start_timestamp = 0;
static volatile systime_t adc_end_timestamp = 0;

/*! \brief ADC conversion group configuration
 */
static ADCConversionGroup adc_conv_grp = {
	.circular        = false,
	.num_channels    = 0,
	.end_cb          = fetch_adc_end_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = 0,
	.smpr2           = 0,
	.sqr1            = 0, // ADC_SQR1_NUM_CH(n) | ADC_SQR1_N( ADC_CHANNEL_INnn ) ...
	.sqr2            = 0,
	.sqr3            = 0
};

static uint16_t fetch_adc_calc_temp(uint16_t t_raw, uint32_t uv_per_bit )
{

	uint32_t t_voltage = (t_raw * uv_per_bit) / 1000000; //>! \todo units are confusing in data sheet....review?

	return((uint16_t )(((t_voltage - ADC_V_25) / ADC_VSLOPE) + 25));
}

/*!
 * ADC end conversion callback
 */
static void fetch_adc_end_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n)
{

	(void) buffer;
	(void) n;
	/* Note, only in the ADC_COMPLETE state because the ADC driver fires an
	   intermediate callback when the buffer is half full.*/
	if (adcp->state == ADC_COMPLETE)
	{
    adc_end_timestamp = chTimeNow();

		chSysLockFromIsr();
    chBSemSignalI(&adc_data_ready_sem);
		chSysUnlockFromIsr();
	}
}

/*! \brief display adc help
 */
static bool fetch_adc_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  util_message_info(chp, "Fetch ADC Help:");
  fetch_display_help(chp, fetch_adc_commands);
	return true;
}


/*! \brief return sample data
 */
static bool fetch_adc_samples_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( adc_drv == NULL )
  {
    util_message_error(chp, "ADC not configured");
    return false;
  }

  if( adc_drv->state != ADC_READY )
  {
    util_message_error(chp, "ADC not ready");
    return false;
  }

  util_message_uint32(chp, "start_time", (uint32_t*)&adc_start_timestamp, 1);
  util_message_uint32(chp, "end_time", (uint32_t*)&adc_end_timestamp, 1);
  util_message_uint32(chp, "count", &adc_sample_depth,1);
  util_message_uint16(chp, "samples", (uint16_t*)&adc_sample_buffer, adc_sample_depth * adc_conv_grp.num_channels);

  return true;
}


/*! \brief Start a conversion
 */
static bool fetch_adc_start_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( adc_drv == NULL )
  {
    util_message_error(chp, "ADC not configured");
    return false;
  }

  if( adc_drv->state != ADC_READY )
  {
    util_message_error(chp, "ADC not ready");
    return false;
  }

  if( chBSemWaitTimeout(&adc_data_ready_sem, TIME_IMMEDIATE) == RDY_TIMEOUT )
  {
    util_message_error(chp, "ADC not ready");
    return false;
  }

	adcStartConversion( adc_drv, &adc_conv_grp, adc_sample_buffer, adc_sample_depth);
  adc_start_timestamp = chTimeNow();

	return true;
}

/*! \brief Stop the current conversion
 */
static bool fetch_adc_stop_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( adc_drv == NULL )
  {
    util_message_error(chp, "ADC not configured");
    return false;
  }

	adcStopConversion(adc_drv);
  
  adc_end_timestamp = chTimeNow();

  chBSemReset(&adc_data_ready_sem, 0);

	return true;
}

/*! \brief wait for adc to finish with timeout
 */
static bool fetch_adc_wait_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  int32_t timeout;
  char * endptr;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  if( adc_drv == NULL )
  {
    util_message_error(chp, "ADC not configured");
    return false;
  }

  timeout = strtol(data_list[0], &endptr, 0);

  if( timeout <= 0 || *endptr != '\0' )
  {
    util_message_error(chp, "invalid timeout");
    return false;
  }

  if( chBSemWaitTimeout(&adc_data_ready_sem, MS2ST(timeout)) == RDY_OK )
  {
    chBSemReset(&adc_data_ready_sem, 0);
  }

  util_message_bool(chp, "ready", (bool)(adc_drv->state == ADC_READY) );
  return true;
}

/*! \brief check if adc is done
 */
static bool fetch_adc_status_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( adc_drv == NULL )
  {
    util_message_error(chp, "ADC not configured");
    return false;
  }

  util_message_bool(chp, "ready", (bool)(adc_drv->state == ADC_READY) );
  return true;
}


/*! \brief Process an ADC configure command
 */
static bool fetch_adc_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  uint32_t adc_sample;
  char * endptr;
  int32_t sample_count;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 5 + FETCH_ADC_MAX_CHANNELS) )
  {
    return false;
  }

  if( adc_drv != NULL )
  {
    util_message_error(chp, "ADC already configured");
    util_message_info(chp, "use adc.reset");
    return false;
  }

  // reset conversion group settings
  adc_conv_grp.circular = false;
  adc_conv_grp.num_channels = 0;
  adc_conv_grp.cr1 = 0;
  adc_conv_grp.cr2 = ADC_CR2_SWSTART;
  adc_conv_grp.smpr1 = 0;
  adc_conv_grp.smpr2 = 0;
  adc_conv_grp.sqr1 = 0;
  adc_conv_grp.sqr2 = 0;
  adc_conv_grp.sqr3 = 0;
  
  switch( token_match( data_list[ADC_CONFIG_DEV], FETCH_MAX_DATA_STRLEN,
                       adc_dev_tok, NELEMS(adc_dev_tok)) )
  {
#if STM32_ADC_USE_ADC1
    case 0:
      adc_drv = &ADCD1;
      break;
#endif
#if STM32_ADC_USE_ADC2
    case 1:
      adc_drv = &ADCD2;
      break;
#endif
#if STM32_ADC_USE_ADC3
    case 2:
      adc_drv = &ADCD3;
      break;
#endif
    default:
      util_message_error(chp, "invalid adc device");
      return false;
  }

  switch( token_match( data_list[ADC_CONFIG_RES], FETCH_MAX_DATA_STRLEN,
                       adc_res_tok, NELEMS(adc_res_tok)) )
  {
    case 0: // 12
      break;
    case 1: // 10
      adc_conv_grp.cr1 |= ADC_CR1_RES_0;
      break;
    case 2: // 8
      adc_conv_grp.cr1 |= ADC_CR1_RES_1;
      break;
    case 3: // 6
      adc_conv_grp.cr1 |= ADC_CR1_RES_0 | ADC_CR1_RES_1;
      break;
    default:
      util_message_error(chp, "invalid adc resolution");
      adc_drv = NULL;
      return false;
  }

  int32_t vref_config = strtol(data_list[ADC_CONFIG_VREF], &endptr, 0);

  if( vref_config <= 0 || *endptr != '\0' )
  {
    util_message_error(chp, "invalid adc vref mv");
    adc_drv = NULL;
    return false;
  }
  else
  {
    adc_vref_mv = vref_config;
  }

  switch( token_match( data_list[ADC_CONFIG_CLK], FETCH_MAX_DATA_STRLEN,
                       adc_sample_tok, NELEMS(adc_sample_tok)) )
  {
    case 0:
      adc_sample = ADC_SAMPLE_3;
      break;
    case 1:
      adc_sample = ADC_SAMPLE_15;
      break;
    case 2:
      adc_sample = ADC_SAMPLE_28;
      break;
    case 3:
      adc_sample = ADC_SAMPLE_56;
      break;
    case 4:
      adc_sample = ADC_SAMPLE_84;
      break;
    case 5:
      adc_sample = ADC_SAMPLE_112;
      break;
    case 6:
      adc_sample = ADC_SAMPLE_144;
      break;
    case 7:
      adc_sample = ADC_SAMPLE_480;
      break;
    default:
      util_message_error(chp, "invalid adc sample clocks");
      adc_drv = NULL;
      return false;
  }
	
  adc_conv_grp.smpr1 =  ADC_SMPR1_SMP_AN10(   adc_sample ) |
                        ADC_SMPR1_SMP_AN13(   adc_sample ) |
	                      ADC_SMPR1_SMP_AN14(   adc_sample ) |
	                      ADC_SMPR1_SMP_AN15(   adc_sample ) |
	                      ADC_SMPR1_SMP_SENSOR( adc_sample ) |
	                      ADC_SMPR1_SMP_VREF(   adc_sample ) |
	                      ADC_SMPR1_SMP_VBAT(   adc_sample );

	adc_conv_grp.smpr2 =  ADC_SMPR2_SMP_AN0( adc_sample ) |
                        ADC_SMPR2_SMP_AN1( adc_sample ) |
                        ADC_SMPR2_SMP_AN2( adc_sample ) |
                        ADC_SMPR2_SMP_AN4( adc_sample ) |
                        ADC_SMPR2_SMP_AN5( adc_sample ) |
                        ADC_SMPR2_SMP_AN6( adc_sample ) |
                        ADC_SMPR2_SMP_AN7( adc_sample ) |
                        ADC_SMPR2_SMP_AN8( adc_sample ) |
                        ADC_SMPR2_SMP_AN9( adc_sample );

  sample_count = strtol(data_list[ADC_CONFIG_COUNT], &endptr, 0);

  if( sample_count <= 0 || *endptr != '\0' )
  {
    util_message_error(chp, "invalid sample count");
    adc_drv = NULL;
    return false;
  }

  // sample depths larger than 1 need to be even so add an extra sample if it is odd
  if( (sample_count > 1) && (sample_count & 1) )
  {
    adc_sample_depth = sample_count + 1;
  }
  else
  {
    adc_sample_depth = sample_count;
  }

  if( data_list[ADC_CONFIG_CHANNELS] == NULL )
  {
    util_message_error(chp, "missing adc channels");
    adc_drv = NULL;
    return false;
  }

  adc_enabled_channels = 0;

  int tok_num;

  for( int i = 0; i < FETCH_ADC_MAX_CHANNELS; i++ )
  {
    if( data_list[ADC_CONFIG_CHANNELS + i] == NULL )
    {
      break;
    }

    tok_num = token_match( data_list[ADC_CONFIG_CHANNELS + i], FETCH_MAX_DATA_STRLEN, 
                           adc_ch_tok, NELEMS(adc_ch_tok) );
    if( tok_num == TOKEN_NOT_FOUND )
    {
      util_message_error(chp, "invalid adc channel");
      adc_drv = NULL;
      return false;
    }
    else if( adc_enabled_channels & ADC_ENABLE_CH(tok_num) )
    {
      util_message_error(chp, "duplicate channels");
      adc_drv = NULL;
      return false;
    }
    else
    {
      adc_enabled_channels |= ADC_ENABLE_CH(tok_num);

      switch( adc_conv_grp.num_channels )
      {
        case 0:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ1_N(tok_num);
          break;
        case 1:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ2_N(tok_num);
          break;
        case 2:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ3_N(tok_num);
          break;
        case 3:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ4_N(tok_num);
          break;
        case 4:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ5_N(tok_num);
          break;
        case 5:
          adc_conv_grp.sqr3 |= ADC_SQR3_SQ6_N(tok_num);
          break;
        case 6:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ7_N(tok_num);
          break;
        case 7:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ8_N(tok_num);
          break;
        case 8:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ9_N(tok_num);
          break;
        case 9:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ10_N(tok_num);
          break;
        case 10:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ11_N(tok_num);
          break;
        case 11:
          adc_conv_grp.sqr2 |= ADC_SQR2_SQ12_N(tok_num);
          break;
        case 12:
          adc_conv_grp.sqr1 |= ADC_SQR1_SQ13_N(tok_num);
          break;
        case 13:
          adc_conv_grp.sqr1 |= ADC_SQR1_SQ14_N(tok_num);
          break;
        case 14:
          adc_conv_grp.sqr1 |= ADC_SQR1_SQ15_N(tok_num);
          break;
        case 15:
          adc_conv_grp.sqr1 |= ADC_SQR1_SQ16_N(tok_num);
          break;
      }
      adc_conv_grp.num_channels++;
    }
  }
  adc_conv_grp.sqr1 |= ADC_SQR1_NUM_CH(adc_conv_grp.num_channels);

  if( data_list[ADC_CONFIG_CHANNELS + FETCH_ADC_MAX_CHANNELS] != NULL )
  {
    util_message_error(chp, "too many channels");
    adc_drv = NULL;
    return false;
  }
  
  if( adc_sample_depth > FETCH_ADC_MAX_SAMPLES(adc_conv_grp.num_channels) )
  {
    util_message_error(chp, "sample count too large");
    adc_drv = NULL;
    return false;
  }


  // claim pins for adc use
#if STM32_ADC_USE_ADC1
  if( adc_drv == &ADCD1 )
  {
    for( int i = 0; i < FETCH_ADC_MAX_CHANNELS; i++ )
    {
      if( (adc_enabled_channels & ADC_ENABLE_CH(i)) &&
          !io_manage_set_mode( adc1_pins[i].port, adc1_pins[i].pin, PAL_MODE_INPUT_ANALOG, IO_ADC) )
      {
        util_message_error(chp, "unable to allocate %s:%d", port_to_string(adc1_pins[i].port), adc1_pins[i].pin);
        // loop backwards unrolling what we have done, starting with i-1
        for( i--; i >= 0; i-- )
        {
          io_manage_set_default_mode( adc1_pins[i].port, adc1_pins[i].pin, IO_ADC );
        }
        adc_drv = NULL;
        return false;
      }
    }
  }
#endif
#if STM32_ADC_USE_ADC2
  if( adc_drv == &ADCD2 )
  {
    for( int i = 0; i < FETCH_ADC_MAX_CHANNELS; i++ )
    {
      if( (adc_enabled_channels & ADC_ENABLE_CH(i)) &&
          !io_manage_set_mode( adc2_pins[i].port, adc2_pins[i].pin, PAL_MODE_INPUT_ANALOG, IO_ADC) )
      {
        util_message_error(chp, "unable to allocate %s:%d", port_to_string(adc2_pins[i].port), adc2_pins[i].pin);
        // loop backwards unrolling what we have done, starting with i-1
        for( i--; i >= 0; i-- )
        {
          io_manage_set_default_mode( adc2_pins[i].port, adc2_pins[i].pin, IO_ADC);
        }
        adc_drv = NULL;
        return false;
      }
    }
  }
#endif
#if STM32_ADC_USE_ADC3
  if( adc_drv == &ADCD3 )
  {
    for( int i = 0; i < FETCH_ADC_MAX_CHANNELS; i++ )
    {
      if( (adc_enabled_channels & ADC_ENABLE_CH(i)) &&
          !io_manage_set_mode( adc3_pins[i].port, adc3_pins[i].pin, PAL_MODE_INPUT_ANALOG, IO_ADC) )
      {
        util_message_error(chp, "unable to allocate %s:%d", port_to_string(adc3_pins[i].port), adc3_pins[i].pin);
        // loop backwards unrolling what we have done, starting with i-1
        for( i--; i >= 0; i-- )
        {
          io_manage_set_default_mode( adc3_pins[i].port, adc3_pins[i].pin, IO_ADC );
        }
        adc_drv = NULL;
        return false;
      }
    }
  }
#endif

  chBSemReset(&adc_data_ready_sem, 0);

  return true;
}

/*! \brief Reset adc1
 */
static bool fetch_adc_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  return fetch_adc_reset(chp);
}

void fetch_adc_init(BaseSequentialStream * chp)
{
#if STM32_ADC_USE_ADC1
  adcStart(&ADCD1,NULL);
#endif
#if STM32_ADC_USE_ADC2
  adcStart(&ADCD2,NULL);
#endif
#if STM32_ADC_USE_ADC3
  adcStart(&ADCD3,NULL);
#endif
  
  // enable the thermal sensor and VREFINT.
  adcSTM32EnableTSVREFE();
  adcSTM32EnableVBATE();

  chBSemInit(&adc_data_ready_sem, 0);

  adc_init_flag = false;
}

/*! \brief dispatch an ADC command
 */
bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( adc_init_flag )
  {
    fetch_adc_init(chp);
  }

  return fetch_dispatch(chp, fetch_adc_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}

bool fetch_adc_reset(BaseSequentialStream * chp)
{
  if( adc_drv == NULL )
  {
    return true;
  }

#if STM32_ADC_USE_ADC1
  if( adc_drv == &ADCD1 )
  {
    for( uint32_t i = 0; i < NELEMS(adc1_pins); i++ )
    {
      if( io_manage_get_current_alloc( adc1_pins[i].port, adc1_pins[i].pin ) == IO_ADC )
      {
        io_manage_set_default_mode( adc1_pins[i].port, adc1_pins[i].pin, IO_ADC );
      }
    }
  }
#endif
#if STM32_ADC_USE_ADC2
  if( adc_drv == &ADCD2 )
  {
    for( uint32_t i = 0; i < NELEMS(adc2_pins); i++ )
    {
      if( io_manage_get_current_alloc( adc2_pins[i].port, adc2_pins[i].pin ) == IO_ADC )
      {
        io_manage_set_default_mode( adc2_pins[i].port, adc2_pins[i].pin, IO_ADC );
      }
    }
  }
#endif
#if STM32_ADC_USE_ADC3
  if( adc_drv == &ADCD3 )
  {
    for( uint32_t i = 0; i < NELEMS(adc3_pins); i++ )
    {
      if( io_manage_get_current_alloc( adc3_pins[i].port, adc3_pins[i].pin ) == IO_ADC )
      {
        io_manage_set_default_mode( adc3_pins[i].port, adc3_pins[i].pin, IO_ADC );
      }
    }
  }
#endif

  adc_drv = NULL;

  chBSemReset(&adc_data_ready_sem, 0);
  
  return true;
}

/*! @} */


