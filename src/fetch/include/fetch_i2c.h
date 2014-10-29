/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_I2C_H_
#define FETCH_FETCH_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_i2c_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_i2c_reset(BaseSequentialStream * chp);

void fetch_i2c_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
