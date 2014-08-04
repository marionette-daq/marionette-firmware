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
#include "util_version.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"

#define FETCH_DEFAULT_VREF_MV         3300
#define FETCH_ADC_DATA_STACKSIZE      2048

/*! Total number of channels to be sampled by a single ADC operation.*/
#define FETCH_ADC1_DEMO_GRP_NUM_CHANNELS      2
#define FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS   1
#define FETCH_ADC1_PA_GRP_NUM_CHANNELS      1

/*! Depth of the conversion buffer, channels are sampled four times each.*/
#define FETCH_ADC1_DEMO_GRP_BUF_DEPTH         1
#define FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH      1
#define FETCH_ADC1_PA_GRP_BUF_DEPTH         1

//#ifdef BOARD_ST_STM32F4_DISCOVERY

/*! \brief pin and port definitions */
#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static const ADC_input ADC1_IN0   = { GPIOA, GPIOA_PIN0 } ;
static const ADC_input ADC1_IN1   = { GPIOA, GPIOA_PIN1 } ;
static const ADC_input ADC1_IN2   = { GPIOA, GPIOA_PIN2 } ;
//static const ADC_input ADC1_IN3   = { GPIOA, GPIOA_PIN3 } ;  // used for VBUS
static const ADC_input ADC1_IN4   = { GPIOA, GPIOA_PIN4 } ;
static const ADC_input ADC1_IN5   = { GPIOA, GPIOA_PIN5 } ;
static const ADC_input ADC1_IN6   = { GPIOA, GPIOA_PIN6 } ;
static const ADC_input ADC1_IN7   = { GPIOA, GPIOA_PIN7 } ;
static const ADC_input ADC1_IN8   = { GPIOB, GPIOB_PIN0 } ;
static const ADC_input ADC1_IN9   = { GPIOB, GPIOB_PIN9 } ;
static const ADC_input ADC1_IN10  = { GPIOC, GPIOC_PIN0 } ;
//static const ADC_input ADC1_IN11  = { GPIOC, GPIOC_PIN1 } ;  // used for OTG
//static const ADC_input ADC1_IN12  = { GPIOC, GPIOC_PIN2 } ;  // used for OTG
static const ADC_input ADC1_IN13  = { GPIOC, GPIOC_PIN3 } ;
static const ADC_input ADC1_IN14  = { GPIOC, GPIOC_PIN4 } ;
static const ADC_input ADC1_IN15  = { GPIOC, GPIOC_PIN5 } ;
#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for ADC"
#else
#error "Board not defined for ADC"
#endif

static void   fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);

static adcsample_t adc1_demo_sample_buf[FETCH_ADC1_DEMO_GRP_NUM_CHANNELS * FETCH_ADC1_DEMO_GRP_BUF_DEPTH];
static adcsample_t adc1_default_sample_buf[FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS * FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH];
static adcsample_t adc1_pa_sample_buf[FETCH_ADC1_PA_GRP_NUM_CHANNELS * FETCH_ADC1_PA_GRP_BUF_DEPTH];

static EventSource fetch_adc1_data_ready;

/*! \brief ADC default conversion group.
 * Mode:        Linear buffer, 1 adc1_sample_buf of 2 channels, SW triggered.
 * Channels:    IN13   (56 cycles sample time)
 *              Sensor (144 cycles sample time)
 * Analog Watchdog: TBD  (need to set adc_ltr and adc_htr which are not part of ADCConversionGroup)
 */
static ADCConversionGroup adc1_demo_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_DEMO_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_144),
	.smpr2           = 0,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_DEMO_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ2_N(ADC_CHANNEL_IN13) | ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)
};

/*! \brief ADC default conversion group.
 * Mode:        Linear buffer, 1 adc1_sample_buf of 1 channels, SW triggered.
 * Channels:    IN13   (3 cycles sample time)
 * Analog Watchdog: TBD  (need to set adc_ltr and adc_htr which are not part of ADCConversionGroup)
 */
static ADCConversionGroup adc1_default_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56),
	.smpr2           = 0,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN13)
};

/*! \brief ADC default conversion group.
 * Mode:        Linear buffer, 1 adc1_sample_buf of 1 channels, SW triggered.
 * Channels:    IN13   (3 cycles sample time)
 * Analog Watchdog: TBD  (need to set adc_ltr and adc_htr which are not part of ADCConversionGroup)
 */
static ADCConversionGroup adc1_pa_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_PA_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	//.smpr1           = ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56),
	.smpr1           = 0,
	.smpr2           = ADC_SMPR2_SMP_AN0(ADC_SAMPLE_56),
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)
};


/*! \brief ADC1 Demo profile
 * oneshot
 * adc1_default_cfg
 * adc1_sample_buf
 */
static const  FETCH_adc_profile adc1_demo_profile =
{
	.name                 = FETCH_ADC_DEMO,
	.oneshot              = true,
	.adcgrpcfg            = &adc1_demo_cfg,
	.adc_grp_num_channels = FETCH_ADC1_DEMO_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_DEMO_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_demo_sample_buf
};


