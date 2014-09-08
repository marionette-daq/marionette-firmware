/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_GPIO_H_
#define FETCH_FETCH_GPIO_H_

#include "chprintf.h"
#include "fetch_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
