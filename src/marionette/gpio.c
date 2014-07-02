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

static enum GPIO_tokens
{
	CMD = 0,
	ACTION,
	PORT,
	PIN,
	DIRECTION,
	SENSE
} gpio_toks;

static enum GPIO_pinnums
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
} gpio_pinnums;


static void gpio_get_port_pin(BaseSequentialStream * chp, char * commandl[],
                              GPIO_TypeDef ** port, int * pin)
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

void gpio_set(BaseSequentialStream * chp, char * commandl[])
{
	util_infomsg(chp, "%s: Not yet\r\n", __func__);
	//palSetPad(GPIOB, 12);
}

void gpio_clear(BaseSequentialStream * chp, char * commandl[])
{
	util_infomsg(chp, "%s: Not yet\r\n", __func__);
}

void gpio_config(BaseSequentialStream * chp, char * commandl[])
{
	GPIO_TypeDef * port = NULL;
	int pin       = 0;
	int sense     = 0;
	int direction = 0;

	gpio_get_port_pin(chp, commandl, &port, &pin);

	DBG_VMSG(chp, "dir: %s", commandl[DIRECTION]);
	DBG_VMSG(chp, "sense: %s", commandl[SENSE]);
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
		DBG_MSG(chp, "sense float");
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


	//palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |

	//PAL_STM32_OSPEED_HIGHEST);           [> NSS.     <]
	//palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
	//PAL_STM32_OSPEED_HIGHEST);           [> SCK.     <]
	//palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              [> MISO.    <]
	//palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
	//PAL_STM32_OSPEED_HIGHEST);           [> MOSI.    <]
	///
}




