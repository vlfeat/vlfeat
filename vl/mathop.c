/** @file    mathop.c
 ** @brief   Math operations - Definition
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file mathop.h

 @section mathop-usage-vector-comparison Comparing vectors

 @ref mathop.h includes a number of functions to quickly compute
 distances or similarity of pairs of vector. Applications include
 clustering and evaluation of SVM-like classifiers.

 Use ::vl_get_vector_comparison_function_f or
 ::vl_get_vector_comparison_function_d obtain an approprite function
 to comprare vectors of floats or doubles, respectively.  Such
 functions are usually optimized (for instance, on X86 platforms they
 use the SSE vector extension) and are several times faster than a
 naive implementation.  ::vl_eval_vector_comparison_on_all_pairs_f and
 ::vl_eval_vector_comparison_on_all_pairs_d can be used to evaluate
 the comparison function on all pairs of one or two sequences of
 vectors.

 Let @f$ \mathbf{x} = (x_1,\dots,x_d) @f$ and @f$ \mathbf{y} =
 (y_1,\dots,y_d) @f$ be two vectors.  The following comparison
 functions are supported:

 <table>
 <tr>
 <td>@f$ l^1 @f$</td>
 <td>::VlDistanceL1</td>
 <td>@f$ \sum_{i=1}^d |x_i - y_i| @f$</td>
 <td>l1 distance (squared intersection metric)</td>
 </tr>
 <tr>
 <td>@f$ l^2 @f$</td>
 <td>::VlDistanceL2</td>
 <td>@f$\sum_{i=1}^d (x_i - y_i)^2@f$</td>
 <td>Squared Euclidean disance</td>
 </tr>
 <tr>
 <td>@f$ \chi^2 @f$</td>
 <td>::VlDistanceChi2</td>
 <td>@f$\sum_{i=1}^d \frac{(x_i - y_i)^2}{x_i + y_i}@f$</td>
 <td>Squared chi-square distance</td>
 </tr>
 <tr>
 <td>-</td>
 <td>::VlDistanceHellinger</td>
 <td>@f$\sum_{i=1}^d (\sqrt{x_i} - \sqrt{y_i})^2@f$</td>
 <td>Squared Hellinger's distance</td>
 </tr>
 <tr>
 <td>-</td>
 <td>::VlDistanceJS</td>
 <td>@f$
 \sum_{i=1}^d
 \left(
   x_i \log\frac{2x_i}{x_i+y_i}
 + y_i \log\frac{2y_i}{x_i+y_i}
 \right)
 @f$
 </td>
 <td>Squared Jensen-Shannon distance</td>
 </tr>
 <tr>
 <td>@f$ l^1 @f$</td>
 <td>::VlKernelL1</td>
 <td>@f$ \sum_{i=1}^d \min\{ x_i, y_i \} @f$</td>
 <td>intersection kernel</td>
 </tr>
 <tr>
 <td>@f$ l^2 @f$</td>
 <td>::VlKernelL2</td>
 <td>@f$\sum_{i=1}^d x_iy_i @f$</td>
 <td>linear kernel</td>
 </tr>
 <tr>
 <td>@f$ \chi^2 @f$</td>
 <td>::VlKernelChi2</td>
 <td>@f$\sum_{i=1}^d 2 \frac{x_iy_i}{x_i + y_i}@f$</td>
 <td>chi-square kernel</td>
 </tr>
 <tr>
 <td>-</td>
 <td>::VlKernelHellinger</td>
 <td>@f$\sum_{i=1}^d 2 \sqrt{x_i y_i}@f$</td>
 <td>Hellinger's kernel (Bhattacharya coefficient)</td>
 </tr>
 <tr>
 <td>-</td>
 <td>::VlKernelJS</td>
 <td>@f$
 \sum_{i=1}^d
 \left(
   \frac{x_i}{2} \log_2\frac{x_i+y_i}{x_i}
 + \frac{y_i}{2} \log_2\frac{x_i+y_i}{y_i}
 \right)
 @f$
 </td>
 <td>Jensen-Shannon kernel</td>
 </tr>
 </table>

 @remark The definitions have been choosen so that corresponding kernels and
 distances are related by the equation:
 @f[
  d^2(\mathbf{x},\mathbf{y})
  =
  k(\mathbf{x},\mathbf{x})
  +k(\mathbf{y},\mathbf{y})
  -k(\mathbf{x},\mathbf{y})
  -k(\mathbf{y},\mathbf{x})
 @f]
 This means that each of these distances can be interpreted as a
 squared distance or metric in the corresponding reproducing kernel
 Hilbert space. Notice in particular that the @f$ l^1 @f$ or Manhattan
 distance is also a <em>squared</em> distance in this sense.

 **/

