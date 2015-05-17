/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_SPI_H_
#define FETCH_FETCH_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_dac124_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_dac124_reset(BaseSequentialStream * chp);

void fetch_dac124_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
