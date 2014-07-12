/*! \file gpio.h

  */

#ifndef MGPIO_H_
#define MGPIO_H_

#include "chprintf.h"
#include "fetch_defs.h"

#define          MAX_PIN_STR_LEN       8
#define          MAX_PORT_STR_LEN      8

typedef enum GPIO_token
{
	CMD = 0,
	ACTION,
	PORT,
	PIN,
	DIRECTION,
	SENSE,
	NO_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} GPIO_token;

typedef enum GPIO_pinval {
	LOW  =  0,
	HIGH
} GPIO_pinval;

typedef enum GPIO_pinnum
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
	NO_GPIO_PIN=99
} GPIO_pinnum;

typedef struct StrToPinnum_Map
{
	char    *    pinstring;
	GPIO_pinval  pinnum;
} StrToPinnum_Map;

typedef struct StrToGPIOPort_Map
{
	char      *     portstring;
	GPIO_TypeDef  * portptr;
} StrToGPIOPort_Map;

int gpio_is_valid_port_subcommand(BaseSequentialStream * chp, Fetch_terminals * fetch_terms, char * chkport_subcommand);
int gpio_is_valid_pin_subcommand(BaseSequentialStream * chp, Fetch_terminals * fetch_terms, char * chkpin_subcommand);

bool gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[], Fetch_terminals * fetch_terms);

#endif
