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
#include "util_io.h"
#include "util_arg_parse.h"

#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_i2c.h"

#ifndef MAX_I2C_BYTES
#define MAX_I2C_BYTES 256
#endif

#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT   MS2ST(100)
#endif

#ifndef I2C_DRV
#define I2C_DRV       I2CD2
#endif

#ifndef I2C_SDA_PORT
#define I2C_SDA_PORT  GPIOF
#endif

#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN   GPIOF_PF0_I2C2_SDA
#endif

#ifndef I2C_SCL_PORT
#define I2C_SCL_PORT  GPIOF
#endif

#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN   GPIOF_PF1_I2C2_SCL
#endif

static I2CConfig  i2c_cfg = { OPMODE_I2C, 100000, STD_DUTY_CYCLE }; // standard 100khz mode

#if 0
static fetch_command_t fetch_i2c_commands[] = {
    { fetch_i2c_write_cmd,  "write",    "TX data to slave\n" \
                                        "Usage: write(<addr>,<base>,<byte 0>,[...,<byte n>])" },
    { fetch_i2c_read_cmd,   "read",     "RX data from slave\n" \
                                        "Usage: read(<addr>,<count>)" },
    { fetch_i2c_config_cmd, "config",   "Configure I2C driver\n" \
                                        "Usage: config()" },
    { fetch_i2c_reset_cmd,  "reset",    "Reset I2C driver\n" \
                                        "Usage: reset()" },
    { fetch_i2c_help_cmd,   "help",     "I2C command help" },
    { NULL, NULL, NULL } // null terminate list
  };
#endif

static void fetch_print_i2c_error(BaseSequentialStream * chp)
{
  i2cflags_t errors = i2cGetErrors(&I2C_DRV);
  
  util_message_hex_uint32(chp, "error_flags", errors);

  if( errors & I2C_BUS_ERROR )
  {
    util_message_error(chp, "I2C_BUS_ERROR");
  }
  if( errors & I2C_ARBITRATION_LOST )
  {
    util_message_error(chp, "I2C_ARBITRATION_LOST");
  }
  if( errors & I2C_ACK_FAILURE )
  {
    util_message_error(chp, "I2C_ACK_FAILURE");
  }
  if( errors & I2C_OVERRUN )
  {
    util_message_error(chp, "I2C_OVERRUN");
  }
  if( errors & I2C_PEC_ERROR )
  {
    util_message_error(chp, "I2C_PEC_ERROR");
  }
  if( errors & I2C_TIMEOUT )
  {
    util_message_error(chp, "I2C_TIMEOUT");
  }
  if( errors & I2C_SMB_ALERT )
  {
    util_message_error(chp, "I2C_SMB_ALERT");
  }
}

bool fetch_i2c_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  // set gpio mode to alternate function
  palSetPadMode(I2C_SDA_PORT, I2C_SDA_PIN, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);
  palSetPadMode(I2C_SCL_PORT, I2C_SCL_PIN, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);

  // apply configuration
  i2cStart(&I2C_DRV, &i2c_cfg);

  return true;
}

bool fetch_i2c_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 3);
  FETCH_MAX_ARGS(chp, argc, MAX_I2C_BYTES + 2);

  static uint8_t tx_buffer[MAX_I2C_BYTES];
  uint32_t byte_count = 0;
  uint8_t number_base = 0;
  char * endptr;
  int byte_value;
  i2caddr_t address;
  
  if( I2C_DRV.state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  if( !util_parse_uint16(argv[0], &address) || address > 127 )
  {
    util_message_error(chp, "invalid address");
    return false;
  }

  if( !util_parse_uint8(argv[1], &number_base) || number_base == 1 || number_base > 36 )
  {
    util_message_error(chp, "invalid number base");
    return false;
  }

  for( int i = 0; i < MAX_I2C_BYTES && argv[i+2] != NULL; i++ )
  {
    byte_value = strtol(argv[i+2], &endptr, number_base);
    
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

  util_message_info(chp, "byte_count %U", byte_count);

  switch( i2cMasterTransmitTimeout(&I2C_DRV, address, tx_buffer, byte_count, NULL, 0, I2C_TIMEOUT) )
  {
    case MSG_TIMEOUT:
      util_message_error(chp, "TIMEOUT");
      fetch_print_i2c_error(chp);
      i2cStart(&I2C_DRV, &i2c_cfg);
      return false;
    case MSG_RESET:
      util_message_error(chp, "RESET");
      fetch_print_i2c_error(chp);
      i2cStart(&I2C_DRV, &i2c_cfg);
      return false;
    case MSG_OK:
      return true;
    default:
      util_message_error(chp, "unknown error");
      return false;
  }
}

bool fetch_i2c_read_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 2);

  static uint8_t rx_buffer[MAX_I2C_BYTES];
  uint32_t byte_count;
  char * endptr;
  i2caddr_t address;

  if( I2C_DRV.state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  if( !util_parse_uint16(argv[0], &address) || address > 127 )
  {
    util_message_error(chp, "invalid address");
    return false;
  }
  
  if( !util_parse_uint32(argv[1], &byte_count) || byte_count > MAX_I2C_BYTES )
  {
    util_message_error(chp, "invalid byte count");
    return false;
  }

  util_message_info(chp, "byte_count %U", byte_count);

  switch( i2cMasterReceiveTimeout(&I2C_DRV, address, rx_buffer, byte_count, I2C_TIMEOUT) )
  {
    case MSG_TIMEOUT:
      util_message_error(chp, "TIMEOUT");
      fetch_print_i2c_error(chp);
      i2cStart(&I2C_DRV, &i2c_cfg);
      return false;
    case MSG_RESET:
      util_message_error(chp, "RESET");
      fetch_print_i2c_error(chp);
      i2cStart(&I2C_DRV, &i2c_cfg);
      return false;
    case MSG_OK:
      break;
    default:
      util_message_error(chp, "unknown error");
      return false;
  }

  util_message_uint32(chp, "count", byte_count);
  util_message_hex_uint8_array( chp, "rx", rx_buffer, byte_count);

  return true;
}

bool fetch_i2c_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  fetch_i2c_reset(chp);

  return true;
}

bool fetch_i2c_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");

  util_message_info(chp, "base = (reference strtol c function)");
  util_message_info(chp, "addr = 7 bit address, no r/w bit");

  // FIXME add help output
  return true;
}

void fetch_i2c_init(void)
{
  // TODO do we need anything here?
}

bool fetch_i2c_reset(BaseSequentialStream * chp)
{
  i2cStop(&I2C_DRV);

  // reset to default gpio mode
  palSetPadMode(I2C_SDA_PORT, I2C_SDA_PIN, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(I2C_SCL_PORT, I2C_SCL_PIN, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);

  return true;
}

/*! @} */