/** @fn vl_get_vector_comparison_function_f(VlVectorComparisonType)
 **
 ** @brief Get vector comparison function from comparison type
 ** @param type vector comparison type.
 ** @return comparison function.
 **/

/** @fn vl_get_vector_comparison_function_d(VlVectorComparisonType)
 ** @brief Get vector comparison function from comparison type
 ** @sa vl_get_vector_comparison_function_f
 **/

/** @fn vl_eval_vector_comparison_on_all_pairs_f(float*,vl_size,
 **     float const*,vl_size,float const*,vl_size,VlFloatVectorComparisonFunction)
 **
 ** @brief Evaluate vector comparison function on all vector pairs
 ** @param result comparison matrix (output).
 ** @param dimension number of vector components (rows of @a X and @a Y).
 ** @param X data matrix X.
 ** @param Y data matrix Y.
 ** @param numDataX number of vectors in @a X (columns of @a X)
 ** @param numDataY number of vectros in @a Y (columns of @a Y)
 ** @param function vector comparison function.
 **
 ** The function evaluates @a function on all pairs of columns
 ** from matrices @a X and @a Y, filling a @a numDataX by @a numDataY
 ** matrix.
 **
 ** If @a Y is a null pointer the function compares all columns from
 ** @a X with themselves.
 **/

/** @fn vl_eval_vector_comparison_on_all_pairs_d(double*,vl_size,
 **     double const*,vl_size,double const*,vl_size,VlDoubleVectorComparisonFunction)
 ** @brief Evaluate vector comparison function on all vector pairs
 ** @sa vl_eval_vector_comparison_on_all_pairs_f
 **/

/* ---------------------------------------------------------------- */
#ifndef VL_MATHOP_INSTANTIATING
#define VL_MATHOP_INSTANTIATING

#include "mathop.h"
#include "mathop_sse2.h"
#include <math.h>

#undef FLT
#define FLT VL_TYPE_FLOAT
#include "mathop.c"

#undef FLT
#define FLT VL_TYPE_DOUBLE
#include "mathop.c"

/* ---------------------------------------------------------------- */
/* VL_MATHOP_SSE2_INSTANTIATING */
#else

#include "float.th"

#undef COMPARISONFUNCTION_TYPE
#if (FLT == VL_TYPE_FLOAT)
#  define COMPARISONFUNCTION_TYPE VlFloatVectorComparisonFunction
#else
#  define COMPARISONFUNCTION_TYPE VlDoubleVectorComparisonFunction
#endif

/* ---------------------------------------------------------------- */

VL_EXPORT T
VL_XCAT(_vl_distance_l2_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T d = *X++ - *Y++ ;
    acc += d * d ;
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_distance_l1_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T d = *X++ - *Y++ ;
    acc += VL_MAX(d, -d) ;
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_distance_chi2_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
    T delta = a - b ;
    T denom = (a + b) ;
    T numer = delta * delta ;
    if (denom) {
      T ratio = numer / denom ;
      acc += ratio ;
    }
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_distance_hellinger_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
#if (FLT == VL_TYPE_FLOAT)
    acc += a + b - 2.0 * sqrtf (a*b) ;
#else
    acc += a + b - 2.0 * sqrt (a*b) ;
#endif
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_distance_js_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T x = *X++ ;
    T y = *Y++ ;
    if (x) acc += x - x * VL_XCAT(vl_log2_,SFX)(1 + y/x) ;
    if (y) acc += y - y * VL_XCAT(vl_log2_,SFX)(1 + x/y) ;
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_kernel_l2_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
    acc += a * b ;
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_kernel_l1_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
    T a_ = VL_XCAT(vl_abs_, SFX) (a) ;
    T b_ = VL_XCAT(vl_abs_, SFX) (b) ;
    acc += a_ + b_ - VL_XCAT(vl_abs_, SFX) (a - b) ;
  }
  return acc / ((T)2) ;
}

VL_EXPORT T
VL_XCAT(_vl_kernel_chi2_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
    T denom = (a + b) ;
    if (denom) {
      T numer = 2 * a * b ;
      T ratio = numer / denom ;
      acc += ratio ;
    }
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_kernel_hellinger_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T a = *X++ ;
    T b = *Y++ ;
#if (FLT == VL_TYPE_FLOAT)
    acc += sqrtf (a*b) ;
#else
    acc += sqrt (a*b) ;
#endif
  }
  return acc ;
}

