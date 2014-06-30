/*! \file util_messages.c
 *
 *  Standard message interface
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "chprintf.h"

#include "util_messages.h"

void util_debugmsg(BaseSequentialStream * chp, char * format, ...)
{
	va_list argList;
	va_start(argList, format);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

void util_errormsg(BaseSequentialStream * chp, char * format, ...)
{
	va_list argList;
	chprintf(chp, "\r\n*** ERROR:\t");
	va_start(argList, format);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

void util_infomsg(BaseSequentialStream * chp, char * format, ...)
{
	va_list argList;
	chprintf(chp, "*** INFO:\t");
	va_start(argList, format);
	chvprintf(chp, format, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

