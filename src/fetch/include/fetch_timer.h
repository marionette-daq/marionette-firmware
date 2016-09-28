#ifndef __FETCH_TIMER_H_
#define __FETCH_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_timer_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_timer_reset(BaseSequentialStream * chp);

void fetch_timer_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
