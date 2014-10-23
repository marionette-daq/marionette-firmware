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
#include "fetch_spi.h"

static SPIDriver * spi_drv = NULL;
static SPIConfig   spi_cfg = {NULL, NULL, 0, 0};
static bool spi_init_flag = true;

enum {
  SPI_CONFIG_DEV = 0,
  SPI_CONFIG_CPOL,
  SPI_CONFIG_CPHA,
  SPI_CONFIG_CLK_DIV,
  SPI_CONFIG_MSB_LSB,
  SPI_CONFIG_CS_PORT,
  SPI_CONFIG_CS_PIN
};

static const char * spi_dev_tok[]     = {"SPI_1","SPI_2","SPI_3"};
static const char * spi_cpol_tok[]    = {"CPOL_0", "CPOL_1"};
static const char * spi_cpha_tok[]    = {"CPHA_0", "CPHA_1"};
static const char * spi_msb_lsb_tok[] = {"MSB_FIRST", "LSB_FIRST"};
static const char * spi_clk_div_tok[] = {"CLK_DIV_0","CLK_DIV_1","CLK_DIV_2",
                                         "CLK_DIV_3","CLK_DIV_4","CLK_DIV_5",
                                         "CLK_DIV_6","CLK_DIV_7"};

// list all command function prototypes here 
static bool fetch_spi_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char spi_config_help_string[] = "Configure SPI driver\n" \
    "Usage: config(<dev>,<cpol>,<cpha>,<clk div>,<order>,<cs port>,<cs pin>)\n" \
    "\tdev = SPI_1 | SPI_2 | SPI_3\n" \
    "\tcpol = CPOL_0 | CPOL_1\n" \
    "\tcpha = CPHA_0 | CPHA_1\n" \
    "\tclk div = CLK_DIV_0...CLK_DIV_7\n" \
    "\torder = MSB_FIRST | LSB_FIRST\n" \
    "\tcs port = PORTA...PORTI or none\n" \
    "\tcs pin = 0...15 or none";

static fetch_command_t fetch_spi_commands[] = {
    { fetch_spi_exchange_cmd,  "exchange", "TX/RX bytes\nUsage: exchange([hex],<byte n>,...,<byte n>)" },
    { fetch_spi_config_cmd,    "config",    spi_config_help_string},
    { fetch_spi_reset_cmd,     "reset",    "Reset SPI driver" },
    { fetch_spi_help_cmd,      "help",     "SPI command help" },
    { NULL, NULL, NULL } // null terminate list
  };

