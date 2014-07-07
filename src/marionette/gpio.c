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

static void gpio_get_port_pin(BaseSequentialStream * chp, char * commandl[],
                              GPIO_TypeDef ** port, GPIO_pinnums * pin)
{
	size_t maxlen = 0;

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
	}
}

GPIO_pinval gpio_get(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);
	return(palReadPad(port, pin));
}

void gpio_set(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);
	palSetPad(port, pin);
}

void gpio_clear(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);
	palClearPad(port, pin);

}

void gpio_config(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port    = NULL;
	GPIO_pinnums pin       = 0;
	int sense     = 0;
	int direction = 0;

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

