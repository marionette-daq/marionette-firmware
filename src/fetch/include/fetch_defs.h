/*! \file fetch_defs.h
 * Definitions for fetch grammar
 * @addtogroup fetch_dsl
 * @{
 */


#ifndef FETCH_DEFS_H_
#define FETCH_DEFS_H_


#define         FETCH_MAX_TERMINALS                     32

typedef struct fetch_terminals
{
	// All elements of the Terminal set (∑) have definitions here.
	 const char * command[FETCH_MAX_TERMINALS]          ;
	 const char * gpio_subcommandA[FETCH_MAX_TERMINALS] ;
	 const char * gpio_direction[FETCH_MAX_TERMINALS]   ;
	 const char * gpio_sense[FETCH_MAX_TERMINALS]       ;
	 const char * adc_subcommandA[FETCH_MAX_TERMINALS]  ;
	 const char * adc_configure[FETCH_MAX_TERMINALS]    ;
	 const char * adc_sampletype[FETCH_MAX_TERMINALS]   ;
	 const char * port_subcommand[FETCH_MAX_TERMINALS]  ;
	 const char * pin_subcommand[FETCH_MAX_TERMINALS]   ;
	 const char * subcommandD[FETCH_MAX_TERMINALS]      ;
	 const char * digit[FETCH_MAX_TERMINALS]            ;
	 const char * EOL[FETCH_MAX_TERMINALS]              ;
	 const char * whitespace[FETCH_MAX_TERMINALS]       ;

} Fetch_terminals;

// Support dictionaries
typedef struct command_dictionary
{
	bool     	   enabled;
	uint16_t 	   max_data_bytes;
	const char  *  helpstring;
} Command_dictionary;

#endif
/*! @} */
