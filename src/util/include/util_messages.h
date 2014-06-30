/*! \file util_message.h
 *  Consistent messages
 */

#ifndef UTIL_MESSAGE_H_
#define UTIL_MESSAGE_H_

#include "hal.h"
#include "chprintf.h"

#ifndef DBG_MSG_ENABLE
#define         DBG_MSG_ENABLE          0
#endif

#define DBG_MSG(chp, msg) \
       do { if (DBG_MSG_ENABLE) util_debugmsg(chp, msg); } while (0)


#define DBG_VMSG(chp, fmt, ...) \
       do { if (DBG_MSG_ENABLE) util_debugmsg(chp, fmt, __VA_ARGS__); } while (0)

void util_debugmsg(BaseSequentialStream * chp, char * format, ...) ;
void util_errormsg(BaseSequentialStream * chp, char * msg, ...) ;
void util_infomsg(BaseSequentialStream * chp, char * msg, ...) ;
#endif


