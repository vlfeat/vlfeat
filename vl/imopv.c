/** @file   imopv.c
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations - Definition
 **/

/* AUTORIGHTS */

/** @file imopv.h
 ** 
 ** This module includes vectorized image opearations.
 **/

#include "imopv.h"
#include "imopv_sse2.h"

#define FLOAT_TYPE_FLOAT 1
#define FLOAT_TYPE_DOUBLE 2

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_FLOAT
#include "imopv.tc"

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_DOUBLE
#include "imopv.tc"

/** @fn vl_imconvcol_vf(float*,int,float const*,int,int,int,float const*,int,int,int,unsigned int)
 ** @brief Convolve image along columns
 **
 ** @param dst destinaton image.
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
 ** The function convolves the column of the image @a src
 ** by the filter @a filt and saves the result to the image
 ** @a dst. The size of @a dst must be equal to the size of @a src.
 ** Formally, this results in the calculation
 ** @f[
 ** \mathrm{dst} [x,y] = \sum_{p=y-\mathrm{filt\_end}}^{y-\mathrm{filt\_begin}}
 ** \mathrm{src}[x,y] \mathrm{filt}[y - p - \mathrm{filt\_begin}]
 ** @f]
 **
 ** The function subsamples the image along the columns according
 ** to the parameter @a step. Setting @a step to 1 (one) computes
 ** the elements @f$\mathrm{dst}[x,y]@f$ for all pairs (x,0), (x,1), (x,2)
 ** and so on. Setting @a step two 2 (two) computes only (x,0), (x,2) and so on
 ** (in this case the height of the destination image is <code>floor(src_height/step)+1)</code>.
 **
 ** Calling twice the function can be used to compute 2-D separable convolutions.
 ** Use the flag ::VL_TRANSPOSE to transpose the result (in this case
 ** @a dst has transposed dimension as well).
 **
 ** The function allows the support of the filter to be any range.
 ** Usually the support is <code>@a filt_end = -@a filt_begin</code>.
 **
 ** The convolution operation may pick up values outside
 ** the image boundary. To cope with this edge cases, the function either 
 ** pads the image by zero (::VL_PAD_BY_ZERO) or 
 ** with the values at the boundary (::VL_PAD_BY_CONTINUITY).
 ** 
 ** The function attempts to use the host processor SIMD technology.
 ** This requires the data @a src_stride 
 ** to be properly aligned (for instance,
 ** Intel SSE2 requires the data to be aligned to 16 bytes). 
 ** There may be also
 ** a small computational advantage if @a src is aligned as well. Use
 ** the flag ::VL_NO_SIMD to force disabling SIMD based calculations.
 **/
