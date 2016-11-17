#ifndef __FETCH_TIMER_H_
#define __FETCH_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_timer_init(void);
bool fetch_timer_reset(BaseSequentialStream * chp);

bool fetch_timer_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_period_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_frequency_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_pwm_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_cap_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_clear_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_timer_count_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
