/** @file    host.c
 ** @author  Andrea Vedaldi
 ** @brief   Host - Definition
 **/

/* AUTORIGHTS
 Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
 
 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include "host.h"
#include "generic.h"

/** 
 @file host.h
 
 This module provides functionalities to identify the 
 host operating system, C compiler,
 and CPU architecture. It also provides a few features
 to abstract from such details.
 
 @section host-compiler Host compiler 
 
 For each supported compiler, this module defines a corresponding
 symbol (e.g. ::VL_COMPILER_MSC, ::VL_COMPILER_GNUC) whose
 integer value is equal to the version of the compiler. It also defines
 one of ::VL_COMPILER_ILP32, ::VL_COMPILER_LP32, ::VL_COMPILER_LP64
 according to the compiler data model and defines the a set of atomic data types
 (::vl_int8, ::vl_int16, ::vl_int32, etc) of given width.

 @remark While some of such functionalities can be found in the 
 standard header @c stdint.h, this unfortunately is not supported
 by all platforms.
  
 The C language defines a number of atomic data types (such as @c char, @c short,
 @c int and so on). Unfortunately the number of bits (width)
 used to represent each data type depends on the compiler data model.
 The following table summarizes the relevant conventions:
 
 <table><caption><b>Compiler data models.</b> The table shows
 how many bits are allocated to each tomic data type according to
 each model.</caption>
 <tr style="font-weight:bold;">
 <td>Data model</td>
 <td><code>short</code></td>
 <td><code>int</code></td>
 <td><code>long</code></td>
 <td><code>long long</code></td>
 <td><code>void*</code></td>
 <td>Compiler</td>
 </tr>
 <tr>
 <td>ILP32</td>
 <td style="background-color:#ffa;">16</td>
 <td style="background-color:#afa;">32</td>
 <td style="background-color:#afa;">32</td>
 <td >64</td>
 <td style="background-color:#afa;">32</td>
 <td>common 32 bit architectures</td>
 </tr>
 <tr>
 <td>LP64</td>
 <td style="background-color:#ffa;">16</td>
 <td style="background-color:#afa;">32</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td>UNIX-64 (Linux, Mac OS X)</td>
 </tr>
 <tr>
 <td>ILP64</td>
 <td style="background-color:#ffa;">16</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td>Alpha, Cray</td>
 </tr>
 <tr>
 <td>SLIP64</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td>64</td>
 <td></td>
 </tr>
 <tr>
 <td>LLP64</td>
 <td style="background-color:#ffa;">16</td>
 <td style="background-color:#afa;">32</td>
 <td style="background-color:#afa;">32</td>
 <td>64</td>
 <td>64</td>
 <td>Windows-64</td>
 </tr>
 </table>
 
 @section host-arch Host CPU architecture
 
 For each supported architecture this module defines a corresponding
 symbol (e.g. ::VL_ARCH_IX86, ::VL_ARCH_IA64).

 @subsection host-arch-endianness CPU endianness
 
 The macros ::VL_ARCH_BIG_ENDIAN and ::VL_ARCH_LITTLE_ENDIAN
 can be used to detect the architecture endianness
 the host endianness and the functions 
 ::vl_swap_host_big_endianness_8(), ::vl_swap_host_big_endianness_4(),
 ::vl_swap_host_big_endianness_2() to change the endianness
 of data (from/to host and network order).
 
 <b>Endianness</b> concerns how multi-byte data types 
 (such as 16, 32 and 64 bits
 integres) are stored into the addressable memory. 
 All CPUs uses a continguous address range to store atomic data types
 (e.g. a 16-bit integer could
 be assigned to the addresses <c>0x10001</c> and <c>0x10002</c>), but
 the order may differ.
  
 - The convention is <em>big endian</em>, or in <em>network order</em>,
 if the most significant byte of the multi-byte data type is assigned to 
 the smaller memory address. This is the convention used for instance
 by the PPC architecture.
 
 - The convention is <em>little endian</em> if  the least significant
 byte is assigned to the smaller memory address. This is the convention
 used for instance by the x86 architecture.
 
 @remark The names &ldquo;big endian&rdquo; and &ldquo;little endian&rdquo; are 
 a little confusing. &ldquo;Big endian&rdquo; means &ldquo;big endian first&rdquo;, i.e.
 the address of the most significant byte comes first. Similarly,
 &ldquo;little endian&rdquo; means &ldquo;little endian first&rdquo;, 
 in the sense that the address of the least significant byte comes first.

 Endianness becomes a concern when data is exchangend with processors
 that use different conventions, 
 transmitted over a network, or stored
 to a file. For the latter two cases, one usually saves data in 
 big endian (network) order regardless of the host CPU. 
 
 **/

