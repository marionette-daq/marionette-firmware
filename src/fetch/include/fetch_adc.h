/*! \file fetch_adc.h
 *
 * @addtogroup fetch_adc
 * @{
 */

#ifndef FETCH_ADC_H_
#define FETCH_ADC_H_

#include <stdbool.h>

#define 		  ADC_V_25 			(0.76)   //!< From stm32f4 datasheet. Voltage at 25C
#define 		  ADC_VSLOPE 			(2.5)    //!< units: mv/C

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

#ifdef __cplusplus
}
#endif


#endif

/*! @} */
