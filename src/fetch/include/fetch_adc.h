/*! \file fetch_adc.h
 *
 * @addtogroup fetch_adc
 * @{
 */

#ifndef FETCH_ADC_H_
#define FETCH_ADC_H_

#include <stdbool.h>

typedef enum FETCH_ADC_token
{
	ADC_CMD = 0,
	ADC_ACTION,
	ADC_CONFIGURE,
	NO_ADC_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_ADC_token;


typedef struct adc_input {
    ioportid_t port;
    uint16_t   pin;
} ADC_input;

typedef struct fetch_adc_profile
{
	bool                 oneshot;
	ADCConversionGroup * adcgrpcfg;
	uint8_t              adc_grp_num_channels;
	uint8_t              adc_grp_buf_depth;
	adcsample_t     *    adc_sample_buf;
} FETCH_adc_profile;


typedef struct fetch_adc_state {
	bool                   init;
	BaseSequentialStream * chp;
	bool                   oneshot;
} FETCH_adc_state;


void fetch_adc_init(BaseSequentialStream* chp);
bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                         Fetch_terminals * fetch_terms);

#endif

/*! @} */
