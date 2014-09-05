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

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_messages.h"

#include "mshell_state.h"
#include "io_manage.h"
#include "io_manage_defs.h"

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

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static io_table_t * io_manage_tables[] = { &io_porta, &io_portb, &io_portc, &io_portd, &io_porte, &io_portf, &io_portg, &io_porth, &io_porti };
#endif

io_namestr_t   io_manage_namestr[] = {{IO_NONE, "None"}, {IO_GPIO, "GPIO"}, {IO_ADC, "ADC"}, {IO_DAC, "DAC"}, {IO_SPI, "SPI"}, {IO_I2C, "I2C"}, {IO_USB, "USB"}};

static const char * io_manage_get_namestr(io_alloc_t alloc)
{
	for(uint8_t i = 0; i < NELEMS(io_manage_namestr); ++i)
	{
		if(io_manage_namestr[i].alloc == alloc)
		{
			return io_manage_namestr[i].name;
		}
	}
	return "NA";
}

/*! \brief return pointer to the io port allocation table
 */
static io_table_t * io_manage_get_table(ioportid_t port)
{
	for(uint8_t i = 0; i < NELEMS(io_manage_tables); ++i)
	{
		if(io_manage_tables[i]->port == port)
		{
			return io_manage_tables[i];
		}
	}
	return NULL;
}

/*! \brief check availability of requested function against table
 */
static bool io_manage_fn_avail(ioportid_t port, uint32_t pin, io_alloc_t request_alloc, io_table_t * table)
{
	io_alloc_t         curr_alloc;

	if(table != NULL)
	{
		curr_alloc = table->pin[pin].current_alloc;
		if(curr_alloc == request_alloc)
		{
			return true;
		}
		if((request_alloc & table->pin[pin].fn_available) != 0)
		{
			return true;
		};
	}
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


/*! \brief Update the port allocation table
 */
bool io_manage_set_default_mode(ioportid_t port, uint32_t pin)
{
	io_table_t    *    table = io_manage_get_table(port);

	table->pin[pin].current_mode  = table->pin[pin].default_mode;
	table->pin[pin].current_alloc = table->pin[pin].default_alloc;
	palSetPadMode(port, pin, table->pin[pin].default_mode);
	return true;
}

/*! \brief Update the port allocation table
 */
bool io_manage_set_mode(ioportid_t port, uint32_t pin, iomode_t new_mode, io_alloc_t request_alloc)
{
	io_alloc_t         curr_alloc;
	io_table_t    *    table = io_manage_get_table(port);

	curr_alloc = table->pin[pin].current_alloc;
	if(curr_alloc == request_alloc)
	{
		return true;
	}

	if(curr_alloc != IO_NONE)
	{
		util_message_error(getMShellStreamPtr(), "Function already allocated to: %s", io_manage_get_namestr(curr_alloc) );
		return false;
	}

	if(io_manage_fn_avail(port, pin, request_alloc, table))
	{

		table->pin[pin].current_mode  = new_mode;
		table->pin[pin].current_alloc = request_alloc;
		palSetPadMode(port, pin, new_mode);
		return true;
	}
	util_message_error(getMShellStreamPtr(), "Function not available. %s", io_manage_get_namestr(request_alloc) );
	return false;
}


/*! \brief Reset port allocation table to defaults
 */
void io_manage_table_to_defaults(void)
{
	io_table_t    *    table;
	for(uint8_t i = 0; i < NELEMS(io_manage_tables); ++i)
	{
		table = io_manage_tables[i];
		for(uint8_t j = 0; j < NELEMS(table->pin); ++j)
		{
			table->pin[j].current_mode    = table->pin[j].default_mode;
			table->pin[j].current_alloc   = table->pin[j].default_alloc;
			//palSetPadMode(table->port, table->pin[j].pin, table->pin[j].current_mode);
		}
	}
}

/*! \brief Query whether a function is available on that port
 */
bool io_manage_query_fn_avail(ioportid_t port, uint32_t pin, io_alloc_t request_alloc)
{
	io_table_t    *    table = io_manage_get_table(port);

	if(io_manage_fn_avail(port, pin, request_alloc, table))
	{
		return true;
	}
	return false;
}

/*! \brief Query the current pin allocation
 */
io_alloc_t io_manage_query_current_fn(ioportid_t port, uint32_t pin)
{
  io_table_t * table = io_manage_get_table(port);

	return table->pin[pin].current_alloc;
}

/*! \brief Query the current pin allocation name string
 */
const char * io_manage_query_pin_current_fn_name(ioportid_t port, uint32_t pin)
{
  io_table_t * table = io_manage_get_table(port);
  return io_manage_get_namestr(table->pin[pin].current_alloc);
}

//! @}


