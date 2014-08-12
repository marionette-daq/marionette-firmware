/*! \file util_messages.h
 *
 * Consistent messages
 *
 * @addtogroup util_messages
 */

#ifndef UTIL_MESSAGE_H_
#define UTIL_MESSAGE_H_

#include "hal.h"
#include "chprintf.h"

#ifndef DBG_MSG_ENABLE
#define         DBG_MSG_ENABLE          0
#endif

#define         UTIL_MAXDATA            256

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Allocate one structure for reporting */
typedef struct
{
	uint32_t data[UTIL_MAXDATA];
	uint8_t  datalen;
} Util_rpt_data;
extern Util_rpt_data           util_report_data;

typedef enum report_types
{
	RPT_INFO = 0,    //>! Information
	RPT_ERROR,       //>! Error Report
	RPT_DEBUG,       //>! Debug
	RPT_ADC,         //>! ADC Data
	RPT_LOGIC,       //>! Logic Data
	RPT_VOLTS,       //>! Voltage Data
	RPT_TIME,        //>! Time
	RPT_HDATA,       //>! Hex data
	RPT_DDATA,       //>! Decimal data
	RPT_COMMENT,     //>! Comments start with a '#' or comment symbol
	RPT_QUERYPIN,    //>! Pin allocation
	RPT_MISC = 99    //>! Uncatagorized
} Report_types;

#define DBG_MSG(chp, fmt) \
       do { if (DBG_MSG_ENABLE) util_debugmsg(chp, __FILE__, __LINE__, __func__, fmt ); } while (0)


#define DBG_VMSG(chp, fmt, ...) \
       do { if (DBG_MSG_ENABLE) util_debugmsg(chp,__FILE__, __LINE__, __func__, fmt, __VA_ARGS__); } while (0)

void util_debugmsg(BaseSequentialStream * chp,  char * file, int line, const char * func,
                   char * format, ...);

void util_logic_data(BaseSequentialStream * chp, Util_rpt_data * d, char * fmt, ...) ;
void util_time_data(BaseSequentialStream * chp, Util_rpt_data * d, char * fmt, ...) ;
void util_adc_data(BaseSequentialStream * chp,  Util_rpt_data * d, char * fmt, ...) ;
void util_query_pin(BaseSequentialStream * chp, char * fmt, ... );
void util_info(BaseSequentialStream * chp, char * fmt, ... ) ;
void util_error(BaseSequentialStream * chp, char * fmt, ... ) ;

void util_report(BaseSequentialStream * chp, Report_types rpt, Util_rpt_data * d, char * fmt, va_list argptr );

#ifdef __cplusplus
}
#endif

#endif

//! @}


