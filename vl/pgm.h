/** @file    pgm.h
 ** @author  Andrea Vedaldi
 ** @brief   Portable graymap format (PGM) parser
 **
 ** To read a PGM image, first call ::vl_pgm_extract_head() to extract
 ** the image meta data (size and bit-depth). Then allocate the
 ** appropriate buffer and call ::vl_pgm_extract_data().
 **
 ** To write a PGM image use ::vl_pgm_insert().
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "generic.h"
#include <stdio.h>

#ifndef VL_PGM
#define VL_PGM

/** @name PGM parser error codes */
/*@{*/
#define VL_ERR_PGM_INV_HEAD  101 /**< Invalid PGM heder serction. */
#define VL_ERR_PGM_INV_META  102 /**< Invalid PGM meta section.   */
#define VL_ERR_PGM_INV_DATA  103 /**< Invalid PGM data section.   */
#define VL_ERR_PGM_IO        104 /**< Generic I/O error.          */
/*@}*/

/** @brief PGM image meta data
 **
 ** A PGM image is a 2-D array of pixels of width #width and height
 ** #height. Each pixel is an integer one or two bytes wide, depending
 ** wether #max_value is smaller than 256.
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

int vl_pgm_extract_head  (FILE *f, VlPgmImage       *im) ;
int vl_pgm_extract_data  (FILE *f, VlPgmImage const *im, void *data) ;
int vl_pgm_insert        (FILE *f, 
                          VlPgmImage const *im, 
                          void *data ) ;
int vl_pgm_get_data_size (VlPgmImage const *im) ;
int vl_pgm_get_bpp       (VlPgmImage const *im) ;

/* VL_PGM */
#endif
