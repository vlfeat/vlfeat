/** @file    generic.c
 ** @author  Andrea Vedaldi
 ** @brief   Generic - Definition
 **/

/* AUTORIGHTS */

/** @mainpage Vision Lab Features Library
 ** @version alpha-1
 ** @author Andrea Vedaldi (@email vedaldi@cs.ucla.edu)
 ** @author Brian Fulkerson (@email brian@cs.ucla.edu)
 ** @copy 2007 Andrea Vedaldi and Brian Fulkerson
 **
 ** <em>This library contains implementations of common computer
 ** vision algorithms, with a special focus on visual features for
 ** matching of image regions. Applications include structure from
 ** motion, object and category detection, object and category
 ** recognition.</em>
 **
 ** @section main-contents Contents
 **
 ** - @ref main-design "Design philosophy"
 ** - Support functionalities
 **   - @ref generic.h  "Atomic data types, errors, and others"
 **   - @ref mathop.h   "Math operations"
 **   - @ref stringop.h "String operations"
 **   - @ref imop.h     "Image operations"
 ** - Main functionalities
 **   - @ref sift.h "Scale Invariant Feature Transform (SIFT)"
 **   - @ref mser.h "Maximally Stable Extremal Regions (MSER)"
 **
 ** @section main-design Design philosphy
 **
 ** Some design goals are:
 **
 ** - <b>Minimality and flexibility</b>. The code implements a set of
 **   relatively limited but powerful functionalities. Each
 **   functionality is made accessible by a command line driver and a
 **   MATLAB driver. Additional functionalities, such as visualization,
 **   demos and other interfaces, should not be part of this core library.
 ** - <b>Flatness</b>. The code intra-dependencies are flat. Ideally, each
 **   algorithm depends only on a few basic support modules (e.g. math
 **   operations), but it is largely independent on other chunks
 **   of the library. This is meant to promote robustness and make it
 **   easy to cut-and-past code from the library.
 ** - <b>Compatibility</b>. The library is fully self-contained as it
 **   requires only the standard C-90 environment (except for the
 **   MATLAB drivers which require MATLAB).
 **/

#include "generic.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

int vl_err_no = 0 ;
char vl_err_msg [VL_ERR_MSG_LEN] = "" ;

/** ---------------------------------------------------------------- */
/** @brief Get version string
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

