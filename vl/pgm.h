/** @file    pgm.h
 ** @brief   Portable graymap format (PGM) parser
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_PGM_H
#define VL_PGM_H

#include "generic.h"
#include "mathop.h"
#include <stdio.h>

/** @name PGM parser error codes
 ** @{ */

#define VL_ERR_PGM_INV_HEAD  101 /**< Invalid PGM header section */
#define VL_ERR_PGM_INV_META  102 /**< Invalid PGM meta section */
#define VL_ERR_PGM_INV_DATA  103 /**< Invalid PGM data section */
#define VL_ERR_PGM_IO        104 /**< Generic I/O error */

/** @} */

/** @brief PGM image meta data
 **
 ** A PGM image is a 2-D array of pixels of width #width and height
 ** #height. Each pixel is an integer one or two bytes wide, depending
 ** whether #max_value is smaller than 256.
 **/

struct _VlPgmImage
{
  int        width ;     /**< image width.                     */
  int        height ;    /**< image height.                    */
  int        max_value ; /**< pixel maximum value (<= 2^16-1). */
  vl_bool    is_raw ;    /**< is RAW format?                   */
};

/** @brief PGM image meta data type
 ** @see ::_VlPgmImage.
 **/
typedef struct  _VlPgmImage  VlPgmImage ;

/** @name Core operations
 ** @{ */
VL_EXPORT int vl_pgm_extract_head  (FILE *f, VlPgmImage       *im) ;
VL_EXPORT int vl_pgm_extract_data  (FILE *f, VlPgmImage const *im, void *data) ;
VL_EXPORT int vl_pgm_insert        (FILE *f, 
                                    VlPgmImage const *im, 
                                    void const*data ) ;
VL_EXPORT int vl_pgm_get_npixels   (VlPgmImage const *im) ;
VL_EXPORT int vl_pgm_get_bpp       (VlPgmImage const *im) ;
/** @} */

/** @name Helper functions
 ** @{ */
VL_EXPORT int vl_pgm_write         (char const *name,
                                    vl_uint8 const *data, 
                                    int width, int height) ;
VL_EXPORT int vl_pgm_write_f       (char const *name,
                                    float const *data, 
                                    int width, int height) ;
VL_EXPORT int vl_pgm_read_new      (char const *name,
                                    VlPgmImage *im,
                                    vl_uint8 **data) ;
VL_EXPORT int vl_pgm_read_new_f    (char const *name,
                                    VlPgmImage *im,
                                    float **data) ;

/** @} */

/* VL_PGM_H */
#endif