/*! \brief ADC1 Default profile
 * oneshot
 * adc1_default_cfg
 * adc1_sample_buf
 */
static const  FETCH_adc_profile adc1_default_profile =
{
	.name                 = FETCH_ADC_DEFAULT,
	.oneshot              = true,
	.adcgrpcfg            = &adc1_default_cfg,
	.adc_grp_num_channels = FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_default_sample_buf
};

static const  FETCH_adc_profile adc1_pa_profile =
{
	.name                 = FETCH_ADC_PA,
	.oneshot              = true,
	.adcgrpcfg            = &adc1_pa_cfg,
	.adc_grp_num_channels = FETCH_ADC1_PA_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_PA_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_pa_sample_buf
};


/*! \brief track the state of the conversion
 *
 * Idea is that starting from a default profile, user may change
 * configuration of conversion.
 * Example instead of oneshot user may select continuous conversion
 * There may be more....
 * Keeping the profiles const allows reset
 */
static FETCH_adc_state fetch_adc1_state =
{
	.vref_mv              = 3300,
	.init                 = false,
	.oneshot              = true,
	.profile              = &adc1_default_profile,
	.chp                  = NULL
};

static uint16_t fetch_adc_calc_temp(uint16_t t_raw, uint32_t uv_per_bit ) {

		uint32_t t_voltage = (t_raw * uv_per_bit)/1000000;  // \todo units are confusing in data sheet....review?

		return((uint16_t )(((t_voltage-ADC_V_25)/ADC_VSLOPE) +25));
}

/*! \brief Process new data based on current profile
 */
static void adc1_new_data(eventid_t id UNUSED) 
{
	BaseSequentialStream * chp        = fetch_adc1_state.chp;
	uint32_t               uv_per_bit = ((fetch_adc1_state.vref_mv*1000) / 4096); 

	if((fetch_adc1_state.profile->adc_grp_buf_depth > 0) && (fetch_adc1_state.chp != NULL))
	{
		systime_t   timenow = 0 ;    
		adcsample_t avg_ch1 = 0 ;
		adcsample_t avg_ch2 = 0 ;
		switch(fetch_adc1_state.profile->name)
		{
			case FETCH_ADC_DEFAULT:
				avg_ch1  = fetch_adc1_state.profile->adc_sample_buf[0] / fetch_adc1_state.profile->adc_grp_buf_depth;
				timenow = chTimeNow();
				chprintf(fetch_adc1_state.chp, "DEF:\t%u,%u\r\n", timenow, avg_ch1 * uv_per_bit);
				break;
			case FETCH_ADC_DEMO:
				avg_ch1  = fetch_adc1_state.profile->adc_sample_buf[0] / fetch_adc1_state.profile->adc_grp_buf_depth;
				avg_ch2  = fetch_adc1_state.profile->adc_sample_buf[1] / fetch_adc1_state.profile->adc_grp_buf_depth;
				timenow = chTimeNow();
				chprintf(fetch_adc1_state.chp, "DEMO:\t%u,T(raw): %u\tT(C): %u\tIN13(raw): %u\tIN13(uV):%u\r\n",timenow, avg_ch1, fetch_adc_calc_temp(avg_ch1, uv_per_bit), avg_ch2, avg_ch2 * uv_per_bit);
				break;
			case FETCH_ADC_PA:
				avg_ch1  = fetch_adc1_state.profile->adc_sample_buf[0] / fetch_adc1_state.profile->adc_grp_buf_depth;
				timenow = chTimeNow();
				chprintf(fetch_adc1_state.chp, "PA:\t%u,%u\r\n", timenow, avg_ch1 * uv_per_bit);
				break;
			case FETCH_ADC_PB:
				break;
			default:
				break;
		}
	}
}

/*! \brief Thread for new data event
  *
  * chprintf can not be called from within an interrupt ('I')
  *   context in ChibiOS. The interrupt callback generates an
  *   event which this thread receives.
  */
static WORKING_AREA(wa_fetch_adc_data, FETCH_ADC_DATA_STACKSIZE);
static msg_t fetch_adc_data(void * p UNUSED)
{
	struct       EventListener el0;

	static const evhandler_t   evhndl[] =
	{
		adc1_new_data
	};

	chRegSetThreadName("fetch_adc_data");
	chEvtRegister(&fetch_adc1_data_ready, &el0, 0);

	while (TRUE)
	{
		chEvtDispatch(evhndl, chEvtWaitAny(ALL_EVENTS));
	}

	/* Never executed, silencing a warning.*/
	return 0;
}

/*
 * ADC end conversion callback
 */
static void fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n)
{

	(void) buffer;
	(void) n;
	/* Note, only in the ADC_COMPLETE state because the ADC driver fires an
	   intermediate callback when the buffer is half full.*/
	if (adcp->state == ADC_COMPLETE)
	{
		// check analog watchdog?
		chSysLockFromIsr();
		chEvtBroadcastI(&fetch_adc1_data_ready);
		chSysUnlockFromIsr();

		chSysLockFromIsr();
		if(!fetch_adc1_state.oneshot)
		{
			adcStartConversionI(&ADCD1, fetch_adc1_state.profile->adcgrpcfg, fetch_adc1_state.profile->adc_sample_buf, fetch_adc1_state.profile->adc_grp_buf_depth);
		}
		chSysUnlockFromIsr();
	}
}

