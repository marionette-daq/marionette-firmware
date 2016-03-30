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

#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_spi.h"

#ifndef MAX_SPI_BYTES
#define MAX_SPI_BYTES   256
#endif

static SPIConfig  spi_configs[6] =  { {NULL, NULL, 0, 0},
                                      {NULL, NULL, 0, 0},
                                      {NULL, NULL, 0, 0},
                                      {NULL, NULL, 0, 0},
                                      {NULL, NULL, 0, 0},
                                      {NULL, NULL, 0, 0} };

enum {
  SPI_CONFIG_DEV = 0,
  SPI_CONFIG_CPOL,
  SPI_CONFIG_CPHA,
  SPI_CONFIG_CLK_DIV,
  SPI_CONFIG_MSB_LSB,
  SPI_CONFIG_CS_PORT,
  SPI_CONFIG_CS_PIN
};

// list all command function prototypes here 
static bool fetch_spi_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_spi_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);


static fetch_command_t fetch_spi_commands[] = {
    { fetch_spi_exchange_cmd,  "exchange",  "TX/RX bytes\n" \
                                            "Usage: exchange(<dev>,<base>,<byte 0>,[...,<byte n>])" },
    { fetch_spi_config_cmd,    "config",    "Configure SPI driver\n" \
                                            "Usage: config(<dev>,<cpol>,<cpha>,<clk div>,<order>,[<ss port>, <ss pin>])\n" \
                                            "\tcpol = 0 | 1\n" \
                                            "\tcpha = 0 | 1\n" \
                                            "\tclk div = 0 ... 7\n" \
                                            "\torder = 0 {MSB first} | 1 {LSB first}\n" \
                                            "\tss port = PORTA ... PORTI {optional}\n" \
                                            "\tss pin = 0 ... 15 {required if port specified}\n" },
    { fetch_spi_reset_cmd,     "reset",     "Reset SPI driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_spi_help_cmd,      "help",      "SPI command help" },
    { NULL, NULL, NULL } // null terminate list
  };

static SPIDriver * parse_spi_dev( char * str, int32_t * dev )
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
#if STM32_SPI_USE_SPI2
    case 2:
      return &SPID2;
#endif
#if STM32_SPI_USE_SPI6
    case 6:
      return &SPID6;
#endif
    default:
      return NULL;
  }
}

static bool fetch_spi_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;
  int32_t spi_dev;
  SPIDriver * spi_drv;
  SPIConfig * spi_cfg;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 7) )
  {
    return false;
  }

  if( (spi_drv = parse_spi_dev(data_list[0], &spi_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  spi_cfg = &spi_configs[spi_dev-1];

  spi_cfg->end_cb = NULL;
  spi_cfg->ssport = NULL;
  spi_cfg->sspad = 0;
  spi_cfg->cr1 = 0;

  int32_t spi_cpol = strtol(data_list[SPI_CONFIG_CPOL],&endptr,0);

  if( *endptr != '\0' || spi_cpol > 1 || spi_cpol < 0 ) {
    util_message_error(chp, "invalid CPOL value");
    return false;
  } else if( spi_cpol == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_CPOL;
  }
  
  int32_t spi_cpha = strtol(data_list[SPI_CONFIG_CPHA],&endptr,0);

  if( *endptr != '\0' || spi_cpha > 1 || spi_cpha < 0 ) {
    util_message_error(chp, "invalid CPHA value");
    return false;
  } else if( spi_cpha == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_CPHA;
  }

  int32_t spi_msb_lsb = strtol(data_list[SPI_CONFIG_MSB_LSB],&endptr,0);
  
  if( *endptr != '\0' || spi_msb_lsb > 1 || spi_msb_lsb < 0 ) {
    util_message_error(chp, "invalid MSB/LSB value");
    return false;
  } else if( spi_msb_lsb == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_LSBFIRST;
  }

  int32_t spi_clk_div = strtol(data_list[SPI_CONFIG_CLK_DIV],&endptr,0);

  if( *endptr != '\0' || spi_clk_div < 0 || spi_clk_div > 7 )
  {
    util_message_error(chp, "invalid clock divisor");
    return false;
  }

  switch( spi_clk_div )
  {
    case 0:
      break;
    case 1:
      spi_cfg->cr1 |= SPI_CR1_BR_0;
      break;
    case 2:
      spi_cfg->cr1 |= SPI_CR1_BR_1;
      break;
    case 3:
      spi_cfg->cr1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
    case 4:
      spi_cfg->cr1 |= SPI_CR1_BR_2;
      break;
    case 5:
      spi_cfg->cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
      break;
    case 6:
      spi_cfg->cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
      break;
    case 7:
      spi_cfg->cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
  }

  if( data_list[SPI_CONFIG_CS_PORT] != NULL )
  {
    spi_cfg->ssport = string_to_port(data_list[SPI_CONFIG_CS_PORT]);
    spi_cfg->sspad  = string_to_pin(data_list[SPI_CONFIG_CS_PIN]);

    if( (spi_cfg->ssport == NULL) || (spi_cfg->sspad == INVALID_PIN) )
    {
      util_message_error(chp, "invalid chip select port/pin");
      return false;
    }
  }

  // apply configuration
  spiStart(spi_drv, spi_cfg);

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
  int32_t spi_dev;
  SPIDriver * spi_drv;
  SPIConfig * spi_cfg;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, MAX_SPI_BYTES + 1) )
  {
    return false;
  }
  
  if( (spi_drv = parse_spi_dev(data_list[0], &spi_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  spi_cfg = &spi_configs[spi_dev-1];

  if( spi_drv->state != SPI_READY )
  {
    util_message_error(chp, "SPI not ready");
    return false;
  }

  number_base = strtol(data_list[1], &endptr, 0);

  if( *endptr != '\0' || number_base == 1 || number_base < 0 || number_base > 36 )
  {
    util_message_error(chp, "invalid number base");
    return false;
  }

  for( int i = 0; i < MAX_SPI_BYTES && data_list[i+2] != NULL; i++ )
  {
    byte_value = strtol(data_list[i+2], &endptr, number_base);
    
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

  if( spi_cfg->ssport != NULL )
  {
    spiSelect(spi_drv);
  }

  spiExchange(spi_drv, byte_count, tx_buffer, rx_buffer);

  if( spi_cfg->ssport != NULL )
  {
    spiUnselect(spi_drv);
  }

  util_message_uint32(chp, "count", &byte_count, 1);
  util_message_hex_uint8( chp, "rx", rx_buffer, byte_count);

  return true;
}

static bool fetch_spi_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  int32_t spi_dev;
  SPIDriver * spi_drv;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }
  
  if( (spi_drv = parse_spi_dev(data_list[0], &spi_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  spiStop(spi_drv);

  return true;
}

static bool fetch_spi_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");
  
  util_message_info(chp, "dev = 2 | 6");
  util_message_info(chp, "base = (reference strtol c function)");

  fetch_display_help(chp, fetch_spi_commands);
	return true;
}

void fetch_spi_init(BaseSequentialStream * chp)
{
  static bool spi_init_flag = false;

  if( spi_init_flag )
    return;

  // put spi initialization stuff here
  // ...

  spi_init_flag = true;
}

/*! \brief dispatch a specific spi command
 */
bool fetch_spi_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_spi_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}


bool fetch_spi_reset(BaseSequentialStream * chp)
{
#if STM32_SPI_USE_SPI2
  spiStop(&SPID2);
#endif
#if STM32_SPI_USE_SPI6
  spiStop(&SPID6);
#endif
  return true;
}

/*! @} */

