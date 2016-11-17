#ifndef UTIL_ARG_PARSE_H
#define UTIL_ARG_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct {
  const char * str;
  uint32_t value;
} str_table_t;

#define util_match_str(str1, str2) (strcasecmp((str1),(str2)) == 0)
#define util_match_nstr(str1, str2, max_len) (strncasecmp((str1),(str2),(max_len)) == 0)

bool util_parse_bool(char * arg, bool * output);
bool util_parse_uint8(char * arg, uint8_t * output);
bool util_parse_uint16(char * arg, uint16_t * output);
bool util_parse_uint32(char * arg, uint32_t * output);
bool util_parse_uint64(char * arg, uint64_t * output);
bool util_parse_int8(char * arg, int8_t * output);
bool util_parse_int16(char * arg, int16_t * output);
bool util_parse_int32(char * arg, int32_t * output);
bool util_parse_int64(char * arg, int64_t * output);

bool util_match_str_table(char * arg, uint32_t * output, const str_table_t stbl[]);
bool util_match_str_ntable(char * arg, uint32_t * output, const str_table_t stbl[], size_t tbl_size);

bool util_match_str_array(char * arg, uint32_t * output, const char * str_array[]);
bool util_match_str_narray(char * arg, uint32_t * output, const char * str_array[], size_t array_size);

uint32_t util_parse_uint8_array(char * args[], uint8_t * output, size_t size, uint32_t base);
uint32_t util_parse_uint16_array(char * args[], uint16_t * output, size_t size, uint32_t base);
uint32_t util_parse_uint32_array(char * args[], uint32_t * output, size_t size, uint32_t base);

#ifdef __cplusplus
}
#endif

#endif
