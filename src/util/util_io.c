/*! \file io_manage.c
  *
  * Keep an accounting of i/o pin configurations
  *
  * @defgroup io_manage IO Management
  * @{
   */

/*!
 * <hr>
 *
 * Track current state of IO pins
 *
 * Marionette should not be able to set a pin for a function that isn't available.
 * For example: DAC is only available on 2 output pins.
 * <hr>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "util_general.h"
#include "util_io.h"

const str_pin_map_t fetch_pin_map[] =
{
	{"pin0", PIN0 },
	{"pin1", PIN1 },
	{"pin2", PIN2 },
	{"pin3", PIN3 },
	{"pin4", PIN4 },
	{"pin5", PIN5 },
	{"pin6", PIN6 },
	{"pin7", PIN7 },
	{"pin8", PIN8 },
	{"pin9", PIN9 },
	{"pin10", PIN10 },
	{"pin11", PIN11 },
	{"pin12", PIN12 },
	{"pin13", PIN13 },
	{"pin14", PIN14 },
	{"pin15", PIN15 },

	{"0", PIN0 },
	{"1", PIN1 },
	{"2", PIN2 },
	{"3", PIN3 },
	{"4", PIN4 },
	{"5", PIN5 },
	{"6", PIN6 },
	{"7", PIN7 },
	{"8", PIN8 },
	{"9", PIN9 },
	{"10", PIN10 },
	{"11", PIN11 },
	{"12", PIN12 },
	{"13", PIN13 },
	{"14", PIN14 },
	{"15", PIN15 }
};

const str_port_map_t fetch_port_map[] =
{
	{"porta", GPIOA},
	{"portb", GPIOB},
	{"portc", GPIOC},
	{"portd", GPIOD},
	{"porte", GPIOE},
	{"portf", GPIOF},
	{"portg", GPIOG},
	{"porth", GPIOH},
	{"porti", GPIOI},
	
  {"a", GPIOA},
	{"b", GPIOB},
	{"c", GPIOC},
	{"d", GPIOD},
	{"e", GPIOE},
	{"f", GPIOF},
	{"g", GPIOG},
	{"h", GPIOH},
	{"i", GPIOI}
};

const alt_pin_mode_t fetch_alt_map[] =
{
  // TODO
};


bool set_alt_pin_mode( ioportid_t port, uint32_t pin )
{
  // TODO
  return false;
}

/*! \brief convert string to port pointer
 */
ioportid_t string_to_port( char * str )
{
  if( str == NULL )
  {
    return NULL;
  }
  
  for( unsigned int i = 0; i < NELEMS(fetch_port_map); i++ )
  {
    if( strncasecmp(str, fetch_port_map[i].string, MAX_PIN_STRLEN) == 0 )
    {
      return fetch_port_map[i].port;
    }
  }

  return NULL;
}

/*! \brief convert string to pin number
 */
iopin_t string_to_pin( char * str )
{
  if( str == NULL )
  {
    return INVALID_PIN;
  }

  for( unsigned int i = 0; i < NELEMS(fetch_pin_map); i++ )
  {
    if( strncasecmp(str, fetch_pin_map[i].string, MAX_PIN_STRLEN) == 0 )
    {
      return fetch_pin_map[i].pin;
    }
  }

  return INVALID_PIN;
}

/*! \brief convert port to string name
 */
const char * port_to_string( ioportid_t port )
{
  for( unsigned int i = 0; i < NELEMS(fetch_port_map); i++ )
  {
    if( fetch_port_map[i].port == port )
    {
      return fetch_port_map[i].string;
    }
  }
  return NULL;
}

void set_status_led(bool r, bool g, bool b)
{
  if(r) {
    palClearPad(GPIOD, GPIOD_PD13_LED_STATUS_R);
  } else {
    palSetPad(GPIOD, GPIOD_PD13_LED_STATUS_R);
  }
  if(g) {
    palClearPad(GPIOD, GPIOD_PD14_LED_STATUS_G);
  } else {
    palSetPad(GPIOD, GPIOD_PD14_LED_STATUS_G);
  }
  if(b) {
    palClearPad(GPIOD, GPIOD_PD15_LED_STATUS_B);
  } else {
    palSetPad(GPIOD, GPIOD_PD15_LED_STATUS_B);
  }
}

//! @}


