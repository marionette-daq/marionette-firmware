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
#include "fetch_parser.h"

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
  SPI_CONFIG_CS_IO,
};

#if 0
static fetch_command_t fetch_spi_commands[] = {
    { fetch_spi_exchange_cmd,  "exchange",  "TX/RX bytes\n" \
                                            "Usage: exchange(<dev>,<base>,<byte 0>,[...,<byte n>])" },
    { fetch_spi_config_cmd,    "config",    "Configure SPI driver\n" \
                                            "Usage: config(<dev>,<cpol>,<cpha>,<clk div>,<order>,[<ss io>])\n" \
                                            "\tcpol = 0 | 1\n" \
                                            "\tcpha = 0 | 1\n" \
                                            "\tclk div = 0 ... 7\n" \
                                            "\torder = 0 {MSB first} | 1 {LSB first}\n" },
    { fetch_spi_reset_cmd,     "reset",     "Reset SPI driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_spi_help_cmd,      "help",      "SPI command help" },
    { fetch_spi_clocks_cmd,    "clocks",    "SPI clock divider values" },
    { NULL, NULL, NULL } // null terminate list
  };
#endif

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
    case 2:
      return &SPID2;
    case 6:
      return &SPID6;
    default:
      return NULL;
  }
}

bool fetch_spi_clocks_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);
  
  util_message_uint32(chp, "SPI2_PCLK", STM32_PCLK1);
  util_message_uint32(chp, "SPI2_DIV_0", STM32_PCLK1 / 2);
  util_message_uint32(chp, "SPI2_DIV_1", STM32_PCLK1 / 4);
  util_message_uint32(chp, "SPI2_DIV_2", STM32_PCLK1 / 8);
  util_message_uint32(chp, "SPI2_DIV_3", STM32_PCLK1 / 16);
  util_message_uint32(chp, "SPI2_DIV_4", STM32_PCLK1 / 32);
  util_message_uint32(chp, "SPI2_DIV_5", STM32_PCLK1 / 64);
  util_message_uint32(chp, "SPI2_DIV_6", STM32_PCLK1 / 128);
  util_message_uint32(chp, "SPI2_DIV_7", STM32_PCLK1 / 256);

  util_message_uint32(chp, "SPI6_PCLK", STM32_PCLK2);
  util_message_uint32(chp, "SPI6_DIV_0", STM32_PCLK2 / 2);
  util_message_uint32(chp, "SPI6_DIV_1", STM32_PCLK2 / 4);
  util_message_uint32(chp, "SPI6_DIV_2", STM32_PCLK2 / 8);
  util_message_uint32(chp, "SPI6_DIV_3", STM32_PCLK2 / 16);
  util_message_uint32(chp, "SPI6_DIV_4", STM32_PCLK2 / 32);
  util_message_uint32(chp, "SPI6_DIV_5", STM32_PCLK2 / 64);
  util_message_uint32(chp, "SPI6_DIV_6", STM32_PCLK2 / 128);
  util_message_uint32(chp, "SPI6_DIV_7", STM32_PCLK2 / 256);

  return true;
}

