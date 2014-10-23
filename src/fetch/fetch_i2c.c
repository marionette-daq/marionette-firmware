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

#include "chprintf.h"
#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"

#include "io_manage.h"

#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_i2c.h"

#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT MS2ST(100)
#endif

static I2CDriver * i2c_drv = NULL;
static I2CConfig   i2c_cfg = {OPMODE_I2C, 100000, STD_DUTY_CYCLE};
static bool i2c_init_flag = true;

static const char * i2c_dev_tok[]     = {"I2C_1","I2C_2","I2C_3"};

// list all command function prototypes here 
static bool fetch_i2c_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_transmit_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_receive_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_i2c_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char i2c_config_help_string[] = "Configure I2C driver\n" \
    "Usage: config(<dev>)\n" \
    "\tdev = I2C_1 | I2C_2 | I2C_3";

static fetch_command_t fetch_i2c_commands[] = {
    { fetch_i2c_transmit_cmd,  "transmit",  "TX data to slave\nUsage: transmit(<addr>,[hex],<byte n>,...,<byte n>)" },
    { fetch_i2c_receive_cmd,  "receive",   "RX data from slave\nUsage: receive(<addr>,<count>)" },
    { fetch_i2c_config_cmd,    "config",    i2c_config_help_string},
    { fetch_i2c_reset_cmd,     "reset",     "Reset I2C driver" },
    { fetch_i2c_help_cmd,      "help",      "I2C command help" },
    { NULL, NULL, NULL } // null terminate list
  };

static bool fetch_i2c_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  if( i2c_drv != NULL )
  {
    util_message_error(chp, "I2C already configured");
    util_message_info(chp, "use i2c.reset");
    return false;
  }
  
  i2c_cfg.op_mode = OPMODE_I2C;
  i2c_cfg.clock_speed = 100000;
  i2c_cfg.duty_cycle = STD_DUTY_CYCLE;

  switch( token_match(  data_list[0], FETCH_MAX_DATA_STRLEN,
                        i2c_dev_tok, NELEMS(i2c_dev_tok)) )
  {
#if STM32_I2C_USE_I2C1
    case 0:
      i2c_drv = &I2CD1;
      break;
#endif
#if STM32_I2C_USE_I2C2
    case 1:
      i2c_drv = &I2CD2;
      break;
#endif
#if STM32_I2C_USE_I2C3
    case 2:
      i2c_drv = &I2CD3;
      break;
#endif
    default:
      util_message_error(chp, "invalid i2c device");
      return false;
  }

#if STM32_I2C_USE_I2C1
  if( i2c_drv == &I2CD1 )
  {
    if( !io_manage_set_mode( GPIOB, PIN6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTB:PIN6");
      i2c_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOB, PIN7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTB:PIN7");
      io_manage_set_default_mode( GPIOB, PIN6 );
      i2c_drv = NULL;
      return false;
    }
  }
#endif
#if STM32_I2C_USE_I2C2
  if( i2c_drv == &I2CD2 )
  {
    if( !io_manage_set_mode( GPIOB, PIN10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTB:PIN10");
      i2c_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOB, PIN11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTB:PIN11");
      io_manage_set_default_mode( GPIOB, PIN10 );
      i2c_drv = NULL;
      return false;
    }
  }
#endif
#if STM32_I2C_USE_I2C3
  if( i2c_drv == &I2CD3 )
  {
    if( !io_manage_set_mode( GPIOH, PIN7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTH:PIN7");
      i2c_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOH, PIN8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUDR_PULLUP, IO_I2C) )
    {
      util_message_error(chp, "unable to allocate PORTH:PIN8");
      io_manage_set_default_mode( GPIOH, PIN7 );
      i2c_drv = NULL;
      return false;
    }
  }
#endif
  
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
  int offset;
  i2caddr_t address;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, MAX_I2C_BYTES + 2) )
  {
    return false;
  }

  if( i2c_drv == NULL || i2c_drv->state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  address = strtol(data_list[0], &endptr, 0);

  if( *endptr != '\0' || address > 127 )
  {
    util_message_error(chp, "invalid address byte");
    return false;
  }

  if( strcasecmp("HEX", data_list[1]) == 0 )
  {
    number_base = 16;
    offset = 2;
  }
  else
  {
    number_base = 0;
    offset = 1;
  }

  for( int i = 0; i < MAX_I2C_BYTES && data_list[i+offset] != NULL; i++ )
  {
    byte_value = strtol(data_list[i+offset], &endptr, number_base);
    
    if( *endptr != '\0' )
    {
      util_message_error(chp, "invalid data argument");
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

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( i2c_drv == NULL || i2c_drv->state != I2C_READY )
  {
    util_message_error(chp, "I2C not ready");
    return false;
  }

  address = strtol(data_list[0], &endptr, 0);

  if( *endptr != '\0' || address > 127 )
  {
    util_message_error(chp, "invalid address byte");
    return false;
  }
  
  byte_count = strtol(data_list[1], &endptr, 0);

  if( *endptr != '\0' || byte_count > MAX_I2C_BYTES )
  {
    util_message_error(chp, "invalid byte count");
    return false;
  }

  switch( i2cMasterReceiveTimeout(i2c_drv, address, rx_buffer, byte_count, I2C_TIMEOUT) )
  {
    case RDY_TIMEOUT:
      util_message_error(chp, "i2c timeout");
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
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( i2c_drv == NULL )
  {
    return true;
  }

#if STM32_I2C_USE_I2C1
  if( i2c_drv == &I2CD1 )
  {
    io_manage_set_default_mode( GPIOB, PIN6 );
    io_manage_set_default_mode( GPIOB, PIN7 );
  }
#endif
#if STM32_I2C_USE_I2C2
  if( i2c_drv == &I2CD2 )
  {
    io_manage_set_default_mode( GPIOB, PIN10 );
    io_manage_set_default_mode( GPIOB, PIN11 );
  }
#endif
#if STM32_I2C_USE_I2C3
  if( i2c_drv == &I2CD3 )
  {
    io_manage_set_default_mode( GPIOH, PIN7 );
    io_manage_set_default_mode( GPIOH, PIN8 );
  }
#endif
  
  i2cStop(i2c_drv);
  i2c_drv = NULL;

  return true;
}

static bool fetch_i2c_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Fetch I2C Help:");
  fetch_display_help(chp, fetch_i2c_commands);
	return true;
}

static void fetch_i2c_init(void)
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
    fetch_i2c_init();
  }
  return fetch_dispatch(chp, fetch_i2c_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}


bool fetch_i2c_reset(BaseSequentialStream * chp)
{
  // TODO reset i2c
  
  return true;
}

/*! @} */

