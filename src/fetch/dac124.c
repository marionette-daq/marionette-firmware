/*! \file dac124.c
 * Marionette dac124 routines
 * @defgroup dac124 Fetch external DAC
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
#include "fetch_spi.h"
#include "dac124.h"

#ifndef MAX_SPI_BYTES
#define MAX_SPI_BYTES   256
#endif

static SPIConfig  spi_configs[] =   {{NULL, NULL, 0, 0}};

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

bool dac124_config_cmd(void)
{
  SPIDriver * spi_drv = &SPID2;
  SPIConfig * spi_cfg;
  
  spi_cfg = &spi_configs[0];

  spi_cfg->end_cb = NULL;
  spi_cfg->ssport = NULL;
  spi_cfg->sspad = 0;
  spi_cfg->cr1 = 0;

  spi_cfg->cr1 |= SPI_CR1_CPHA;

  spi_cfg->cr1 |= SPI_CR1_BR_2; //Maximum clock speed needs to tested
  spi_cfg->ssport = GPIOI; //Hard coded for now
  spi_cfg->sspad = GPIOI_PIN0;
  //Verify that this is correct
  if(spi_cfg->ssport !=NULL && !io_manage_set_mode(spi_cfg->ssport, spi_cfg->sspad, PAL_MODE_OUTPUT_PUSHPULL,IO_GPIO))
  {
    return false;
  }
      if( !io_manage_set_mode( spi2_pins[0].port, spi2_pins[0].pin, PAL_MODE_ALTERNATE(5), IO_SPI) ||
          !io_manage_set_mode( spi2_pins[1].port, spi2_pins[1].pin, PAL_MODE_ALTERNATE(5), IO_SPI) ||
          !io_manage_set_mode( spi2_pins[2].port, spi2_pins[2].pin, PAL_MODE_ALTERNATE(5), IO_SPI) )
      {
        if( spi_cfg->ssport != NULL )
        {
          io_manage_set_default_mode( spi_cfg->ssport, spi_cfg->sspad, IO_GPIO );
        }
        io_manage_set_default_mode( spi2_pins[0].port, spi2_pins[0].pin, IO_SPI );
        io_manage_set_default_mode( spi2_pins[1].port, spi2_pins[1].pin, IO_SPI );
        io_manage_set_default_mode( spi2_pins[2].port, spi2_pins[2].pin, IO_SPI );
        return false;
      }
  //Set up spi frame to use 16bit instead of 8bit
  spi_cfg->cr1 |=SPI_CR1_DFF;
  // apply configuration
  spiStart(spi_drv, spi_cfg);

  return true;
}

bool dac124_write_cmd(int dac_channel,int dac_output_type, uint32_t tx_data)
{
  static uint16_t tx_buffer[MAX_SPI_BYTES]; //Changed to use 16 bit frame
  uint32_t byte_count = 3;
  int number_base = 10;
  char * endptr;
  int byte_value;
  int32_t spi_dev;
  SPIDriver * spi_drv;
  SPIConfig * spi_cfg;
  palSetPadMode(GPIOD, GPIOD_PIN11, PAL_MODE_OUTPUT_PUSHPULL);
 
 tx_buffer[0] = dac_channel << 14 | dac_output_type << 12 | tx_data;
  spi_drv = &SPID2;
  spi_cfg = &spi_configs[0];

  if( spi_drv->state != SPI_READY )
  {
palTogglePad(GPIOD, GPIOD_PIN11); 
	  
    return false;
  }

  if( spi_cfg->ssport != NULL )
  {
    spiSelect(spi_drv);
 
  }

  spiSend(spi_drv, byte_count, tx_buffer);
  if( spi_cfg->ssport != NULL )
  {
    spiUnselect(spi_drv);
  }
   return true;
}

bool dac124_reset_cmd(void)
{

static uint16_t tx_buffer[MAX_SPI_BYTES];
uint32_t byte_count = 3;
SPIConfig * spi_cfg = &spi_configs[0];
SPIDriver * spi_drv = &SPID2;
spiStart(spi_drv, spi_cfg);
tx_buffer[0] = 0x3FFF;
if( spi_cfg->ssport != NULL )
  {
    spiSelect(spi_drv);
  }
else
  {
    return false;
  }
spiSend(spi_drv, byte_count, tx_buffer);

if( spi_cfg->ssport != NULL )
  {
    spiUnselect(spi_drv);
  }


#if STM32_SPI_USE_SPI3
if( spi_configs[1].ssport != NULL )
{
  io_manage_set_default_mode( spi_configs[1].ssport, spi_configs[1].sspad, IO_GPIO );
}
io_manage_set_default_mode( spi2_pins[0].port, spi2_pins[0].pin, IO_SPI );
io_manage_set_default_mode( spi2_pins[1].port, spi2_pins[1].pin, IO_SPI );
io_manage_set_default_mode( spi2_pins[2].port, spi2_pins[2].pin, IO_SPI );
spiStop(&SPID2);
#endif

  return true;
}



/*! @} */

