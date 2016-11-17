/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_I2C_H_
#define FETCH_FETCH_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_i2c_init(void);
bool fetch_i2c_reset(BaseSequentialStream * chp);

bool fetch_i2c_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_i2c_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_i2c_read_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_i2c_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_i2c_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
