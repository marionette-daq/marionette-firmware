/*! \file fetch_defs.h
 * Definitions for fetch grammar
 * @addtogroup fetch
 * @{
 */


#ifndef FETCH_DEFS_H_
#define FETCH_DEFS_H_

#include "hal.h"

#define 		FETCH_MAX_LINE_CHARS					1024
#define 		FETCH_MAX_DATA_TOKS 					260
#define     FETCH_MAX_DATA_STRLEN         FETCH_MAX_LINE_CHARS
#define     FETCH_SHARED_BUFFER_SIZE      1024
#define     FETCH_MAX_SPI_BYTES           256

#define     FETCH_DEFAULT_PIN_MODE        (PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING)

#if FETCH_SHARED_BUFFER_SIZE < FETCH_MAX_LINE_CHARS
#error FETCH_SHARED_BUFFER_SIZE should be >= FETCH_MAX_LINE_CHARS
#endif

#if FETCH_SHARED_BUFFER_SIZE < (FETCH_MAX_SPI_BYTES * 2)
#error FETCH_SHARED_BUFFER_SIZE should be >= (FETCH_MAX_SPI_BYTES * 2), needed for SPI tx/rx buffer
#endif

typedef bool (*fetch_func_t)(BaseSequentialStream *chp, uint32_t argc, char * argv[]);

#endif
/*! @} */
