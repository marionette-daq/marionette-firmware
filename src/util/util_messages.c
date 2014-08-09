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

#include "util_general.h"
#include "util_messages.h"
 
struct formats
{
	Report_types      t;
	char       *      report_code;
};

static const struct formats formats[] =
{
	{ RPT_INFO,      "I"},
	{ RPT_ERROR,     "E"},
	{ RPT_DEBUG,     "?"},
	{ RPT_ADC,       "uV"},
	{ RPT_VOLTS,     "V"},
	{ RPT_TIME,      "T"},
	{ RPT_HDATA,     "XD"},
	{ RPT_DDATA,     "DD"},
	{ RPT_COMMENT,   "#"},
	{ RPT_MISC,      "Q"}
};

static char * util_getformat(Report_types rpt)
{
	for(uint32_t i = 0; i < NELEMS(formats); ++i)
	{
		if(formats[i].t == rpt)
		{
			return(formats[i].report_code);
		}
	}
	return "I";
}

void util_debugmsg(BaseSequentialStream * chp,  char * file, int line, const char * func, char * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	chprintf(chp, "%s: %s:%d:%s()->", util_getformat(RPT_DEBUG), file, line, func);
	chvprintf(chp, fmt, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

void util_time_data(BaseSequentialStream * chp, uint32_t data[], uint8_t datalen, char * fmt, ...) {
	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_TIME, data, datalen, fmt, argList);
	va_end(argList);
}

void util_adc_data(BaseSequentialStream * chp, uint32_t data[], uint8_t datalen, char * fmt, ...) {
	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_ADC, data, datalen, fmt, argList);
	va_end(argList);
}

void util_info(BaseSequentialStream * chp, char * fmt, ... ) {

	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_INFO, NULL, 0, fmt, argList);
	va_end(argList);
}

void util_error(BaseSequentialStream * chp, char * fmt, ... ) {

	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_ERROR, NULL, 0, fmt, argList);
	va_end(argList);
}

void util_report(BaseSequentialStream * chp, Report_types rpt, uint32_t data[], uint8_t datalen, char * fmt, va_list argptr )
{
	switch(rpt)
	{
		case RPT_INFO:
		case RPT_ERROR:
		case RPT_DEBUG:
		case RPT_COMMENT:
		case RPT_MISC:
			if(fmt != NULL) {
				chprintf(chp, "%s:", util_getformat(rpt));
				chvprintf(chp, fmt, argptr);
		    }
			break;
		case RPT_ADC:
			if(data != NULL && datalen>0)
			{
				chprintf(chp, "%s:adc:%d", util_getformat(rpt), data[0]);
				for(int i = 1; i < datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, ",%d", util_getformat(rpt), data[i]);
				}
				chprintf(chp, ":");
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_VOLTS:
			if(data != NULL)
			{
				for(int i = 0; i < datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%s:%d", util_getformat(rpt), data);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_TIME:
			if(data != NULL)
			{
				chprintf(chp, "%s:time:%d:", util_getformat(rpt), data[0]);
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_HDATA:
			if(data != NULL)
			{
				chprintf(chp, "%s:", util_getformat(rpt), data);
				for(int i = 0; i < datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%2x", util_getformat(rpt), data);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_DDATA:
			if(data != NULL)
			{
				chprintf(chp, "%s:", util_getformat(rpt), data);
				for(int i = 0; i < datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%2d ", util_getformat(rpt), data);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		default:
			break;
	};
	chprintf(chp, "\r\n");
}


//! @}
