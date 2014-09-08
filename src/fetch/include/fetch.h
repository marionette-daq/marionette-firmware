/*! \file fetch.h
 * Support implementation of Fetch Language
 * \addtogroup fetch
 * @{
 */


#ifndef FETCH_H_
#define FETCH_H_

#include "fetch_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

void fetch_init(BaseSequentialStream * chp);
bool fetch_parse(BaseSequentialStream* chp, char * inputline);
bool fetch_dispatch(BaseSequentialStream* chp, fetch_command_t cmd_fn[], char * command, char * command_list[], char * data_list[]);
void fetch_display_help(BaseSequentialStream * chp, fetch_command_t cmd_fn[]);
int  fetch_find_command(fetch_command_t cmd_fn[], char * command);
bool fetch_input_check( BaseSequentialStream * chp, char * cmd_list[], uint32_t max_cmd, char * data_list[], uint32_t max_data );

#ifdef __cplusplus
}
#endif

#endif
/*! @} */
