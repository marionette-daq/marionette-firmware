/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef __FETCH_SD_H_
#define __FETCH_SD_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_sd_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_sd_reset(BaseSequentialStream * chp);

void fetch_sd_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
