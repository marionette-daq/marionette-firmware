/*! \file fetch.h
  * Support implementation of Fetch Language
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

#define 		VERSION_MAX_DATA_BYTES 					0
#define 		VERSION_HELPSTRING   					((const char *) "\r\n\tVERSION:\tversion")

#define 		RESETPINS_MAX_DATA_BYTES 				0
#define 		RESETPINS_HELPSTRING   					((const char *) "\r\n\tRESETPINS:\tresetpins")

#define         FETCH_MAX_TERMINALS                     32

typedef struct Command_terminals
{
	// All elements of the Terminal set (∑) have definitions here.
	 const char * command[FETCH_MAX_TERMINALS]          ;

	 const char * gpio_subcommandA[FETCH_MAX_TERMINALS] ;
	 const char * gpio_direction[FETCH_MAX_TERMINALS]   ;
	 const char * gpio_sense[FETCH_MAX_TERMINALS]       ;
	 const char * port_subcommand[FETCH_MAX_TERMINALS]  ;
	 const char * pin_subcommand[FETCH_MAX_TERMINALS]   ;
	 const char * subcommandD[FETCH_MAX_TERMINALS]      ;
	 const char * digit[FETCH_MAX_TERMINALS]            ;
	 const char * EOL[FETCH_MAX_TERMINALS]              ;
	 const char * whitespace[FETCH_MAX_TERMINALS]       ;

} Command_terminals;


// Support dictionaries
typedef struct Command_dictionary
{
	bool     	   enabled;
	uint16_t 	   max_data_bytes;
	const char  *  helpstring;
} Command_dictionary;

// Functions
int  fetch_token_match(BaseSequentialStream * chp, const char * tok_array[], char * chk_tok, int num_elems);

void fetch_init(BaseSequentialStream *  chp) ;

bool fetch_parse(BaseSequentialStream* chp, char * inputline);
bool fetch_dispatch(BaseSequentialStream* chp, char * command_list[], char * data_list[]);

#endif