/** @def VL_OS_LINUX  
 ** @brief Defined if the host operating system is Linux. 
 **/

/** @def VL_OS_MACOSX 
 ** @brief Defined if the host operating system is Mac OS X.
 **/

/** @def VL_OS_WIN  
 ** @brief Defined if the host operating system is Windows (32 or 64)
 **/

/** @def VL_OS_WIN64  
 ** @brief Defined if the host operating system is Windows-64.
 **/

/** @def VL_COMPILER_GNUC 
 ** @brief Defined if the host compiler is GNU C.
 **
 ** This macro is defined if the compiler is GNUC.
 ** Its value is caluclated as
 ** @code
 ** 10000 * MAJOR + 100 * MINOR + PATCHLEVEL
 ** @endcode
 **/

/** @def VL_COMPILER_MSC
 ** @brief Defined if the host compiler is Microsoft Visual C++.
 **/

/** @def VL_ARCH_IX86
 ** @brief Defined if the host CPU is of the Intel x86 family.
 **/

/** @def VL_ARCH_IA64
 ** @brief Defined if the host CPU is of the Intel Architecture-64 family.
 **/

/** @def VL_INLINE
 ** @brief Adds appropriate inline function qualifier
 ** @sa main-style-inline
 **/

/** @def VL_EXPORT
 ** @brief Declares a DLL exported symbol
 ** @sa @ref generic-symbols
 **/

/** --------------------------------------------------------------- */

#if defined(VL_ARCH_IX86) || defined(VL_ARCH_IA64)
#define HAS_CPUID
#endif

#if defined(HAS_CPUID) & defined(VL_COMPILER_MSC)
#include <intrin.h>
VL_INLINE
void _vl_cpuid (vl_int32* info, int function) 
{
  __cpuid(info, function) ;
}
#endif

#if defined(HAS_CPUID) & defined(VL_COMPILER_GNUC)
VL_INLINE
void _vl_cpuid (vl_int32* info, int function)
{
  /* this version is compatible with -fPIC */
  __asm__ __volatile__ 
  ("pushl %%ebx      \n\t" /* save %ebx */
   "cpuid            \n\t"
   "movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
   "popl %%ebx       \n\t" /* restore the old %ebx */
   : "=a"(info[0]), "=r"(info[1]), "=c"(info[2]), "=d"(info[3])
   : "a"(function)
   : "cc") ;
}
#endif

#ifdef HAS_CPUID
struct x86cpu_
{
  char vendor_string [0x20] ;
  vl_bool has_sse42 ;
  vl_bool has_sse41 ;
  vl_bool has_sse3 ;
  vl_bool has_sse2 ;
  vl_bool has_sse ;
  vl_bool has_mmx ;
} x86cpu ;

vl_bool x86cpu_initialized = 0 ;

void _vl_x86cpu_init () 
{
  vl_int32 info [4] ;
  int max_func = 0 ;
  _vl_cpuid(info, 0) ;
  max_func = info[0] ;
  *((vl_int32*)x86cpu.vendor_string+0) = info[1] ;
  *((vl_int32*)x86cpu.vendor_string+1) = info[3] ;
  *((vl_int32*)x86cpu.vendor_string+2) = info[2] ;

  if (max_func >= 1) {
    _vl_cpuid(info, 1) ;
    x86cpu.has_mmx   = info[3] & (1 << 23) ;
    x86cpu.has_sse   = info[3] & (1 << 25) ;
    x86cpu.has_sse2  = info[3] & (1 << 26) ;
    x86cpu.has_sse3  = info[2] & (1 <<  0) ;
    x86cpu.has_sse41 = info[2] & (1 << 19) ;
    x86cpu.has_sse42 = info[2] & (1 << 20) ;
  }
}

