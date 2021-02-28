/*
fetch_tsar.c --- functin definitions for TSAR's Marionette firmware revisions
Author: Seth Rohrbach
Modified: Feb 27, 2021
----------------------
Description:



*/

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"
#include "adc_lld.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_io.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_adc.h"
#include "mpipe.h"


//ALL functions needed to support my driver need to go in here.
//These functions will be called on by the fetch_parser.rl
//The commands will also need to be translated to Ragel syntax and put in the fetch_commands.rl file

/*
fetch_tsar_help - when called, sends a simple help menu over serial, to be printed over stdout
*/

bool fetch_tsar_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
    FETCH_MAX_ARGS(chp, argc, 0);

    FETCH_HELP_BREAK(chp);
    FETCH_HELP_LEGEND(chp);
    FETCH_HELP_BREAK(chp);
    FETCH_HELP_TITLE(chp, "TSAR Commands Help");
    FETCH_HELP_BREAK(chp);
    FETCH_HELP_CMD(chp, "MORE TO FOLLOW! - A command will go here");
    FETCH_HELP_DES(chp, "A description will go here");
    FETCH_HELP_ARG(chp, "And the arguments go here");
    FETCH_HELP_BREAK(chp);


}
