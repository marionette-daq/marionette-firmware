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

#define FETCH_MIN_ARGS(chp, argc, arg_min) do{ \
          if( argc < (arg_min) ) { util_message_error(chp, "Missing arguments"); return false; } \
        }while(0);

#define FETCH_MAX_ARGS(chp, argc, arg_max) do{ \
          if( argc > (arg_max) ) { util_message_error(chp, "Extra arguments"); return false; } \
        }while(0);

#define FETCH_MOD_ARGS(chp, argc, arg_mod) do{ \
          if( (argc % (arg_mod)) != 0 ) { util_message_error(chp, "Invalid argument count"); return false; } \
        }while(0);

#define FETCH_HELP_TITLE(chp, msg) do{ \
          util_message_info(chp, "  == %s ==", msg); \
        }while(0);

#define FETCH_HELP_LEGEND(chp)  do{ \
          util_message_info(chp, "Legend: <> required, [] optional, | or,"); \
          util_message_info(chp, "        ... continuation, {} comment"); \
        }while(0);

#define FETCH_HELP_CMD(chp, cmd) do{ \
          util_message_info(chp, "  %s", cmd); \
        }while(0);

#define FETCH_HELP_DES(chp, desc) do{ \
          util_message_info(chp, "    %s", desc); \
        }while(0);

#define FETCH_HELP_ARG(chp, name, value) do{ \
          util_message_info(chp, "      %s = %s", name, value); \
        }while(0);

#define FETCH_HELP_BREAK(chp) do{ \
          util_message_info(chp,""); \
        }while(0);

/* HELP FORMAT

#: == TITLE ==
#: test(<arg1>,<arg2>)
#:   Command description
#:     arg1 = name
#:     arg2 = HIGH | LOW

*/

void fetch_init(void);
bool fetch_execute( BaseSequentialStream * chp, const char * input_line );
void fetch_display_help_legend(BaseSequentialStream * chp);

bool fetch_help_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_reset_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_version_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_chip_id_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_test_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_clocks_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );
bool fetch_sleep_cmd( BaseSequentialStream  * chp, uint32_t argc, char * argv[] );

#ifdef __cplusplus
}
#endif

#endif
/*! @} */
