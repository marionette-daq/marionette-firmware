#ifndef __FETCH_MBUS_H_
#define __FETCH_MBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_mbus_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_mbus_reset(BaseSequentialStream * chp);

void fetch_mbus_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
