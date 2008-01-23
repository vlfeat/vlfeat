/** @file    imop.h
 ** @author  Andrea Vedaldi
 ** @brief   Image operations
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_IMOP
#define VL_IMOP

#include "generic.h"

/** @name Convolution padding modes
 ** @{ 
 **/
#define VL_CONV_ZERO 0 /**< Zero padding         */
#define VL_CONV_CIRC 1 /**< Circular convolution */
#define VL_CONV_CONT 2 /**< Pad by continuity    */
/* @} */

/* @{ */
void vl_convtransp_f(vl_single       *dst,
                     vl_single const *src, 
                     vl_single const *filt,
                     int width, int height, int filt_width,
                     int mode) ;

void vl_convtransp_d(vl_double       *dst,
                     vl_double const *src, 
                     vl_double const *filt,
                     int width, int height, int filt_width,
                     int mode) ;
/* @} */


/* @{ */
void vl_imsmooth_f(vl_single       *dst, 
                   vl_single       *temp,
                   vl_single const *src,
                   int width, int height, double sigma) ;

void vl_imsmooth_d(vl_double       *dst, 
                   vl_double       *temp,
                   vl_double const *src,
                   int width, int height, double sigma) ;
/*@}*/

/* VL_IMOP */
#endif 
