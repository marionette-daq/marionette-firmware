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

bool fetch_adc_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_single_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_stream_start_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_stream_stop_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_status_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_timer_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_adc_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

void fetch_adc_free_sample_set( adc_sample_set_t *ssp );

bool fetch_adc_reset(BaseSequentialStream * chp);

void fetch_adc_init(void);

#ifdef __cplusplus
}
#endif

#endif

/*! @} */
