/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_GPIO_H_
#define FETCH_FETCH_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_gpio_init(void);
bool fetch_gpio_reset( BaseSequentialStream * chp );

bool fetch_gpio_help_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_read_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_read_latch_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_read_port_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_read_port_latch_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_read_all_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_write_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_write_port_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_set_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_clear_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_config_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_info_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_reset_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_clock_out_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );
bool fetch_gpio_shift_out_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] );

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
