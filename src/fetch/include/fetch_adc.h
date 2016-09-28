/*! \file fetch_adc.h
 *
 * @addtogroup fetch_adc
 * @{
 */

#ifndef FETCH_ADC_H_
#define FETCH_ADC_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_SAMPLE_SET_SIZE 7

typedef struct {
  adcsample_t sample[ADC_SAMPLE_SET_SIZE];
  uint16_t sequence_number;
  volatile int16_t mem_ref_count;
} adc_sample_set_t;

void fetch_adc_free_sample_set( adc_sample_set_t *ssp );

bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_adc_reset(BaseSequentialStream * chp);

void fetch_adc_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