static bool fetch_spi_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 7) )
  {
    return false;
  }

  if( spi_drv != NULL )
  {
    util_message_error(chp, "SPI already configured");
    util_message_info(chp, "use spi.reset");
    return false;
  }
  
  spi_cfg.end_cb = NULL;
  spi_cfg.ssport = NULL;
  spi_cfg.sspad = 0;
  spi_cfg.cr1 = 0;

  switch( token_match(  data_list[SPI_CONFIG_DEV], FETCH_MAX_DATA_STRLEN,
                        spi_dev_tok, NELEMS(spi_dev_tok)) )
  {
#if STM32_SPI_USE_SPI1
    case 0:
      spi_drv = &SPID1;
      break;
#endif
#if STM32_SPI_USE_SPI2
    case 1:
      spi_drv = &SPID2;
      break;
#endif
#if STM32_SPI_USE_SPI3
    case 2:
      spi_drv = &SPID3;
      break;
#endif
    default:
      util_message_error(chp, "invalid spi device");
      return false;
  }

  switch( token_match(  data_list[SPI_CONFIG_CPOL], FETCH_MAX_DATA_STRLEN,
                        spi_cpol_tok, NELEMS(spi_cpol_tok)) )
  {
    case 0:
      break;
    case 1:
      spi_cfg.cr1 |= SPI_CR1_CPOL;
      break;
    default:
      util_message_error(chp, "invalid CPOL value");
      spi_drv = NULL;
      return false;
  }

  switch( token_match(  data_list[SPI_CONFIG_CPHA], FETCH_MAX_DATA_STRLEN, 
                        spi_cpha_tok, NELEMS(spi_cpha_tok)) )
  {
    case 0:
      break;
    case 1:
      spi_cfg.cr1 |= SPI_CR1_CPHA;
      break;
    default:
      util_message_error(chp, "invalid CPHA value");
      spi_drv = NULL;
      return false;
  }
  
  switch( token_match(  data_list[SPI_CONFIG_MSB_LSB], FETCH_MAX_DATA_STRLEN, 
                        spi_msb_lsb_tok, NELEMS(spi_msb_lsb_tok)) )
  {
    case 0:
      break;
    case 1:
      spi_cfg.cr1 |= SPI_CR1_LSBFIRST;
      break;
    default:
      util_message_error(chp, "invalid CPHA value");
      spi_drv = NULL;
      return false;
  }

  switch( token_match(  data_list[SPI_CONFIG_CLK_DIV], FETCH_MAX_DATA_STRLEN, 
                        spi_clk_div_tok, NELEMS(spi_clk_div_tok)) )
  {
    case 0:
      break;
    case 1:
      spi_cfg.cr1 |= SPI_CR1_BR_0;
      break;
    case 2:
      spi_cfg.cr1 |= SPI_CR1_BR_1;
      break;
    case 3:
      spi_cfg.cr1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
    case 4:
      spi_cfg.cr1 |= SPI_CR1_BR_2;
      break;
    case 5:
      spi_cfg.cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
      break;
    case 6:
      spi_cfg.cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
      break;
    case 7:
      spi_cfg.cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
    default:
      util_message_error(chp, "invalid clock divisor");
      spi_drv = NULL;
      return false;
  }

  spi_cfg.ssport = string_to_port(data_list[SPI_CONFIG_CS_PORT]);
  spi_cfg.sspad  = string_to_pin(data_list[SPI_CONFIG_CS_PIN]);

  if( spi_cfg.ssport == NULL && strncasecmp("none", data_list[SPI_CONFIG_CS_PORT], FETCH_MAX_DATA_STRLEN) != 0 )
  {
    util_message_error(chp, "invalid chip select port");
    spi_drv = NULL;
    return false;
  }

  if( spi_cfg.sspad == INVALID_PIN && strncasecmp("none", data_list[SPI_CONFIG_CS_PIN], FETCH_MAX_DATA_STRLEN) != 0 )
  {
    util_message_error(chp, "invalid chip select pin");
    spi_drv = NULL;
    return false;
  }

  if( (spi_cfg.ssport == NULL) ^ (spi_cfg.sspad == INVALID_PIN) )
  {
    util_message_error(chp, "invalid chip select port/pin");
    spi_drv = NULL;
    return false;
  }

  // setup io pins
  if( spi_cfg.ssport != NULL &&
      !io_manage_set_mode( spi_cfg.ssport, spi_cfg.sspad, PAL_MODE_OUTPUT_PUSHPULL, IO_GPIO ) )
  {
    util_message_error(chp, "unable to allocate cs pin");
    spi_drv = NULL;
    return false;
  }

#if STM32_SPI_USE_SPI1
  if( spi_drv == &SPID1 )
  {
    if( !io_manage_set_mode( GPIOA, PIN5, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTA:PIN5");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOA, PIN6, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTA:PIN6");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOA, PIN5 );
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOA, PIN7, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTA:PIN7");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOA, PIN5 );
      io_manage_set_default_mode( GPIOA, PIN6 );
      spi_drv = NULL;
      return false;
    }
  }
#endif
#if STM32_SPI_USE_SPI2
  if( spi_drv == &SPID2 )
  {
    if( !io_manage_set_mode( GPIOI, PIN1, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTI:PIN1");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOI, PIN2, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTI:PIN2");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOI, PIN1 );
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOI, PIN3, PAL_MODE_ALTERNATE(5), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTI:PIN3");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOI, PIN1 );
      io_manage_set_default_mode( GPIOI, PIN2 );
      spi_drv = NULL;
      return false;
    }
  }