VL_EXPORT T
VL_XCAT(_vl_kernel_js_, SFX)
(vl_size dimension, T const * X, T const * Y)
{
  T const * X_end = X + dimension ;
  T acc = 0.0 ;
  while (X < X_end) {
    T x = *X++ ;
    T y = *Y++ ;
    if (x) acc += x * VL_XCAT(vl_log2_,SFX)(1 + y/x) ;
    if (y) acc += y * VL_XCAT(vl_log2_,SFX)(1 + x/y) ;
  }
  return (T)0.5 * acc ;
}

/* ---------------------------------------------------------------- */

VL_EXPORT COMPARISONFUNCTION_TYPE
VL_XCAT(vl_get_vector_comparison_function_, SFX)(VlVectorComparisonType type)
{
  COMPARISONFUNCTION_TYPE function = 0 ;
  switch (type) {
    case VlDistanceL2        : function = VL_XCAT(_vl_distance_l2_,        SFX) ; break ;
    case VlDistanceL1        : function = VL_XCAT(_vl_distance_l1_,        SFX) ; break ;
    case VlDistanceChi2      : function = VL_XCAT(_vl_distance_chi2_,      SFX) ; break ;
    case VlDistanceHellinger : function = VL_XCAT(_vl_distance_hellinger_, SFX) ; break ;
    case VlDistanceJS        : function = VL_XCAT(_vl_distance_js_,        SFX) ; break ;
    case VlKernelL2          : function = VL_XCAT(_vl_kernel_l2_,          SFX) ; break ;
    case VlKernelL1          : function = VL_XCAT(_vl_kernel_l1_,          SFX) ; break ;
    case VlKernelChi2        : function = VL_XCAT(_vl_kernel_chi2_,        SFX) ; break ;
    case VlKernelHellinger   : function = VL_XCAT(_vl_kernel_hellinger_,   SFX) ; break ;
    case VlKernelJS          : function = VL_XCAT(_vl_kernel_js_,          SFX) ; break ;
    default: abort() ;
  }

#ifndef VL_DISABLE_SSE2
  /* if a SSE2 implementation is available, use it */
  if (vl_cpu_has_sse2() && vl_get_simd_enabled()) {
    switch (type) {
      case VlDistanceL2   : function = VL_XCAT(_vl_distance_l2_sse2_,   SFX) ; break ;
      case VlDistanceL1   : function = VL_XCAT(_vl_distance_l1_sse2_,   SFX) ; break ;
      case VlDistanceChi2 : function = VL_XCAT(_vl_distance_chi2_sse2_, SFX) ; break ;
      case VlKernelL2     : function = VL_XCAT(_vl_kernel_l2_sse2_,     SFX) ; break ;
      case VlKernelL1     : function = VL_XCAT(_vl_kernel_l1_sse2_,     SFX) ; break ;
      case VlKernelChi2   : function = VL_XCAT(_vl_kernel_chi2_sse2_,   SFX) ; break ;
      default: break ;
    }
  }
#endif

  return function ;
}

/* ---------------------------------------------------------------- */

VL_EXPORT void
VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)
(T * result, vl_size dimension,
 T const * X, vl_size numDataX,
 T const * Y, vl_size numDataY,
 COMPARISONFUNCTION_TYPE function)
{
  vl_uindex xi ;
  vl_uindex yi ;

  if (dimension == 0) return ;
  if (numDataX == 0) return ;
  assert (X) ;

  if (Y) {
    if (numDataY == 0) return ;
    for (yi = 0 ; yi < numDataY ; ++ yi) {
      for (xi = 0 ; xi < numDataX ; ++ xi) {
        *result++ = (*function)(dimension, X, Y) ;
        X += dimension ;
      }
      X -= dimension * numDataX ;
      Y += dimension ;
    }
  } else {
    T * resultTransp = result ;
    Y = X ;
    for (yi = 0 ; yi < numDataX ; ++ yi) {
      for (xi = 0 ; xi <= yi ; ++ xi) {
        T z = (*function)(dimension, X, Y) ;
        X += dimension ;
        *result       = z ;
        *resultTransp = z ;
        result        += 1 ;
        resultTransp  += numDataX ;
      }
      X -= dimension * (yi + 1) ;
      Y += dimension ;
      result       += numDataX - (yi + 1) ;
      resultTransp += 1        - (yi + 1) * numDataX ;
    }
  }
}

/* VL_MATHOP_INSTANTIATING */
#endif
