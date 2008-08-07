/** @file   imop.h
 ** @author Andrea Vedaldi
 ** @brief  Image operations
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

/** @name Convolution
 ** @{ 
 **/
VL_EXPORT
void vl_convtransp_f(float       *dst,
                     float const *src, 
                     float const *filt,
                     int width, int height, int filt_width,
                     int mode) ;

VL_EXPORT
void vl_convtransp_d(double       *dst,
                     double const *src, 
                     double const *filt,
                     int width, int height, int filt_width,
                     int mode) ;
/* @} */


/** @name Image Smoothing
 ** @{ 
 **/
VL_EXPORT
void vl_imsmooth_f(float       *dst, 
                   float       *temp,
                   float const *src,
                   int width, int height, double sigma) ;

VL_EXPORT
void vl_imsmooth_d(double       *dst, 
                   double       *temp,
                   double const *src,
                   int width, int height, double sigma) ;
/*@}*/

/* VL_IMOP */
#endif 
