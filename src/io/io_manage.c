/*! \file io_manage.c
  *
  * Keep an accounting of i/o pin configurations
  *
  * \sa fetch.c
  * @defgroup io_manage IO Management
  * @{
   */

/*!
 * <hr>
 * 
 * Track current state of IO pins and try to prevent conflict settings.
 *
 * Structures referenced here will have to be defined as extern in header file to be visible.
 *
 * Open question: Will mutexes be needed for write access? Probably?
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

#include "io_manage.h"






//! @}
