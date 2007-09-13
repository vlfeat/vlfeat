/** @file    generic.c
 ** @author  Andrea Vedaldi
 ** @brief   Generic - Definition
 **/

/* AUTORIGHTS */

/** @mainpage VisionLab Features Library
 ** @version 0.1
 ** @author Andrea Vedaldi (mailto:vedaldi@cs.ucla.edu)
 ** @author Brian Fulkerson (mailto:brian@cs.ucla.edu)
 **
 ** @par Copyright &copy; 2007 Andrea Vedaldi and Brian Fulkerson
 **
 ** <em>This library contains implementations of common computer
 ** vision algorithms, with a special focus on visual features for
 ** matching image regions. Applications include structure from
 ** motion, object and category detection, object and category
 ** recognition.</em>
 **
 ** @section main-contents Contents
 **
 ** - @ref main-design "Design philosophy"
 **   - @ref main-design-guidelines "Guidelines"
 **   - @ref main-design-opaque     "Opaque structures"
 ** - Support functionalities
 **   - @ref generic.h   "Atomic data types, errors, and others"
 **   - @ref mathop.h    "Math operations"
 **   - @ref stringop.h  "String operations"
 **   - @ref imop.h      "Image operations"
 **   - @ref rodrigues.h "Rodrigues formuals"
 ** - Main functionalities
 **   - @ref sift.h    "Scale Invariant Feature Transform (SIFT)"
 **   - @ref mser.h    "Maximally Stable Extremal Regions (MSER)"
 **   - @ref ikmeans.h "Integer K-means"
 **
 ** @section main-design Library design
 **
 ** Some design goals are:
 **
 ** - <b>Accuracy</b>. Algorithms pre and post conditions must be
 **   accurately described. Where possible, the behaviour should be close to the
 **   description or implementation provided by the original authors of the algorithms.
 ** - <b>Usability</b>. The code should be easy to compile and link. The library
 **   interface should be simple. The documentation should be clear and complete.
 ** - <b>Openess</b>. The code should be easy to read, understand and modify.
 **   It should also be possible to cut and paste portions of it.
 **
 ** @subsection main-design-guidelines Guidelines
 **
 ** This is a list coding guidelines that we think are important to
 ** meet the library design goals:
 **
 ** - <b>Minimality</b>. The code implements a small set important
 **   functionalities. Additional functionalities, such as
 **   visualization and demos, are not part of this core library.
 ** - <b>Flatness</b>. The code intra-dependencies are flat. Ideally,
 **   each algorithm implementations depends only on a few basic
 **   support modules (e.g. math operations), but is largely
 **   independent on other chunks of the library. This is meant to
 **   promote robustness and make it easy to cut-and-past code from
 **   the library.
 ** - <b>Compatibility</b>. The library is in standard C-90 and has no
 **   external dependencies.
 ** - <b>Self-documenting</b>. Documentation is embedded in the source code.
 **
 ** @subsection main-design-opaque Opaque structures
 **
 ** Most large structures in the library are <em>opaque</em>. The
 ** members of such structures are not meant to be accessed directly,
 ** but through appropriate functions. This improves robustness (as it
 ** limits the operations that can be mande) and compatibility with
 ** future versions.
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

