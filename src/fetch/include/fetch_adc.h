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
	FETCH_ADC1_DEFAULT = 0,
	FETCH_ADC1_DEMO,
	FETCH_ADC1_PA, 
	FETCH_ADC1_PB  
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
	//NO_ADC_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_ADC_token;

typedef struct adc_input {
    const ioportid_t port;
    const uint32_t   pad;
} ADC_input;

typedef struct fetch_adc_state
{
	int32_t                     vref_mv;	
	bool                        init;
	bool                        oneshot;
	bool                        printheader;
	FETCH_adc_profile  const     *   profile;
	BaseSequentialStream    *   chp;
} FETCH_adc_state;

void fetch_adc_init(BaseSequentialStream* chp);
bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
#ifdef __cplusplus
}
#endif


#endif

/*! @} */
