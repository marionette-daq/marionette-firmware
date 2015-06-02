/*! \file fetch_can.h
 *
 * @addtogroup fetch_can
 * @{
 */

#ifndef FETCH_CAN_H_
#define FETCH_CAN_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_can_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_can_reset(BaseSequentialStream * chp);

void fetch_can_init(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif


#endif

/*! @} */
