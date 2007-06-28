/** @file    generic.h
 ** @author  Andrea Vedaldi
 ** @brief   Generic - Declaration
 **
 ** This modules provides basic facilities such as basic types, error
 ** handling, endianness detection and serialization.
 **/

/* AUTORIGHTS */

#ifndef VL_GENERIC_H
#define VL_GENERIC_H

/** @brief Library version string */
#define VL_VERSION_STRING "alpha-1"

/** @brief Inline function declaration */
#define VL_INLINE __inline__

/** ---------------------------------------------------------------- */
/** @name Atomic data types
 ** @{
 **/

typedef long long           vl_int64 ;   /**< signed 64-bit integer. */
typedef int                 vl_int32 ;   /**< signed 32-bit integer. */
typedef short               vl_int16 ;   /**< signed 16-bit integer. */
typedef char                vl_int8  ;   /**< signed  8-bit integer. */

typedef long long unsigned  vl_uint64 ;  /**< unsigned 64-bit integer. */
typedef int       unsigned  vl_uint32 ;  /**< unsigned 32-bit integer. */
typedef short     unsigned  vl_uint16 ;  /**< unsigned 16-bit integer. */
typedef char      unsigned  vl_uint8 ;   /**< unsigned  8-bit integer. */

typedef int                 vl_int ;     /**< integer.                */
typedef unsigned int        vl_uint ;    /**< unsigned integer.       */
typedef float               vl_single ;  /**< single precision float. */
typedef double              vl_double ;  /**< double precision float. */

typedef int                 vl_bool ;    /**< boolean */
/** @} */

/** @brief Big integer */
#define VL_BIG_INT    2147483647

/** @brief Small integer */
#define VL_SMALL_INT  (- VL_BIG_INT - 1)

/** ---------------------------------------------------------------- */
/** @name Error handling 
 ** @{ 
 **/

/** @brief Last error code */
extern int vl_err_no ;

/** @brief Error description maximum length 
 **
 ** This is the size of the ::vl_err_msg buffer.
 **/
#define VL_ERR_MSG_LEN 1024

/** @brief Last error description */
extern char vl_err_msg [VL_ERR_MSG_LEN] ;

#define VL_ERR_OK       0  /**< No error */
#define VL_ERR_OVERFLOW 1  /**< Buffer overflow error */
#define VL_ERR_ALLOC    2  /**< Resource allocation error */
#define VL_ERR_BAD_ARG  3  /**< Bad argument error */
#define VL_ERR_IO       4  /**< Input/output error */
#define VL_ERR_NO_MORE  5  /**< Sequence exhausted */

/** @} */

/** ---------------------------------------------------------------- */
/** @name Common operations
 ** @{
 **/

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

/** ---------------------------------------------------------------- */

vl_bool      vl_is_big_endian () ;
char const * vl_get_version_string () ;

/* VL_GENERIC_H */
#endif
