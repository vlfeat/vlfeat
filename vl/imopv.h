/** @file   imopv.h
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations
 **/

#ifndef VL_IMOPV_H
#define VL_IMPOV_H

#include "generic.h"

/** @name Image convolution flags 
 ** @{ */
#define VL_PAD_BY_ZERO       (0x0 << 0) /**< zero padding */
#define VL_PAD_BY_CONTINUITY (0x1 << 0) /**< pad by continuity */
#define VL_PAD_MASK          (0x3)      /**< padding control field selector */
#define VL_TRANSPOSE         (0x1 << 2) /**< traspose result */
#define VL_NO_SIMD           (0x1 << 3) /**< do not use SIMD acceleration */
/** @} */

/** @name Image convolution
 ** @{ */
VL_EXPORT
void vl_imconvcol_vf(float* dst, int dst_stride,                 
                 float const* src,
                 int src_width, int src_height, int src_stride,
                 float const* filt, int filt_begin, int filt_end,
                 int step, unsigned int flags) ;

VL_EXPORT
void vl_imconvcol_vd(double* dst, int dst_stride,
                 double const* src,
                 int src_width, int src_height, int src_stride,
                 double const* filt, int filt_begin, int filt_end,
                 int step, unsigned int flags) ;
/** @} */


/** @fn ::vl_imconvcol_vf
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
 ** The function may be called twice to compute 2-D separable convolutions.
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

/* VL_IMOPV_H */
#endif
