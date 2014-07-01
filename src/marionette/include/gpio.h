/*! \file gpio.h

  */

#ifndef GPIO_H_
#define GPIO_H_

#include "chprintf.h"

void gpio_set(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_clear(BaseSequentialStream * chp, char * commandl[]) ;
void gpio_config(BaseSequentialStream * chp, char * commandl[]) ;

#endif
