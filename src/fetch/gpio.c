/*! \file gpio.c

 Marionette gpio routines
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include "chprintf.h"
#include "util_messages.h"
#include "util_strings.h"

#include "gpio.h"

static inline int fetch_is_valid_gpio_direction(BaseSequentialStream * chp,
                char * chkgpio_direction)
{
	return( fetch_token_match(chp, fetch_terminals.gpio_direction, chkgpio_direction,
	                          ((int) NELEMS(fetch_terminals.gpio_direction))) );
}

static inline int fetch_is_valid_gpio_sense(BaseSequentialStream * chp, char * chkgpio_sense)
{
	return(fetch_token_match(chp, fetch_terminals.gpio_sense, chkgpio_sense,
	                         ((int) NELEMS(fetch_terminals.gpio_sense))) );
}

static inline int fetch_is_valid_gpio_subcommandA(BaseSequentialStream * chp,
                char * chkgpio_subcommandA)
{
	return(fetch_token_match(chp, fetch_terminals.gpio_subcommandA, chkgpio_subcommandA,
	                         ((int) NELEMS(fetch_terminals.gpio_subcommandA)) ) );
}


static bool gpio_get_port_pin(BaseSequentialStream * chp, char * commandl[],
                              GPIO_TypeDef ** port, GPIO_pinnums * pin)
{
	size_t maxlen = 0;

	/*TODO I sometimes find it helpful to create a structure, with an array of elements, and iterate over. Something like this
	 * typedef struct {
	 *    char *port_string;
	 *    GPIO_TypeDef *port;
	 * } string_port_mapping_t;
	 *
	 * string_port_mapping_t    port_array[] = {
	 *      {"porta", GPIOA},
	 *      {"portb", GPIOB},
	 *      {"portc", GPIOC}
	 *      }
	 */

	/*FIXME I think validation is done in the fetch part of the code, however, that would imply an assumed consistency between the
	 * other validation part of the code and this part of the code. I prefer to keep the definition of validity and the useful
	 * operation being performed in one location, preferably identical code. One alternate approach may be to define this function
	 * such that it can return an error, then do validation by running this function and checking for an error. I'm not sure if that
	 * will work cleanly with the rest of the code base, but it's food for thought.
	 */

	if(fetch_is_valid_port_subcommand(chp, cmd_list[PORT]) >= 0)
	{
		if(strncasecmp(commandl[PORT], "porta", strlen("porta") ) == 0)
		{
			*port = GPIOA;
		}
		else if(strncasecmp(commandl[PORT], "portb", strlen("porta") ) == 0)
		{
			*port = GPIOB;
		}
		else if(strncasecmp(commandl[PORT], "portc", strlen("porta") ) == 0)
		{
			*port = GPIOC;
		}
		else if(strncasecmp(commandl[PORT], "portd", strlen("porta") ) == 0)
		{
			*port = GPIOD;
		}
		else if(strncasecmp(commandl[PORT], "porte", strlen("porta") ) == 0)
		{
			*port = GPIOE;
		}
		else if(strncasecmp(commandl[PORT], "portf", strlen("porta") ) == 0)
		{
			*port = GPIOF;
		}
		else if(strncasecmp(commandl[PORT], "portg", strlen("porta") ) == 0)
		{
			*port = GPIOG;
		}
		else if(strncasecmp(commandl[PORT], "porth", strlen("porta") ) == 0)
		{
			*port = GPIOH;
		}
		else if(strncasecmp(commandl[PORT], "porti", strlen("porta") ) == 0)
		{
			*port = GPIOI;
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
	if(fetch_is_valid_pin_subcommand(chp, cmd_list[PIN]) >= 0)
	{

		maxlen = get_longest_str_length(commandl[PIN], "pinX", MAX_PIN_STR_LEN );
		if(maxlen == 4 )
		{
			if(strncasecmp(commandl[PIN], "pin0", maxlen ) == 0)
			{
				*pin = PIN0;
			}
			else if (strncasecmp(commandl[PIN], "pin1", maxlen ) == 0)
			{
				*pin = PIN1;
			}
			else if (strncasecmp(commandl[PIN], "pin2", maxlen ) == 0)
			{
				*pin = PIN2;
			}
			else if (strncasecmp(commandl[PIN], "pin3", maxlen ) == 0)
			{
				*pin = PIN3;
			}
			else if (strncasecmp(commandl[PIN], "pin4", maxlen ) == 0)
			{
				*pin = PIN4;
			}
			else if (strncasecmp(commandl[PIN], "pin5", maxlen ) == 0)
			{
				*pin = PIN5;
			}
			else if (strncasecmp(commandl[PIN], "pin6", maxlen ) == 0)
			{
				*pin = PIN6;
			}
			else if (strncasecmp(commandl[PIN], "pin7", maxlen ) == 0)
			{
				*pin = PIN7;
			}
			else if (strncasecmp(commandl[PIN], "pin8", maxlen + 5 ) == 0)
			{
				*pin = PIN8;
			}
			else if (strncasecmp(commandl[PIN], "pin9", maxlen ) == 0)
			{
				*pin = PIN9;
			}
			else
			{
				*pin = NULL;
				return false;
			}
		}
		else
		{
			if (strncasecmp(commandl[PIN], "pin10", maxlen ) == 0)
			{
				*pin = PIN10;
			}
			else if (strncasecmp(commandl[PIN], "pin11", maxlen ) == 0)
			{
				*pin = PIN11;
			}
			else if (strncasecmp(commandl[PIN], "pin12", maxlen ) == 0)
			{
				*pin = PIN12;
			}
			else if (strncasecmp(commandl[PIN], "pin13", maxlen ) == 0)
			{
				*pin = PIN13;
			}
			else if (strncasecmp(commandl[PIN], "pin14", maxlen ) == 0)
			{
				*pin = PIN14;
			}
			else if (strncasecmp(commandl[PIN], "pin15", maxlen ) == 0)
			{
				*pin = PIN15;
			}
			else
			{
				*pin = NULL;
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}



bool gpio_get(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	if(gpio_get_port_pin(chp, commandl, &port, &pin))
	{
		if(*port != NULL)
		{
			chprintf(chp, "%d\r\n", palReadPad(port, pin));
			return true;
		}
		return false;
	}
}


void gpio_set(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);
	//FIXME if the port cannot be mapped, then *port is null, and this will seg-fault
	palSetPad(port, pin);
}

void gpio_clear(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);
	//FIXME if the port cannot be mapped, then *port is null, and this will seg-fault
	palClearPad(port, pin);

}

void gpio_config(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;
	int sense     = 0;//FIXME always set a reasonable default value, just in case the if-then-else blocks don't match anything, or return an error, which would proably be better
	int direction = 0;//FIXME always set a reasonable default value, just in case the if-then-else blocks don't match anything, or return an error, which would proably be better

	gpio_get_port_pin(chp, commandl, &port, &pin);

	if( (strncasecmp(commandl[DIRECTION], "input", strlen("input") ) == 0) )
	{
		direction = PAL_STM32_MODE_INPUT;
	}
	else if ((strncasecmp(commandl[DIRECTION], "output", strlen("output") ) == 0) )
	{
		direction = PAL_STM32_MODE_OUTPUT;
	}

	if( (strncasecmp(commandl[SENSE], "pullup", strlen("pullup") ) == 0) )
	{
		sense = PAL_STM32_PUDR_PULLUP;
	}
	else if ( (strncasecmp(commandl[SENSE], "pulldown", strlen("pulldown") ) == 0) )
	{
		sense = PAL_STM32_PUDR_PULLDOWN;
	}
	else if ( (strncasecmp(commandl[SENSE], "floating", strlen("floating") ) == 0) )
	{
		sense = PAL_STM32_PUDR_FLOATING;
	}
	else if ( (strncasecmp(commandl[SENSE], "analog", strlen("analog") ) == 0) )
	{
		sense = PAL_STM32_MODE_ANALOG;
	}

	DBG_VMSG(chp, "pin: %d", pin);
	DBG_VMSG(chp, "port: 0x%x\t0x%x", port, GPIOA_BASE);
	DBG_VMSG(chp, "dir: %d", direction);
	DBG_VMSG(chp, "sense: %d", sense);

	palSetPadMode(port, pin, direction | sense);
}

bool gpio_dispatch(chp, cmd_list, data_list, CMD_terminals * fetch_cmds) {
	if(fetch_is_valid_gpio_subcommandA(chp, cmd_list[ACTION]) >= 0)
	{
		if(fetch_is_valid_port_subcommand(chp, cmd_list[PORT]) >= 0)
		{
			if(fetch_is_valid_pin_subcommand(chp, cmd_list[PIN]) >= 0)
			{
				if (strncasecmp(cmd_list[1], "get", strlen("get") ) == 0)
				{
					return(gpio_get(chp, cmd_list));
				}
				else if (strncasecmp(cmd_list[1], "set", strlen("set") ) == 0)
				{
					return(gpio_set(chp, cmd_list));
				}
				else if (strncasecmp(cmd_list[1], "clear", strlen("clear") ) == 0)
				{
					return(gpio_clear(chp, cmd_list));
				}
				else if (strncasecmp(cmd_list[1], "config", strlen("config") ) == 0)
				{
					if( (cmd_list[DIRECTION] != NULL) && (cmd_list[SENSE] != NULL))
					{
						return(gpio_config(chp, cmd_list));
					}
					else
					{
						return false;
					}
				}
				else
				{
					return(fetch_not_yet(chp, cmd_list, data_list)) ;
				}
				return true;
			}
		}
	}
	return false;
}

