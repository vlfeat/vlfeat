/** @file    mathop_sse2.h
 ** @brief   mathop for sse2 declaration
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS */

/* ---------------------------------------------------------------- */
#ifndef VL_MATHOP_SSE2_H_INSTANTIATING
#define VL_MATHOP_SSE2_H_INSTANTIATING

#ifndef VL_MATHOP_SSE2_H
#define VL_MATHOP_SSE2_H

#undef FLT
#define FLT VL_TYPE_DOUBLE
#include "mathop_sse2.h"

#undef FLT
#define FLT VL_TYPE_FLOAT
#include "mathop_sse2.h"

/* VL_MATHOP_SSE2_H */
#endif

/* ---------------------------------------------------------------- */
/* VL_MATHOP_SSE2_H_INSTANTIATING */
#else

#ifdef VL_SUPPORT_SSE2

#include "generic.h"
#include "float.th"

VL_EXPORT T
VL_XCAT(_vl_distance_l2_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

VL_EXPORT T
VL_XCAT(_vl_distance_l1_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

VL_EXPORT T
VL_XCAT(_vl_distance_chi2_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

VL_EXPORT T
VL_XCAT(_vl_kernel_l2_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

VL_EXPORT T
VL_XCAT(_vl_kernel_l1_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

VL_EXPORT T
VL_XCAT(_vl_kernel_chi2_sse2_, SFX)
(vl_size dimension, T const * X, T const * Y) ;

/* VL_SUPPORT_SSE2 */
#endif

/* VL_MATHOP_SSE2_INSTANTIATING */
#endif



