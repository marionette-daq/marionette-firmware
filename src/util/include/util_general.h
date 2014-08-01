/*! \file util_general.h
  * \defgroup util_general General Utility Functions
  * @{
  */
#ifndef UTIL_GENERAL_H_
#define UTIL_GENERAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define                 UNUSED          __attribute__((unused))
#define                 NORETURN        __attribute__((noreturn))
#define                 NELEMS(x)       (sizeof(x) / sizeof(x[0]))

#define max(a,b) \
	({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
        _a > _b ? _a : _b; })


#ifdef __cplusplus
}
#endif



#endif


//! @}
