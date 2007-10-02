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
 ** @version 0.1
 ** @author Andrea Vedaldi (mailto:vedaldi@cs.ucla.edu)
 ** @author Brian Fulkerson (mailto:brian@cs.ucla.edu)

@par Copyright &copy; 2007 Andrea Vedaldi and Brian Fulkerson

<em>This library contains implementations of common computer vision
algorithms, with a special focus on visual features for matching image
regions. Applications include structure from motion, object and
category detection, object and category recognition.</em>

@section main-contents Contents

- @ref main-design "Library Design"
  - @ref main-design-goals      "Goals"
  - @ref main-design-guidelines "Guidelines"
  - @ref main-design-style      "Coding style"
- @ref main-glossary  "Glossary"
- Support functionalities
  - @ref generic.h   "Atomic data types, errors, and others"
  - @ref mathop.h    "Math operations"
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

@section main-design Library design

@subsection main-design-goals Goals

The goal of the VLFeat core library is to provide solid
implementations a small set of useful computer vision algorithms. The
library attempts to be as enjoyable as possible by researcherers by
achieving:

- <b>Accuracy</b>. Algorithms should be accurately defined. When
  appropriate, they should be close to the description/implementation
  provided by the original authors.
 
- <b>Usability</b>. The code should be easy to compile and link. The
  user interface should be simple and flexible. The documentation
  should be clear and complete.
 
- <b>Openess</b>. The code should be easy to understand and modify.
  It should also be easy to isolate each algorithm from the rest of
  the library.

Other characteristic, such as extensiveness or speed, are regarded as
less important. This does not mean that the algorithms implementations
are particularly slow. It just means that they are not necessarily the
fastest implementations possible.

@subsection main-design-guidelines Guidelines

In order to obtain an accurate, usable and open library, the code is
written according to the following guidelins:

- <b>Minimality</b>. The core library includes only the fundamental
  algorithms. Additional functionalities are either included in the
  MATLAB toolbox (part of VLFeat) or left to the user. This redueces
  clutter and makes the library manageable by the authors.
- <b>Flatness</b>. The intra-dependencies among library modules are
  not deep.  In most cases, an algorithm implementation depends only
  on a few basic modules (such as IO or math operations), but is
  otherwise largely independent from other algorithms. This simplifies
  isolating an algorithm from the library.
- <b>Portability</b>. The library is in standard C (fundamentally
  C-89, with a few exceptions) and has no external dependencies except for
  the basic ones. This also makes easier to compile the library.
- <b>Slef-documented</b>. Documentation is embedded in the source code
  in Doxygen format. The documentation includes exhaustive
  descriptions of each algorithm.

@subsection main-design-style Coding style

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

/** @file   generich.h
 ** @author Andrea Vedaldi
 ** @brief  Generic

This module provides basic facilities such as atomi types definitions,
error handling, endianness detection, serialization, logging.

@section generic-error Error handling

Error handling uses the same style of the standard C library. Most
functions return 0 when they succeed and -1 when they fail, and
set the global variable ::vl_err_no with a code identifying the
error occurred. This variable is never set on success and should
be examinated right after an error occurred.

@section generic-endian Endinanness detection and conversions

An architecture is big endiand or little endian depending how
multi-btye data are stored in memory:

- <em>big endian</em> (big end first - Network order) if it
  stores the most significant byte has the smaller memory address.
- <em>little endian</em> (small end first - Intel format) if it
  stores the lesser significant byte at the smaller memory address.

Use the function ::vl_get_endianness() to detect endianness.  To
serialize/deserialize data in big endian (network) order, call the
functions ::vl_adapt_endianness_8(), ::vl_adapt_endianness_4(),
::vl_adapt_endianness_2() after reading and before writing (the
functions change edinanness only if the architecture is little
endian and do nothing otherwise).

**/

#include "generic.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

int vl_err_no = 0 ;
char vl_err_msg [VL_ERR_MSG_LEN] = "" ;

/** ------------------------------------------------------------------
 ** @brief Get version string
 ** @return library version string
 **/

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
 ** Pass NULL to disable printf.
 **/

void 
vl_set_printf_func (int(*printf_func) (char const *format, ...))
{
  vl_printf_func  = printf_func ? printf_func : do_nothing_printf ;
}
