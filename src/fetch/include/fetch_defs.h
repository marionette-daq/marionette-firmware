/*! \file fetch_defs.h

  Definitions for fetch grammar
  */


#ifndef FETCH_DEFS_H_
#define FETCH_DEFS_H_


#define         FETCH_MAX_TERMINALS                     32

typedef struct Fetch_terminals
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

} Fetch_terminals;

// Support dictionaries
typedef struct Command_dictionary
{
	bool     	   enabled;
	uint16_t 	   max_data_bytes;
	const char  *  helpstring;
} Command_dictionary;

#endif
