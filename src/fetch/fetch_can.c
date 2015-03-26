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
static bool fetch_can_exchange_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
/*static bool fetch_can_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);*/
static bool fetch_can_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_can_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_can_commands[] = {
    { fetch_can_exchange_cmd,  "exchange",  "TX data to slave\n" \
                                            "Usage: exchange(<dev>,<addr>,<base>,<byte 0>,[...,<byte n>])" },
   
    { fetch_can_config_cmd,    "config",    "Configure CAN driver\n" \
                                            "Usage: config(<dev>)" },
    { fetch_can_reset_cmd,     "reset",     "Reset CAN driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_can_help_cmd,      "help",      "CAN command help" },
    { NULL, NULL, NULL } // null terminate list
  };
struct can_instance {
  CANDriver     *canp;
  uint32_t      led;
};

static const struct can_instance can1 = {&CAND1,1};
/*static const struct can_instance can2 = {&CAND2, 2}; */

/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};
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
    while (canReceive(cip->canp, CAN_ANY_MAILBOX,
                      &rxmsg, TIME_IMMEDIATE) == RDY_OK) {
      /* Process message.*/
      palTogglePad(GPIOD, cip->led);
    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);
  return 0;
}

/*
 * Transmitter thread.
 */
static WORKING_AREA(can_tx_wa, 256);
static msg_t can_tx(void * p) {
  CANTxFrame txmsg;
 (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  while (!chThdShouldTerminate()) {
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    /*canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));*/
    chThdSleepMilliseconds(500);
  }
  return 0;
}
void fetch_can_init(BaseSequentialStream * chp)
{
 /*gpio_heartbeat_thread = chThdCreateStatic(wa_gpio_heartbeat_thread, sizeof(wa_gpio_heartbeat_thread), NORMALPRIO, (tfunc_t)gpio_heartbeat_tfunc, NULL);*/

  can_init_flag = false;
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
 return true;
}

static bool fetch_can_exchange_cmd(BaseSequentialStream * chp,char * cmd_list[],char *  data_list[])
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
