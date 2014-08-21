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

#include "mshell_state.h"

#include "io_manage.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"

#define        FETCH_DEFAULT_VREF_MV                 3300
#define        FETCH_ADC_DATA_STACKSIZE              2048
#define        FETCH_ADC_MAX_DATA                    64
#define        FETCH_ADC1_MAX_CHANNELS               16   // maximum number of channels that can be scanned. 19 available.

/*! Total number of channels to be sampled by a single ADC operation.*/
#define        FETCH_ADC1_DEMO_GRP_NUM_CHANNELS      2
#define        FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS   1
#define        FETCH_ADC1_PA_GRP_NUM_CHANNELS        16

/*! Depth of the conversion buffer.*/
#define        FETCH_ADC1_DEMO_GRP_BUF_DEPTH         4
#define        FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH      10
#define        FETCH_ADC1_PA_GRP_BUF_DEPTH           1

typedef enum channel_io
{
	ADC_CH0 = 0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_CH7,
	ADC_CH8,
	ADC_CH9,
	ADC_CH10,
	ADC_CH11,
	ADC_CH12,
	ADC_CH13,
	ADC_CH14,
	ADC_CH15,
	ADC_CH16,   // ADC1 only
	ADC_CH17,   // ADC1 only
	ADC_CH18    // ADC1 only
} ADCChannel;

/*! \brief pin and port definitions */
#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static ADC_input ADC1_IN0   = { GPIOA, GPIOA_PIN0 };
static ADC_input ADC1_IN1   = { GPIOA, GPIOA_PIN1 };
static ADC_input ADC1_IN2   = { GPIOA, GPIOA_PIN2 };
static ADC_input ADC1_IN3   = { GPIOA, GPIOA_PIN3 };
static ADC_input ADC1_IN4   = { GPIOA, GPIOA_PIN4 };
static ADC_input ADC1_IN5   = { GPIOA, GPIOA_PIN5 };
static ADC_input ADC1_IN6   = { GPIOA, GPIOA_PIN6 };
static ADC_input ADC1_IN7   = { GPIOA, GPIOA_PIN7 };
static ADC_input ADC1_IN8   = { GPIOB, GPIOB_PIN0 };
static ADC_input ADC1_IN9   = { GPIOB, GPIOB_PIN9 };
static ADC_input ADC1_IN10  = { GPIOC, GPIOC_PIN0 };
//static ADC_input ADC1_IN11  = { GPIOC, GPIOC_PIN1 },    // used for OTG
//static ADC_input ADC1_IN12  = { GPIOC, GPIOC_PIN2 },    // used for OTG
static ADC_input ADC1_IN13  = { GPIOC, GPIOC_PIN3 };
static ADC_input ADC1_IN14  = { GPIOC, GPIOC_PIN4 };
static ADC_input ADC1_IN15  = { GPIOC, GPIOC_PIN5 };

static ADC_input * ADC1_inputs[] = {&ADC1_IN0, &ADC1_IN1, &ADC1_IN2, NULL, &ADC1_IN4, &ADC1_IN5, &ADC1_IN6, &ADC1_IN7, &ADC1_IN8, &ADC1_IN9, &ADC1_IN10, NULL, NULL, &ADC1_IN13, &ADC1_IN14, &ADC1_IN15} ;
#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for ADC"
#else
#error "Board not defined for ADC"
#endif

static void   fetch_adc1_cb(ADCDriver * adcp, adcsample_t * buffer, size_t n);

static adcsample_t adc1_demo_sample_buf[FETCH_ADC1_DEMO_GRP_NUM_CHANNELS * FETCH_ADC1_DEMO_GRP_BUF_DEPTH];
static adcsample_t adc1_default_sample_buf[FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS * FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH];
static adcsample_t adc1_pa_sample_buf[FETCH_ADC1_PA_GRP_NUM_CHANNELS * FETCH_ADC1_PA_GRP_BUF_DEPTH];

static ADCChannel adc1_default_channel = ADC_CH13;

static EventSource fetch_adc1_data_ready;

/*! \brief ADC demo conversion group.
 * Mode:        Linear buffer, 2 channels, SW triggered.
 * Channels:    IN13   (56 cycles sample time)
 *              Sensor (144 cycles sample time)
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
 * Mode:        Linear buffer,  1 channel, SW triggered.
 * Channels:    IN13   (56 cycles sample time)
 */
static ADCConversionGroup adc1_default_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW registers */
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56),
	.smpr2           = 0,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN13)
};

