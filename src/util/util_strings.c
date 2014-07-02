/*! \file util_strings.c
 * 
 * Not all of the string functions in <string.h> are static memory based.
 * (e.g. strtok_r)
 */

/*
	derived from:

    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include <string.h>
#include <ctype.h>
#include "util_general.h"
#include "util_strings.h"

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


