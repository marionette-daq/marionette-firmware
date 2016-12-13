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

#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT   MS2ST(100)
#endif

#ifndef I2C_DRV
#define I2C_DRV       I2CD2
#endif

static I2CConfig  i2c_cfg = { OPMODE_I2C, 100000, STD_DUTY_CYCLE }; // standard 100khz mode


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
  set_alternate_mode(GPIOF, GPIOF_PF0_I2C2_SDA);
  set_alternate_mode(GPIOF, GPIOF_PF1_I2C2_SCL);

  // make sure i2c is reset
  i2cStop(&I2C_DRV);

  // apply configuration
  i2cStart(&I2C_DRV, &i2c_cfg);

  return true;
}

bool fetch_i2c_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 2);

  uint32_t byte_count = 0;
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

  if( !fetch_parse_bytes(chp, argc-1, &argv[1], fetch_shared_buffer, sizeof(fetch_shared_buffer), &byte_count) )
  {
    util_message_error(chp, "fetch_parse_bytes failed");
    return false;
  }

  util_message_info(chp, "byte_count %U", byte_count);

  switch( i2cMasterTransmitTimeout(&I2C_DRV, address, fetch_shared_buffer, byte_count, NULL, 0, I2C_TIMEOUT) )
  {
    case MSG_TIMEOUT:
      util_message_error(chp, "TIMEOUT");
      fetch_print_i2c_error(chp);
      i2cStop(&I2C_DRV); // a simple start again doesnt seem to do it
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

  uint32_t byte_count;
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
  
  if( !util_parse_uint32(argv[1], &byte_count) || byte_count > FETCH_SHARED_BUFFER_SIZE )
  {
    util_message_error(chp, "invalid byte count");
    return false;
  }

  util_message_info(chp, "byte_count %U", byte_count);

  switch( i2cMasterReceiveTimeout(&I2C_DRV, address, fetch_shared_buffer, byte_count, I2C_TIMEOUT) )
  {
    case MSG_TIMEOUT:
      util_message_error(chp, "TIMEOUT");
      fetch_print_i2c_error(chp);
      i2cStop(&I2C_DRV); // a simple start again doesnt seem to do it
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
  util_message_hex_uint8_array( chp, "rx", fetch_shared_buffer, byte_count);

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

  FETCH_HELP_BREAK(chp);
  FETCH_HELP_LEGEND(chp);
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_TITLE(chp, "I2C Help");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"write(<addr>,<data 0>[,<data 1>...])");
  FETCH_HELP_DES(chp,"Write data to slave");
  FETCH_HELP_ARG(chp,"addr","7 bit address, no r/w bit");
  FETCH_HELP_ARG(chp,"data","list of bytes");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read(<addr>,<count>)");
  FETCH_HELP_DES(chp,"Read data from slave");
  FETCH_HELP_ARG(chp,"addr","7 bit address, no r/w bit");
  FETCH_HELP_ARG(chp,"count","number of bytes to receive");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"config");
  FETCH_HELP_DES(chp,"Configure I2C module");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"reset");
  FETCH_HELP_DES(chp,"Reset I2C module");
  FETCH_HELP_BREAK(chp);

  return true;
}

void fetch_i2c_init(void)
{
  // TODO do we need anything here?
}

bool fetch_i2c_reset(BaseSequentialStream * chp)
{
  i2cStop(&I2C_DRV);

  reset_alternate_mode(GPIOF, GPIOF_PF0_I2C2_SDA);
  reset_alternate_mode(GPIOF, GPIOF_PF1_I2C2_SCL);

  return true;
}

/*! @} */

