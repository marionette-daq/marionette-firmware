/*! \file fetch_can.c
 * Marionette fetch_can routines
 * @defgroup fetch_can
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"

#include "io_manage.h"
#include "io_manage_defs.h"
#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_can.h"

static bool can_init_flag = true;
static bool fetch_can_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_transmit_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_can_commands[] = {
    { fetch_can_transmit_cmd,  "transmit",  "TX data to slave\n" \
                                            "Usage: transmit(<dev>,<addr>,<base>,<byte 0>,[...,<byte n>])" },
    { fetch_can_receive_cmd,   "receive",   "RX data from slave\n" \
                                            "Usage: receive(<dev>,<addr>,<count>)" },

   
    { fetch_can_config_cmd,    "config",    "Configure CAN driver\n" \
                                            "Usage: config(<dev>,<DLC>,<data32[2]>)" },
    { fetch_can_reset_cmd,     "reset",     "Reset CAN driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_can_help_cmd,      "help",      "CAN command help" },
    { NULL, NULL, NULL } // null terminate list
  };

static CANDriver * parse_can_dev(char * str, int32_t * dev)
{
 char * endptr;
 int32_t num = strtol(str, &endptr, 0);
 if (*endptr !='\0')
 {
  return NULL;
 }
 if (dev != NULL)
 {
  *dev = num;
 }
 switch(num)
 {
  #if STM32_CAN_USE_CAN1
   case 1:
     return &CAND1;
  #endif
  
  #if STM32_CAN_USE_CAN2
   case 2:
     return &CAND2;
  #endif  
   default:
     return NULL;
 }
}

/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig can_cfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};

void fetch_can_init(BaseSequentialStream * chp)
{
 /*gpio_heartbeat_thread = chThdCreateStatic(wa_gpio_heartbeat_thread, sizeof(wa_gpio_heartbeat_thread), NORMALPRIO, (tfunc_t)gpio_heartbeat_tfunc, NULL);*/

  can_init_flag = false; /*Not sure what this is doing or if it is needed*/
}

/*! \brief dispatch a specific gpio command
 */
bool fetch_can_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( can_init_flag )
  {
    fetch_can_init(chp);
  }

  return fetch_dispatch(chp, fetch_can_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}

bool fetch_can_reset(BaseSequentialStream * chp)
{
  // TODO iterate through pins resetting any assigned to GPIO
  // this may not be needed since the fetch_reset_cmd function resets all pins anyways

  return true;
}

static bool fetch_can_config_cmd(BaseSequentialStream * chp, char * cmd_list[],char * data_list[])
{
 int32_t can_dev; /*Can Device */
 CANDriver * can_drv; /*Can driver */
 
 if(!fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3)) //max data field is temporary till I figure this out
 {
  return false;
 }
 if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL)
 {
    util_message_error(chp, "invalid device identifier");
    return false;
 }
 switch( can_dev )
 {
    case 1:
      if( !io_manage_set_mode( can1_pins[0].port, can1_pins[0].pin, PAL_MODE_ALTERNATE(9) | PAL_STM32_OTYPE_OPENDRAIN, IO_CAN) ||
          !io_manage_set_mode( can1_pins[1].port, can1_pins[1].pin, PAL_MODE_ALTERNATE(9) | PAL_STM32_OTYPE_OPENDRAIN, IO_CAN) )
      {
        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( can1_pins[0].port, can1_pins[0].pin, IO_CAN );
        io_manage_set_default_mode( can1_pins[1].port, can1_pins[1].pin, IO_CAN );
        return false;
      }
      break;
    case 2:
      if( !io_manage_set_mode( can2_pins[0].port, can2_pins[0].pin, PAL_MODE_ALTERNATE(9) | PAL_STM32_OTYPE_OPENDRAIN, IO_CAN) ||
          !io_manage_set_mode( can2_pins[1].port, can2_pins[1].pin, PAL_MODE_ALTERNATE(9) | PAL_STM32_OTYPE_OPENDRAIN, IO_CAN) )
      {        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( can2_pins[0].port, can2_pins[0].pin, IO_CAN );
        io_manage_set_default_mode( can2_pins[1].port, can2_pins[1].pin, IO_CAN );
        return false;
      }
      break;
 }

 #if STM32_CAN_USE_CAN1
  canStart(&CAND1, &can_cfg);
 #endif
 #if STM32_CAN_USE_CAN2
  canStart(&CAND2, &can_cfg);
 #endif
 return true;
}

static bool fetch_can_transmit_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
 return true;
}
static bool fetch_can_receive_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
 return true;
}

static bool fetch_can_reset_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
  return true;
}
static bool fetch_can_help_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");

  util_message_info(chp, "dev = "
#if STM32_CAN_USE_CAN1
  "1 "
#endif
#if STM32_CAN_USE_CAN2
  "2 "
#endif
  );
  util_message_info(chp, "base = (reference strtol c function)");

  fetch_display_help(chp, fetch_can_commands);
        return true;

}
