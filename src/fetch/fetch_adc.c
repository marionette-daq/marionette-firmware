/*! \file fetch_adc.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @addtogroup fetch_adc
  * @{
  */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"
#include "adc_lld.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"

/* Total number of channels to be sampled by a single ADC operation.*/
#define FETCH_ADC1_GRP_NUM_CHANNELS   2

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define FETCH_ADC1_GRP_BUF_DEPTH      1


//#ifdef BOARD_ST_STM32F4_DISCOVERY
//#ifdef BOARD_WAVESHARE_CORE407I
// #endif

static const ADC_input ADC1_IN0   = { GPIOA, GPIOA_PIN0 } ;
static const ADC_input ADC1_IN1   = { GPIOA, GPIOA_PIN1 } ;
static const ADC_input ADC1_IN2   = { GPIOA, GPIOA_PIN2 } ;
static const ADC_input ADC1_IN3   = { GPIOA, GPIOA_PIN3 } ;
static const ADC_input ADC1_IN4   = { GPIOA, GPIOA_PIN4 } ;
static const ADC_input ADC1_IN5   = { GPIOA, GPIOA_PIN5 } ;
static const ADC_input ADC1_IN6   = { GPIOA, GPIOA_PIN6 } ;
static const ADC_input ADC1_IN7   = { GPIOA, GPIOA_PIN7 } ;
static const ADC_input ADC1_IN8   = { GPIOB, GPIOA_PIN0 } ;
static const ADC_input ADC1_IN9   = { GPIOB, GPIOB_PIN9 } ;
static const ADC_input ADC1_IN10  = { GPIOC, GPIOC_PIN0 } ;
//static const ADC_input ADC1_IN11  = { GPIOC, GPIOC_PIN1 } ;  // used for OTG
//static const ADC_input ADC1_IN12  = { GPIOC, GPIOC_PIN2 } ;  // used for OTG
static const ADC_input ADC1_IN13  = { GPIOC, GPIOC_PIN3 } ;
static const ADC_input ADC1_IN14  = { GPIOC, GPIOC_PIN4 } ;
static const ADC_input ADC1_IN15  = { GPIOC, GPIOC_PIN5 } ;

static adcsample_t adc1_sample_buf[FETCH_ADC1_GRP_NUM_CHANNELS * FETCH_ADC1_GRP_BUF_DEPTH];
void fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);

/*! \brief track the state of the conversion
 *
 * Idea is that starting from a default profile, user may change
 * configuration of conversion.
 * Example instead of oneshot user may select continuous conversion
 * There may be more....
 */
static FETCH_adc_state fetch_adc1_state =
{
	.oneshot = true,
	.chp     = NULL
};


/*! \brief ADC default conversion group.
 * Mode:        Linear buffer, 1 adc1_sample_buf of 2 channels, SW triggered.
 * Channels:    IN13   (56 cycles sample time)
 *              Sensor (144 cycles sample time)
 * Analog Watchdog: TBD  (need to set adc_ltr and adc_htr which are not part of ADCConversionGroup)
 */
static ADCConversionGroup adc1_default_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_144),
	.smpr2           = 0,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ2_N(ADC_CHANNEL_IN13) | ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)
};

/*! \brief ADC1 Default profile
 * oneshot
 * adc1_default_cfg
 * adc1_sample_buf
 */
static  FETCH_adc_profile adc1_default_profile =
{
	.oneshot              = true,
	.adcgrpcfg            = &adc1_default_cfg,
	.adc_grp_num_channels = FETCH_ADC1_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_sample_buf    // should this be &adc1_sample buf?
};

void fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n) ;

/*! \brief ADC conversion group.
 * Mode:        Linear buffer, 4 adc1_sample_buf of 2 channels, SW triggered.
 * Channels:    IN11   (48 cycles sample time)
 *              Sensor (192 cycles sample time)
 */
static const ADCConversionGroup adcgrpcfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0, // watchdog too?
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN11(ADC_SAMPLE_56) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_144),
	.smpr2           = 0,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)
};

//Wed 23 July 2014 18:22:27 (PDT)
// define adc state structure.
// test current configuration
// define profile
// continuous/oneshot
// conversiongrp

/*
 * ADC end conversion callback.
 */
void fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n)
{

	(void) buffer;
	(void) n;
	/* Note, only in the ADC_COMPLETE state because the ADC driver fires an
	   intermediate callback when the buffer is half full.*/
	if (adcp->state == ADC_COMPLETE)
	{
		adcsample_t avg_ch1, avg_ch2;

		// change this to reflect numadc1_sample_buf?  and depth of adc1_sample_buf...

		// check analog watchdog?

		/* Calculates the average values from the ADC adc1_sample_buf.*/
		//avg_ch1 = (adc1_sample_buf[0] + adc1_sample_buf[2] + adc1_sample_buf[4] + adc1_sample_buf[6]) /
				  //4;
		//avg_ch2 = (adc1_sample_buf[1] + adc1_sample_buf[3] + adc1_sample_buf[5] + adc1_sample_buf[7]) /
				  //4;

		if(FETCH_ADC1_GRP_BUF_DEPTH > 0) {
			avg_ch1  = adc1_sample_buf[0]/FETCH_ADC1_GRP_BUF_DEPTH;
			avg_ch2  = adc1_sample_buf[1]/FETCH_ADC1_GRP_BUF_DEPTH;
		} else {
			avg_ch1 = 0;
			avg_ch2 = 0;
		}

		chSysLockFromIsr();

		// read 64 bit timer
		//  print result (timestamp ch1 ch2) here...
		if(fetch_adc1_state.chp != NULL) {
			chprintf(fetch_adc1_state.chp, "%d,%d\r\n\r\n", avg_ch1, avg_ch2); 
		}
		// converted value

		if(!fetch_adc1_state.oneshot)
		{
			adcStartConversionI(&ADCD1, adc1_default_profile.adcgrpcfg, adc1_default_profile.adc_sample_buf, adc1_default_profile.adc_grp_buf_depth);
		}

		chSysUnlockFromIsr();
	}
}

/*
 *have different adc profiles defined
 *default - int ref
 *default - ext ref
 *Then configure
 *
 */

//void fetch_adc_init(struct * adc_profile....)
void fetch_adc_init(void)
{
	/*
	 * Initializes the ADC driver 1 and enable the thermal sensor.
	 * The pin PC1 on the port GPIOC is programmed as analog input.
	 */
	adcStart(&ADCD1, NULL);
	//adcConfigWatchdog()...
	adcSTM32EnableTSVREFE();
	palSetPadMode(ADC1_IN13.port, ADC1_IN13.pin, PAL_MODE_INPUT_ANALOG);
}

static bool fetch_adc1_start(void)
{
	adcStartConversion(&ADCD1, adc1_default_profile.adcgrpcfg, adc1_default_profile.adc_sample_buf,
	                   adc1_default_profile.adc_grp_buf_depth);
	return true;
}


static bool fetch_adc1_stop(void)
{
	adcStopConversion(&ADCD1) ;
	return true;
}

static inline int fetch_adc_is_valid_adc_configure(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkadc_configure)
{

	return(token_match(chp, fetch_terms->adc_configure, chkadc_configure,
	                   ((int) NELEMS(fetch_terms->adc_configure)) ) );
}


static bool fetch_adc1_configure(BaseSequentialStream * chp ,
                                Fetch_terminals * fetch_terms, char * cmd_list[])
{
	if(fetch_adc_is_valid_adc_configure(chp, fetch_terms, cmd_list[ADC_CONFIGURE]) >= 0)
	{
		if (strncasecmp(cmd_list[ADC_CONFIGURE], "oneshot", strlen("oneshot") ) == 0)
		{
			fetch_adc1_state.oneshot = true;
			return true;
		}
		else if (strncasecmp(cmd_list[ADC_CONFIGURE], "continuous", strlen("continuous") ) == 0)
		{
			fetch_adc1_state.oneshot = false;
			return true;
		}
		else
		{
			DBG_MSG(chp, "configure command not ready yet...");
			return false ;
		}
	}
	return false;
}


static inline int fetch_adc_is_valid_adc_subcommandA(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkadc_subcommandA)
{

	return(token_match(chp, fetch_terms->adc_subcommandA, chkadc_subcommandA,
	                   ((int) NELEMS(fetch_terms->adc_subcommandA)) ) );
}


bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                        Fetch_terminals * fetch_terms)
{
	fetch_adc1_state.chp = chp;
	if(fetch_adc_is_valid_adc_subcommandA(chp, fetch_terms, cmd_list[ADC_ACTION]) >= 0)
	{

		if (strncasecmp(cmd_list[ADC_ACTION], "conf_adc1", strlen("conf_adc1") ) == 0) {
			return(fetch_adc1_configure(chp, fetch_terms, cmd_list));
		}
		else if (strncasecmp(cmd_list[ADC_ACTION], "start", strlen("start") ) == 0) {
			return(fetch_adc1_start());
		}
		else if (strncasecmp(cmd_list[ADC_ACTION], "stop", strlen("stop") ) == 0) {
			return(fetch_adc1_stop());
		}
		else
		{
			DBG_MSG(chp, "sub-command not ready yet...");
			return(false) ;
		}
	}
	return false;
}

/*! @} */

