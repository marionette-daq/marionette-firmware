/*! \file fetch_dac.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @defgroup fetch_dac Fetch DAC
  * @{
  */

/*!
 * <hr>
 *
 *  In ChibiOS 2.6.x there is no DAC module in the HAL. It is available
 *  in the kernel_3_dev branch.
 *
 * <hr>
 */

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "mshell_state.h"

#include "io_manage.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "dac.h"
#include "fetch_dac.h"

/* Reference STF4 Reference
 *   Once the DAC channelx is enabled, the corresponding GPIO pin (PA4 or PA5) is
 *   automatically connected to the analog converter output (DAC_OUTx). In order to avoid
 *   parasitic consumption, the PA4 or PA5 pin should first be configured to analog (AIN).
 */

/*! \brief Channel output pins */
#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static port_pin_t dac_outputs[] = { { GPIOA, GPIOA_PIN4 }, 
                                    { GPIOA, GPIOA_PIN5 } };
#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for DAC"
#else
#error "Board not defined for DAC"
#endif

static bool fetch_dac_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_dac_commands[] = {
    { fetch_dac_help_cmd,   "help",   "DAC command help" },
    { fetch_dac_config_cmd, "config", "Configure DAC driver and pins" },
    { fetch_dac_write_cmd,  "write",  "Write values to DAC\nUsage: write(<ch1>,<ch2>)" },
    { fetch_dac_reset_cmd,  "reset",  "Reset DAC driver and pins" },
    { NULL, NULL, NULL }
  };

static DACConfig  fetch_dac_config =
{
	.dhrm     = DAC_DHRM_12BIT_RIGHT,
	.cr_flags = STM32_DAC_CR_EN
};

static bool dac_init_flag = true;


static bool fetch_dac_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  util_message_info(chp, "Fetch DAC Help:");
  fetch_display_help(chp, fetch_dac_commands);
	return true;
}

static bool fetch_dac_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( DACD1.state != DAC_STOP )
  {
    util_message_error(chp, "DAC already configured");
    return false;
  }

  if( !io_manage_set_mode( dac_outputs[0].port, dac_outputs[0].pin, PAL_STM32_MODE_ANALOG, IO_DAC) )
  {
    util_message_error(chp, "unable to allocate dac pins");
    return false;
  }

  if( !io_manage_set_mode( dac_outputs[1].port, dac_outputs[1].pin, PAL_STM32_MODE_ANALOG, IO_DAC) )
  {
    util_message_error(chp, "unable to allocate dac pins");
    io_manage_set_default_mode( dac_outputs[0].port, dac_outputs[0].pin );
    return false;
  }

	DACD1.dac->DHR12R1 = 0;
	DACD1.dac->DHR12R2 = 0;

  dacStart(&DACD1, &fetch_dac_config);

  return true;
}

static bool fetch_dac_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( DACD1.state != DAC_READY )
  {
    util_message_error(chp, "DAC not configured");
    return false;
  }

  // FIXME add error handling on input values

	DACD1.dac->DHR12R1 = strtol(data_list[0], &endptr, 0);
	DACD1.dac->DHR12R2 = strtol(data_list[1], &endptr, 0);

  return true;
}

static bool fetch_dac_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

	dacStop(&DACD1);

	// reset dac pins
  if( io_manage_get_current_alloc( dac_outputs[0].port, dac_outputs[0].pin ) == IO_DAC )
  {
    io_manage_set_default_mode( dac_outputs[0].port, dac_outputs[0].pin );
  }
  if( io_manage_get_current_alloc( dac_outputs[1].port, dac_outputs[1].pin ) == IO_DAC )
  {
    io_manage_set_default_mode( dac_outputs[1].port, dac_outputs[1].pin );
  }
  return true;
}


static void fetch_dac_init(void)
{
	dacInit();
  dac_init_flag = false;
}

/*! \brief dispatch an DAC command
 */
bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	if( dac_init_flag )
	{
		fetch_dac_init();
	}

  return fetch_dispatch(chp, fetch_dac_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}
//! @}

