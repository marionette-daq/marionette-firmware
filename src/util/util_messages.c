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
#include "chbsem.h"

#include "util_general.h"
#include "util_messages.h"
#include "mshell_sync.h"

/*! This is declared extern in util_messages.h 
 */
Util_rpt_data           util_report_data;

struct formats
{
	Report_types      t;
	char       *      report_code;
};

/*! \todo codereview: static const or const static? */
static const struct formats formats[] =
{
	{ RPT_INFO,      "I"},
	{ RPT_ERROR,     "E"},
	{ RPT_DEBUG,     "?"},
	{ RPT_ADC,       "uV"},
	{ RPT_LOGIC,     "L"},
	{ RPT_VOLTS,     "V"},
	{ RPT_TIME,      "mS"},
	{ RPT_HDATA,     "XD"},
	{ RPT_DDATA,     "DD"},
	{ RPT_COMMENT,   "#"},
	{ RPT_MISC,      "Q"}
};

/*! \brief Return the text symbol of a msg format 
 */
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

/*! \brief Special function for handling debug messages
 *  \sa DBG_MSG  in util_messages.h
 *  \sa DBG_VMSG  in util_messages.h
 */
void util_debugmsg(BaseSequentialStream * chp,  char * file, int line, const char * func, char * fmt, ...)
{
	va_list argList;

	va_start(argList, fmt);
	chprintf(chp, "%s: %s:%d:%s()->", util_getformat(RPT_DEBUG), file, line, func);
	chvprintf(chp, fmt, argList);
	chprintf(chp, "\r\n");
	va_end(argList);
}

/*! \brief Report a logical value
 * \warning Util_rpt_data structure must be correct for correct result
 */
void util_logic_data(BaseSequentialStream * chp, Util_rpt_data * d, char * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	d->datalen = 1;
	util_report(chp, RPT_LOGIC, d, fmt, argList);
	va_end(argList);
}


/*! \brief Report a time value
 * \warning Util_rpt_data structure must be correct for correct result
 */
void util_time_data(BaseSequentialStream * chp, Util_rpt_data * d, char * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	d->datalen = 1;
	util_report(chp, RPT_TIME, d, fmt, argList);
	va_end(argList);
}

/*! \brief Report an adc result
 *  \warning Util_rpt_data structure must be correct for correct result
 */
void util_adc_data(BaseSequentialStream * chp,  Util_rpt_data * d, char * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_ADC, d, fmt, argList);
	va_end(argList);
}

/*! \brief User Information Message
 */
void util_info(BaseSequentialStream * chp, char * fmt, ... )
{

	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_INFO, NULL, fmt, argList);
	va_end(argList);
}

/*! \brief Error message report
 */
void util_error(BaseSequentialStream * chp, char * fmt, ... )
{

	va_list argList;
	va_start(argList, fmt);
	util_report(chp, RPT_ERROR, NULL, fmt, argList);
	va_end(argList);
}

/*! \brief Base function for reporting
 *
 *  \warn Exception is debug messages (currently).
 *
 * Mutex (BSemaphore) the reporting so that separate threads don't interleave output.
 *
 * Not using Mutex. Mutex in ChibiOS has thread ownership issues. Semaphores can be
 * signaled from any thread.
*  See:
 * http://www.chibios.org/dokuwiki/doku.php?id=chibios:guides:mutual_exclusion_guide
 *
 */
void util_report(BaseSequentialStream * chp, Report_types rpt, Util_rpt_data * d, char * fmt, va_list argptr )
{
	chBSemWait( &mshell_io_sem );
	switch(rpt)
	{
		case RPT_INFO:
		case RPT_ERROR:
		case RPT_DEBUG:
		case RPT_COMMENT:
		case RPT_MISC:
			if(fmt != NULL)
			{
				chprintf(chp, "%s:", util_getformat(rpt));
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_ADC:
			if(d != NULL && d->datalen > 0)
			{
				chprintf(chp, "%s:adc:%d", util_getformat(rpt), d->data[0]);
				for(int i = 1; i < d->datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, ",%d", d->data[i]);   //>! CSV format
				}
				chprintf(chp, ":");
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_VOLTS:
			if(d != NULL && d->datalen > 0)
			{
				for(int i = 0; i < d->datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%s:%d", util_getformat(rpt), d->data[i]);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_LOGIC:
			if(d != NULL && d->datalen > 0)
			{
				chprintf(chp, "%s:logic:%d:", util_getformat(rpt), d->data[0]);
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_TIME:
			if(d != NULL && d->datalen > 0)
			{
				chprintf(chp, "%s:time:%d:", util_getformat(rpt), d->data[0]);
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_HDATA:
			if(d != NULL && d->datalen > 0)
			{
				chprintf(chp, "%s:", util_getformat(rpt), d->data[0]);
				for(int i = 0; i < d->datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%2x",  d->data);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		case RPT_DDATA:
			if(d != NULL && d->datalen > 0)
			{
				chprintf(chp, "%s:", util_getformat(rpt), d->data);
				for(int i = 0; i < d->datalen && i < UTIL_MAXDATA; ++i)
				{
					chprintf(chp, "%2d ", d->data);
				}
				chvprintf(chp, fmt, argptr);
			}
			break;
		default:
			break;
	};
	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}


//! @}


