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
#define VL_VERSION_STRING "0.9.5"

/** ------------------------------------------------------------------
 ** @name C preprocessor helper macros
 ** @{ */

/** @brief Convert the argument to a string
 **
 ** @param x value to be stringified.
 **
 ** This macro stringifies the argument @a x by means of the
 ** <code>#</code> prerpocessor operator.
 **
 ** The standard C preprocessor does not prescan arguments which are
 ** stringified, so
 **
 ** @code
 ** #define A B
 ** char const * str = VL_STRINGIFY(A) ;
 ** @endcode
 **
 ** initializes <code>str</code> with a pointer to the string
 ** <code>"A"</code>, which mihgt be unexpected. To fix this issue,
 ** you can use ::VL_XSTRINGIFY.
 **
 ** @sa ::VL_XSTRINGIFY
 **/

#define VL_STRINGIFY(x) # x

/** @brief Expand and then convert the argument to a string
 **
 ** @param x value to be macro-expanded and converted.
 **
 ** This macro macro-expands the argument @a x and stringifies the
 ** result of the expansion. For instance
 **
 ** @code
 ** #define A B
 ** char const * str = VL_STRINGIFY(A) ;
 ** @endcode
 **
 ** initializes <code>str</code> with a pointer to the string
 ** <code>"B"</code>.
 **
 ** @sa ::VL_STRINGIFY
 **/
#define VL_XSTRINGIFY(x) VL_STRINGIFY(x)

/** @brief Concatenate the arguments into a lexical unit
 **
 ** @param x first argument to be concatenated.
 ** @param y second argument to be concatenated.
 **
 ** This macro concatenates its arguments into a single lexical unit
 ** by means of the <code>##</code> preprocessor operator. Notice that
 ** arguments concatenated by <code>##</code> are not pre-expanded by
 ** the C preprocessor. To macro-expand the arguments and then
 ** concatenate them,use ::VL_XCAT.
 **
 ** @see ::VL_XCAT
 **/
#define VL_CAT(x,y) x ## y

/** @brief Expand and then concatenate the arguments into a lexical unit
 **
 ** @param x first argument to be concatenated.
 ** @param y second argument to be concatenated.
 **
 ** This macro is the same as ::VL_CAT, except that the arguments are
 ** macro expanded before being concatenated.
 **
 ** @see ::VL_CAT
 **/
#define VL_XCAT(x,y) VL_CAT(x,y)

/** @} */

/** @brief Convert a boolean to "yes" or "no" strings
 ** @param x boolean to convert.
 ** A pointer to either the string "yes" (if @a x is true)
 ** or the string "no".
 ** @par Example
 ** @code
 ** VL_PRINTF("Is x true? %s.", VL_YESNO(x))
 ** @endcode
 **/
#define VL_YESNO(x) ((x)?"yes":"no")

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