#endif
#if STM32_SPI_USE_SPI3
  if( spi_drv == &SPID3 )
  {
    if( !io_manage_set_mode( GPIOC, PIN10, PAL_MODE_ALTERNATE(6), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTC:PIN10");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOC, PIN11, PAL_MODE_ALTERNATE(6), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTC:PIN11");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOC, PIN10 );
      spi_drv = NULL;
      return false;
    }
    if( !io_manage_set_mode( GPIOC, PIN12, PAL_MODE_ALTERNATE(6), IO_SPI) )
    {
      util_message_error(chp, "unable to allocate PORTC:PIN12");
      if( spi_cfg.ssport != NULL )
      {
        io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
      }
      io_manage_set_default_mode( GPIOC, PIN10 );
      io_manage_set_default_mode( GPIOC, PIN11 );
      spi_drv = NULL;
      return false;
    }
  }
#endif
  
  // apply configuration
  spiStart(spi_drv, &spi_cfg);

  return true;
}

static bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  static uint8_t tx_buffer[MAX_SPI_BYTES];
  static uint8_t rx_buffer[MAX_SPI_BYTES];
  uint32_t byte_count = 0;
  int number_base = 0;
  char * endptr;
  int byte_value;
  int offset;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, MAX_SPI_BYTES + 1) )
  {
    return false;
  }

  if( spi_drv == NULL || spi_drv->state != SPI_READY )
  {
    util_message_error(chp, "SPI not ready");
    return false;
  }

  if( strcasecmp("HEX", data_list[0]) == 0 )
  {
    number_base = 16;
    offset = 1;
  }
  else
  {
    number_base = 0;
    offset = 0;
  }

  for( int i = 0; i < MAX_SPI_BYTES && data_list[i+offset] != NULL; i++ )
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

  if( spi_cfg.ssport != NULL )
  {
    spiSelect(spi_drv);
  }

  spiExchange(spi_drv, byte_count, tx_buffer, rx_buffer);

  if( spi_cfg.ssport != NULL )
  {
    spiUnselect(spi_drv);
  }

  util_message_uint32(chp, "count", &byte_count, 1);
  util_message_hex_uint8( chp, "rx", rx_buffer, byte_count);

  return true;
}

static bool fetch_spi_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  if( spi_drv == NULL )
  {
    return true;
  }

  if( spi_cfg.ssport != NULL )
  {
    io_manage_set_default_mode( spi_cfg.ssport, spi_cfg.sspad );
  }

#if STM32_SPI_USE_SPI1
  if( spi_drv == &SPID1 )
  {
    io_manage_set_default_mode( GPIOA, PIN5 );
    io_manage_set_default_mode( GPIOA, PIN6 );
    io_manage_set_default_mode( GPIOA, PIN7 );
  }
#endif
#if STM32_SPI_USE_SPI2
  if( spi_drv == &SPID2 )
  {
    io_manage_set_default_mode( GPIOI, PIN1 );
    io_manage_set_default_mode( GPIOI, PIN2 );
    io_manage_set_default_mode( GPIOI, PIN3 );
  }
#endif
#if STM32_SPI_USE_SPI3
  if( spi_drv == &SPID3 )
  {
    io_manage_set_default_mode( GPIOC, PIN10 );
    io_manage_set_default_mode( GPIOC, PIN11 );
    io_manage_set_default_mode( GPIOC, PIN12 );
  }
#endif
  
  spiStop(spi_drv);
  spi_drv = NULL;

  return true;
}

static bool fetch_spi_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Fetch SPI Help:");
  fetch_display_help(chp, fetch_spi_commands);
	return true;
}

static void fetch_spi_init(void)
{
  // TODO do we need anything here?
  
  spi_init_flag = false;
}

/*! \brief dispatch a specific spi command
 */
bool fetch_spi_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( spi_init_flag )
  {
    fetch_spi_init();
  }
  return fetch_dispatch(chp, fetch_spi_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}


bool fetch_spi_reset(BaseSequentialStream * chp)
{
  // TODO reset spi
  
  return true;
}

/*! @} */

