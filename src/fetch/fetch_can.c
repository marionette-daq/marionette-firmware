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

//Subject to change when the Config, Tx, and Rx parameters get sorted
enum {
  CAN_CONFIG_DEV = 0,
  CAN_CONFIG_BTR,
  CAN_CONFIG_DLC,
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
                                            "Usage: transmit(<dev>)" },
    { fetch_can_receive_cmd,   "receive",   "RX data from slave\n" \
                                            "Usage: receive(<dev>)" },

   
    { fetch_can_config_cmd,    "config",    "Configure CAN driver\n" \
                                            "Usage: config(<dev>,[<bitrate>])\n" \
   					    "\tBitrate {In progress defaults to 500k}\n" },
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
/*static const CANConfig can_cfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};*/
/*static WORKING_AREA(can_tx_wa, 256);*/
/*static msg_t can_tx(void * p) {
  CANTxFrame txmsg;
  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT; //Set bit for extended frame format
  txmsg.EID = 0x01234567; //Extended identifier 
  txmsg.RTR = CAN_RTR_DATA; //Remote transmission Request
  txmsg.DLC = 8; //Data length code, can be 0-8
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;
  while (!chThdShouldTerminate()) {
        canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
        canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
        palTogglePad(GPIOH,GPIOH_PIN3);
        chThdSleepMilliseconds(500);
      }
  return 0;
}*/
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
 /*gpio_heartbeat_thread = chThdCreateStatic(wa_gpio_heartbeat_thread, sizeof(wa_gpio_heartbeat_thread), NORMALPRIO, (tfunc_t)gpio_heartbeat_tfunc, NULL);*/

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
	
 int32_t can_dev; 
 CANDriver * can_drv; 
 CANConfig * can_cfg;


 if(!fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2)) //max data field may change depending on feedback
 {
  return false;
 }
 if( (can_drv = parse_can_dev(data_list[0], &can_dev)) == NULL)
 {
    util_message_error(chp, "invalid device identifier");
    return false;
 }

 /*TODO Set up checks for baud rate (Dependent on TS2, TS1, and BRP.
  * BAUD = APB1(42MHz)/((BRP+1)*(3+TS1+TS2)) 
  * Current setup is for 500k. BRP =6, TS1 = TS2 = 2 yields 1M. */
 can_cfg = &can_configs[can_dev -1];
 can_cfg->mcr = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP;
 can_cfg->btr = CAN_BTR_SJW(0) | CAN_BTR_TS2(1) | CAN_BTR_TS1(8) | CAN_BTR_BRP(6);



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
      util_message_info(chp, "Configuring Can 2");
      if( !io_manage_set_mode( can2_pins[0].port, can2_pins[0].pin, PAL_MODE_ALTERNATE(9), IO_CAN) ||
          !io_manage_set_mode( can2_pins[1].port, can2_pins[1].pin, PAL_MODE_ALTERNATE(9), IO_CAN) )
      {        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( can2_pins[0].port, can2_pins[0].pin, IO_CAN );
        io_manage_set_default_mode( can2_pins[1].port, can2_pins[1].pin, IO_CAN );
        return false;
      }
      canStart(&CAND2, can_cfg);
      break;
 }

/* #if STM32_CAN_USE_CAN1
  canStart(&CAND1, &can_cfg);
 #endif
 #if STM32_CAN_USE_CAN2
  canStart(&CAND2, &can_cfg);
 #endif*/
/*canStart(can_drv,can_cfg); */
 return true;
}

static bool fetch_can_transmit_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
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
//Toggle LED for debugging 
 palSetPadMode(GPIOH, GPIOH_PIN3, PAL_MODE_OUTPUT_PUSHPULL);
 CANTxFrame txmsg;
 txmsg.IDE = CAN_IDE_EXT; //Set bit for extended frame format
 txmsg.EID = 0x01234567; //Extended identifier 
 txmsg.RTR = CAN_RTR_DATA; //Remote transmission Request
 txmsg.DLC = 8; //Data length code, can be 0-8
 txmsg.data32[0] = 0x55AA55AA;
 txmsg.data32[1] = 0x00FF00FF;
 
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
 chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, (void *)&can1);
 return true;
}

static bool fetch_can_reset_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
{
  int32_t can_dev;
  CANDriver * can_drv;

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
      io_manage_set_default_mode( can1_pins[0].port, can1_pins[0].pin, IO_GPIO );
      io_manage_set_default_mode( can1_pins[1].port, can1_pins[1].pin, IO_GPIO );
      canStop(&CAND1);
      break;
#endif
#if STM32_CAN_USE_CAN2
    case 2:
      io_manage_set_default_mode( can2_pins[0].port, can2_pins[0].pin, IO_GPIO );
      io_manage_set_default_mode( can2_pins[1].port, can2_pins[1].pin, IO_GPIO );
      canStop(&CAND2);
      break;
#endif
  }
  chThdShouldTerminate();
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
