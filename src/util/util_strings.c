/*! \file util_strings.c
 * 
 * String utility functions
 *
 * @defgroup util_strings  Utility Functions for Strings
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <ctype.h>
#include "util_general.h"
#include "util_strings.h"


/*! \brief Remove all space types including '\t' and '_'
 *   \sa isspace() in cytpe.h
 */
char * fetch_remove_spaces(char * spstr)
{
	if(spstr == NULL)
	{
		return spstr;
	}
	char * newstr = spstr;
	unsigned int i, j;

	for (i = 0, j = 0; i < strlen(spstr); i++, j++)
	{
		if(!isspace((unsigned char)spstr[i]) && spstr[i] != '_')
		{
			newstr[j] = spstr[i];
		}
		else
		{
			j--;  // unincrement j
		}
	}
	newstr[j] = '\0';
	return newstr;
}

/*! \brief return length of longest string 
  */
size_t get_longest_str_length(const char * s1, const char * s2, int max_length)
{
	size_t lens1  = strnlen(s1, max_length);
	size_t lens2  = strnlen(s2, max_length);

	chDbgAssert(((s1 != NULL) && (s2 != NULL)), "get_longest_str_length() #1, NULL pointer");

	return (max(lens1, lens2));
}

/*! \brief a version of strtok_r but STATIC  memory
 */
char * _strtok(char * str, const char * delim, char ** saveptr)
{
	
	char * token;
	if (str)
	{
		*saveptr = str;
	}
	token = *saveptr;
	if (!token)
	{
		return NULL;
	}
	token += strspn(token, delim);
	*saveptr = strpbrk(token, delim);
	if (*saveptr)
	{
		*(*saveptr)++ = '\0';
	}
	return *token ? token : NULL;
}

/*! \brief a version of strncpy that only copies up to the first NULL or n chars
 *         also guarenteeing a null terminated string
 *
 *   This function is similar to the bsd strlcpy()
 *
 *   Conditions for proper execution: n > 0, dest != NULL, src != NULL
 */
char * _strncpy(char * dest, const char * src, size_t n)
{
  while(n > 1 && *src != '\0')
  {
    *dest=*src;
    n--;
    dest++;
    src++;
  }
  *dest = '\0';

  return dest;
}

/*! \brief search an array of string tokens for a match
 *  \param[in]  tok_str         string to match
 *  \param[in]  tok_max_len     max search string length
 *  \param[in]  tok_array       array of string tockens to search
 *  \param[in]  tok_max_elems   max number of elements in tok_array
 *
 *  Searching will stop at tok_max_elems or the first NULL in tok_array.
 *
 *  \returns index into tok_array if found and TOKEN_NOT_FOUND otherwise
 */
int token_match( const char * tok_str, int tok_max_len, const char * tok_array[], int tok_max_elems )
{
  if( tok_str == NULL || tok_array == NULL )
  {
    return TOKEN_NOT_FOUND;
  }

  for( int i = 0; i < tok_max_elems && tok_array[i] != NULL; i++ )
  {
    if( strncasecmp(tok_array[i], tok_str, tok_max_len) == 0 )
    {
      return i;
    }
  }

  return TOKEN_NOT_FOUND;
}

//! @}


