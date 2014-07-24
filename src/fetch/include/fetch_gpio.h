/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 
  */

#ifndef FETCH_FETCH_GPIO_H_
#define FETCH_FETCH_GPIO_H_

#include "chprintf.h"
#include "fetch_defs.h"

#define          MAX_PIN_STR_LEN       8
#define          MAX_PORT_STR_LEN      8

typedef enum FETCH_GPIO_token
{
	CMD = 0,
	ACTION,
	PORT,
	PIN,
	DIRECTION,
	SENSE,
	NO_FETCH_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_GPIO_token;

typedef enum FETCH_GPIO_pinval {
	LOW  =  0,
	HIGH
} FETCH_GPIO_pinval;

typedef enum FETCH_GPIO_pinnum
{
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	NO_FETCH_GPIO_PIN=99
} FETCH_GPIO_pinnum;

typedef struct strtopinnum_map
{
	char    *    pinstring;
	FETCH_GPIO_pinval  pinnum;
} StrToPinnum_Map;

typedef struct strtogpioport_map
{
	char      *     portstring;
	GPIO_TypeDef  * portptr;
} StrToGPIOPort_Map;

int fetch_gpio_is_valid_port_subcommand(BaseSequentialStream * chp, Fetch_terminals * fetch_terms, char * chkport_subcommand);
int fetch_gpio_is_valid_pin_subcommand(BaseSequentialStream * chp, Fetch_terminals * fetch_terms, char * chkpin_subcommand);

bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[], Fetch_terminals * fetch_terms);

#endif
/*! @} */
