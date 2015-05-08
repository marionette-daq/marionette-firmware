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



static CANConfig can_configs[] ={ {0,0}, {0,0}};
static Thread *tp;
static CANTxFrame txmsg;
//Subject to change when the Config, Tx, and Rx parameters get sorted
enum {
  CAN_CONFIG_DEV = 0,
  CAN_CONFIG_BTR,
  CAN_CONFIG_DLC,
  CAN_CONFIG_EID,
  CAN_CONFIG_RTR
};

struct can_instance {
	  CANDriver     *canp;
	    uint32_t      led;
};

static const struct can_instance can1 = {&CAND1, GPIOH_PIN2};

static bool can_init_flag = true;
static bool fetch_can_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_transmit_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_can_commands[] = {
    { fetch_can_transmit_cmd,  "transmit",  "TX data to slave\n" \
                                            "\tUsage: transmit(<dev>,<data 0>,[<byte 1>])\n"},
    { fetch_can_receive_cmd,   "receive",   "RX data from slave\n" \
                                            "Usage: receive(<dev>)" },

   
    { fetch_can_config_cmd,    "config",    "Configure CAN driver\n" \
                                            "Usage: config(<dev>,<bitrate>,<DLC>,<EID>,<RTR>)\n" \
   					    "\t{ Enter 10, 20, 100, 125, 250, 500, or 1000. Values are in kb}\n" \
   					    "\tDLC =  0 ... 8\n" \
   					    "\t{EID: Enhancement Mode ID}\n" \
   					    "\tRTR = 0 | 1  \n"},
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

static WORKING_AREA(can_rx1_wa, 256);
static WORKING_AREA(can_rx2_wa, 256);
static msg_t can_rx(void *p) {
 struct can_instance *cip = p;
 EventListener el;
 CANRxFrame rxmsg;

 (void)p;
 chRegSetThreadName("receiver");
 chEvtRegister(&cip->canp->rxfull_event, &el, 0);
 while(!chThdShouldTerminate()) {
   if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
     continue;
     while (canReceive(cip->canp, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {
      /* Process message.*/
       palTogglePad(GPIOH, cip->led);
    }
  }
 chEvtUnregister(&CAND1.rxfull_event, &el);
 return 0;
}


void fetch_can_init(BaseSequentialStream * chp)
{

  can_init_flag = false; /*Not sure what this is doing or if it is needed*/
}

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
 char *endptr;	
 int32_t can_dev; 
 CANDriver * can_drv; 
 CANConfig * can_cfg;
// CANTxFrame txmsg;
 
 int BRP = 6;
 int TS1 = 8;
 int TS2 = 1;

 if(!fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 5)) //max data field may change depending on feedback
 {
  return false;
 }
 if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL)
 {
    util_message_error(chp, "invalid device identifier");
    return false;
 }
 
int32_t baud_rate = strtol(data_list[CAN_CONFIG_BTR],&endptr,0); //Need to set up a data_list variable or enum
if( *endptr != '\0')
 {
  util_message_error(chp, "invalid device identifier");
  return false;
 }
  switch (baud_rate)
  {
   case 10:
     BRP = 199;
     TS1 = 12;
     TS2 = 6;
     break;
   case 20:
     BRP = 99;
     TS1 = 12;
     TS2 = 6;
     break;

   case 50:
     BRP = 34;
     TS1 = 14;
     TS2 = 7;
     break;

   case 100:
     BRP = 19;
     TS1 = 12;
     TS2 = 6;
     break;

   case 125:
     BRP = 19;
     TS1 = 12;
     TS2 = 6;
     break;
     
   case 250:
     BRP = 12;
     TS1 = 5;
     TS2 = 4;
     break;

   case 500:
     BRP = 6;
     TS1 = 8;
     TS2 = 1;
     break;

   case 1000:
     BRP = 6;
     TS1 = 2;
     TS2 = 1;
     break;
   default:
     util_message_error(chp, "Invalid baud rate, see can.help for available baud rates");
     return false;
     break;

  }


 /*TODO Set up checks for baud rate (Dependent on TS2, TS1, and BRP.
  * BAUD = APB1/((BRP+1)*(3+TS1+TS2)), where APB1 is currently equals 42 MHz 
  * Current setup is for 500k. BRP =6, TS1 = TS2 = 2 yields 1M. */
 can_cfg = &can_configs[can_dev -1];
 can_cfg->mcr = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP;
 can_cfg->btr = CAN_BTR_SJW(0) | CAN_BTR_TS2(TS2) | CAN_BTR_TS1(TS1) | CAN_BTR_BRP(BRP);

 int32_t data_length_code = strtol(data_list[CAN_CONFIG_DLC],&endptr,0); //Need to set up a data_list variable or enum
 if( *endptr != '\0')
 {
  util_message_error(chp, "invalid DLC");
  return false;

}
  txmsg.DLC = data_length_code;
 
int32_t extended_identifier = strtol(data_list[CAN_CONFIG_EID], &endptr,0);
if( *endptr !='\0')
{
  util_message_error(chp, "invalid extended identifier" );
  return false;
}
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = extended_identifier; //May need some check for length of identifier

int32_t remote_transmission_request = strtol(data_list[CAN_CONFIG_RTR], &endptr, 0);
if (*endptr != '\0')
{
  util_message_error(chp, "invalid remote transmission");
  return false;
}
  if(remote_transmission_request == 1)
  {	  
  txmsg.RTR = CAN_RTR_DATA;
  }

 switch( can_dev )
 {
    case 1:
      if( !io_manage_set_mode( can1_pins[0].port, can1_pins[0].pin, PAL_MODE_ALTERNATE(9), IO_CAN) ||
          !io_manage_set_mode( can1_pins[1].port, can1_pins[1].pin, PAL_MODE_ALTERNATE(9), IO_CAN) )
      {
        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( can1_pins[0].port, can1_pins[0].pin, IO_CAN );
        io_manage_set_default_mode( can1_pins[1].port, can1_pins[1].pin, IO_CAN );
        return false;
      }
      util_message_info(chp, "Can 1 is configured");
     canStart(&CAND1, can_cfg);
      break;
    case 2:
      if( !io_manage_set_mode( can2_pins[0].port, can2_pins[0].pin, PAL_MODE_ALTERNATE(9), IO_CAN) ||
          !io_manage_set_mode( can2_pins[1].port, can2_pins[1].pin, PAL_MODE_ALTERNATE(9), IO_CAN) )
      {        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( can2_pins[0].port, can2_pins[0].pin, IO_CAN );
        io_manage_set_default_mode( can2_pins[1].port, can2_pins[1].pin, IO_CAN );
        return false;
      }
      util_message_error(chp, "Can 2 is configured");
      canStart(&CAND2, can_cfg);
      break;
 }

 return true;
}

