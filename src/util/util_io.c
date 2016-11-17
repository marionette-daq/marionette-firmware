#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "util_general.h"
#include "util_io.h"


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



