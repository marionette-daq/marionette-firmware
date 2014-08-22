/*! \file fetch_dac.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @defgroup fetch_dac Fetch DAC
  * @{
   */

/*!
 * <hr>
 *
 *  In ChibiOS 2.6.x there is no DAC module in the HAL. It is available
 *  in the kernel_3_dev branch.
 *
 * <hr>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "mshell_state.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_dac.h"

/*! \brief track the state of the dac
 *
 */
static FETCH_dac_state fetch_dac_state =
{
	.dc_mv_ch1            = 0,
	.dc_mv_ch2            = 0,
	.init                 = false,
	.chp                  = NULL
};

static void fetch_dac_init(BaseSequentialStream * chp)
{
	util_message_error(chp, "init: Command not yet available");
};

static inline int fetch_dac_is_valid_dac_configure(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkdac_configure)
{

	return(token_match(chp, fetch_terms->dac_configure, chkdac_configure,
	                   ((int) NELEMS(fetch_terms->dac_configure)) ) );
}


static bool fetch_dac_ch1_configure(BaseSequentialStream * chp ,
                                    Fetch_terminals * fetch_terms, char * cmd_list[], char * data_list[])
{

	if(fetch_dac_is_valid_dac_configure(chp, fetch_terms, cmd_list[DAC_CONFIGURE])) {};

	util_message_error(chp, "ch1 Command not yet available");
	return false;
}

static bool fetch_dac_ch2_configure(BaseSequentialStream * chp ,
                                    Fetch_terminals * fetch_terms, char * cmd_list[], char * data_list[])
{

	if(fetch_dac_is_valid_dac_configure(chp, fetch_terms, cmd_list[DAC_CONFIGURE])) {};

	util_message_error(chp, "Command not yet available");
	return false;
}

bool fetch_dac_start(void)
{
	util_message_error(getMShellStreamPtr(), "Command not yet available");
	return false;
}

bool fetch_dac_stop(void)
{
	util_message_error(getMShellStreamPtr(), "Command not yet available");
	return false;
}

static inline int fetch_dac_is_valid_dac_subcommandA(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkdac_subcommandA)
{
	return(token_match(chp, fetch_terms->dac_subcommandA, chkdac_subcommandA, ((int) NELEMS(fetch_terms->dac_subcommandA)) ) );
}

/*! \brief dispatch an DAC command
 */
bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                        Fetch_terminals * fetch_terms)
{
	fetch_dac_state.chp = chp;

	if(!fetch_dac_state.init)
	{
		fetch_dac_init(chp);
	};

	if(fetch_dac_is_valid_dac_subcommandA(chp, fetch_terms, cmd_list[DAC_SCA]) >= 0)
	{
		if (strncasecmp(cmd_list[DAC_SCA], "conf_ch1", strlen("conf_ch1") ) == 0)
		{
			return(fetch_dac_ch1_configure(chp, fetch_terms, cmd_list, data_list));
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "conf_ch2", strlen("conf_ch2") ) == 0)
		{
			return(fetch_dac_ch2_configure(chp, fetch_terms, cmd_list, data_list));
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "start", strlen("start") ) == 0)
		{
			return(fetch_dac_start());
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "stop", strlen("stop") ) == 0)
		{
			return(fetch_dac_stop());
		}
		else
		{
			DBG_MSG(chp, "sub-command not ready yet...");
			return(false) ;
		}
	}
	return false;
}
//! @}

