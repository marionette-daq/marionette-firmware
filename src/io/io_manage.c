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

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_messages.h"

#include "mshell_state.h"
#include "io_manage.h"
#include "io_manage_defs.h"

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static IO_table * io_manage_tables[] = { &io_porta, &io_portb, &io_portc, &io_portd, &io_porte, &io_portf, &io_portg, &io_porth, &io_porti };
#endif

IO_namestr   io_manage_namestr[] = {{IO_NONE, "None"}, {IO_GPIO, "GPIO"}, {IO_ADC, "ADC"}, {IO_DAC, "DAC"}, {IO_SPI, "SPI"}, {IO_I2C, "I2C"}, {IO_USB, "USB"}};

static const char * io_manage_get_namestr(IO_alloc alloc)
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
static IO_table * io_manage_get_table(ioportid_t port)
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
static bool io_manage_fn_avail(ioportid_t port, uint32_t pad, IO_alloc request_alloc, IO_table * table)
{
	IO_alloc         curr_alloc;

	if(table != NULL)
	{
		curr_alloc = table->pin[pad].current_alloc;
		if(curr_alloc == request_alloc)
		{
			return true;
		}
		if((request_alloc & table->pin[pad].fn_available) != 0)
		{
			return true;
		};
	}
	return false;
}

/*! \brief Update the port allocation table
 */
bool io_manage_set_default_mode(ioportid_t port, uint32_t pad)
{
	IO_table    *    table = io_manage_get_table(port);

	table->pin[pad].current_mode  = table->pin[pad].default_mode;
	table->pin[pad].current_alloc = table->pin[pad].default_alloc;
	palSetPadMode(port, pad, table->pin[pad].default_mode);
	return true;
}

/*! \brief Update the port allocation table
 */
bool io_manage_set_mode(ioportid_t port, uint32_t pad, iomode_t new_mode, IO_alloc request_alloc)
{
	IO_table    *    table = io_manage_get_table(port);

	if(io_manage_fn_avail(port, pad, request_alloc, table))
	{
		table->pin[pad].current_mode  = new_mode;
		table->pin[pad].current_alloc = request_alloc;
		palSetPadMode(port, pad, new_mode);
		return true;
	}
	util_message_error(getMShellStreamPtr(), "Function not available.\r\n");
	return false;
}

/*! \brief Reset port allocation table to defaults and reset pad modes to defaults.
 */
void io_manage_to_defaults(void)
{
	IO_table    *    table;
	for(uint8_t i = 0; i < NELEMS(io_manage_tables); ++i)
	{
		table = io_manage_tables[i];
		for(uint8_t j = 0; j < NELEMS(table->pin); ++j)
		{
			table->pin[j].current_mode    = table->pin[j].default_mode;
			table->pin[j].current_alloc   = table->pin[j].default_alloc;
			palSetPadMode(table->port, table->pin[j].pad, table->pin[j].current_mode);
		}
	}
}

/*! \brief Query the current pin allocation
 */
void io_manage_query_pin(BaseSequentialStream * chp, ioportid_t port, uint32_t pad)
{
	IO_table    *    table = io_manage_get_table(port);
	util_message_info(chp, "%s",  io_manage_get_namestr(table->pin[pad].current_alloc));
}

//! @}

