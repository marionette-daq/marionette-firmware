/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_PWM_H_
#define FETCH_FETCH_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_pwm_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_pwm_reset(BaseSequentialStream * chp);

void fetch_pwm_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
