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

#include "fetch_defs.h"
#include "fetch_gpio.h"

const StrToPinnum_Map fetch_gpio_pinmap[] =
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
	{"pin15", PIN15 }
};

const StrToGPIOPort_Map fetch_gpio_portmap[] =
{
	{"porta", GPIOA},
	{"portb", GPIOB},
	{"portc", GPIOC},
	{"portd", GPIOD},
	{"porte", GPIOE},
	{"portf", GPIOF},
	{"portg", GPIOG},
	{"porth", GPIOH},
	{"porti", GPIOI}
};

static inline int fetch_gpio_is_valid_gpio_direction(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkgpio_direction)
{
	return(token_match(chp, fetch_terms->gpio_direction, chkgpio_direction,
	                   ((int) NELEMS(fetch_terms->gpio_direction))) );
}

static inline int fetch_gpio_is_valid_gpio_sense(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkgpio_sense)
{
	return(token_match(chp, fetch_terms->gpio_sense, chkgpio_sense,
	                   ((int) NELEMS(fetch_terms->gpio_sense))) );
}

static inline int fetch_gpio_is_valid_gpio_subcommandA(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkgpio_subcommandA)
{

	return(token_match(chp, fetch_terms->gpio_subcommandA, chkgpio_subcommandA,
	                   ((int) NELEMS(fetch_terms->gpio_subcommandA)) ) );
}

inline int fetch_gpio_is_valid_port_subcommand(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkport_subcommand)
{
	return(token_match(chp, fetch_terms->port_subcommand, chkport_subcommand,
	                   ((int) NELEMS(fetch_terms->port_subcommand))) );
}

inline int fetch_gpio_is_valid_pin_subcommand(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkpin_subcommand)
{
	return(token_match(chp, fetch_terms->pin_subcommand, chkpin_subcommand,
	                   ((int) NELEMS(fetch_terms->pin_subcommand))) );
}

/*! \brief support validation functions
 *  \warning don't call string functions on NULL pointers
 *
 *  Return NO_FETCH_GPIO_PIN on failed match
 */
static FETCH_GPIO_pinnum string_to_pinnum(BaseSequentialStream * chp, char * pinstr)
{
	size_t maxlen    = 1;
	int    num_elems = NELEMS(fetch_gpio_pinmap);
	for(int i = 0; i < num_elems; ++i)
	{
		chDbgAssert(((fetch_gpio_pinmap[i].pinstring != NULL)
		             && (pinstr != NULL)), "string_to_pinnum() #1", "NULL pointer");

		maxlen = get_longest_str_length(fetch_gpio_pinmap[i].pinstring, pinstr, MAX_PIN_STR_LEN);

		if (strncasecmp(fetch_gpio_pinmap[i].pinstring, pinstr, maxlen ) == 0)
		{
			return fetch_gpio_pinmap[i].pinnum;
		}
	}
	return NO_FETCH_GPIO_PIN;
}

/*! \brief convert string to gpioport
 *
 * \param[in] portstr      Input String representing port 
 * \return GPIO_TypeDef Port
 * \return NULL on error
 */
static GPIO_TypeDef * string_to_gpioport(BaseSequentialStream * chp, char * portstr)
{
	size_t maxlen    = 1;
	int    num_elems = NELEMS(fetch_gpio_portmap);
	for(int i = 0; i < num_elems; ++i)
	{
		chDbgAssert(((fetch_gpio_portmap[i].portstring != NULL)
		             && (portstr != NULL)), "string_to_gpioport() #1", "NULL pointer");

		maxlen = get_longest_str_length(fetch_gpio_portmap[i].portstring, portstr, MAX_PIN_STR_LEN);

		if (strncasecmp(fetch_gpio_portmap[i].portstring, portstr, maxlen ) == 0)
		{
			return fetch_gpio_portmap[i].portptr;
		}
	}
	return NULL;
}

/*! \brief get the port and pin information 
 */
