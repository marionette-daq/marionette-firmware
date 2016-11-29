/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_SERIAL_H_
#define FETCH_FETCH_SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_serial_init(void);
bool fetch_serial_reset(BaseSequentialStream * chp);

bool fetch_serial_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_read_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_break_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_timeout_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_status_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_serial_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
