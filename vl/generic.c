/** @file    generic.c
 ** @author  Andrea Vedaldi
 ** @brief   Generic - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

/** @mainpage VisionLab Features Library

 @version __VLFEAT_VERSION__
 @author Andrea Vedaldi  (vedaldi@cs.ucla.edu)
 @author Brian Fulkerson (bfulkers@cs.ucla.edu)
 
 @par Copyright &copy; 2007 Andrea Vedaldi and Brian Fulkerson

 <em>VLFeat C library contains implementations of common computer
 vision algorithms, with a special focus on visual features for
 matching image regions. Applications include structure from motion and
 object and category detection and recognition.

 The library is accurate and easy to use, understand and modify. In
 order to maximize simplicity, the library is free of clutter and
 different algorithms are only weakly dependent. The code is portable
 (largely C-89 compatible) and embeds extensive documentation.</em>

 @section main-contents Contents

 - @ref design          "Design Concepts"
   - @ref design-objects     "Objects" 
   - @ref design-resources   "Memory and resource management" 
   - @ref design-threads     "Multi-threading"
   - @ref design-portability "Portability"
 - @ref main-glossary "Glossary"
 - Support functionalities
   - @ref generic.h   "Atomic data types, errors, and others"
   - @ref mathop.h    "Math operations"
   - @ref random.h    "Random number generator"
   - @ref stringop.h  "String operations"
   - @ref imop.h      "Image operations"
   - @ref rodrigues.h "Rodrigues formuals"
   - @ref mexutils.h  "MATLAB MEX helper functions"
 - Algorithms
   - @ref sift.h     "Scale Invariant Feature Transform (SIFT)"
   - @ref mser.h     "Maximally Stable Extremal Regions (MSER)"
   - @ref ikmeans.h  "Integer K-means (IKM)"
   - @ref hikmeans.h "Hierarchical Integer K-means (HIKM)"
   - @ref aib.h      "Agglomerative Information Bottleneck (AIB)"

 @section design VLFeat Design Concepts

 VLFeat follows a simple but rigorous design that makes it portable
 and simple to use in conjunction with high level language such as
 MATLAB. This section illustrates and motivates the aspects of the
 design that are relevant to the users of the library. Most of the
 features discussed in here are implemented in the @ref generic.h
 module.

 @subsection design-objects Objects

 Most of VLFeat functionalities are implemented as opaque data
 structures, to which we refer as "objects". Typically, you create an
 object by means of a constructor function and dispose of it by means
 of a destructor function. The data member of the object should not be
 accessed directly, but by means of appropriate accessor functions
 (typically containing the @c _get and _set keywords in their names).

 @subsection design-resources Memory and Resource Management

 Resource management in VLFeat is minimal. In most cases, you can
 assume that VLFeat does not provide any resource management
 functionality at all. Objects or memory blocks allocated by the
 library but owned by the client must be explicitly disposed. The
 following rule helps identifying such blocks and objects:
 
 <b> The client owns a memory block or object if, and only if, it is
   returned by a library call containing the keywords @c _new or @c
   _copy, or by the allocator functions ::vl_malloc, ::vl_calloc,
   ::vl_realloc.</b>
 
 More in detail, the following rules apply:

 - Memory is the only managed resource. Other resources used by the
   library (e.g. files) are either passed to the library by the
   client, or acquired and released within the scope of a library
   function call.
 - The memory manager can be customized through ::vl_set_alloc_func
   (which sets the implementations of ::vl_malloc, ::vl_realloc,
   ::vl_calloc and ::vl_free). The library allocates memory only through
   these functions.
 - The memory manager is global to all threads.
 - At any moment, there is only one memory manager in
   existence. ::vl_set_alloc_func can be used only before any other
   function is invoked, or right before or after the previous memory
   manager has relinquished all memory.
 - Such rules apply both to the library client and to the library
   implementation. The only exception regards the memory allocated in
   the global library state, which uses the native memory manager.

  These rules make it possible to undo all the work done by the
  library at any given instant. Disposing the memory allocated by the
  custom memory manager essentially "resets" the library (except for
  its global state). This functionality is used extensively in the
  implementation of MATLAB MEX functions that access the library to
  support abrupt interruption.

   @note The rules guarantee that all memory allocated by the library
     at any given time is allocated by the same memory manager, except
     for the global state that should survive the "reset"
     operation. In order to support multiple memory managers, one
     should keep track of the allocator of each object (or memory
     block). Moreover, partial de-allocation of a pool of objects is
     dangerous, as such objects may be referred by other objects that
     are not being de-allocated, corrupting their state. A proper
     solution to the latter problem is the retain/release mechanism
     implemented, for instance, by Apple Core Foundation or Cocoa.

  @subsection design-threads Multi-threading

  The library is currently <em>not</em> thread safe, but this support
  will be added in a future release.

  The library is almost entirely reentrant. The only thread-sensitive
  operations are on the global library state and are limited to:

  - Global library configuration (e.g. ::vl_set_alloc_func).
  - Random number generator state (@ref random.h).
  - Error handling (e.g. ::vl_err_no).

  @subsection design-portability Portability features

  Most host dependent details are isolated in the @ref generic.h
  library modules. These include:

  - Portable atomic types (e.g. ::vl_int32).
  - Declaration of symbols, inline functions (e.g. ::VL_EXPORT).
  - Host endianness conversion (e.g. ::vl_swap_host_big_endianness_8()).

  @section main-glossary Glossary

  - <b>Column-major.</b> A <em>M x N </em> matrix <em>A</em> is
  stacked with column-major order as the sequence \f$(A_{11}, A_{21},
  \dots, A_{12}, \dots)\f$. More in general, when stacking a multi
  dimensional array this indicates that the first index is the one
  varying most quickly, with the other followed in the natural order.
  - <b>Opaque structure.</b> A structure is opaque if the user is not supposed
  to access its member directly, but through appropriate interface functions.
  Opaque structures are commonly used to define objects.
  - <b>Row-major.</b> A <em>M x N </em> matrix <em>A</em> is
  stacked with row-major order as the sequence \f$(A_{11}, A_{12},
  \dots, A_{21}, \dots)\f$. More in general, when stacking a multi
  dimensional array this indicates that the last index is the one
  varying most quickly, with the other followed in reverse order.

**/