VL_INLINE
struct x86cpu_ const* _vl_x86cpu_get() 
{
  if (!x86cpu_initialized) _vl_x86cpu_init() ;
  return &x86cpu ;
}
#endif
 
/** --------------------------------------------------------------- */

vl_bool simd_enabled = 1 ;

/** @brief Enalbe/disable usage of SIMD instructions
 ** @param x if @c true SIMD instructions are used (if available).
 **/
void vl_set_simd_enabled (vl_bool x)
{
  simd_enabled = x ;
}

/** @brief Are SIMD instructons enabled?
 ** @return @c true is SIMD is enabled.
 **/
vl_bool vl_get_simd_enabled ()
{
  return simd_enabled ;
}

/** @brief Check for SSE3 instruction set
 ** @return @c true if SSE3 is present.
 **/
vl_bool vl_cpu_has_sse3 ()
{
#ifdef HAS_CPUID
  return _vl_x86cpu_get()->has_sse3 ;
#else
  return 0 ;
#endif
}

/** @brief Check for SSE2 instruction set
 ** @return @c true if SSE2 is present.
 **/
vl_bool vl_cpu_has_sse2 ()
{
#ifdef HAS_CPUID
  return _vl_x86cpu_get()->has_sse2 ;
#else
  return 0 ;
#endif
}

/** ------------------------------------------------------------------
 ** @brief Print host information
 **/

void vl_print_host_info ()
{
  char const *arch = 0, *endian = 0, *comp = 0, *dm = 0 ;
  int compver ;
#ifdef VL_ARCH_IA6
  arch = "IA64" ;
#endif
#ifdef VL_ARCH_IX86
  arch = "IX86" ;
#endif
#ifdef VL_ARCH_PPC
  arch = "PPC" ;
#endif
  
#ifdef VL_ARCH_BIN_ENDIAN
  endian = "big endian" ;
#endif
#ifdef VL_ARCH_LITTLE_ENDIAN
  endian = "little endian" ;
#endif
  
#ifdef VL_COMPILER_MSC
  comp = "Microsoft Visual C++" ;
  compver = VL_COMPILER_MSC ;
#endif
#ifdef VL_COMPILER_GNUC
  comp = "GNU C" ;
  compver = VL_COMPILER_GNUC ;
#endif

#ifdef VL_COMPILER_LP64
  dm = "LP64" ;
#endif
#ifdef VL_COMPILER_LLP64
  dm = "LP64" ;
#endif
#ifdef VL_COMPILER_ILP32
  dm = "ILP32" ;
#endif
  
#define YESNO(x) ((x)?"yes":"no")

  VL_PRINTF("Host: Compiler: %s %d\n", comp, compver) ;
  VL_PRINTF("      Compiler data model: %s\n", dm) ;
  VL_PRINTF("      CPU architecture: %s\n", arch) ;
  VL_PRINTF("      CPU endianness: %s\n", endian) ;
  
#ifdef HAS_CPUID
  {
    struct x86cpu_ const* c = _vl_x86cpu_get() ;
    VL_PRINTF("      CPU vendor string: %s\n", c->vendor_string) ;
    VL_PRINTF("      CPU has MMX: %s\n",    YESNO(c->has_mmx)) ;
    VL_PRINTF("      CPU has SSE: %s\n",    YESNO(c->has_sse)) ;
    VL_PRINTF("      CPU has SSE2: %s\n",   YESNO(c->has_sse2)) ;
    VL_PRINTF("      CPU has SSE3: %s\n",   YESNO(c->has_sse3)) ;
    VL_PRINTF("      CPU has SSE4.1: %s\n", YESNO(c->has_sse41)) ;
    VL_PRINTF("      CPU has SSE4.2: %s\n", YESNO(c->has_sse42)) ;
    VL_PRINTF("VLFeat uses SIMD: %s\n", YESNO(vl_get_simd_enabled())) ;
  }        
#endif
}