static bool fetch_can_transmit_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
  int32_t can_dev;
  char * endptr;
  CANDriver * can_drv;
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3) )
  {
    return false;
  }
  if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL)
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }
  if( can_drv->state != CAN_READY )
  {
    util_message_error(chp, "CAN not ready");
    return false;
  }
 uint32_t data_input1 = strtoul(data_list[1], &endptr, 0);
 if( *endptr != '\0')
 {
   util_message_error(chp, "invalid input data");
   return false;
 }
 uint32_t data_input2 = strtoul(data_list[2], &endptr, 0);
//Toggle LED for debugging 
 palSetPadMode(GPIOH, GPIOH_PIN3, PAL_MODE_OUTPUT_PUSHPULL);
 txmsg.data32[0] = data_input1;
 if(*endptr == '\0')
 {
 txmsg.data32[1] = data_input2;
 }

 switch( can_dev )
 {
  case 1:
   canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
   palTogglePad(GPIOH,GPIOH_PIN3);
   break;
  case 2:
   canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
   palTogglePad(GPIOH,GPIOH_PIN3);
   break;
 }
 return true;
}

static bool fetch_can_receive_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
 int32_t can_dev;
 CANDriver * can_drv;
 if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
 { 
  return false;
 }
 if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL)
 { 
   util_message_error(chp, "invalid device identifier");
   return false;
 }

 palSetPadMode(GPIOH, GPIOH_PIN2, PAL_MODE_OUTPUT_PUSHPULL);
 tp =  chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, (void *)&can1);
 return true;
}

static bool fetch_can_reset_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
  int32_t can_dev;
  CANDriver * can_drv;
  chThdTerminate(tp);
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }
  
  if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }
  switch( can_dev )
  {
#if STM32_CAN_USE_CAN1
    case 1:
      canStop(&CAND1);
      io_manage_set_default_mode( can1_pins[0].port, can1_pins[0].pin, IO_GPIO );
      io_manage_set_default_mode( can1_pins[1].port, can1_pins[1].pin, IO_GPIO );
            break;
#endif
#if STM32_CAN_USE_CAN2
    case 2:
      canStop(&CAND2);      
      io_manage_set_default_mode( can2_pins[0].port, can2_pins[0].pin, IO_GPIO );
      io_manage_set_default_mode( can2_pins[1].port, can2_pins[1].pin, IO_GPIO );
            break;
#endif
  }
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
