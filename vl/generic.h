/** @file    generic.h
 ** @author  Andrea Vedaldi
 ** @brief   Generic - Declaration
 **
 ** This modules provides basic facilities such as basic types, error
 ** handling, endianness detection and serialization.
 **
 ** @section generic-error Error handling
 **
 ** Error handling uses the same style of the standard C library. Most
 ** functions return 0 when they succeed and -1 when they fail, and
 ** set the global variable ::vl_err_no with a code identifying the
 ** error occurred. This variable is never set on success and should
 ** be examinated right after an error occurred.
 **
 ** @section generic-endian Endinanness detection and conversions
 **
 ** An architecture is big endiand or little endian depending how
 ** multi-btye data are stored in memory:
 **
 ** - <em>big endian</em> (big end first - Network order) if it
 **   stores the most significant byte has the smaller memory address.
 ** - <em>little endian</em> (small end first - Intel format) if it
 **   stores the lesser significant byte at the smaller memory address.
 **
 ** Use the function ::vl_get_endianness() to detect endianness.  To
 ** serialize/deserialize data in big endian (network) order, call the
 ** functions ::vl_adapt_endianness_8(), ::vl_adapt_endianness_4(),
 ** ::vl_adapt_endianness_2() after reading and before writing (the
 ** functions change edinanness only if the architecture is little
 ** endian and do nothing otherwise).
 **/

/* AUTORIGHTS */

#ifndef VL_GENERIC_H
#define VL_GENERIC_H

/** @brief Library version string */
#define VL_VERSION_STRING "0.1"

/** @brief Inline function declaration */
#define VL_INLINE __inline__

/** @internal @brief Stringify argument helper
 ** @see ::VL_STRINGIFY
 **/
#define VL_STRINGIFY_(x) # x

/** @brief Stringify macro
 **
 ** @param x macro to stringify
 **
 ** For instance, the following fragment
 ** @code
 ** #define A x.y.z
 ** printf(VL_STRINGIFY(A)) ;
 ** @endcode
 ** will print the string @c x.y.z.
 **/
#define VL_STRINGIFY(x) VL_STRINGIFY_(x)

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

typedef int                 vl_int ;     /**< integer.                 */
typedef unsigned int        vl_uint ;    /**< unsigned integer.        */
typedef float               vl_single ;  /**< single precision float.  */
typedef double              vl_double ;  /**< double precision float.  */
typedef unsigned int        vl_uidx ;    /**< unsigned index data type.*/

typedef int                 vl_bool ;    /**< boolean */
/** @} */

/** @brief Big integer */
#define VL_BIG_INT    2147483647

/** @brief Small integer */
#define VL_SMALL_INT  (- VL_BIG_INT - 1)

/** @brief Logarithm of 2 */
#define VL_LOG_OF_2 0.693147180559945

/* 
   For the code below: An ANSI C compiler takes the two expressions,
   LONG_VAR and CHAR_VAR, and implicitly casts them to the type of the
   first member of the union. Refer to K&R Second Edition Page 148,
   last paragraph.
*/

/** @brief IEEE single quiet NaN constant */
static union { vl_uint32 raw ; vl_single value ; } 
  const vl_nan_f = 
    { 0x7FC00000UL } ;

/** @brief IEEE single infinity constant */
static union { vl_uint32 raw ; vl_single value ; } 
  const vl_infinity_f = 
    { 0x7F800000UL } ;

/** @brief IEEE double quiet NaN constant */
static union { vl_uint64 raw ; vl_double value ; } 
  const vl_nan_d = 
    { 0x7FF8000000000000ULL } ;

/** @brief IEEE double infinity constant */
static union { vl_uint64 raw ; vl_double value ; } 
  const vl_infinity_d = 
    { 0x7FF0000000000000ULL } ;

/** @brief Single NaN (not signaling) */
#define VL_NAN_F (vl_nan_f.value)

/** @brief Single Infinity (not signaling) */
#define VL_INFINITY_F (vl_infinity_f.value)

/** @brief Double NaN (not signaling) */
#define VL_NAN_D (vl_nan_d.value)

/** @brief Double Infinity (not signaling) */
#define VL_INFINITY_D (vl_infinity_d.value)

#ifdef __VISUALC__
#undef VL_INLINE
#define VL_INLINE __inline
#define fscanf fscanf_s
#define snprintf _snprintf
#define isnan _isnan
#endif

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
#define VL_ERR_BAD_ARG  3  /**< Bad argument or illegal data error */
#define VL_ERR_IO       4  /**< Input/output error */
#define VL_ERR_EOF      5  /**< End-of-file or end-of-sequence error */
#define VL_ERR_NO_MORE  5  /**< End-of-sequence @deprecated */

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

