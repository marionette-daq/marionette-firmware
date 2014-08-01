/*! \file fetch_adc.h
 *
 * @addtogroup fetch_adc
 * @{
 */

#ifndef FETCH_ADC_H_
#define FETCH_ADC_H_

#include <stdbool.h>

#define 		ADC_V_25 			(0.76)   //!< From stm32f4 datasheet. Voltage at 25C
#define 		ADC_VSLOPE 			(2.5)    //!< units: mv/C

#ifdef __cplusplus
extern "C" {
#endif

typedef enum fetch_adc_profile_name
{
	FETCH_ADC_DEFAULT = 0,
	FETCH_ADC_DEMO,
	FETCH_ADC_PA, 
	FETCH_ADC_PB  
} FETCH_ADC_profile_name;

typedef struct fetch_adc_profile
{
    FETCH_ADC_profile_name      name;
	bool                        oneshot;
	ADCConversionGroup *        adcgrpcfg;
	uint8_t                     adc_grp_num_channels;
	uint8_t                     adc_grp_buf_depth;
	adcsample_t     *           adc_sample_buf;
} FETCH_adc_profile;

typedef enum FETCH_ADC_token
{
	ADC_CMD = 0,
	ADC_ACTION,
	ADC_CONFIGURE,
	ADC_PROFILE,
	NO_ADC_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_ADC_token;

typedef struct adc_input {
    ioportid_t port;
    uint16_t   pin;
} ADC_input;

typedef struct fetch_adc_state
{
	int32_t                     vref_mv;	
	bool                        init;
	bool                        oneshot;
	FETCH_adc_profile  const     *   profile;
	BaseSequentialStream    *   chp;
} FETCH_adc_state;

void fetch_adc_init(BaseSequentialStream* chp);
bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                         Fetch_terminals * fetch_terms);
#ifdef __cplusplus
}
#endif


#endif

/*! @} */
