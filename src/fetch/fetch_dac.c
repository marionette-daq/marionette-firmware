/*! \file fetch_dac.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @defgroup fetch_dac Fetch DAC
  * @{
   */

/*!
 * <hr>
 * 
 *  In ChibiOS 2.6.x there is no DAC module in the HAL. It is available
 *  in the kernel_3_dev branch.
 *
 * <hr>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_dac.h"






//! @}
