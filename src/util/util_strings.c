/*! \file util_strings.c
 * 
 * String utility functions
 *
 * @defgroup util_strings  Utility Functions for Strings
 * @{
 */

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include <string.h>
#include <ctype.h>
#include "util_general.h"
#include "util_strings.h"


/*! \brief Remove all space types including '\t'
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
		if(!isspace((unsigned char)spstr[i]))
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

	chDbgAssert(((s1 != NULL) && (s2 != NULL)), "get_longest_str_length() #1", "NULL pointer");

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

/*! \brief support validation functions
 *   \warning don't call string functions on NULL pointers
 *
 *  \param[in] tok_array   array to iterate comparison
 *  \param[in] chk_tok     token to match to array
 *
 *  \return int An index into the array on match
 *  \return -1 on no match
 */
int token_match(BaseSequentialStream * chp, const char * tok_array[],
                char * chk_tok,
                int num_elems)
{
	chDbgAssert(((chk_tok != NULL) ), "token_match() #1", "NULL pointer");

	for(int i = 0; i < num_elems; ++i)
	{
		size_t maxlen = 1;
		if(tok_array[i] == NULL)
		{
			break;
		}

		if (strncasecmp(tok_array[i], "\0", maxlen ) == 0)
		{
			break;
		}

		maxlen = get_longest_str_length(tok_array[i], chk_tok, UTIL_STRINGS_MAX_SEARCH_CHARS);

		if (strncasecmp(tok_array[i], chk_tok, maxlen ) == 0)
		{
			return i;
		}
	}
	chprintf(chp, "\r\n");
	return -1;
}

//! @}


