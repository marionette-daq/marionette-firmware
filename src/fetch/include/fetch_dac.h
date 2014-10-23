/*! \file fetch_dac.h
 *
 * @addtogroup fetch_dac
 * @{
 */

#ifndef FETCH_DAC_H_
#define FETCH_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dac.h"

bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

bool fetch_dac_reset(BaseSequentialStream * chp);

#ifdef __cplusplus
}
#endif

#endif

//! @}

