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
#include "fetch_spi.h"
#include "fetch_parser.h"

#ifndef FETCH_MAX_SPI_BYTES
#error FETCH_MAX_SPI_BYTES not defined
#endif

#define SPI_DRIVER_COUNT 2

static SPIDriver * spi_drivers[SPI_DRIVER_COUNT] = { &SPID2, &SPID6 };
static SPIConfig  spi_configs[SPI_DRIVER_COUNT];

static SPIDriver * parse_spi_dev( char * str, uint32_t * dev )
{
  uint32_t dev_id = str[0] - '0';

  if( str[1] != '\0' )
  {
    return NULL;
  }
  
  if( str[0] < '0' || str[0] >= ('0' + SPI_DRIVER_COUNT) )
  {
    return NULL;
  }

  if( dev )
  {
    *dev = dev_id;
  }

  return spi_drivers[dev_id];
}

bool fetch_spi_clock_div_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);
  
  // SPI0 = STM SPI2, which is on APB1
  util_message_uint32(chp, "SPI_0_DIV_0", STM32_PCLK1 / 2);
  util_message_uint32(chp, "SPI_0_DIV_1", STM32_PCLK1 / 4);
  util_message_uint32(chp, "SPI_0_DIV_2", STM32_PCLK1 / 8);
  util_message_uint32(chp, "SPI_0_DIV_3", STM32_PCLK1 / 16);
  util_message_uint32(chp, "SPI_0_DIV_4", STM32_PCLK1 / 32);
  util_message_uint32(chp, "SPI_0_DIV_5", STM32_PCLK1 / 64);
  util_message_uint32(chp, "SPI_0_DIV_6", STM32_PCLK1 / 128);
  util_message_uint32(chp, "SPI_0_DIV_7", STM32_PCLK1 / 256);

  // SPI1 = STM SPI6, which is on APB2
  util_message_uint32(chp, "SPI_1_DIV_0", STM32_PCLK2 / 2);
  util_message_uint32(chp, "SPI_1_DIV_1", STM32_PCLK2 / 4);
  util_message_uint32(chp, "SPI_1_DIV_2", STM32_PCLK2 / 8);
  util_message_uint32(chp, "SPI_1_DIV_3", STM32_PCLK2 / 16);
  util_message_uint32(chp, "SPI_1_DIV_4", STM32_PCLK2 / 32);
  util_message_uint32(chp, "SPI_1_DIV_5", STM32_PCLK2 / 64);
  util_message_uint32(chp, "SPI_1_DIV_6", STM32_PCLK2 / 128);
  util_message_uint32(chp, "SPI_1_DIV_7", STM32_PCLK2 / 256);

  return true;
}

bool fetch_spi_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 5);
  FETCH_MIN_ARGS(chp, argc, 5);

  uint32_t spi_dev;
  SPIDriver * spi_drv = parse_spi_dev(argv[0], &spi_dev);

  if( spi_drv == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  switch(spi_dev)
  {
    case 0:
      set_alternate_mode(GPIOI, GPIOI_PI1_SPI2_SCK);
      set_alternate_mode(GPIOI, GPIOI_PI2_SPI2_MISO);
      set_alternate_mode(GPIOI, GPIOI_PI3_SPI2_MOSI);
      break;
    case 1:
      set_alternate_mode(GPIOG, GPIOG_PG13_SPI6_SCK);
      set_alternate_mode(GPIOG, GPIOG_PG12_SPI6_MISO);
      set_alternate_mode(GPIOG, GPIOG_PG14_SPI6_MOSI);
      break;
  }

  spi_configs[spi_dev].end_cb = NULL;
  spi_configs[spi_dev].ssport = NULL;
  spi_configs[spi_dev].sspad = 0;
  spi_configs[spi_dev].cr1 = 0;

  bool spi_cpol;

  if( !util_parse_bool(argv[1], &spi_cpol) )
  {
    util_message_error(chp, "invalid CPOL value");
    return false;
  }
  else if( spi_cpol )
  {
    spi_configs[spi_dev].cr1 |= SPI_CR1_CPOL;
  }

  bool spi_cpha;

  if( !util_parse_bool(argv[2], &spi_cpha) )
  {
    util_message_error(chp, "invalid CPHA value");
    return false;
  }
  else if( spi_cpha )
  {
    spi_configs[spi_dev].cr1 |= SPI_CR1_CPHA;
  }

  bool spi_lsb;
  
  if( !util_parse_bool(argv[3], &spi_lsb) ) {
    util_message_error(chp, "invalid MSB/LSB value");
    return false;
  }
  else if( spi_lsb )
  {
    spi_configs[spi_dev].cr1 |= SPI_CR1_LSBFIRST;
  }

  uint8_t spi_clk_div;

  if( !util_parse_uint8(argv[4], &spi_clk_div) || spi_clk_div > 7 )
  {
    util_message_error(chp, "invalid clock divisor");
    return false;
  }

  switch( spi_clk_div )
  {
    case 0:
      break;
    case 1:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_0;
      break;
    case 2:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_1;
      break;
    case 3:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
    case 4:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_2;
      break;
    case 5:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
      break;
    case 6:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
      break;
    case 7:
      spi_configs[spi_dev].cr1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
      break;
  }

  spiStart(spi_drv, &spi_configs[spi_dev]);

  return true;
}

bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 4);

  uint8_t * tx_buffer = fetch_shared_buffer;
  uint8_t * rx_buffer = fetch_shared_buffer + FETCH_MAX_SPI_BYTES;
  uint32_t byte_count = 0;

  bool cs_pol;
  port_pin_t pp_cs;
  uint32_t spi_dev;
  SPIDriver * spi_drv = parse_spi_dev(argv[0], &spi_dev);

  if( spi_drv == NULL )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }

  if( spi_drv->state != SPI_READY )
  {
    util_message_error(chp, "SPI driver not ready");
    return false;
  }

  if( !fetch_gpio_parser(argv[1], FETCH_MAX_DATA_STRLEN, &pp_cs) )
  {
    if( strcasecmp(argv[1], "none") == 0 )
    {
      pp_cs.port = NULL;
    }
    else
    {
      util_message_error(chp, "invalid chip select io pin");
      return false;
    }
  }

  if( !util_parse_bool(argv[2], &cs_pol) )
  {
    util_message_error(chp, "invalid chip select polarity");
    return false;
  }

  if( !fetch_parse_bytes(chp, argc-3, &argv[3], tx_buffer, FETCH_MAX_SPI_BYTES, &byte_count) )
  {
    util_message_error(chp, "fetch_parse_bytes failed");
    return false;
  }

  if( pp_cs.port != NULL )
  {
    if( cs_pol )
    {
      palSetPad(pp_cs.port, pp_cs.pin);
    }
    else
    {
      palClearPad(pp_cs.port, pp_cs.pin);
    }
  }

  spiExchange(spi_drv, byte_count, tx_buffer, rx_buffer);

  if( pp_cs.port != NULL )
  {
    if( cs_pol )
    {
      palClearPad(pp_cs.port, pp_cs.pin);
    }
    else
    {
      palSetPad(pp_cs.port, pp_cs.pin);
    }
  }

  util_message_uint32(chp, "count", byte_count);
  util_message_hex_uint8_array( chp, "rx", rx_buffer, byte_count);

  return true;
}

bool fetch_spi_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  SPIDriver * spi_drv = parse_spi_dev(argv[0], NULL);
  
  if( spi_drv == NULL )
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

  FETCH_HELP_BREAK(chp);
  FETCH_HELP_LEGEND(chp);
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_TITLE(chp,"SPI Help");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"exchange(<dev>,<io_cs>,<cs_pol>,<data 0>[,<data 1> ...])");
  FETCH_HELP_DES(chp,"TX/RX bytes on SPI bus")
  FETCH_HELP_ARG(chp,"dev","SPI device");
  FETCH_HELP_ARG(chp,"io_cs","chip select io pin name | NONE");
  FETCH_HELP_ARG(chp,"cs_pol","chip select polarity, 0 {active low} | 1 {active high}")
  FETCH_HELP_ARG(chp,"data","list of bytes or strings");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"config(<dev>,<cpol>,<cpha>,<clock div>,<byte order>)");
  FETCH_HELP_DES(chp,"Configure SPI device");
  FETCH_HELP_ARG(chp,"dev","0 | 1");
  FETCH_HELP_ARG(chp,"cpol","0 | 1");
  FETCH_HELP_ARG(chp,"cpha","0 | 1");
  FETCH_HELP_ARG(chp,"clock div","see clockdiv command for clock divider values");
  FETCH_HELP_ARG(chp,"byte order","0 {MSB} | 1 {LSB}");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"reset");
  FETCH_HELP_DES(chp,"Reset SPI module");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"clockdiv");
  FETCH_HELP_DES(chp,"Query possible clock rates");
  FETCH_HELP_BREAK(chp);

	return true;
}

void fetch_spi_init(void)
{
  // put spi initialization stuff here
  // ...
}

bool fetch_spi_reset(BaseSequentialStream * chp)
{
  for( uint32_t i = 0; i < SPI_DRIVER_COUNT; i++ )
  {
    spiStop(spi_drivers[i]);
  }

  reset_alternate_mode(GPIOI, GPIOI_PI1_SPI2_SCK);
  reset_alternate_mode(GPIOI, GPIOI_PI2_SPI2_MISO);
  reset_alternate_mode(GPIOI, GPIOI_PI3_SPI2_MOSI);
  reset_alternate_mode(GPIOG, GPIOG_PG13_SPI6_SCK);
  reset_alternate_mode(GPIOG, GPIOG_PG12_SPI6_MISO);
  reset_alternate_mode(GPIOG, GPIOG_PG14_SPI6_MOSI);
  return true;
}

/*! @} */