bool fetch_spi_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 6);
  FETCH_MIN_ARGS(chp, argc, 6);

  char * endptr;
  int32_t spi_dev;
  SPIDriver * spi_drv;
  SPIConfig * spi_cfg;


  if( (spi_drv = parse_spi_dev(argv[0], &spi_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  switch(spi_dev)
  {
    case 2:
      palSetPadMode(GPIOI, GPIOI_PI1_SPI2_SCK, PAL_MODE_ALTERNATE(5));
      palSetPadMode(GPIOI, GPIOI_PI2_SPI2_MISO, PAL_MODE_ALTERNATE(5));
      palSetPadMode(GPIOI, GPIOI_PI3_SPI2_MOSI, PAL_MODE_ALTERNATE(5));
      break;
    case 6:
      palSetPadMode(GPIOG, GPIOG_PG13_SPI6_SCK, PAL_MODE_ALTERNATE(5));
      palSetPadMode(GPIOG, GPIOG_PG12_SPI6_MISO, PAL_MODE_ALTERNATE(5));
      palSetPadMode(GPIOG, GPIOG_PG14_SPI6_MOSI, PAL_MODE_ALTERNATE(5));
      break;
  }

  spi_cfg = &spi_configs[spi_dev-1];

  spi_cfg->end_cb = NULL;
  spi_cfg->ssport = NULL;
  spi_cfg->sspad = 0;
  spi_cfg->cr1 = 0;

  int32_t spi_cpol = strtol(argv[SPI_CONFIG_CPOL],&endptr,0);

  if( *endptr != '\0' || spi_cpol > 1 || spi_cpol < 0 ) {
    util_message_error(chp, "invalid CPOL value");
    return false;
  } else if( spi_cpol == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_CPOL;
  }
  
  int32_t spi_cpha = strtol(argv[SPI_CONFIG_CPHA],&endptr,0);

  if( *endptr != '\0' || spi_cpha > 1 || spi_cpha < 0 ) {
    util_message_error(chp, "invalid CPHA value");
    return false;
  } else if( spi_cpha == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_CPHA;
  }

  int32_t spi_msb_lsb = strtol(argv[SPI_CONFIG_MSB_LSB],&endptr,0);
  
  if( *endptr != '\0' || spi_msb_lsb > 1 || spi_msb_lsb < 0 ) {
    util_message_error(chp, "invalid MSB/LSB value");
    return false;
  } else if( spi_msb_lsb == 1 ) {
    spi_cfg->cr1 |= SPI_CR1_LSBFIRST;
  }

  int32_t spi_clk_div = strtol(argv[SPI_CONFIG_CLK_DIV],&endptr,0);

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

  if( argv[SPI_CONFIG_CS_IO] != NULL )
  {
    port_pin_t pp;

    if( !fetch_gpio_parser(argv[0], FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid chip select io pin");
      return false;
    }

    spi_cfg->ssport = pp.port;
    spi_cfg->sspad  = pp.pin;
  }

  // apply configuration
  spiStart(spi_drv, spi_cfg);

  return true;
}

bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, MAX_SPI_BYTES + 2);
  FETCH_MIN_ARGS(chp, argc, 2);

  static uint8_t tx_buffer[MAX_SPI_BYTES];
  static uint8_t rx_buffer[MAX_SPI_BYTES];
  uint32_t byte_count = 0;
  int number_base = 0;
  char * endptr;
  int byte_value;
  int32_t spi_dev;
  SPIDriver * spi_drv;
  SPIConfig * spi_cfg;

  if( (spi_drv = parse_spi_dev(argv[0], &spi_dev)) == NULL )
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

  number_base = strtol(argv[1], &endptr, 0);

  if( *endptr != '\0' || number_base == 1 || number_base < 0 || number_base > 36 )
  {
    util_message_error(chp, "invalid number base");
    return false;
  }

  for( int i = 0; i < MAX_SPI_BYTES && argv[i+2] != NULL; i++ )
  {
    byte_value = strtol(argv[i+2], &endptr, number_base);
    
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

  util_message_uint32(chp, "count", byte_count);
  util_message_hex_uint8_array( chp, "rx", rx_buffer, byte_count);

  return true;
}

bool fetch_spi_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  int32_t spi_dev;
  SPIDriver * spi_drv;
  
  if( (spi_drv = parse_spi_dev(argv[0], &spi_dev)) == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  spiStop(spi_drv);

  return true;
}

bool fetch_spi_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  // FIXME output help text
	return true;
}

void fetch_spi_init(void)
{
  // put spi initialization stuff here
  // ...
}

bool fetch_spi_reset(BaseSequentialStream * chp)
{
  spiStop(&SPID2);
  spiStop(&SPID6);
  // FIXME change this to only modify the pin mode if its in the spi alternate setting
  palSetPadMode(GPIOI, GPIOI_PI1_SPI2_SCK,   PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(GPIOI, GPIOI_PI2_SPI2_MISO,  PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(GPIOI, GPIOI_PI3_SPI2_MOSI,  PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(GPIOG, GPIOG_PG13_SPI6_SCK,  PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(GPIOG, GPIOG_PG12_SPI6_MISO, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  palSetPadMode(GPIOG, GPIOG_PG14_SPI6_MOSI, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  return true;
}

/*! @} */