/** @file   generic.h
 ** @author Andrea Vedaldi
 ** @brief  Generic

This module provides basic functionalities:

- @ref generic-data-models
- @ref generic-error  
- @ref generic-heap
- @ref generic-logging

@section generic-data-models Data models

VLFeat's main target is to support common UNIX and Windows
architectures. The data model of these architectures differ mainly
in:

- <em>endianness</em> and
- <em>size of the atomic data types</em>.

An host is big endian or little endian depending how multi-byte
data values are stored in memory:

- <em>big endian</em> (big end first, also known as network order) if it
  stores the most significant byte has the smaller memory address.
- <em>little endian</em> (small end first, used by x86 processors) if it
  stores the lesser significant byte at the smaller memory address.

Use the function ::vl_get_endianness() to detect endianness.  To
convert data from/to the host to/from big endian (network) order, use the
functions ::vl_swap_host_big_endianness_8(), ::vl_swap_host_big_endianness_4(),
::vl_swap_host_big_endianness_2() (if the host is already big endian, these
 functions simply copy the data).
 
Hosts also differ in the size of the atomic data type (such as
@c short, @c int, @c long and so on):

 <table><caption>32-bit and 64-bit data models</caption>
 <tr style="font-weight:bold;">
   <td>Data model</td>
   <td >short</td>
   <td>int</td>
   <td>long</td>
   <td>long long</td>
   <td>pointers</td>
   <td>architecture</td>
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
 
Most 32-bit architectures are equivalent for the size of
the atomic data types. 64-bit architectures vary more, but for the 
atomic data types defined by VLFeat, the only important difference
between architectures is in practice limited the size of 
<code>long</code> and the size of the pointers.

@note For uniformity, VLFeat introduces appropriate atomic data types of
fixed width.  Notice that in C-99 the <code>stdint.h</code> header
could be used for this purpose instead, but unfortunately it is not
supported by Microsoft Visual C/C++.

 @section generic-symbols Exported symbols and inline functions

 The library must rely on non-standard features of the C compiler to
 support dynamic linking and inline functions in a portable way. Such
 details are hidden by the following macros:

 - ::VL_EXPORT declares an API symbols. The compiler and linker make
    sure that these symbols (and, usually, no other symbol) are
    visible to the library client.    
 - ::VL_INLINE declares an inline function.

<table>
<caption>Platform-dependent support for dynamic linking</caption>
<tr>
<td>Platform</td>
<td>Macro</td>
<td>Building</td>
<td>Importing</td>
</tr>
<tr>
<td>Unix/GCC</td>
<td>::VL_EXPORT</td>
<td>empty (assumes <c>-visibility=hidden</c> GCC option)</td>
<td><c>__attribute__((visibility ("default")))</c></td>
</tr>
<tr>
<td>Win/Visual C++</td>
<td>::VL_EXPORT</td>
<td>@c __declspec(dllexport)</td>
<td>@c __declspec(dllimport)</td>
</tr>
</table>

<table>
<caption>Platform-dependent support for inline functions</caption>
<tr>
<td>Platform</td>
<td>Macro</td>
<td>Implementation</td>
</tr>
<tr>
<td>Unix/GCC</td>
<td>::VL_INLINE</td>
<td>static inline</td>
</tr>
<tr>
<td>Win/Visual C++</td>
<td>::VL_INLINE</td>
<td>static __inline</td>
</tr>
</table>

@section generic-error Error handling

Error handling uses the same style of the standard C library. Most
functions return 0 when they succeed and -1 when they fail, and
set the global variable ::vl_err_no with a code identifying the
error occurred. This variable is never set on success and should
be examined right after an error had occurred.

@section generic-heap Heap allocation

VLFeat uses the ::vl_malloc(), ::vl_realloc(), ::vl_calloc() and
::vl_free() functions to allocate the heap. Normally these functions
are mapped to the underlying standard C library implementations. However
::vl_set_alloc_func() can be used to map them to other implementations.
For instance, in MATALB MEX files these functions are mapped to 
the MATLAB equivalent which has a garbage collection mechanism to cope
with interruptions during execution.

@section generic-logging Logging

VLFeat uses the macros ::VL_PRINT and ::VL_PRINTF to print progress or
debug informations. These functions are normally mapped to the @c
printf function of the underlying standard C library. However
::vl_set_printf_func() can be used to map it to a different
implementation. For instance, in MATLAB MEX files this function is
mapped to @c mexPrintf. Setting the function to @c NULL disables
logging.


**/

