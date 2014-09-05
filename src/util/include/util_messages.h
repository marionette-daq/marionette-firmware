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
#define DBG_MSG_ENABLE          0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DBG_MSG(chp, fmt) \
       do { if (DBG_MSG_ENABLE) util_message_debug(chp, __FILE__, __LINE__, __func__, fmt ); } while (0)


#define DBG_VMSG(chp, fmt, ...) \
       do { if (DBG_MSG_ENABLE) util_message_debug(chp,__FILE__, __LINE__, __func__, fmt, __VA_ARGS__); } while (0)

void util_message_debug( BaseSequentialStream * chp, char * file, int line, const char * func, char * fmt, ...);
void util_message_info( BaseSequentialStream * chp, char * fmt, ...);
void util_message_error( BaseSequentialStream * chp, char * fmt, ...);
void util_message_comment( BaseSequentialStream * chp, char * fmt, ...);
void util_message_string( BaseSequentialStream * chp, char * name, char * fmt, ...);
void util_message_double( BaseSequentialStream * chp, char * name, double * data, uint32_t count);
void util_message_int8( BaseSequentialStream * chp, char * name, int8_t * data, uint32_t count);
void util_message_uint8( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count);
void util_message_int16( BaseSequentialStream * chp, char * name, int16_t * data, uint32_t count);
void util_message_uint16( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count);
void util_message_int32( BaseSequentialStream * chp, char * name, int32_t * data, uint32_t count);
void util_message_uint32( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count);
void util_message_hex_uint8( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count);
void util_message_hex_uint16( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count);
void util_message_hex_uint32( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif

//! @}

