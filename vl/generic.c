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
@author Andrea Vedaldi  (mailto:vedaldi@cs.ucla.edu)
@author Brian Fulkerson (mailto:brian@cs.ucla.edu)

@par Copyright &copy; 2007 Andrea Vedaldi and Brian Fulkerson

<em>This library contains implementations of common computer vision
algorithms, with a special focus on visual features for matching image
regions. Applications include structure from motion, object and
category detection, object and category recognition.</em>

@section main-contents Contents

- @ref main                "Overview"
- @ref main-style          "Coding style"
  - @ref main-style-memory "Memory management" 
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

@section main Overview

We want VLFeat C library to provide solid implementations a small 
set of useful computer vision algorithms. Our 
target users are other computer vision researchers. The library aims to
be:

- <b>Accurate</b>. Algorithms should be well defined and (when
  possible) close to the description/implementation of the original
  authors. 
- <b>Easy to use</b>. The library should be easy to compile and
  setup. The user interface should be simple and flexible. The
  documentation should be clear and complete.
- <b>Open</b>. The code should be easy to read, understand and modify.
  It should also be easy to extract from the library a particular
  algorithm.

We think that the best way of achieving many of these results is to keep the library
 as simple and fruible as possible. The library is therefore designed to be:

- <b>Minimal</b>. The VLFeat C library implements only the core algorithms. 
  This reduce clutter, ease maintenance and make it possible to produce
  high quality code for the parts that really matter.
- <b>Flat</b>. The intra-dependencies among library modules are
  not deep.  Preferably, each algorithm depends only on the basic
  modules (IO, math, ...). This simplifies understanding and maintaining
  each algorithm, and make it possible to "copy and paste" code from
  the library to other projcets.
- <b>Portable</b>. The library adheres to the commom C standars 
  (mostly  C-89, with a few exceptions) and has no external dependencies.
  This makes much easier for new users to compile and modify the library.
- <b>Slef-documenting</b>. Documentation is embedded in the source code
  in Doxygen format. The documentation includes accurate
  descriptions of each algorithm.

@section main-style Coding conventions and style

This section describes the coding conventions that the library uses.

@subsection main-style-memory Memory management

Memory is allocated and freed by means of ::vl_malloc(), vl_free(),
::vl_realloc() functions, which are stubs pointing to the actual
function to use. So when the library is linked to a MATLAB MEX file,
it is possible to use MATLAB memory allocation functions instead of the
operating system ones. The advantage is that MATLAB memory handler
will free the allocated buffers in the case a MEX file is interrupted
(Ctl-C).

This also requires a bit of care to work as intended:

- The library must be thread safe.

- Objects must have allocation and de-allocation methods. Garbage collection 
  (e.g. MATLAB exiting a MEX file) can be performed only after 
  all objects have been deleted. So in effect garbage collection is useful
  only to clean-up after an interrupted computation.

- Static objects are not safe as they would violate the garbage collection rule.
  For instance, one should not alocate static buffers in functions, as
  the buffer could be stealed by sucessive garbage collection.

@section main-glossary Glossary

- <b>Column-major.</b> A <em>M x N </em> matrix <em>A</em> is
stacked with column-major order as the sequence \f$(A_{11}, A_{21},
\dots, A_{12}, \dots)\f$. More in general, when stacking a multi
dimensional array this indicates that the first index is the one
varying most quickly, with the other followed in the natural order.
- <b>Opaque structure.</b> A structure is opaque if its fields are not
meant to be accessed directly but by means of appropriate interface
functions. This techniuqe improves robustness and and isolates the
implementation from the interface, much in the spirit of
object-oriented programming.
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

- @ref generic-error  
- @ref generic-heap
- @ref generic-logging
- @ref generic-data-model


@warning Currently <code>generic.h</code> works correctly only with
ILP32 (all 32 bit), LP64 (UNIX-64) and LLP64 (Windows-64)
architectures.

@section generic-error Error handling

Error handling uses the same style of the standard C library. Most
functions return 0 when they succeed and -1 when they fail, and
set the global variable ::vl_err_no with a code identifying the
error occurred. This variable is never set on success and should
be examinated right after an error had occurred.

@section generic-heap Heap allocation

VLFeat uses the ::vl_malloc(), ::vl_realloc(), ::vl_calloc() and
::vl_free() functions to allocate the heap. Normally these functions
are mapped to the underlying standar C library implementations. However
::vl_set_alloc_func() can be used to map them to other implementations.
For instance, in MATALB MEX files these functions are mapped to 
the MATLAB equivalent, with the benefits of garbage collection 
in case of interrupted execution.

@section generic-logging Logging

VLFeat uses the macros ::VL_PRINT and ::VL_PRINTF to print progress or
debug informations. These functions are normally mapped to the @c
printf function of the underlying standard C library. However
::vl_set_printf_func() can be used to map it to a different
implementation. For instance, in MATLAB MEX files this function is
mapped to @c mexPrintf. Setting the function to @c NULL disables
logging.

@section generic-data-model Data models

VLFeat main target is to support common UNIX and Windows
architectures. The data model of these architectures difffer mainly
in:

- <em>endianness</em> and
- <em>size of the atomic data types</em>.

An architecture is big endiand or little endian depending how
multi-byte data are stored in memory:

- <em>big endian</em> (big end first, also known as network order) if it
  stores the most significant byte has the smaller memory address.
- <em>little endian</em> (small end first, used by x86 processors) if it
  stores the lesser significant byte at the smaller memory address.

Use the function ::vl_get_endianness() to detect endianness.  To
serialize/deserialize data in big endian (network) order, call the
functions ::vl_adapt_endianness_8(), ::vl_adapt_endianness_4(),
::vl_adapt_endianness_2() after reading and before writing (the
functions change edinanness only if the architecture is little endian
and do nothing otherwise).

Architecture differ also by the size of the atomic data type (such as
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

For uniformity, VLFeat introduces appropriate atomic data types of
fixed width.  Notice that in C-99 the <code>stdint.h</code> header
could be used for this purpose instead, but unfortunately it is not
(yet) supported by Microsoft Visual C/C++.

Most 32-bit architectures are equivalent for what concerns the size of
the atomic data types.  64-bit vary more; for the atomic data types
defined by VLFEat, the only important difference between architectures
is in practice limited the size of <code>long</code>. Relatively to
the 32 bit architectures, the other important difference is the size
of the pointers.

**/

#include "generic.h"

#include <assert.h>
#include <stdlib.h>
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
void *(*vl_malloc_func)  (vl_size)          = &malloc ;
                                            
/** @internal@brief Customizable @c realloc function pointer */
void *(*vl_realloc_func) (void*,vl_size)    = &realloc ;

/** @internal@brief Customizable @c calloc function pointer */
void *(*vl_calloc_func)  (vl_size, vl_size) = &calloc ;

/** @internal@brief Customizable @c free function pointer */
void  (*vl_free_func)    (void*)            = &free ;
                                            
/** @internal@brief Customizable @c printf function pointer */
int   (*vl_printf_func)  (char const *, ...)= do_nothing_printf ;

/** ------------------------------------------------------------------
 ** @brief Set memory allocation functions
 ** @param malloc_func  pointer to @c malloc.
 ** @param realloc_func pointer to @c realloc.
 ** @param calloc_func  pointer to @c calloc.
 ** @param free_func    pointer to @c free.
 **/

VL_EXPORT
void vl_set_alloc_func (void *(*malloc_func)  (vl_size),
                        void *(*realloc_func) (void*, vl_size),
                        void *(*calloc_func)  (vl_size, vl_size),
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