/*! \brief ADC PA conversion group.
 *  Mode:        Linear buffer, 16 channels, SW triggered.
 *  Channels:    All available inputs for Waveshare (56 cycles sample time)
 */
static ADCConversionGroup adc1_pa_cfg =
{
	.circular        = FALSE,
	.num_channels    = FETCH_ADC1_PA_GRP_NUM_CHANNELS,
	.end_cb          = fetch_adc1_cb,
	.error_cb        = NULL,
	/* HW registers */
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_AN13(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_AN14(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_AN15(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_VREF(ADC_SAMPLE_56) \
	| ADC_SMPR1_SMP_VBAT(ADC_SAMPLE_56)
	,
	.smpr2           = ADC_SMPR2_SMP_AN0(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN1(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN2(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN4(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN5(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN6(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN7(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN8(ADC_SAMPLE_56) \
	| ADC_SMPR2_SMP_AN9(ADC_SAMPLE_56) \
	,
	.sqr1            = ADC_SQR1_NUM_CH(FETCH_ADC1_PA_GRP_NUM_CHANNELS) \
	| ADC_SQR1_SQ13_N(ADC_CHANNEL_IN15)   \
	| ADC_SQR1_SQ14_N(ADC_CHANNEL_SENSOR)   \
	| ADC_SQR1_SQ15_N(ADC_CHANNEL_VREFINT)   \
	| ADC_SQR1_SQ16_N(ADC_CHANNEL_VBAT)   \
	,
	.sqr2            = ADC_SQR2_SQ7_N(ADC_CHANNEL_IN7)   \
	| ADC_SQR2_SQ8_N(ADC_CHANNEL_IN8)   \
	| ADC_SQR2_SQ9_N(ADC_CHANNEL_IN9)   \
	| ADC_SQR2_SQ10_N(ADC_CHANNEL_IN10)   \
	| ADC_SQR2_SQ11_N(ADC_CHANNEL_IN13)   \
	| ADC_SQR2_SQ12_N(ADC_CHANNEL_IN14)   \
	,
	.sqr3            = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)   \
	| ADC_SQR3_SQ2_N(ADC_CHANNEL_IN1)   \
	| ADC_SQR3_SQ3_N(ADC_CHANNEL_IN2)   \
	| ADC_SQR3_SQ4_N(ADC_CHANNEL_IN4)   \
	| ADC_SQR3_SQ5_N(ADC_CHANNEL_IN5)   \
	| ADC_SQR3_SQ6_N(ADC_CHANNEL_IN6)
};

static const  FETCH_adc_profile adc1_demo_profile =
{
	.name                 = FETCH_ADC1_DEMO,
	.oneshot              = true,
	.adcgrpcfg            = &adc1_demo_cfg,
	.adc_grp_num_channels = FETCH_ADC1_DEMO_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_DEMO_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_demo_sample_buf
};

static const  FETCH_adc_profile adc1_default_profile =
{
	.name                 = FETCH_ADC1_DEFAULT,
	.oneshot              = true,
	.adcgrpcfg            = &adc1_default_cfg,
	.adc_grp_num_channels = FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS,
	.adc_grp_buf_depth    = FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH,
	.adc_sample_buf       = adc1_default_sample_buf
};

static const  FETCH_adc_profile adc1_pa_profile =
{
	.name                 = FETCH_ADC1_PA,
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

static uint16_t fetch_adc_calc_temp(uint16_t t_raw, uint32_t uv_per_bit )
{

	uint32_t t_voltage = (t_raw * uv_per_bit) / 1000000; //>! \todo units are confusing in data sheet....review?

	return((uint16_t )(((t_voltage - ADC_V_25) / ADC_VSLOPE) + 25));
}

/*! \brief Average result by the number of ADC conversions in sample buffer
 */
static void sum_reduce_samples(uint32_t * results)

{
	// sum
	for(uint16_t j = 0; j < fetch_adc1_state.profile->adc_grp_num_channels; ++j)
	{
		for(uint16_t i = j; i < fetch_adc1_state.profile->adc_grp_buf_depth * fetch_adc1_state.profile->adc_grp_num_channels ;
		                i += fetch_adc1_state.profile->adc_grp_num_channels )
		{
			results[j] += fetch_adc1_state.profile->adc_sample_buf[i] ;
		}
	}

	// reduce
	for(uint16_t i = 0; i < fetch_adc1_state.profile->adc_grp_num_channels; ++i)
	{
		results[i] /=  fetch_adc1_state.profile->adc_grp_buf_depth;
	}
}

/*! \brief Process new data based on current profile
 */
static void adc1_new_data(eventid_t id UNUSED)
{
	BaseSequentialStream * chp        = fetch_adc1_state.chp;

	if((fetch_adc1_state.profile->adc_grp_buf_depth > 0) && (fetch_adc1_state.chp != NULL))
	{
		systime_t               timenow = chTimeNow() ;
		uint32_t                uv_per_bit = ((fetch_adc1_state.vref_mv * 1000) / 4096); // 12 bit conversion
		uint32_t                avg_vals[FETCH_ADC1_MAX_CHANNELS] = {0};
		uint32_t                degC, adcUV;
		if(getMShellVisiblePrompt())
		{
			mshell_putnewline();
		}
		switch(fetch_adc1_state.profile->name)
		{
			case FETCH_ADC1_DEFAULT: // 1 channel in this profile
				sum_reduce_samples(avg_vals);
				util_message_uint32(chp, "time", &timenow, 1);
				avg_vals[0] *= uv_per_bit; // convert to micro-volt
				util_message_uint32(chp, "adc", avg_vals, 1);
				util_message_uint32(chp, "adc", avg_vals, 1);
				break;
			case FETCH_ADC1_DEMO:  // 2 channels in this profile
				sum_reduce_samples(avg_vals);
				util_message_uint32(chp, "time", &timenow, 1);
				util_message_uint32(chp, "adc-degC-raw", &avg_vals[0], 1);
				degC = fetch_adc_calc_temp(avg_vals[0], uv_per_bit);
				util_message_uint32(chp, "adc-degC", &degC, 1);
				util_message_uint32(chp, "adc-raw", &avg_vals[1], 1);
				adcUV = avg_vals[1] * uv_per_bit;
				util_message_uint32(chp, "adc", &adcUV, 1);
				break;
			case FETCH_ADC1_PA:  // 16 channels in this profile
				util_message_uint32(chp, "time", &timenow, 1);
				sum_reduce_samples(avg_vals);

				// convert values
				for(int i = 0; i < FETCH_ADC1_PA_GRP_NUM_CHANNELS; ++i)
				{
					if(i == 13)
					{
						avg_vals[i] = fetch_adc_calc_temp(avg_vals[i], uv_per_bit); // T (C)
					}
					else
					{
						avg_vals[i] = avg_vals[i] * uv_per_bit;
					}
				}
				util_message_uint32(chp, "adc", avg_vals, FETCH_ADC1_PA_GRP_NUM_CHANNELS);
				break;
			case FETCH_ADC1_PB:
				break;
			default:
				break;
		}
	}
	mshell_putprompt();
}

/*! \brief Thread for new data event
 *
 * chprintf can not be called from within an interrupt ('I')
 *   context in ChibiOS. The interrupt callback generates an
 *   event which this thread receives.
 */
static WORKING_AREA(wa_fetch_adc_data, FETCH_ADC_DATA_STACKSIZE);
NORETURN static msg_t fetch_adc_data(void * p UNUSED)
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
}

/*!
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
			adcStartConversionI(&ADCD1, fetch_adc1_state.profile->adcgrpcfg, fetch_adc1_state.profile->adc_sample_buf,
			                    fetch_adc1_state.profile->adc_grp_buf_depth);
		}
		chSysUnlockFromIsr();
	}
}

static void fetch_adc_io_set_defaults(void)
{
	for(uint8_t i = 0; i < NELEMS(ADC1_inputs); ++i)
	{
		if(ADC1_inputs[i] != NULL)
		{
			io_manage_set_default_mode(ADC1_inputs[i]->port, ADC1_inputs[i]->pad);
		}
	}
}

/*! Set a channel to Analog Mode
 */
static bool fetch_adc1_io_to_analog(ADCChannel channel)
{
	if(ADC1_inputs[channel] != NULL)
	{
		return(io_manage_set_mode(ADC1_inputs[channel]->port, ADC1_inputs[channel]->pad, PAL_MODE_INPUT_ANALOG, IO_ADC));
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
	// ADC1
	chEvtInit(&fetch_adc1_data_ready);

	// State initialization
	fetch_adc1_state.vref_mv                = FETCH_DEFAULT_VREF_MV;
	fetch_adc1_state.init                   = false;
	fetch_adc1_state.printheader            = false;
	fetch_adc1_state.oneshot                = true;
	fetch_adc1_state.profile                = &adc1_default_profile;
	fetch_adc1_state.chp                    = chp;

	// Default input channel to Analog Mode
	fetch_adc1_io_to_analog(adc1_default_channel);

	adcStart(&ADCD1, NULL);

	// enable the thermal sensor.
	adcSTM32EnableTSVREFE();

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
	//>! \warning on stm32f42x and stm32f43x EITHER the Temperature or Battery can be monitored-but not both
	adcSTM32EnableVBATE();
#endif

	// start a thread waiting for data event.....
	chThdCreateStatic(wa_fetch_adc_data, sizeof(wa_fetch_adc_data), NORMALPRIO, fetch_adc_data,
	                  NULL);

	fetch_adc1_state.init = true;
}


/*! \brief Reset adc1
 */
static void fetch_adc1_reset(void)
{
	adcStop(&ADCD1);

	fetch_adc1_state.vref_mv                = FETCH_DEFAULT_VREF_MV;
	fetch_adc1_state.printheader            = false;
	fetch_adc1_state.oneshot                = true;
	fetch_adc1_state.profile                = &adc1_default_profile;

	// Reset inputs
	fetch_adc_io_set_defaults();
	// Default input channel to Analog Mode
	fetch_adc1_io_to_analog(adc1_default_channel);

	adcStart(&ADCD1, NULL);

	// enable the thermal sensor.
	adcSTM32EnableTSVREFE();

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
	//>! \warning on stm32f42x and stm32f43x EITHER the Temperature or Battery can be monitored-but not both
	adcSTM32EnableVBATE();
#endif

}

/*! \brief Change from one profile to another.
 *
 * Reset I/Os to default mode before changing for new profile
 *
 */
static bool fetch_adc_change_profile(FETCH_ADC_profile_name p)
{
	bool          ret    =     false;
	fetch_adc1_reset();
	switch(p)
	{
		case FETCH_ADC1_DEFAULT:
			fetch_adc1_state.profile = &adc1_default_profile;
			fetch_adc_io_set_defaults();
			ret = fetch_adc1_io_to_analog(adc1_default_channel);
			break;
		case FETCH_ADC1_DEMO:
			fetch_adc1_state.profile = &adc1_demo_profile;
			fetch_adc_io_set_defaults();
			ret = fetch_adc1_io_to_analog(ADC_CH13);
			break;
		case FETCH_ADC1_PA:
			fetch_adc1_state.profile = &adc1_pa_profile;
			fetch_adc_io_set_defaults();
			if(!fetch_adc1_io_to_analog(ADC_CH0) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH1) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH2) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH4) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH5) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH6) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH7) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH8) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH9) )
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH10))
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH13))
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH14))
			{
				return false;
			}
			if(!fetch_adc1_io_to_analog(ADC_CH15))
			{
				return false;
			}
			ret = true;
			break;
		default:
			ret = false;
			break;
	}
	return ret;
}

