#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "util_arg_parse.h"

bool util_parse_uint8(char * arg, uint8_t * output)
{
  char * endptr;
  uint32_t value;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  value = strtoul(arg, &endptr, 0);
  *output = (uint8_t)value;

  return (*endptr == '\0' && value <= UINT8_MAX );
}

bool util_parse_uint16(char * arg, uint16_t * output)
{
  char * endptr;
  uint32_t value;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  value = strtoul(arg, &endptr, 0);
  *output = (uint16_t)value;

  return (*endptr == '\0' && value <= UINT16_MAX );
}

bool util_parse_uint32(char * arg, uint32_t * output)
{
  char * endptr;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  *output = strtoul(arg, &endptr, 0);

  return (*endptr == '\0');
}

bool util_parse_uint64(char * arg, uint64_t * output )
{
  char * endptr;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  *output = strtoull(arg, &endptr, 0);

  return (*endptr == '\0');
}

bool util_parse_int8(char * arg, int8_t * output)
{
  char * endptr;
  int32_t value;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  value = strtol(arg, &endptr, 0);
  *output = (int8_t)value;

  return (*endptr == '\0' && value <= INT8_MAX && value >= INT8_MIN );
}

bool util_parse_int16(char * arg, int16_t * output)
{
  char * endptr;
  int32_t value;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  value = strtol(arg, &endptr, 0);
  *output = (int16_t)value;

  return (*endptr == '\0' && value <= INT16_MAX && value >= INT16_MIN );
}

bool util_parse_int32(char * arg, int32_t * output)
{
  char * endptr;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  *output = strtol(arg, &endptr, 0);

  return (*endptr == '\0');
}

bool util_parse_int64(char * arg, int64_t * output)
{
  char * endptr;

  if( arg == NULL || output == NULL )
  {
    return false;
  }

  *output = strtoll(arg, &endptr, 0);

  return (*endptr == '\0');
}

bool util_match_str_table(char * arg, uint32_t * output, const str_table_t stbl[])
{
  if( arg == NULL || output == NULL || stbl == NULL)
  {
    return false;
  }

  // search for string match in NULL terminated table
  for( uint32_t index = 0; stbl[index].str != NULL; index++ )
  {
    if( strcasecmp(arg, stbl[index].str) == 0 )
    {
      *output = stbl[index].value;
      return true;
    }
  }

  return false;
}

bool util_match_str_ntable(char * arg, uint32_t * output, const str_table_t stbl[], size_t table_size)
{
  if( arg == NULL || output == NULL || stbl == NULL)
  {
    return false;
  }

  // search for string match in NULL terminated bounded table
  for( uint32_t index = 0; index < table_size && stbl[index].str != NULL; index++ )
  {
    if( strcasecmp(arg, stbl[index].str) == 0 )
    {
      *output = stbl[index].value;
      return true;
    }
  }

  return false;
}


bool util_match_str_array(char * arg, uint32_t * output, const char * str_array[])
{
  if( arg == NULL || output == NULL || str_array == NULL)
  {
    return false;
  }

  // search for string match in NULL terminated array
  for( uint32_t index = 0; str_array[index] != NULL; index++ )
  {
    if( strcasecmp(arg, str_array[index]) == 0 )
    {
      *output = index;
      return true;
    }
  }

  return false;
}

bool util_match_str_narray(char * arg, uint32_t * output, const char * str_array[], size_t array_size)
{
  if( arg == NULL || output == NULL || str_array == NULL)
  {
    return false;
  }

  // search for string match in NULL terminated  bounded array
  for( uint32_t index = 0; index < array_size && str_array[index] != NULL; index++ )
  {
    if( strcasecmp(arg, str_array[index]) == 0 )
    {
      *output = index;
      return true;
    }
  }

  return false;
}

uint32_t util_parse_uint8_array(char * args[], uint8_t * output, size_t size, uint32_t base)
{
  char * endptr;
  uint32_t value;
  uint32_t index = 0;

  if( args == NULL || output == NULL )
  {
    return false;
  }

  for( index = 0; index < size && args[index] != NULL; index++ )
  {
    value = strtoul(args[index], &endptr, base);
    if( *endptr != '\0' || value > UINT8_MAX )
    {
      return 0;
    }
    output[index] = (uint8_t)value;
  }

  return index;
}

uint32_t util_parse_uint16_array(char * args[], uint16_t * output, size_t size, uint32_t base)
{
  char * endptr;
  uint32_t value;
  uint32_t index = 0;

  if( args == NULL || output == NULL )
  {
    return false;
  }

  for( index = 0; index < size && args[index] != NULL; index++ )
  {
    value = strtoul(args[index], &endptr, base);
    if( *endptr != '\0' || value > UINT16_MAX )
    {
      return 0;
    }
    output[index] = (uint16_t)value;
  }

  return index;
}

uint32_t util_parse_uint32_array(char * args[], uint32_t * output, size_t size, uint32_t base)
{
  char * endptr;
  uint32_t value;
  uint32_t index = 0;

  if( args == NULL || output == NULL )
  {
    return false;
  }

  for( index = 0; index < size && args[index] != NULL; index++ )
  {
    value = strtoul(args[index], &endptr, base);
    if( *endptr != '\0' )
    {
      return 0;
    }
    output[index] = (uint32_t)value;
  }

  return index;
}

