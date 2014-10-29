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

bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_adc_reset(BaseSequentialStream * chp);

void fetch_adc_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif

/*! @} */
