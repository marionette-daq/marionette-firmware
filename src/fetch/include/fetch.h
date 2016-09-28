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

#define FETCH_PARAM_CHECK(chp, cmd_list, min_data, max_data) do{ if(fetch_param_check(chp, cmd_list, min_data, max_data) == false) return false; }while(0)

void fetch_init(BaseSequentialStream * chp);
bool fetch_parse(BaseSequentialStream* chp, char * inputline);
bool fetch_dispatch(BaseSequentialStream* chp, fetch_command_t cmd_lookup_table[], char * cmd_list[], char * data_list[]);
void fetch_display_help_legend(BaseSequentialStream * chp);
void fetch_display_help(BaseSequentialStream * chp, fetch_command_t cmd_lookup_table[]);
int  fetch_find_command(fetch_command_t cmd_lookup_table[], char * command);
bool fetch_param_check( BaseSequentialStream * chp, char * cmd_list[], uint32_t min_data, uint32_t max_data );

#ifdef __cplusplus
}
#endif

#endif
/*! @} */
