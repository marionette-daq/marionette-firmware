/*! \file gpio.h

  */

#ifndef MGPIO_H_
#define MGPIO_H_

#include "chprintf.h"

#define          MAX_PIN_STR_LEN       8
void gpio_set(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_clear(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_config(BaseSequentialStream * chp, char * commandl[]) ;

#endif
