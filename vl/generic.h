/** @file    generic.h
 ** @author  Andrea Vedaldi
 ** @brief   Generic
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_GENERIC_H
#define VL_GENERIC_H

#include <stddef.h>
#include <time.h>
#include <assert.h>

#include "host.h"

/** @brief Library version string */
#define VL_VERSION_STRING "0.9.7"

/** @brief Maximum length (in characters) of an error message */
#define VL_ERR_MSG_LEN 1024

/** @name Type identidifers for atomic data types
 ** @{ */

#define VL_TYPE_FLOAT   1
#define VL_TYPE_DOUBLE  2
#define VL_TYPE_INT8    3
#define VL_TYPE_UINT8   4
#define VL_TYPE_INT16   5
#define VL_TYPE_UINT16  6
#define VL_TYPE_INT32   7
#define VL_TYPE_UINT32  8
#define VL_TYPE_INT64   9
#define VL_TYPE_UINT64  10

/** @brief Get the symbolic name of a data type
 ** @param type data type.
 ** @return data type symbolic name.
 **
 ** @c type is one of ::VL_TYPE_FLOAT, ::VL_TYPE_DOUBLE,
 ** ::VL_TYPE_INT8, ::VL_TYPE_INT16, ::VL_TYPE_INT32, ::VL_TYPE_INT64,
 ** ::VL_TYPE_UINT8, ::VL_TYPE_UINT16, ::VL_TYPE_UINT32, ::VL_TYPE_UINT64.
 **/

VL_INLINE char const *
vl_get_type_name (int type)
{
  switch (type) {
    case VL_TYPE_FLOAT   : return "float"  ;
    case VL_TYPE_DOUBLE  : return "double" ;
    case VL_TYPE_INT8    : return "int8"   ;
    case VL_TYPE_INT16   : return "int16"  ;
    case VL_TYPE_INT32   : return "int32"  ;
    case VL_TYPE_INT64   : return "int64"  ;
    case VL_TYPE_UINT8   : return "int8"   ;
    case VL_TYPE_UINT16  : return "int16"  ;
    case VL_TYPE_UINT32  : return "int32"  ;
    case VL_TYPE_UINT64  : return "int64"  ;
    default: return NULL ;
  }
}

/** @} */

/** ------------------------------------------------------------------
 ** @name Memory allocation
 ** @{ */

VL_EXPORT
void vl_set_alloc_func (void *(*malloc_func)  (size_t),
                        void *(*realloc_func) (void*,size_t),
                        void *(*calloc_func)  (size_t, size_t),
                        void  (*free_func)    (void*)) ;
VL_INLINE void *vl_malloc  (size_t n) ;
VL_INLINE void *vl_realloc (void *ptr, size_t n) ;
VL_INLINE void *vl_calloc  (size_t n, size_t size) ;
VL_INLINE void  vl_free    (void* ptr) ;

/** @} */

/** ------------------------------------------------------------------
 ** @name Logging
 ** @{ */

/** ------------------------------------------------------------------
 ** @brief Customizable printf function pointer type */
typedef int(*printf_func_t) (char const *format, ...) ;

/** @brief Set printf function
 ** @param printf_func  pointer to @c printf.
 ** Let @c print_func be NULL to disable printf.
 **/
VL_EXPORT void vl_set_printf_func (printf_func_t printf_func) ;

/** @def VL_PRINTF
 ** @brief Call user-customizable @c printf function
 **
 ** The function calls the user customizable @c printf.
 **/
#define VL_PRINTF (*vl_printf_func)

/** @def VL_PRINT
 ** @brief Same as ::VL_PRINTF (legacy code)
 **/
#define VL_PRINT (*vl_printf_func)

/** @} */

/** ------------------------------------------------------------------
 ** @name Error handling
 ** @{ */

/** @brief The number of the last error */
extern VL_EXPORT int vl_err_no ;

/** @brief The maximum length of an error description. */
#define VL_ERR_MSG_LEN 1024

/** @brief The description of the last error. */
extern VL_EXPORT char vl_err_msg [VL_ERR_MSG_LEN + 1] ;

#define VL_ERR_OK       0  /**< No error */
#define VL_ERR_OVERFLOW 1  /**< Buffer overflow error */
#define VL_ERR_ALLOC    2  /**< Resource allocation error */
#define VL_ERR_BAD_ARG  3  /**< Bad argument or illegal data error */
#define VL_ERR_IO       4  /**< Input/output error */
#define VL_ERR_EOF      5  /**< End-of-file or end-of-sequence error */
#define VL_ERR_NO_MORE  5  /**< End-of-sequence @deprecated */

/** @} */

/** ------------------------------------------------------------------
 ** @name Common operations
 ** @{ */

/** @brief Min operation
 ** @param x value
 ** @param y value
 ** @return the minimum of @a x and @a y.
 **/
#define VL_MIN(x,y) (((x)<(y))?(x):(y))

/** @brief Max operation
 ** @param x value.
 ** @param y value.
 ** @return the maximum of @a x and @a y.
 **/
#define VL_MAX(x,y) (((x)>(y))?(x):(y))

/** @brief Signed left shift operation
 **
 ** The macro is equivalent to the builtin @c << operator, but it
 ** supports negative shifts too.
 **
 ** @param x value.
 ** @param n number of shift positions.
 ** @return @c x << n .
 **/
#define VL_SHIFT_LEFT(x,n) (((n)>=0)?((x)<<(n)):((x)>>-(n)))
/* @} */

/** --------------------------------------------------------------- */
VL_EXPORT
char const * vl_get_version_string () ;

VL_EXPORT
void vl_print_info () ;

/** ------------------------------------------------------------------
 ** @name Measuring time
 ** @{
 **/
VL_EXPORT void vl_tic() ;
VL_EXPORT double vl_toc() ;
/** @} */

extern VL_EXPORT int   (*vl_printf_func)  (char const * format, ...) ;
extern VL_EXPORT void *(*vl_malloc_func)  (size_t) ;
extern VL_EXPORT void *(*vl_realloc_func) (void*,size_t) ;
extern VL_EXPORT void *(*vl_calloc_func)  (size_t, size_t) ;
extern VL_EXPORT void  (*vl_free_func)    (void*) ;

VL_INLINE
void* vl_malloc (size_t n)
{
  return (*vl_malloc_func)(n) ;
}

VL_INLINE
void* vl_realloc (void* ptr, size_t n)
{
  return (*vl_realloc_func)(ptr, n) ;
}

VL_INLINE
void* vl_calloc (size_t n, size_t size)
{
  return (*vl_calloc_func)(n, size) ;
}

VL_INLINE
void vl_free (void *ptr)
{
  (*vl_free_func)(ptr) ;
}

/* VL_GENERIC_H */
#endif
