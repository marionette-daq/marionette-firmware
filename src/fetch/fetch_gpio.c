/*! \file fetch_gpio.c
 * Marionette fetch_gpio routines
 * @defgroup fetch_gpio Fetch GPIO
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>

#include "chprintf.h"
#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"

#include "io_manage.h"

#include "fetch_defs.h"
#include "fetch_gpio.h"

// list all command function prototypes here 
static bool fetch_gpio_get(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_set(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_clear(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_query(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_config(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_gpio_commands[] = {
    { fetch_gpio_get,     "get",    NULL },
    { fetch_gpio_set,     "set",    NULL },
    { fetch_gpio_clear,   "clear",  NULL },
    { fetch_gpio_query,   "query",  NULL },
    { fetch_gpio_config,  "config", NULL },
    { NULL, NULL, NULL } // null terminate list
  };

/*! \brief get the port and pin information
 */
static bool fetch_gpio_get_port_pin(BaseSequentialStream * chp, char * cmd_list[],
                                    GPIO_TypeDef ** port, iopin_t * pin)
{
	GPIO_TypeDef    *    fetch_gpio_port;
	iopin_t    fetch_gpio_pinnum;

  fetch_gpio_port = string_to_port(cmd_list[FETCH_GPIO_PORT]);
  if(fetch_gpio_port != NULL )
  {
    *port = fetch_gpio_port;
  }
  else
  {
    *port = NULL;
    return false;
  }

  fetch_gpio_pinnum = string_to_pin(cmd_list[FETCH_GPIO_PIN]);
  if(fetch_gpio_pinnum != INVALID_PIN)
  {
    *pin = fetch_gpio_pinnum;
  }
  else
  {
    *pin = INVALID_PIN;
    return false;
  }

	return true;
}

/*! \brief read the input register value for the port and pin
 */
static bool fetch_gpio_get(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	GPIO_TypeDef    *    port    = GPIOA;
	iopin_t    pin     = PIN0;
	uint8_t pin_state;

	if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
	{
		if((port != NULL) && (pin != INVALID_PIN))
		{
			pin_state = palReadPad(port, pin);
			util_message_uint8(chp, "logic", &pin_state, 1);
			return true;
		}
	}
	return false;
}


/*! \brief set the output register value to HIGH for the port and pin
 */
static bool fetch_gpio_set(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	GPIO_TypeDef   *  port       = NULL;
	iopin_t pin        = PIN0;

	if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
	{
		if((port != NULL) && (pin != INVALID_PIN))
		{
			palSetPad(port, pin);
			return true;
		}
	}
	return false;
}

/*! \brief clear the output register value (set to LOW) for the port and pin
 */
static bool fetch_gpio_clear(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	GPIO_TypeDef    *    port    = NULL;
	iopin_t    pin     = PIN0;

	if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
	{
		if((port != NULL) && (pin != INVALID_PIN))
		{
			palClearPad(port, pin);
			return true;
		}
	}
	return false;
}

static bool fetch_gpio_query(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{

	GPIO_TypeDef    *    port    = NULL;
	iopin_t    pin     = PIN0;
  char * fn_name = NULL;

	if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
	{
		if((port != NULL) && (pin != INVALID_PIN))
		{
			fn_name = (char*)io_manage_query_pin_current_fn_name(port, pin);
      util_message_string(chp,"allocation",fn_name);
			return true;
		}
	}
	return false;
}

/*! \brief configure a pin
 */
static bool fetch_gpio_config(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	GPIO_TypeDef    *    port       = NULL;
	iopin_t    pin        = PIN0;

	int            sense      = PAL_STM32_PUDR_FLOATING;
	int            direction  = PAL_STM32_MODE_INPUT;

  if( (strncasecmp(cmd_list[FETCH_GPIO_DIRECTION], "input", strlen("input") ) == 0) )
  {
    direction = PAL_STM32_MODE_INPUT;
  }
  else if ((strncasecmp(cmd_list[FETCH_GPIO_DIRECTION], "output", strlen("output") ) == 0) )
  {
    direction = PAL_STM32_MODE_OUTPUT;
  }
  else
  {
    return false;
  }

	if(direction == PAL_STM32_MODE_INPUT)
	{
    if( (strncasecmp(cmd_list[FETCH_GPIO_SENSE], "pullup", strlen("pullup") ) == 0) )
    {
      sense = PAL_STM32_PUDR_PULLUP;
    }
    else if ( (strncasecmp(cmd_list[FETCH_GPIO_SENSE], "pulldown", strlen("pulldown") ) == 0) )
    {
      sense = PAL_STM32_PUDR_PULLDOWN;
    }
    else if ( (strncasecmp(cmd_list[FETCH_GPIO_SENSE], "floating", strlen("floating") ) == 0) )
    {
      sense = PAL_STM32_PUDR_FLOATING;
    }
    else if ( (strncasecmp(cmd_list[FETCH_GPIO_SENSE], "analog", strlen("analog") ) == 0) )
    {
      //! Ref: F4 Reference section 8.3.12 Analog configuration
      if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
      {
        if(io_manage_query_fn_avail(port, pin, IO_ADC))    // ADC pins are available Analog pins
        {
          sense = PAL_STM32_MODE_ANALOG;
        }
        else
        {
          util_message_error(chp, "GPIO analog mode not available on this pin.");
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
	}


	if(fetch_gpio_get_port_pin(chp, cmd_list, &port, &pin))
	{
		if((port != NULL) && (pin != INVALID_PIN))
		{
			if(direction == PAL_STM32_MODE_INPUT)
			{
				return(io_manage_set_mode(port, pin, direction | sense, IO_GPIO));
			}
			else
			{
				return(io_manage_set_mode(port, pin, direction, IO_GPIO));
			}
		}
	}
	return false;
}

/*! \brief dispatch a specific gpio command
 */
bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "get", strlen("get") ) == 0)
  {
    return(fetch_gpio_get(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "set", strlen("set") ) == 0)
  {
    return(fetch_gpio_set(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "clear", strlen("clear") ) == 0)
  {
    return(fetch_gpio_clear(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "query", strlen("query") ) == 0)
  {
    return(fetch_gpio_query(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "config", strlen("config") ) == 0)
  {
    if( (cmd_list[FETCH_GPIO_DIRECTION] != NULL) && (cmd_list[FETCH_GPIO_SENSE] != NULL))
    {
      return(fetch_gpio_config(chp, cmd_list, data_list));
    }
    else
    {
      return false;
    }
  }
  else
  {
    util_message_error(chp, "GPIO command not available.");
    return(false);
  }
  return true;
}

/*! @} */

