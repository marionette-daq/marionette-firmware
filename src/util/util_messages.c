/*! \file util_messages.c
 * 
 * @defgroup util_messages Message Utilities 
 * @{
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "chprintf.h"

#include "util_messages.h"

void util_debugmsg(BaseSequentialStream * chp,  char * file, int line, const char * func,
                   char * format, ...)
{
	va_list argList;
	va_start(argList, format);
	chprintf(chp, "\tDEBUG:  %s:%d:%s()->", file, line, func);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

void util_errormsg(BaseSequentialStream * chp, char * format, ...)
{
	va_list argList;
	chprintf(chp, "\r\n*** E:\t");
	va_start(argList, format);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

void util_infomsg(BaseSequentialStream * chp, char * format, ...)
{
	va_list argList;
	chprintf(chp, "*** I:\t");
	va_start(argList, format);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

//! @}
