/*! \file fetch.h
 * Support implementation of Fetch Language
 * @addtogroup fetch_dsl
 * @{
 */


#ifndef FETCH_H_
#define FETCH_H_

#define 		FETCH_MAX_COMMANDS 						8
#define 		FETCH_MAX_DATA_ITEMS					8
#define 		FETCH_MAX_LINE_CHARS					256
#define 		FETCH_MAX_CMD_TOKS					    32
#define 		FETCH_MAX_DATA_TOKS					    256
#define 		FETCH_MAX_CMD_STRLEN					25

#define 		HELP_MAX_DATA_BYTES 					0
#define 		HELP_MAX_DATA_BYTES 					0
#define         HELP_HELPSTRING                         "\r\n"

#define 		GPIO_MAX_DATA_BYTES 					0
#define 		GPIO_HELPSTRING   					    ((const char *) "\r\n\tGPIO:\tgpio:<get,set,clear,configure>:port:pin:<input,output>:<pullup,pulldown,floating,analog>")

#define 		ADC_MAX_DATA_BYTES 					    0
#define 		ADC_HELPSTRING   					    ((const char *) "\r\n\tADC:\tadc:<configure,start,stop>:<oneshot,run>")

#define 		VERSION_MAX_DATA_BYTES 					0
#define 		VERSION_HELPSTRING   					((const char *) "\r\n\tVERSION:\tversion")

#define 		RESETPINS_MAX_DATA_BYTES 				0
#define 		RESETPINS_HELPSTRING   					((const char *) "\r\n\tRESETPINS:\tresetpins")

#include "fetch_defs.h"

int fetch_is_valid_digit(BaseSequentialStream * chp, char * chkdigit);
int fetch_is_valid_EOL(BaseSequentialStream * chp, char * chkEOL);
int fetch_is_valid_whitespace(BaseSequentialStream * chp, char * chkwhitespace);

void fetch_init(BaseSequentialStream *  chp) ;

bool fetch_parse(BaseSequentialStream* chp, char * inputline);
bool fetch_dispatch(BaseSequentialStream* chp, char * command_list[], char * data_list[]);

#endif

/*! @} */
