#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "util_general.h"
#include "util_io.h"

// null terminated list of alternate modes for GPIO pins
static const alt_pin_mode_t alternate_modes[] = {
  {GPIOB, GPIOB_PB8_TIM4_CH3,       PAL_MODE_ALTERNATE(2) },
  {GPIOB, GPIOB_PB9_TIM4_CH4,       PAL_MODE_ALTERNATE(2) },
  {GPIOE, GPIOE_PE5_TIM9_CH1,       PAL_MODE_ALTERNATE(3) },
  {GPIOE, GPIOE_PE6_TIM9_CH2,       PAL_MODE_ALTERNATE(3) },
  {GPIOE, GPIOE_PE9_TIM1_CH1,       PAL_MODE_ALTERNATE(1) },
  {GPIOE, GPIOE_PE13_TIM1_CH3,      PAL_MODE_ALTERNATE(1) },
  {GPIOH, GPIOH_PH10_TIM5_CH1,      PAL_MODE_ALTERNATE(2) },
  {GPIOH, GPIOH_PH11_TIM5_CH2,      PAL_MODE_ALTERNATE(2) },
  {GPIOH, GPIOH_PH12_TIM5_CH3,      PAL_MODE_ALTERNATE(2) },
  {GPIOA, GPIOA_PA15_TIM2_CH1_ETR,  PAL_MODE_ALTERNATE(1) },
  {GPIOI, GPIOI_PI1_SPI2_SCK,       PAL_MODE_ALTERNATE(5) },
  {GPIOI, GPIOI_PI0_SPI2_NSS,       PAL_MODE_ALTERNATE(5) },
  {GPIOI, GPIOI_PI2_SPI2_MISO,      PAL_MODE_ALTERNATE(5) },
  {GPIOI, GPIOI_PI3_SPI2_MOSI,      PAL_MODE_ALTERNATE(5) },
  {GPIOG, GPIOG_PG13_SPI6_SCK,      PAL_MODE_ALTERNATE(5) },
  {GPIOG, GPIOG_PG8_SPI6_NSS,       PAL_MODE_ALTERNATE(5) },
  {GPIOG, GPIOG_PG12_SPI6_MISO,     PAL_MODE_ALTERNATE(5) },
  {GPIOG, GPIOG_PG14_SPI6_MOSI,     PAL_MODE_ALTERNATE(5) },
  {GPIOF, GPIOF_PF0_I2C2_SDA,       PAL_MODE_ALTERNATE(4) },
  {GPIOF, GPIOF_PF1_I2C2_SCL,       PAL_MODE_ALTERNATE(4) },
  {GPIOA, GPIOA_PA0_UART4_TX,       PAL_MODE_ALTERNATE(8) },
  {GPIOA, GPIOA_PA1_UART4_RX,       PAL_MODE_ALTERNATE(8) },
  {GPIOD, GPIOD_PD8_USART3_TX,      PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD9_USART3_RX,      PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD11_USART3_CTS,    PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD12_USART3_RTS,    PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD5_USART2_TX,      PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD6_USART2_RX,      PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD3_USART2_CTS,      PAL_MODE_ALTERNATE(7) },
  {GPIOD, GPIOD_PD4_USART2_RTS,      PAL_MODE_ALTERNATE(7) },

  {NULL,0,0},
};


void set_status_led(bool r, bool g, bool b)
{
  if(r) {
    palClearPad(GPIOD, GPIOD_PD13_LED_STATUS_R);
  } else {
    palSetPad(GPIOD, GPIOD_PD13_LED_STATUS_R);
  }
  if(g) {
    palClearPad(GPIOD, GPIOD_PD14_LED_STATUS_G);
  } else {
    palSetPad(GPIOD, GPIOD_PD14_LED_STATUS_G);
  }
  if(b) {
    palClearPad(GPIOD, GPIOD_PD15_LED_STATUS_B);
  } else {
    palSetPad(GPIOD, GPIOD_PD15_LED_STATUS_B);
  }
}

bool set_alternate_mode( ioportid_t port, uint32_t pin )
{
  // search for port/pin in alternate_modes, if found change pin mode
  const alt_pin_mode_t * alt_mode = alternate_modes;

  while( alt_mode->port != NULL )
  {
    if( alt_mode->port == port && alt_mode->pin == pin )
    {
      palSetPadMode(port, pin, alt_mode->mode);
      return true;
      break;
    }
    alt_mode++;
  }
  return false;
}

bool reset_alternate_mode( ioportid_t port, uint32_t pin )
{
  // search for port/pin in alternate_modes, if found change pin mode
  const alt_pin_mode_t * alt_mode = alternate_modes;

  while( alt_mode->port != NULL )
  {
    if( alt_mode->port == port && alt_mode->pin == pin )
    {
      if( (port->MODER >> (pin*2)) & PAL_STM32_MODE_ALTERNATE )
      {
        palSetPadMode(port, pin, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING );
      }
      return true;
      break;
    }
    alt_mode++;
  }
  return false;
}


