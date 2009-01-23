/** @file    host.h
 ** @author  Andrea Vedaldi
 ** @brief   Host
 **/

#ifndef VL_HOST_H
#define VL_HOST_H

/* 
 The following macros identify the host OS, architecture and compiler. 
 They are derived from http://predef.sourceforge.net/ 
 */

/** @name Identifying the host operating system
 ** @{ */
#if defined(linux)     || \
    defined(__linux)   || \
    defined(__linux__) || \
    defined(__DOXYGEN__)
#define VL_OS_LINUX 1
#endif

#if (defined(__APPLE__) & defined(__MACH__)) || \
     defined(__DOXYGEN__)
#define VL_OS_MACOSX 1
#endif

#if defined(__WIN32__) || \
    defined(_WIN32)    || \
    defined(__DOXYGEN__)
#define VL_OS_WIN 1
#endif

#if defined(_WIN64) || \
    defined(__DOXYGEN__)
#define VL_OS_WIN64 1
#endif
/** @} */

/** @name Identifying the host compiler
 ** @{ */
#if defined(__GNUC__) || defined(__DOXYGEN__)
# if defined(__GNUC_PATCHLEVEL__)
#  define VL_COMPILER_GNUC (__GNUC__ * 10000 \
+ __GNUC_MINOR__ * 100 \
+ __GNUC_PATCHLEVEL__)
# else
#  define VL_COMPILER_GNUC (__GNUC__ * 10000 \
+ __GNUC_MINOR__ * 100)
# endif
#endif

#if defined(_MSC_VER) || defined(__DOXYGEN__)
#define VL_COMPILER_MSC _MSC_VER
#endif

#if defined(__LCC__) || defined(__DOXYGEN__)
#warning "LCC support is experimental!"
#define VL_COMPILER_LCC 1
#endif

/** @} */

/** @name Identifying the host CPU architecture
 ** @{ */
#if defined(i386)     || \
    defined(__i386__) || \
    defined(__DOXYGEN__)
#define VL_ARCH_IX86 300
#elif defined(__i486__)
#define VL_ARCH_IX86 400
#elif defined(__i586__)
#define VL_ARCH_IX86 500
#elif defined(__i686__)
#define VL_ARCH_IX86 600
#elif defined(_M_IX86)
#define VL_ARCH_IX86 _M_IX86
#endif

#if defined(__ia64__) || \
    defined(_IA64)    || \
    defined(__IA64)   || \
    defined(__ia64)   || \
    defined(_M_IA64)  || \
    defined(__DOXYGEN__)
#define VL_ARCH_IA64
#endif
/** @} */

/** @name Identifying the host data model
 ** @{ */
#if defined(__LLP64__) || \
    defined(__LLP64)   || \
    defined(__LLP64)   || \
    (defined(VL_COMPILER_MSC) & defined(VL_OS_WIN64)) || \
    (defined(VL_COMPILER_LCC) & defined(VL_OS_WIN64)) || \
    defined(__DOXYGEN__)
#define VL_COMPILER_LLP64
#endif

#if defined(__LP64__) || \
    defined(__LP64)   || \
    defined(__LP64)   || \
    (defined(VL_OS_MACOSX) & defined(VL_ARCH_IA64)) || \
    defined(__DOXYGEN__)
#define VL_COMPILER_LP64
#endif

#if (!defined(VL_COMPILER_LLP64) & !defined(VL_COMPILER_LP64)) || \
    defined(__DOXYGEN__)
#define VL_COMPILER_ILP32
#endif
/** @} */

/** @name Identifying the host endianness
 ** @{ */
#if defined(__LITTLE_ENDIAN__) || \
    defined(VL_ARCH_IX86)      || \
    defined(VL_ARCH_IA64)      || \
    defined(__DOXYGEN__)
#define VL_ARCH_LITTLE_ENDIAN
#endif

#if  defined(__DOXYGEN__) || \
    !defined(VL_ARCH_LITTLE_ENDIAN)
#define VL_ARCH_BIG_ENDIAN
#endif
/** @} */

#if defined(VL_COMPILER_MSC)
#define VL_INLINE static __inline
#define snprintf _snprintf
#define isnan _isnan
#ifdef VL_BUILD_DLL
#define VL_EXPORT __declspec(dllexport)
#else
#define VL_EXPORT __declspec(dllimport)
#endif
#endif

#if defined(VL_COMPILER_LCC)
#define VL_INLINE static __inline
#define snprintf _snprintf
#define isnan _isnan
#ifdef VL_BUILD_DLL
#define VL_EXPORT __declspec(dllexport)
#else
#define VL_EXPORT 
#endif
#endif

#if defined(VL_COMPILER_GNUC) || \
    defined(__DOXYGEN__)
#define VL_INLINE static __inline__
#ifdef VL_BUILD_DLL
#define VL_EXPORT __attribute__((visibility ("default")))
#else
#define VL_EXPORT
#endif
#endif

/** ------------------------------------------------------------------
 ** @name Atomic data types
 ** @{
 **/

#define VL_TRUE 1   /**< @brief @c true (1) constant */
#define VL_FALSE 0  /**< @brief @c false (0) constant */

#if defined(VL_COMPILER_LP64) || defined(VL_COMPILER_LLP64)
typedef long long           vl_int64 ;   /**< @brief Signed 64-bit integer. */
typedef int                 vl_int32 ;   /**< @brief Signed 32-bit integer. */
typedef short               vl_int16 ;   /**< @brief Signed 16-bit integer. */
typedef char                vl_int8  ;   /**< @brief Signed  8-bit integer. */

