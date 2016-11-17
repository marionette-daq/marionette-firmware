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


void fetch_dac_init(void);
bool fetch_dac_reset(BaseSequentialStream * chp);

bool fetch_dac_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_dac_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_dac_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif

#endif

//! @}

