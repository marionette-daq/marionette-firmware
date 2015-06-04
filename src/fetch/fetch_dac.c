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

#include "io_manage.h"
#include "io_manage_defs.h"
#include "fetch_defs.h"
#include "fetch.h"
#include "dac.h"
#include "fetch_dac.h"
#include "dac124.h"
/* Reference STF4 Reference
 *   Once the DAC channelx is enabled, the corresponding GPIO pin (PA4 or PA5) is
 *   automatically connected to the analog converter output (DAC_OUTx). In order to avoid
 *   parasitic consumption, the PA4 or PA5 pin should first be configured to analog (AIN).
 */
#define MAX_BYTES 256
#define REFERENCE_VOLTAGE 3300

static bool fetch_dac_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_dac_commands[] = {
    { fetch_dac_help_cmd,   "help",   "DAC command help" },
    { fetch_dac_config_cmd, "config", "Configure DAC driver and pins\n"\
   				      "Usage: config(<dev>)\n" \
   				      "\tdev = 1 {DAC 1} | 2  {DAC 2-5}\n" },
    { fetch_dac_write_cmd,  "write",  "Write values to DAC\n"\
	    			      "Usage: write(<dev>, <data>)\n" 
   				      "\tdev = 1 | 2 | 3 | 4 | 5\n" \
    				      "\tdata is in millivolts from 0 to Vref (Currently Vref max is 3300)}\n"},
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
  char * endptr;
  bool ext_dac_config;
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }
  int32_t dac_dev = strtol(data_list[0], &endptr, 0);
  switch(dac_dev)
  {
	  case 1:
          if( DACD1.state != DAC_STOP )
    	  {
      	    util_message_error(chp, "DAC 1 already configured");
      	    return false;
    	  }

	    if( !io_manage_set_mode( dac_pins[0].port, dac_pins[0].pin, PAL_STM32_MODE_ANALOG, IO_DAC) )
	    {
	      util_message_error(chp, "unable to allocate dac pins");
	      io_manage_set_default_mode( dac_pins[0].port, dac_pins[0].pin, IO_DAC );
	     
	      return false;
	    }
  
	    DACD1.dac->DHR12R1 = 0;
	    //DACD1.dac->DHR12R2 = 0;
	    dacStart(&DACD1, &fetch_dac_config);
	    break;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	    if(!(ext_dac_config = dac124_config_cmd()))
	    {
	    util_message_error(chp, "unable to allocate dac pins");
	    return false;
	    }
	    break;
	  default:
	    util_message_error(chp, "Enter a number from 1 -5");
	    return false;
  }
  return true;
}

static bool fetch_dac_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;
  bool ext_dac_write;
  int byte_value;
  int dev_dac;
  int voltage_wanted;
//  uint32_t byte_count = 0;
  uint16_t tx_buffer[MAX_BYTES];
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) ) //DAC 1, a, b,c , & d
  {
    return false;
  }
  
  dev_dac = strtol(data_list[0], &endptr, 0);
 
  if(dev_dac == 1 &&  DACD1.state != DAC_READY )
  {
    util_message_error(chp, "DAC not configured");
    return false;
  }

  // FIXME add error handling on input values

/*	DACD1.dac->DHR12R1 = strtol(data_list[0], &endptr, 0);
	DACD1.dac->DHR12R2 = strtol(data_list[1], &endptr, 0);
*/
  byte_value = (strtol(data_list[1], &endptr, 0)*4096)/REFERENCE_VOLTAGE - 1;
  if(byte_value < 0)
  {
	  byte_value = 0;
  }
  if( *endptr != '\0' )
    {
      util_message_error(chp, "invalid data argument");
      return false;
    }
    else if( byte_value < 0 || byte_value > 0xfff )
    {
      util_message_error(chp, "invalid data byte");
      return false;
    }
  if (*endptr != '\0')
  {
     util_message_error(chp, "invalid device identifier");
     return false;
  }
  switch(dev_dac)
  {
    case 1:
      DACD1.dac->DHR12R1 = byte_value;
      return true;
    case 2:
      ext_dac_write = dac124_write_cmd(0, 1, byte_value);
      return true;
    case 3:
      ext_dac_write = dac124_write_cmd(1, 1,  byte_value);
      return true;
    case 4:
      ext_dac_write = dac124_write_cmd(2, 1, byte_value);
      return true;
    case 5:
      ext_dac_write = dac124_write_cmd(3, 1, byte_value);
      return true;
    default:
      util_message_error(chp, "Invalid DAC channel");
      return false;
  } 
  if(!ext_dac_write)
  {
    util_message_error(chp,"DAC 2 - 5 is not configured");
    return ext_dac_write;
  } 
  return true;
}

static bool fetch_dac_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;
  bool ext_dac_reset;
  int reset_channel;
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }
  reset_channel = strtol(data_list[0], &endptr, 0);
  switch (reset_channel)
  {
	case 1:
	  return fetch_dac_reset(chp);
	  break;
	case 2:
	case 3:
	case 4:
	case 5:
	  ext_dac_reset = dac124_reset_cmd();
	  if(!ext_dac_reset)
 	  {
    	   util_message_error(chp, "DAC 2 - 5 is not currently configured"); //Refine this check
  	  }
	  return ext_dac_reset;
	  break;
	default:
	  util_message_error(chp, "Enter 1 or 2 for DAC configuration");
	  return false;
	  break;
  }
  return true;
}


void fetch_dac_init(BaseSequentialStream * chp)
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
		fetch_dac_init(chp);
	}

  return fetch_dispatch(chp, fetch_dac_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}

bool fetch_dac_reset(BaseSequentialStream * chp)
{
	dacStop(&DACD1);

	// reset dac pins
  io_manage_set_default_mode( dac_pins[0].port, dac_pins[0].pin, IO_DAC );
 // io_manage_set_default_mode( dac_pins[1].port, dac_pins[1].pin, IO_DAC );
  
  return true;
}

//! @}

