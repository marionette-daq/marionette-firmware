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

/*! \brief semantics to position of token in command string
 */
typedef enum FETCH_GPIO_token
{
	FETCH_GPIO_CMD = 0,
	FETCH_GPIO_ACTION,
	FETCH_GPIO_PORT,
	FETCH_GPIO_PIN,
	FETCH_GPIO_DIRECTION,
	FETCH_GPIO_SENSE
	//NO_FETCH_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_GPIO_token;

typedef enum FETCH_GPIO_pinval {
	GPIO_LOW  =  0,
	GPIO_HIGH
} FETCH_GPIO_pinval;

bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
