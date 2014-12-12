/*! \file fetch_gpio.c
 * Marionette fetch_gpio routines
 * @defgroup fetch_gpio Fetch GPIO
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
#include "fetch_i2c.h"

#ifndef MAX_I2C_BYTES
#define MAX_I2C_BYTES   256
#endif

#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT MS2ST(100)
#endif

static I2CConfig   i2c_cfg = {OPMODE_I2C, 100000, STD_DUTY_CYCLE};
static bool i2c_init_flag = true;

// list all command function prototypes here 
static bool fetch_i2c_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_transmit_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_i2c_commands[] = {
    { fetch_i2c_transmit_cmd,  "transmit",  "TX data to slave\n" \
                                            "Usage: transmit(<dev>,<addr>,<base>,<byte 0>,[...,<byte n>])" },
    { fetch_i2c_receive_cmd,   "receive",   "RX data from slave\n" \
                                            "Usage: receive(<dev>,<addr>,<count>)" },
    { fetch_i2c_config_cmd,    "config",    "Configure I2C driver\n" \
                                            "Usage: config(<dev>)" },
    { fetch_i2c_reset_cmd,     "reset",     "Reset I2C driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_i2c_help_cmd,      "help",      "I2C command help" },
    { NULL, NULL, NULL } // null terminate list
  };


static I2CDriver * parse_i2c_dev( char * str, int32_t * dev )
{
  char * endptr;
  int32_t num = strtol(str, &endptr, 0);

  if(*endptr != '\0')
  {
    return NULL;
  }

  if( dev != NULL )
  {
    *dev = num;
  }

  switch( num )
  {
#if STM32_I2C_USE_I2C1
    case 1:
      return &I2CD1;
#endif
#if STM32_I2C_USE_I2C2
    case 2:
      return &I2CD2;
#endif
#if STM32_I2C_USE_I2C3
    case 3:
      return &I2CD3;
#endif
    default:
      return NULL;
  }
}

static bool fetch_i2c_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  int32_t i2c_dev;
  I2CDriver * i2c_drv;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  if( (i2c_drv = parse_i2c_dev(data_list[0], &i2c_dev)) == NULL)
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  // standard i2c configuration at 100kHz
  i2c_cfg.op_mode = OPMODE_I2C;
  i2c_cfg.clock_speed = 100000;
  i2c_cfg.duty_cycle = STD_DUTY_CYCLE;

  switch( i2c_dev )
  {
    case 1:
      if( !io_manage_set_mode( i2c1_pins[0].port, i2c1_pins[0].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) ||
          !io_manage_set_mode( i2c1_pins[1].port, i2c1_pins[1].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
      {
        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( i2c1_pins[0].port, i2c1_pins[0].pin, IO_I2C );
        io_manage_set_default_mode( i2c1_pins[1].port, i2c1_pins[1].pin, IO_I2C );
        return false;
      }
      break;
    case 2:
      if( !io_manage_set_mode( i2c2_pins[0].port, i2c2_pins[0].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) ||
          !io_manage_set_mode( i2c2_pins[1].port, i2c2_pins[1].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
      {
        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( i2c2_pins[0].port, i2c2_pins[0].pin, IO_I2C );
        io_manage_set_default_mode( i2c2_pins[1].port, i2c2_pins[1].pin, IO_I2C );
        return false;
      }
      break;
    case 3:
      if( !io_manage_set_mode( i2c3_pins[0].port, i2c3_pins[0].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) ||
          !io_manage_set_mode( i2c3_pins[1].port, i2c3_pins[1].pin, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
      {
        util_message_error(chp, "unable to allocate pins");
        io_manage_set_default_mode( i2c3_pins[0].port, i2c3_pins[0].pin, IO_I2C );
        io_manage_set_default_mode( i2c3_pins[1].port, i2c3_pins[1].pin, IO_I2C );
        return false;
      }
      break;
  }

  // apply configuration
  i2cStart(i2c_drv, &i2c_cfg);

  return true;
}

static bool fetch_i2c_transmit_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  static uint8_t tx_buffer[MAX_I2C_BYTES];
  uint32_t byte_count = 0;
  int number_base = 0;
  char * endptr;
  int byte_value;
  i2caddr_t address;
  I2CDriver * i2c_drv;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, MAX_I2C_BYTES + 3) )
  {
    return false;
  }
  
  if( (i2c_drv = parse_i2c_dev(data_list[0], NULL)) == NULL)
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  if( i2c_drv->state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  address = strtol(data_list[1], &endptr, 0);

  if( *endptr != '\0' || address > 127 )
  {
    util_message_error(chp, "invalid address byte");
    return false;
  }

  number_base = strtol(data_list[2], &endptr, 0);

  if( *endptr != '\0' || number_base == 1 || number_base < 0 || number_base > 36 )
  {
    util_message_error(chp, "invalid number base");
    return false;
  }

  for( int i = 0; i < MAX_I2C_BYTES && data_list[i+3] != NULL; i++ )
  {
    byte_value = strtol(data_list[i+3], &endptr, number_base);
    
    if( *endptr != '\0' )
    {
      util_message_error(chp, "invalid data byte");
      return false;
    }
    else if( byte_value < 0 || byte_value > 0xff )
    {
      util_message_error(chp, "invalid data byte");
      return false;
    }
    else
    {
      tx_buffer[i] = byte_value;
      byte_count++;
    }
  }

  switch( i2cMasterTransmitTimeout(i2c_drv, address, tx_buffer, byte_count, NULL, 0, I2C_TIMEOUT) )
  {
    case RDY_TIMEOUT:
      util_message_error(chp, "i2c timeout");
      i2cStart(i2c_drv, &i2c_cfg);
      return false;
    case RDY_RESET:
      util_message_error(chp, "i2c error");
      return false;
    case RDY_OK:
    default:
      return true;
  }
}

static bool fetch_i2c_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  static uint8_t rx_buffer[MAX_I2C_BYTES];
  uint32_t byte_count;
  char * endptr;
  i2caddr_t address;
  I2CDriver * i2c_drv;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3) )
  {
    return false;
  }

  if( (i2c_drv = parse_i2c_dev(data_list[0], NULL)) == NULL)
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  if( i2c_drv->state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  address = strtol(data_list[1], &endptr, 0);

  if( *endptr != '\0' || address > 127 )
  {
    util_message_error(chp, "invalid address byte");
    return false;
  }
  
  byte_count = strtol(data_list[2], &endptr, 0);

  if( *endptr != '\0' || byte_count > MAX_I2C_BYTES )
  {
    util_message_error(chp, "invalid byte count");
    return false;
  }

  switch( i2cMasterReceiveTimeout(i2c_drv, address, rx_buffer, byte_count, I2C_TIMEOUT) )
  {
    case RDY_TIMEOUT:
      util_message_error(chp, "i2c timeout");
      i2cStart(i2c_drv, &i2c_cfg);
      return false;
    case RDY_RESET:
      util_message_error(chp, "i2c error");
      return false;
    case RDY_OK:
    default:
      break;
  }

  util_message_uint32(chp, "count", &byte_count, 1);
  util_message_hex_uint8( chp, "rx", rx_buffer, byte_count);

  return true;
}

static bool fetch_i2c_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  int32_t i2c_dev;
  I2CDriver * i2c_drv;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  if( (i2c_drv = parse_i2c_dev(data_list[0], &i2c_dev)) == NULL)
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  switch( i2c_dev )
  {
    case 1:
      io_manage_set_default_mode( i2c1_pins[0].port, i2c1_pins[0].pin, IO_I2C );
      io_manage_set_default_mode( i2c1_pins[1].port, i2c1_pins[1].pin, IO_I2C );
      i2cStop(i2c_drv);
      break;
    case 2:
      io_manage_set_default_mode( i2c2_pins[0].port, i2c2_pins[0].pin, IO_I2C );
      io_manage_set_default_mode( i2c2_pins[1].port, i2c2_pins[1].pin, IO_I2C );
      i2cStop(i2c_drv);
      break;
    case 3:
      io_manage_set_default_mode( i2c3_pins[0].port, i2c3_pins[0].pin, IO_I2C );
      io_manage_set_default_mode( i2c3_pins[1].port, i2c3_pins[1].pin, IO_I2C );
      i2cStop(i2c_drv);
      break;
  }

  return true;
}

static bool fetch_i2c_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");

  util_message_info(chp, "dev = "
#if STM32_I2C_USE_I2C1
  "1 "
#endif
#if STM32_I2C_USE_I2C2
  "2 "
#endif
#if STM32_I2C_USE_I2C3
  "3 "
#endif
  );
  util_message_info(chp, "base = (reference strtol c function)");
  util_message_info(chp, "addr = 7 bit address, no r/w bit");

  fetch_display_help(chp, fetch_i2c_commands);
  return true;
}

void fetch_i2c_init(BaseSequentialStream * chp)
{
  // TODO do we need anything here?
  
  i2c_init_flag = false;
}

/*! \brief dispatch a specific i2c command
 */
bool fetch_i2c_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( i2c_init_flag )
  {
    fetch_i2c_init(chp);
  }
  return fetch_dispatch(chp, fetch_i2c_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}


bool fetch_i2c_reset(BaseSequentialStream * chp)
{

#if STM32_I2C_USE_I2C1
  io_manage_set_default_mode( i2c1_pins[0].port, i2c1_pins[0].pin, IO_I2C );
  io_manage_set_default_mode( i2c1_pins[1].port, i2c1_pins[1].pin, IO_I2C );
  i2cStop(&I2CD1);
#endif
#if STM32_I2C_USE_I2C2
  io_manage_set_default_mode( i2c2_pins[0].port, i2c2_pins[0].pin, IO_I2C );
  io_manage_set_default_mode( i2c2_pins[1].port, i2c2_pins[1].pin, IO_I2C );
  i2cStop(&I2CD2);
#endif
#if STM32_I2C_USE_I2C3
  io_manage_set_default_mode( i2c3_pins[0].port, i2c3_pins[0].pin, IO_I2C );
  io_manage_set_default_mode( i2c3_pins[1].port, i2c3_pins[1].pin, IO_I2C );
  i2cStop(&I2CD3);
#endif
  
  return true;
}

/*! @} */

