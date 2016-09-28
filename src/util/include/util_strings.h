/*! \file util_strings.h
 * some routines in <string.h> are dynamically allocated memory based
 * @addtogroup util_strings 
 */


#ifndef UTIL_STRINGS_H
#define UTIL_STRINGS_H

#define TOKEN_NOT_FOUND   -1

#ifdef __cplusplus
extern "C" {
#endif

char * fetch_remove_whitespace(char * str);
size_t get_longest_str_length(const char * s1, const char * s2, int max_length);
char * _strtok(char * str, const char * delim, char ** saveptr);
char * _strncpy(char * dest, const char * src, size_t n);
int token_match( const char * tok_str, int tok_max_len, const char * tok_array[], int tok_max_elems );

#ifdef __cplusplus
}
#endif

#endif

//! @}
