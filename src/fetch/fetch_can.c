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
                                            "Usage: config(<dev>)" },
    { fetch_can_reset_cmd,     "reset",     "Reset CAN driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_can_help_cmd,      "help",      "CAN command help" },
    { NULL, NULL, NULL } // null terminate list
  };

