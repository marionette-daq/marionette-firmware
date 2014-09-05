/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_SPI_H_
#define FETCH_FETCH_SPI_H_

#include "chprintf.h"
#include "fetch_defs.h"

#define MAX_SPI_BYTES   256

#ifdef __cplusplus
extern "C" {
#endif

void fetch_spi_init(void);
bool fetch_spi_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
