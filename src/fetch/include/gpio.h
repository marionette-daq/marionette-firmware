/*! \file gpio.h

  */

#ifndef MGPIO_H_
#define MGPIO_H_

#include "chprintf.h"

#define          MAX_PIN_STR_LEN       8

typedef enum GPIO_tokens
{
	CMD = 0,
	ACTION,
	PORT,
	PIN,
	DIRECTION,
	SENSE
} GPIO_tokens;

typedef enum GPIO_pinval {
	LOW  =  0,
	HIGH,
	FAIL
} GPIO_pinval;

typedef enum GPIO_pinnums
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
	PIN15
} GPIO_pinnums;

GPIO_pinval gpio_get(BaseSequentialStream * chp, char * commandl[]) ;

void gpio_set(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_clear(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_config(BaseSequentialStream * chp, char * commandl[]) ;


#endif