char const * vl_get_version_string () ;

/** @name Endianness detection and conversions
 ** @{
 **/

#define VL_LITTLE_ENDIAN 0  /**< little endian. */
#define VL_BIG_ENDIAN    1  /**< big endian. */
       
/** @def VL_ENDIANNESS
 ** @brief Host endianness.
 **
 ** This macro is equal to ::VL_BIG_ENDIAN or ::VL_LITTLE_ENDIAN
 ** depending on the endianness of the host.
 **/
#if                                                \
  defined(__LITTLE_ENDIAN__)                   ||  \
  defined(__i386__)  || defined(__ia64__)      ||  \
  defined(WIN32)     || defined(__x86_64)      ||  \
  defined(__alpha__) || defined(__alpha)       ||  \
  defined(__arm__)   || defined(__SYMBIAN32__) ||  \
  (defined(__mips__) && defined(__MIPSEL__)) 
#define VL_ENDIANNESS VL_LITTLE_ENDIAN
#else
#define VL_ENDIANNESS VL_BIG_ENDIAN
#endif

static VL_INLINE int  vl_get_endianness () ;
static VL_INLINE void vl_adapt_endianness_8 (void *dst, void* src) ;
static VL_INLINE void vl_adapt_endianness_4 (void *dst, void* src) ;
static VL_INLINE void vl_adapt_endianness_2 (void *dst, void* src) ;
/** @} */

/** ---------------------------------------------------------------- */
/** @brief Get endianness
 ** @return @c ::VL_BIG_ENDIAN or ::VL_LITTLE_ENDIAN depending on the
 ** host endianness.
 ** @sa @ref generic-endian
 **/

static VL_INLINE int
vl_get_endianness () 
{
  return VL_ENDIANNESS ;
}

/** ---------------------------------------------------------------- */
/** @brief Change endianness of 8-byte value if required
 **
 ** @param dst destination 8-byte buffer.
 ** @param src source 8-byte bufffer.
 ** @see @ref generic-endian.
 **/

static VL_INLINE void
vl_adapt_endianness_8 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if VL_ENDIANNESS == VL_BIG_ENDIAN
    dst_ [0] = src_ [0] ;
    dst_ [1] = src_ [1] ;
    dst_ [2] = src_ [2] ;
    dst_ [3] = src_ [3] ;
    dst_ [4] = src_ [4] ;
    dst_ [5] = src_ [5] ;
    dst_ [6] = src_ [6] ;
    dst_ [7] = src_ [7] ;
#else 
    dst_ [0] = src_ [7] ;
    dst_ [1] = src_ [6] ;
    dst_ [2] = src_ [5] ;
    dst_ [3] = src_ [4] ;
    dst_ [4] = src_ [3] ;
    dst_ [5] = src_ [2] ;
    dst_ [6] = src_ [1] ;
    dst_ [7] = src_ [0] ;
#endif
}

/** ---------------------------------------------------------------- */
/** @brief Change endianness of 4-byte value if required
 **
 ** @param dst destination 4-byte buffer.
 ** @param src source 4-byte bufffer.
 ** @sa @ref generic-endian
 **/

static VL_INLINE void
vl_adapt_endianness_4 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if VL_ENDIANNESS == VL_BIG_ENDIAN
    dst_ [0] = src_ [0] ;
    dst_ [1] = src_ [1] ;
    dst_ [2] = src_ [2] ;
    dst_ [3] = src_ [3] ;
#else 
    dst_ [0] = src_ [3] ;
    dst_ [1] = src_ [2] ;
    dst_ [2] = src_ [1] ;
    dst_ [3] = src_ [0] ;
#endif
}

/** ---------------------------------------------------------------- */
/** @brief Change endianness of 2-byte value if required
 **
 ** @param dst destination 2-byte buffer.
 ** @param src source 2-byte bufffer.
 ** @see generic-endianness.
 **/

static VL_INLINE void
vl_adapt_endianness_2 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if VL_ENDIANNESS == VL_BIG_ENDIAN
    dst_ [0] = src_ [0] ;
    dst_ [1] = src_ [1] ;
#else
    dst_ [0] = src_ [1] ;
    dst_ [1] = src_ [0] ;
#endif
}

/* VL_GENERIC_H */
#endif