/*! \brief Reset adc1
 */
static void fetch_adc1_reset(void)
{
	adcStop(&ADCD1);
	fetch_adc_init(fetch_adc1_state.chp);
}

/*! \brief change the profile for adc1
 */
bool fetch_adc1_profile(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                        char * cmd_list[])
{
	if (strncasecmp(cmd_list[ADC_PROFILE], "default", strlen("default") ) == 0)
	{
		fetch_adc1_reset();
		fetch_adc1_state.profile = &adc1_default_profile;
		return true;
	}
	else if (strncasecmp(cmd_list[ADC_PROFILE], "demo", strlen("demo") ) == 0)
	{
		fetch_adc1_reset();
		fetch_adc1_state.profile = &adc1_demo_profile;
		return true;
	}
	else if (strncasecmp(cmd_list[ADC_PROFILE], "pa", strlen("pa") ) == 0)
	{
		fetch_adc1_reset();
		fetch_adc1_state.profile = &adc1_pa_profile;
		return true;
	}
	else
	{
		DBG_VMSG(chp, "Profile %s not available.", cmd_list[ADC_PROFILE]);
		return false;
	}
	return false;
}

/*! \brief Initialize the ADC 
 *
 * Use default profile
 * Use oneshot by default
 *
 */
void fetch_adc_init(BaseSequentialStream * chp)
{
	/*
	 * Initializes the ADC driver 1 and enable the thermal sensor.
	 * The pin PC1 on the port GPIOC is programmed as analog input.
	 */
	chEvtInit(&fetch_adc1_data_ready);

	fetch_adc1_state.vref_mv                = FETCH_DEFAULT_VREF_MV;
	fetch_adc1_state.init                   = false;
	fetch_adc1_state.oneshot                = true;
	fetch_adc1_state.profile                = &adc1_default_profile;
	fetch_adc1_state.chp                    = chp;

	adcStart(&ADCD1, NULL);
	//adcConfigWatchdog()...
	adcSTM32EnableTSVREFE();
	palSetPadMode(ADC1_IN13.port, ADC1_IN13.pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ADC1_IN0.port, ADC1_IN0.pin, PAL_MODE_INPUT_ANALOG);
	fetch_adc1_state.chp = chp;

	// start a thread waiting for data event.....
	chThdCreateStatic(wa_fetch_adc_data, sizeof(wa_fetch_adc_data), NORMALPRIO, fetch_adc_data,
	                  NULL);

	fetch_adc1_state.init = true;
}

/*! \brief Start a conversion
 */
static bool fetch_adc1_start(void)
{
	adcStartConversion(&ADCD1, fetch_adc1_state.profile->adcgrpcfg,
	                   fetch_adc1_state.profile->adc_sample_buf, fetch_adc1_state.profile->adc_grp_buf_depth);
	return true;
}


/*! \brief Stop the current conversion
 */
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

/*! \brief Process an ADC configure command
 */
static bool fetch_adc1_configure(BaseSequentialStream * chp ,
                                 Fetch_terminals * fetch_terms, char * cmd_list[], char * data_list[])
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
		else if (strncasecmp(cmd_list[ADC_CONFIGURE], "profile", strlen("profile") ) == 0)
		{
			return(fetch_adc1_profile(chp, fetch_terms, cmd_list));
		}
		else if (strncasecmp(cmd_list[ADC_CONFIGURE], "reset", strlen("reset") ) == 0)
		{
			fetch_adc1_reset();
			return true;
		}
		else if (strncasecmp(cmd_list[ADC_CONFIGURE], "vref_mv", strlen("vref_mv") ) == 0)
		{
			fetch_adc1_state.vref_mv = atoi(data_list[0]);
			//DBG_VMSG(chp, "vref_mv: %d\tdata_list[0]: %s\r\n", fetch_adc1_state.vref_mv, data_list[0]);
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

/*! \brief dispatch an ADC command
 */
bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                        Fetch_terminals * fetch_terms)
{
	fetch_adc1_state.chp = chp;
	if(fetch_adc_is_valid_adc_subcommandA(chp, fetch_terms, cmd_list[ADC_ACTION]) >= 0)
	{

		if (strncasecmp(cmd_list[ADC_ACTION], "conf_adc1", strlen("conf_adc1") ) == 0)
		{
			return(fetch_adc1_configure(chp, fetch_terms, cmd_list, data_list));
		}
		else if (strncasecmp(cmd_list[ADC_ACTION], "start", strlen("start") ) == 0)
		{
			return(fetch_adc1_start());
		}
		else if (strncasecmp(cmd_list[ADC_ACTION], "stop", strlen("stop") ) == 0)
		{
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