static bool fetch_gpio_get_port_pin(BaseSequentialStream * chp, char * cmd_list[],
                                    Fetch_terminals * fetch_terms,
                                    GPIO_TypeDef ** port, FETCH_GPIO_pinnum * pin)
{
	size_t               maxlen                  = 0;
	GPIO_TypeDef *       fetch_gpio_port;
	FETCH_GPIO_pinnum    fetch_gpio_pinnum;

	if(fetch_gpio_is_valid_port_subcommand(chp, fetch_terms, cmd_list[FETCH_GPIO_PORT]) >= 0)
	{
		fetch_gpio_port = string_to_gpioport(chp, cmd_list[FETCH_GPIO_PORT]);
		if(fetch_gpio_port != NULL )
		{
			*port = fetch_gpio_port;
		}
		else
		{
			*port = NULL;
			return false;
		}
	}
	else
	{
		return false;
	}

	if(fetch_gpio_is_valid_pin_subcommand(chp, fetch_terms, cmd_list[FETCH_GPIO_PIN]) >= 0)
	{

		fetch_gpio_pinnum = string_to_pinnum(chp, cmd_list[FETCH_GPIO_PIN]);
		if(fetch_gpio_pinnum != NO_FETCH_GPIO_PIN)
		{
			*pin = fetch_gpio_pinnum;
		}
		else
		{
			*pin = NO_FETCH_GPIO_PIN;
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

/*! \brief read the input register value for the port and pin
 */
bool fetch_gpio_get(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                    char * cmd_list[])
{
	GPIO_TypeDef *       port    = GPIOA;
	FETCH_GPIO_pinnum    pin     = PIN0;

	if(fetch_gpio_get_port_pin(chp, cmd_list, fetch_terms, &port, &pin))
	{
		if((port != NULL) && (pin != NO_FETCH_GPIO_PIN))
		{
			chprintf(chp, "%d\r\n", palReadPad(port, pin));
			return true;
		}
	}
	return false;
}


/*! \brief set the output register value to HIGH for the port and pin
 */
bool fetch_gpio_set(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                    char * cmd_list[])
{
	GPIO_TypeDef *    port       = NULL;
	FETCH_GPIO_pinnum pin        = PIN0;

	if(fetch_gpio_get_port_pin(chp, cmd_list, fetch_terms, &port, &pin))
	{
		if((port != NULL) && (pin != NO_FETCH_GPIO_PIN))
		{
			palSetPad(port, pin);
			return true;
		}
	}
	return false;
}

/*! \brief clear the output register value (set to LOW) for the port and pin
 */
bool fetch_gpio_clear(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                      char * cmd_list[])
{
	GPIO_TypeDef *       port    = NULL;
	FETCH_GPIO_pinnum    pin     = PIN0;

	if(fetch_gpio_get_port_pin(chp, cmd_list, fetch_terms, &port, &pin))
	{
		if((port != NULL) && (pin != NO_FETCH_GPIO_PIN))
		{
			palClearPad(port, pin);
			return true;
		}
	}
	return false;
}

/*! \brief configure a pin
 */
bool fetch_gpio_config(BaseSequentialStream * chp, Fetch_terminals * fetch_terms,
                       char * cmd_list[])

{
	GPIO_TypeDef *       port       = NULL;
	FETCH_GPIO_pinnum    pin        = PIN0;

	int            sense      = PAL_STM32_PUDR_FLOATING;
	int            direction  = PAL_STM32_MODE_INPUT;

	if(fetch_gpio_is_valid_gpio_direction(chp, fetch_terms, cmd_list[FETCH_GPIO_DIRECTION]) >= 0)
	{
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
			DBG_MSG(chp, "The port direction is not available");
			return false;
		}
	}
	else
	{
		return false;
	}

	if(fetch_gpio_is_valid_gpio_sense(chp, fetch_terms, cmd_list[FETCH_GPIO_SENSE]) >= 0)
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
			sense = PAL_STM32_MODE_ANALOG;
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


	if(fetch_gpio_get_port_pin(chp, cmd_list, fetch_terms, &port, &pin))
	{
		if((port != NULL) && (pin != NO_FETCH_GPIO_PIN))
		{
			DBG_VMSG(chp, "pin: %d", pin);
			DBG_VMSG(chp, "port: 0x%x", port);
			DBG_VMSG(chp, "dir: %d", direction);
			DBG_VMSG(chp, "sense: %d", sense);
			palSetPadMode(port, pin, direction | sense);
			return true;
		}
	}
	return false;
}

/*! \brief dispatch a specific gpio command
 */
bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                         Fetch_terminals * fetch_terms)
{

	if(fetch_gpio_is_valid_gpio_subcommandA(chp, fetch_terms, cmd_list[FETCH_GPIO_ACTION]) >= 0)
	{
		if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "get", strlen("get") ) == 0)
		{
			return(fetch_gpio_get(chp, fetch_terms, cmd_list));
		}
		else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "set", strlen("set") ) == 0)
		{
			return(fetch_gpio_set(chp, fetch_terms, cmd_list));
		}
		else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "clear", strlen("clear") ) == 0)
		{
			return(fetch_gpio_clear(chp, fetch_terms, cmd_list));
		}
		else if (strncasecmp(cmd_list[FETCH_GPIO_ACTION], "config", strlen("config") ) == 0)
		{
			if( (cmd_list[FETCH_GPIO_DIRECTION] != NULL) && (cmd_list[FETCH_GPIO_SENSE] != NULL))
			{
				return(fetch_gpio_config(chp, fetch_terms, cmd_list));
			}
			else
			{
				return false;
			}
		}
		else
		{
			DBG_MSG(chp, "sub-command not ready yet...");
			return(false) ;
		}
		return true;
	}
	return false;
}


/*! @} */
