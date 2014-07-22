/*! \file fetch_adc.c
  * 
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @addtogroup fetch_adc
  * @{
  */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"
static inline int fetch_adc_is_valid_adc_subcommandA(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkadc_subcommandA)
{

	return(token_match(chp, fetch_terms->adc_subcommandA, chkadc_subcommandA,
	                   ((int) NELEMS(fetch_terms->adc_subcommandA)) ) );
}


bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                         Fetch_terminals * fetch_terms)
{

	if(fetch_adc_is_valid_adc_subcommandA(chp, fetch_terms, cmd_list[ACTION]) >= 0) {
			DBG_MSG(chp, "...Not yet");
			return true;
	}
	return false;	
}


/*! @} */