static inline int fetch_adc_is_valid_adc_profile(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkadc_profile)
{
	return(token_match(chp, fetch_terms->adc_profile, chkadc_profile,
	                   ((int) NELEMS(fetch_terms->adc_profile)) ) );
}

/*! \brief change the profile for adc1
 */
bool fetch_adc1_profile(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                        char * cmd_list[])
{
	if(fetch_adc_is_valid_adc_profile(chp, fetch_terms, cmd_list[ADC_PROFILE]) >= 0)
	{
		if (strncasecmp(cmd_list[ADC_PROFILE], "default", strlen("default") ) == 0)
		{
			return(fetch_adc_change_profile(FETCH_ADC1_DEFAULT));
		}
		else if (strncasecmp(cmd_list[ADC_PROFILE], "demo", strlen("demo") ) == 0)
		{
			return(fetch_adc_change_profile(FETCH_ADC1_DEMO));
		}
		else if (strncasecmp(cmd_list[ADC_PROFILE], "pa", strlen("pa") ) == 0)
		{
			return(fetch_adc_change_profile(FETCH_ADC1_PA));
		}
		else
		{
			util_message_error(chp, "Profile not available: %s.", cmd_list[ADC_PROFILE]);
			return false;
		}
	}
	else
	{
		util_message_error(chp, "Not a valid profile");
	}
	return false;
}

/*! \brief Start a conversion
 */
static bool fetch_adc1_start(void)
{
	adcStartConversion(&ADCD1, fetch_adc1_state.profile->adcgrpcfg,
	                   fetch_adc1_state.profile->adc_sample_buf, fetch_adc1_state.profile->adc_grp_buf_depth);
	fetch_adc1_state.printheader = true;
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

	if(!fetch_adc1_state.init)
	{
		fetch_adc_init(chp);
	};

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