#include "generic.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

VL_EXPORT int vl_err_no = 0 ;
VL_EXPORT char vl_err_msg [VL_ERR_MSG_LEN] = "" ;

/** ------------------------------------------------------------------
 ** @brief Get version string
 ** @return library version string
 **/

VL_EXPORT
char const *
vl_get_version_string ()
{
  return VL_VERSION_STRING 
    " (" 
#if   VL_ENDIANNESS == VL_BIG_ENDIAN
    "big endian"
#elif VL_ENDIANNESS == VL_LITTLE_ENDIAN
    "little endian"
#else
    "unkown endianness"
#endif
    ")" ;
}

/** @internal@brief A printf that does not do anything */
static int
do_nothing_printf (char const* format, ...)
{
  return 0 ;
}

/** @internal@brief Customizable @c malloc function pointer */
void *(*vl_malloc_func)  (size_t)          = &malloc ;
                                            
/** @internal@brief Customizable @c realloc function pointer */
void *(*vl_realloc_func) (void*,size_t)    = &realloc ;

/** @internal@brief Customizable @c calloc function pointer */
void *(*vl_calloc_func)  (size_t, size_t) = &calloc ;

/** @internal@brief Customizable @c free function pointer */
void  (*vl_free_func)    (void*)            = &free ;
                                            
/** @internal@brief Customizable @c printf function pointer */
int   (*vl_printf_func)  (char const *, ...)=  printf ; /* &do_nothing_printf ;*/

/** ------------------------------------------------------------------
 ** @brief Set memory allocation functions
 ** @param malloc_func  pointer to @c malloc.
 ** @param realloc_func pointer to @c realloc.
 ** @param calloc_func  pointer to @c calloc.
 ** @param free_func    pointer to @c free.
 **/

VL_EXPORT
void vl_set_alloc_func (void *(*malloc_func)  (size_t),
                        void *(*realloc_func) (void*, size_t),
                        void *(*calloc_func)  (size_t, size_t),
                        void  (*free_func)    (void*))
{
  vl_malloc_func  = malloc_func ;
  vl_realloc_func = realloc_func ;
  vl_calloc_func  = calloc_func ;
  vl_free_func    = free_func ;
}

/** ------------------------------------------------------------------
 ** @brief Set printf function
 ** @param printf_func  pointer to @c printf.
 ** Let @c print_func be NULL to disable printf.
 **/

VL_EXPORT
void 
vl_set_printf_func (int(*printf_func) (char const *format, ...))
{
  vl_printf_func  = printf_func ? printf_func : do_nothing_printf ;
}
