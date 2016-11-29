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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUG_MSG_ENABLE
#define DEBUG_MSG_ENABLE          1
#endif

#define DEBUG_SERIAL  SD6
#define DEBUG_CHP     ((BaseSequentialStream *) &DEBUG_SERIAL)

#define DEBUG_MSG(chp, msg) \
       do { if (DEBUG_MSG_ENABLE) util_message_debug(chp,__FILE__, __LINE__, __func__, msg); } while (0)

#define DEBUG_VMSG(chp, fmt, ...) \
       do { if (DEBUG_MSG_ENABLE) util_message_debug(chp,__FILE__, __LINE__, __func__, fmt, __VA_ARGS__); } while (0)

void util_message_begin( BaseSequentialStream * chp);
void util_message_end( BaseSequentialStream * chp, bool success);

void util_message_debug( BaseSequentialStream * chp, char * file, int line, const char * func, char * fmt, ...);
void util_message_info( BaseSequentialStream * chp, char * fmt, ...);
void util_message_warning( BaseSequentialStream * chp, char * fmt, ...);
void util_message_error( BaseSequentialStream * chp, char * fmt, ...);

void util_message_string_format( BaseSequentialStream * chp, char * name, char * fmt, ...);
void util_message_string_escape( BaseSequentialStream * chp, char * name, char * str, uint32_t str_len );
void util_message_string_array( BaseSequentialStream * chp, char * name, char * str_list[], uint32_t count );

void util_message_bool( BaseSequentialStream * chp, char * name, bool data);

void util_message_double( BaseSequentialStream * chp, char * name, double data);
void util_message_int8( BaseSequentialStream * chp, char * name, int8_t data);
void util_message_uint8( BaseSequentialStream * chp, char * name, uint8_t data);
void util_message_int16( BaseSequentialStream * chp, char * name, int16_t data);
void util_message_uint16( BaseSequentialStream * chp, char * name, uint16_t data);
void util_message_int32( BaseSequentialStream * chp, char * name, int32_t data);
void util_message_uint32( BaseSequentialStream * chp, char * name, uint32_t data);
void util_message_hex_uint8( BaseSequentialStream * chp, char * name, uint8_t data);
void util_message_hex_uint16( BaseSequentialStream * chp, char * name, uint16_t data);
void util_message_hex_uint32( BaseSequentialStream * chp, char * name, uint32_t data);

void util_message_double_array( BaseSequentialStream * chp, char * name, double * data, uint32_t count);
void util_message_int8_array( BaseSequentialStream * chp, char * name, int8_t * data, uint32_t count);
void util_message_uint8_array( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count);
void util_message_int16_array( BaseSequentialStream * chp, char * name, int16_t * data, uint32_t count);
void util_message_uint16_array( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count);
void util_message_int32_array( BaseSequentialStream * chp, char * name, int32_t * data, uint32_t count);
void util_message_uint32_array( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count);
void util_message_hex_uint8_array( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count);
void util_message_hex_uint16_array( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count);
void util_message_hex_uint32_array( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif

//! @}

