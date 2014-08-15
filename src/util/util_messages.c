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
#include "util_strings.h"

static bool needs_newline(char * str)
{
  if( str == NULL || str[0] == '\0' )
  {
    return true;
  }

  int end = strlen(str) - 1;

  return !(str[end] == '\n' || str[end] == '\r');
}

void util_message_debug( BaseSequentialStream * chp, char * file, int line, const char * func, char * fmt, ...)
{
	if(fmt == NULL || file == NULL || func == NULL)
  {
    return;
  }

	chBSemWait( &mshell_io_sem );

  chprintf(chp, "?:%s:%d:%s:",file,line,func);

	va_list arg_list;
	va_start(arg_list, fmt);
  chvprintf(chp, fmt, arg_list);
	va_end(arg_list);

  if( needs_newline(fmt) )
  {
	  chprintf(chp, "\r\n");
  }
	chBSemSignal( &mshell_io_sem );
}

void util_message_info( BaseSequentialStream * chp, char * fmt, ...)
{
	if(fmt == NULL)
  {
    return;
  }

	chBSemWait( &mshell_io_sem );

  chprintf(chp, "I:");

	va_list arg_list;
	va_start(arg_list, fmt);
  chvprintf(chp, fmt, arg_list);
	va_end(arg_list);

  if( needs_newline(fmt) )
  {
	  chprintf(chp, "\r\n");
  }
	chBSemSignal( &mshell_io_sem );
}

void util_message_error( BaseSequentialStream * chp, char * fmt, ...)
{
	if(fmt == NULL)
  {
    return;
  }

	chBSemWait( &mshell_io_sem );

  chprintf(chp, "E:");

	va_list arg_list;
	va_start(arg_list, fmt);
  chvprintf(chp, fmt, arg_list);
	va_end(arg_list);

  if( needs_newline(fmt) )
  {
	  chprintf(chp, "\r\n");
  }
	chBSemSignal( &mshell_io_sem );
}

void util_message_comment( BaseSequentialStream * chp, char * fmt, ...)
{
	if(fmt == NULL)
  {
    return;
  }

	chBSemWait( &mshell_io_sem );

  chprintf(chp, "#:");

	va_list arg_list;
	va_start(arg_list, fmt);
  chvprintf(chp, fmt, arg_list);
	va_end(arg_list);

  if( needs_newline(fmt) )
  {
	  chprintf(chp, "\r\n");
  }
	chBSemSignal( &mshell_io_sem );
}

void util_message_string( BaseSequentialStream *chp, char * name, char * fmt, ...)
{
	if(fmt == NULL)
  {
    return;
  }

	chBSemWait( &mshell_io_sem );

  chprintf(chp, "S:%s:", name);

	va_list arg_list;
	va_start(arg_list, fmt);
  chvprintf(chp, fmt, arg_list);
	va_end(arg_list);

  if( needs_newline(fmt) )
  {
	  chprintf(chp, "\r\n");
  }
	chBSemSignal( &mshell_io_sem );
}

void util_message_double( BaseSequentialStream * chp, char * name, double * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "F:%s:", name);

  for( uint32_t i = 0; i < count; i++ )
  {
    chprintf(chp, "%f", data[i]);

    if( (i+1) < count )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_int8( BaseSequentialStream * chp, char * name, int8_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "S8:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%d", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_uint8( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "U8:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%u", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_int16( BaseSequentialStream * chp, char * name, int16_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );
  
  chprintf(chp, "S16:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%d", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_uint16( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "U16:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%u", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_int32( BaseSequentialStream * chp, char * name, int32_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );
  
  chprintf(chp, "S32:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%d", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_uint32( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );
  
  chprintf(chp, "U32:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%d", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_hex_uint8( BaseSequentialStream * chp, char * name, uint8_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "H8:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%02X", *(data++));

    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_hex_uint16( BaseSequentialStream * chp, char * name, uint16_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "H16:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%04X", *(data++));
    
    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

void util_message_hex_uint32( BaseSequentialStream * chp, char * name, uint32_t * data, uint32_t count)
{
	chBSemWait( &mshell_io_sem );

  chprintf(chp, "H32:%s:", name);

  for( ; count > 0; count-- )
  {
    chprintf(chp, "%08X", *(data++));
    
    if( count > 1 )
    {
      chprintf(chp, ",");
    }
  }

	chprintf(chp, "\r\n");
	chBSemSignal( &mshell_io_sem );
}

//! @}


