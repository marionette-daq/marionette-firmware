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
#define FETCH_ADC1_MAX_CHANNELS       19

/*! Total number of channels to be sampled by a single ADC operation.*/
#define FETCH_ADC1_DEMO_GRP_NUM_CHANNELS      2
#define FETCH_ADC1_DEFAULT_GRP_NUM_CHANNELS   1
#define FETCH_ADC1_PA_GRP_NUM_CHANNELS        16

/*! Depth of the conversion buffer, channels are sampled four times each.*/
#define FETCH_ADC1_DEMO_GRP_BUF_DEPTH         4
#define FETCH_ADC1_DEFAULT_GRP_BUF_DEPTH      10
#define FETCH_ADC1_PA_GRP_BUF_DEPTH           1

//#ifdef BOARD_ST_STM32F4_DISCOVERY

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
static ADC_input ADC1_IN0   = { GPIOA, GPIOA_PIN0, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN1   = { GPIOA, GPIOA_PIN1, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN2   = { GPIOA, GPIOA_PIN2, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
//static ADC_input ADC1_IN3   = { GPIOA, GPIOA_PIN3,  PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT |PAL_STM32_PUDR_FLOATING }  // used for VBUS
static ADC_input ADC1_IN4   = { GPIOA, GPIOA_PIN4, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN5   = { GPIOA, GPIOA_PIN5, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN6   = { GPIOA, GPIOA_PIN6, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN7   = { GPIOA, GPIOA_PIN7, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN8   = { GPIOB, GPIOB_PIN0, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN9   = { GPIOB, GPIOB_PIN9, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN10  = { GPIOC, GPIOC_PIN0, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
//static ADC_input ADC1_IN11  = { GPIOC, GPIOC_PIN1,  PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT |PAL_STM32_PUDR_FLOATING } ;  // used for OTG
//static ADC_input ADC1_IN12  = { GPIOC, GPIOC_PIN2,  PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT |PAL_STM32_PUDR_FLOATING } ;  // used for OTG
static ADC_input ADC1_IN13  = { GPIOC, GPIOC_PIN3, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN14  = { GPIOC, GPIOC_PIN4, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;
static ADC_input ADC1_IN15  = { GPIOC, GPIOC_PIN5, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING } ;

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




/*! \brief ADC1 Demo profile
 * oneshot
 * adc1_default_cfg
 * adc1_sample_buf
 */
static const  FETCH_adc_profile adc1_demo_profile =
{
	.name                 = FETCH_ADC1_DEMO,
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
	.printheader          = false,
	.profile              = &adc1_default_profile,
	.chp                  = NULL
};

static uint16_t fetch_adc_calc_temp(uint16_t t_raw, uint32_t uv_per_bit )
{

	uint32_t t_voltage = (t_raw * uv_per_bit) / 1000000; // \todo units are confusing in data sheet....review?

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
		systime_t          timenow = chTimeNow() ;
		uint32_t    uv_per_bit = ((fetch_adc1_state.vref_mv * 1000) /
		                          4096);   //>! \todo maybe not all conversions are 12 bit..?;
		uint32_t    avg_vals[FETCH_ADC1_MAX_CHANNELS] = {0};

		switch(fetch_adc1_state.profile->name)
		{
			case FETCH_ADC1_DEFAULT: // 1 channel in this profile
				if(fetch_adc1_state.printheader)
				{
					chprintf(fetch_adc1_state.chp, "t(ms),ch0-mv\r\n");
					fetch_adc1_state.printheader = false;
				}
				sum_reduce_samples(avg_vals);
				chprintf(fetch_adc1_state.chp, "%u,%u\r\n", timenow, avg_vals[0] * uv_per_bit);
				break;
			case FETCH_ADC1_DEMO:  // 2 channels in this profile
				sum_reduce_samples(avg_vals);
				chprintf(fetch_adc1_state.chp, "DEMO:\t%u,T(raw): %u\tT(C): %u\tIN13(raw): %u\tIN13(uV):%u\r\n", timenow, avg_vals[0],
				         fetch_adc_calc_temp(avg_vals[0], uv_per_bit), avg_vals[1], avg_vals[1] * uv_per_bit);
				break;
			case FETCH_ADC1_PA:  // 16 channels in this profile
				if(fetch_adc1_state.printheader)
				{
					chprintf(fetch_adc1_state.chp,
					         "#t(ms),ch0-mV,ch1-mV,ch2-mV,ch4-mV,ch5-mV,ch6-mV,ch7-mV,ch8-mV,ch9-mV,ch10-mV,ch13-mV,ch14-mV,ch15-mV,T(C),VREFINT,VBAT\r\n");
					fetch_adc1_state.printheader = false;
				}
				sum_reduce_samples(avg_vals);
				chprintf(fetch_adc1_state.chp, "%u: %u,%u,%u,%u, %u,%u,%u,%u, %u,%u,%u,%u, %u,T:%u,%u,z%u\r\n",
				         timenow,
				         avg_vals[0] * uv_per_bit,  // in0
				         avg_vals[1] * uv_per_bit,  // in1
				         avg_vals[2] * uv_per_bit,  // in2
				         avg_vals[3] * uv_per_bit,  // in4
				         avg_vals[4] * uv_per_bit,  // in5
				         avg_vals[5] * uv_per_bit,  // in6
				         avg_vals[6] * uv_per_bit,  // in7
				         avg_vals[7] * uv_per_bit,  // in8
				         avg_vals[8] * uv_per_bit,  // in9
				         avg_vals[9] * uv_per_bit,  // in10
				         avg_vals[10] * uv_per_bit, // in 13
				         avg_vals[11] * uv_per_bit,  // in 14
				         avg_vals[12] * uv_per_bit,  // in 15
				         fetch_adc_calc_temp(avg_vals[13], uv_per_bit),  // T (C)
				         avg_vals[14] * uv_per_bit,  // VREFINT
				         avg_vals[15] * uv_per_bit);  // VBAT
				break;
			case FETCH_ADC1_PB:
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
			adcStartConversionI(&ADCD1, fetch_adc1_state.profile->adcgrpcfg, fetch_adc1_state.profile->adc_sample_buf,
			                    fetch_adc1_state.profile->adc_grp_buf_depth);
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

static void fetch_adc_io_set_defaults(void)
{
	for(uint8_t i = 0; i < NELEMS(ADC1_inputs); ++i)
	{
		if(ADC1_inputs[i] != NULL)
		{
			palSetPadMode(ADC1_inputs[i]->port, ADC1_inputs[i]->pin, ADC1_inputs[i]->default_mode);
			ADC1_inputs[i]->current_mode = ADC1_inputs[i]->default_mode;
		}
	}
}

static void fetch_adc1_io_to_analog(ADCChannel channel)
{
	if(ADC1_inputs[channel] != NULL)
	{
		palSetPadMode(ADC1_inputs[channel]->port, ADC1_inputs[channel]->pin, PAL_MODE_INPUT_ANALOG );
		ADC1_inputs[channel]->current_mode = PAL_MODE_INPUT_ANALOG;
	}
}

static void fetch_adc_change_profile(FETCH_ADC_profile_name p)
{
	fetch_adc1_reset();
	switch(p)
	{
		case FETCH_ADC1_DEFAULT:
			fetch_adc1_state.profile = &adc1_default_profile;
			fetch_adc_io_set_defaults();
			fetch_adc1_io_to_analog(ADC_CH13);
			break;
		case FETCH_ADC1_DEMO:
			fetch_adc1_state.profile = &adc1_demo_profile;
			fetch_adc_io_set_defaults();
			fetch_adc1_io_to_analog(ADC_CH13);
			break;
		case FETCH_ADC1_PA:
			fetch_adc1_state.profile = &adc1_pa_profile;
			fetch_adc_io_set_defaults();
			fetch_adc1_io_to_analog(ADC_CH0);
			fetch_adc1_io_to_analog(ADC_CH1);
			fetch_adc1_io_to_analog(ADC_CH2);
			fetch_adc1_io_to_analog(ADC_CH4);
			fetch_adc1_io_to_analog(ADC_CH5);
			fetch_adc1_io_to_analog(ADC_CH6);
			fetch_adc1_io_to_analog(ADC_CH7);
			fetch_adc1_io_to_analog(ADC_CH8);
			fetch_adc1_io_to_analog(ADC_CH9);
			fetch_adc1_io_to_analog(ADC_CH10);
			fetch_adc1_io_to_analog(ADC_CH13);
			fetch_adc1_io_to_analog(ADC_CH14);
			fetch_adc1_io_to_analog(ADC_CH15);
			break;
		default:
			break;
	}
}

/*! \brief change the profile for adc1
 */
bool fetch_adc1_profile(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                        char * cmd_list[])
{
	if (strncasecmp(cmd_list[ADC_PROFILE], "default", strlen("default") ) == 0)
	{
		fetch_adc_change_profile(FETCH_ADC1_DEFAULT);
		return true;
	}
	else if (strncasecmp(cmd_list[ADC_PROFILE], "demo", strlen("demo") ) == 0)
	{
		fetch_adc_change_profile(FETCH_ADC1_DEMO);
		return true;
	}
	else if (strncasecmp(cmd_list[ADC_PROFILE], "pa", strlen("pa") ) == 0)
	{
		fetch_adc_change_profile(FETCH_ADC1_PA);
		return true;
	}
	else
	{
		util_errormsg(chp, "Profile %s not available.", cmd_list[ADC_PROFILE]);
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


