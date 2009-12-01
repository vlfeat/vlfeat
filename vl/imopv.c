/** @file   imopv.c
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file imopv.h
 **
 ** This module provides image operations such as convolution.
 **
 ** Some operations are optimized to exploit possible SIMD
 ** instructions.  This requires image data to be properly aligned,
 ** along with the image stride.
 **
 ** The function ::vl_imconvcol_vf() can be used to compute separable
 ** convolutions. The function vl_imconvcoltri_vf() is an optimized
 ** version that works with triangular kernels.
 **/

#include "imopv.h"
#include "imopv_sse2.h"

#define FLOAT_TYPE_FLOAT 1
#define FLOAT_TYPE_DOUBLE 2

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_DOUBLE
#include "imopv.tc"

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_FLOAT
#include "imopv.tc"

/** @fn vl_imconvcol_vf(float*,int,float const*,int,int,int,float const*,int,int,int,unsigned int)
 ** @brief Convolve image along columns
 **
 ** @param dst destination image.
 ** @param dst_stride width of the destination image including padding.
 ** @param src source image.
 ** @param src_width width of the source image.
 ** @param src_height height of the source image.
 ** @param src_stride width of the source image including padding.
 ** @param filt filter kernel.
 ** @param filt_begin coordinate of the first filter element.
 ** @param filt_end coordinate of the last filter element.
 ** @param step sub-sampling step.
 ** @param flags operation modes.
 **
 ** The function convolves the column of the image @a src by the
 ** filter @a filt and saves the result to the image @a dst. The size
 ** of @a dst must be equal to the size of @a src.  Formally, this
 ** results in the calculation
 **
 ** @f[
 ** \mathrm{dst} [x,y] = \sum_{p=y-\mathrm{filt\_end}}^{y-\mathrm{filt\_begin}}
 ** \mathrm{src}[x,y] \mathrm{filt}[y - p - \mathrm{filt\_begin}]
 ** @f]
 **
 ** The function subsamples the image along the columns according to
 ** the parameter @a step. Setting @a step to 1 (one) computes the
 ** elements @f$\mathrm{dst}[x,y]@f$ for all pairs (x,0), (x,1), (x,2)
 ** and so on. Setting @a step two 2 (two) computes only (x,0), (x,2)
 ** and so on (in this case the height of the destination image is
 ** <code>floor(src_height/step)+1)</code>.
 **
 ** Calling twice the function can be used to compute 2-D separable
 ** convolutions.  Use the flag ::VL_TRANSPOSE to transpose the result
 ** (in this case @a dst has transposed dimension as well).
 **
 ** The function allows the support of the filter to be any range.
 ** Usually the support is <code>@a filt_end = -@a filt_begin</code>.
 **
 ** The convolution operation may pick up values outside the image
 ** boundary. To cope with this edge cases, the function either pads
 ** the image by zero (::VL_PAD_BY_ZERO) or with the values at the
 ** boundary (::VL_PAD_BY_CONTINUITY).
 **/

/** @fn vl_imconvcol_vd(double*,int,double const*,int,int,int,double const*,int,int,int,unsigned int)
 ** @see ::vl_imconvcol_vf()
 **/

/** @fn vl_imconvcoltri_vf(float*,int,float const*,int,int,int,int,int,unsigned int)
 ** @brief Convolve an image along the columns with a triangular kernel
 **
 ** @param dst destination image.
 ** @param dst_stride width of the destination image including padding.
 ** @param src source image.
 ** @param src_width width of the source image.
 ** @param src_height height of the source image.
 ** @param src_stride width of the source image including padding.
 ** @param filt_size size of the triangular filter.
 ** @param step sub-sampling step.
 ** @param flags operation modes.
 **
 ** The function convolves the column of the image @a src by the
 ** discrete kernel
 **
 ** @f[
 **   k(t) = \frac{1}{\Delta^2} \max\{ \Delta -  |t|, 0 \},
 **   \quad t \in \mathbb{Z}
 ** @f]
 **
 ** where @f$ \Delta @f$ is equal to the parameter @a filt_size. The
 ** normalization factor is selected such that
 **
 ** @f[
 **   \sum_{t=-\infty}^{+\infty} k(t) = 1
 ** @f]
 **
 ** for all @f$ \Delta \in \mathbb{Z}_+ @f$. Notice that the support
 ** of @f$ k(x) @f$ as a continuous function is @f$ (-\Delta,\Delta)
 ** @f$ and has length @f$ 2\Delta @f$ and as a discrete function is
 ** @f$ [-\Delta + 1, \Delta-1] @f$ which has length @f$ 2 \Delta - 1
 ** @f$.
 **
 ** The operation of the function is otherwise similar to
 ** ::vl_imconvocl().
 **/

/** @fn vl_imconvcoltri_vd(double*,int,double const*,int,int,int,int,int,unsigned int)
 ** @see ::vl_imconvcoltri_vf()
 **/
