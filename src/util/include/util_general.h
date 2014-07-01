/*! \file util_general.h
  */


#ifndef UTIL_GENERAL_H_
#define UTIL_GENERAL_H_

#define                 UNUSED          __attribute__((unused))
#define                 NORETURN        __attribute__((noreturn))
#define                 NELEMS(x)       (sizeof(x) / sizeof(x[0]))

#define max(a,b) \
	({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
        _a > _b ? _a : _b; })



#endif