typedef long long unsigned  vl_uint64 ;  /**< @brief Unsigned 64-bit integer. */
typedef int       unsigned  vl_uint32 ;  /**< @brief Unsigned 32-bit integer. */
typedef short     unsigned  vl_uint16 ;  /**< @brief Unsigned 16-bit integer. */
typedef char      unsigned  vl_uint8 ;   /**< @brief Unsigned  8-bit integer. */

typedef int                 vl_int ;     /**< @brief Same as @c int. */
typedef unsigned int        vl_uint ;    /**< @brief Same as <code>unsigned int</code>. */

typedef int                 vl_bool ;    /**< @brief Boolean. */
typedef vl_int64            vl_intptr ;  /**< @brief Integer holding a pointer. */
typedef vl_uint64           vl_uintptr ; /**< @brief Unsigned integer holding a pointer. */
#endif

#if defined(VL_COMPILER_ILP32)

#ifdef VL_COMPILER_MSC
typedef __int64             vl_int64 ;  
#else
typedef long long           vl_int64 ;  
#endif

typedef int                 vl_int32 ;  
typedef short               vl_int16 ;  
typedef char                vl_int8  ;  

#ifdef VL_COMPILER_MSC
typedef __int64   unsigned  vl_uint64 ; 
#else
typedef long long unsigned  vl_uint64 ; 
#endif
typedef int       unsigned  vl_uint32 ; 
typedef short     unsigned  vl_uint16 ; 
typedef char      unsigned  vl_uint8 ;  

typedef int                 vl_int ;    
typedef unsigned int        vl_uint ;   

typedef int                 vl_bool ;   
typedef vl_int32            vl_intptr ; 
typedef vl_uint32           vl_uintptr ;
#endif
/** @} */

/** ------------------------------------------------------------------
 ** @name Printing the atomic data types
 ** @{ */


/** @def VL_FL_INT64 
 ** @brief @c prinf length flag for ::vl_int64 and ::vl_uint64. 
 **/

/** @def VL_FL_INT32 
 ** @brief @c prinf length flag for ::vl_int32 and ::vl_uint32. 
 **/

/** @def VL_FL_INT16 
 ** @brief @c prinf length flag for ::vl_int16 and ::vl_uint16. 
 **/

/** @def VL_FL_INT8  
 ** @brief @c prinf length flag for ::vl_int8 and ::vl_uint8.  
 **/
#ifdef VL_COMPILER_MSC
#define VL_FL_INT64  "I64"
#else
#define VL_FL_INT64  "ll"
#endif
#define VL_FL_INT32  ""
#define VL_FL_INT16  "h"
#define VL_FL_INT8   "hh"
/** @} */

/** ------------------------------------------------------------------
 ** @name Atomic data types limits
 ** @{ */

/** @brief Largest integer (math constant) */
#define VL_BIG_INT  0x7FFFFFFFL

/** @brief Smallest integer (math constant) */
#define VL_SMALL_INT  (- VL_BIG_INT - 1)

/** @brief Largest unsigned integer (math constant) */
#define VL_BIG_UINT 0xFFFFFFFFUL

/** @} */

/** ------------------------------------------------------------------
 ** @name Endianness detection and conversion
 ** @{
 **/
VL_INLINE void vl_swap_host_big_endianness_8 (void *dst, void* src) ;
VL_INLINE void vl_swap_host_big_endianness_4 (void *dst, void* src) ;
VL_INLINE void vl_swap_host_big_endianness_2 (void *dst, void* src) ;
/** @} */

/** ------------------------------------------------------------------
 ** @name Obtaining host info at run time
 ** @{ */
VL_EXPORT void vl_print_host_info() ;
VL_EXPORT vl_bool vl_cpu_has_sse3 () ;
VL_EXPORT vl_bool vl_cpu_has_sse2 () ;
/** @} */

VL_EXPORT void vl_set_simd_enabled (vl_bool x) ;
VL_EXPORT vl_bool vl_get_simd_enabled() ;

/** ------------------------------------------------------------------
 ** @brief Host <-> big endian transformation for 8-bytes value
 **
 ** @param dst destination 8-byte buffer.
 ** @param src source 8-byte bufffer.
 ** @see @ref host-arch-endianness.
 **/

VL_INLINE void
vl_swap_host_big_endianness_8 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if defined(VL_ARCH_BIG_ENDIAN)
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

/** ------------------------------------------------------------------
 ** @brief Host <-> big endian transformation for 4-bytes value
 **
 ** @param dst destination 4-byte buffer.
 ** @param src source 4-byte bufffer.
 ** @sa @ref host-arch-endianness.
 **/

VL_INLINE void
vl_swap_host_big_endianness_4 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if defined(VL_ARCH_BIG_ENDIAN)
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

/** ------------------------------------------------------------------
 ** @brief Host <-> big endian transformation for 2-bytes value
 **
 ** @param dst destination 2-byte buffer.
 ** @param src source 2-byte bufffer.
 ** @see @ref host-arch-endianness.
 **/

VL_INLINE void
vl_swap_host_big_endianness_2 (void *dst, void* src)
{
  char *dst_ = (char*) dst ;
  char *src_ = (char*) src ;
#if defined(VL_ARCH_BIG_ENDIAN)
    dst_ [0] = src_ [0] ;
    dst_ [1] = src_ [1] ;
#else
    dst_ [0] = src_ [1] ;
    dst_ [1] = src_ [0] ;
#endif
}

/* VL_HOST_H */
#endif
