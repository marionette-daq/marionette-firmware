/*! \file util_strings.h
 * some routines in <string.h> are dynamically allocated memory based
 */


#ifndef UTIL_STRINGS_H
#define UTIL_STRINGS_H

size_t get_longest_str_length(const char * s1, const char * s2, int max_length);
char * _strtok(char * str, const char * delim, char ** saveptr);

#endif
