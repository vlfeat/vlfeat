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

#ifndef VL_IMOPV_INSTANTIATING

#include "imopv.h"
#include "imopv_sse2.h"

#define FLT VL_TYPE_FLOAT
#define VL_IMOPV_INSTANTIATING
#include "imopv.c"

#define FLT VL_TYPE_DOUBLE
#define VL_IMOPV_INSTANTIATING
#include "imopv.c"

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

/* VL_IMOPV_INSTANTIATING */
#else

#include "float.th"

/* ---------------------------------------------------------------- */
VL_EXPORT
void VL_XCAT(vl_imconvcol_v, SFX)
(T* dst, int dst_stride,
 T const* src,
 int src_width, int src_height, int src_stride,
 T const* filt, int filt_begin, int filt_end,
 int step, unsigned int flags)
{
  int x = 0 ;
  int y ;
  int dheight = (src_height - 1) / step + 1 ;
  vl_bool transp = flags & VL_TRANSPOSE ;
  vl_bool zeropad = (flags & VL_PAD_MASK) == VL_PAD_BY_ZERO ;

  /* dispatch to accelerated version */
#ifdef VL_SUPPORT_SSE2
  if (vl_cpu_has_sse2() && vl_get_simd_enabled()) {
    VL_XCAT3(_vl_imconvcol_v,SFX,_sse2)
    (dst,dst_stride,
     src,src_width,src_height,src_stride,
     filt,filt_begin,filt_end,
     step,flags) ;
    return ;
  }
#endif

  /* let filt point to the last sample of the filter */
  filt += filt_end - filt_begin ;

  while (x < src_width) {
    /* Calculate dest[x,y] = sum_p image[x,p] filt[y - p]
     * where supp(filt) = [filt_begin, filt_end] = [fb,fe].
     *
     * CHUNK_A: y - fe <= p < 0
     *          completes VL_MAX(fe - y, 0) samples
     * CHUNK_B: VL_MAX(y - fe, 0) <= p < VL_MIN(y - fb, height - 1)
     *          completes fe - VL_MAX(fb, height - y) + 1 samples
     * CHUNK_C: completes all samples
     */
    T const *filti ;
    int stop ;

    for (y = 0 ; y < src_height ; y += step) {
      T acc = 0 ;
      T v = 0, c ;
      T const* srci ;

      filti = filt ;
      stop = filt_end - y ;
      srci = src + x - stop * src_stride ;

      if (stop > 0) {
        if (zeropad) {
          v = 0 ;
        } else {
          v = *(src + x) ;
        }
        while (filti > filt - stop) {
          c = *filti-- ;
          acc += v * c ;
          srci += src_stride ;
        }
      }

      stop = filt_end - VL_MAX(filt_begin, y - src_height + 1) + 1 ;
      while (filti > filt - stop) {
        v = *srci ;
        c = *filti-- ;
        acc += v * c ;
        srci += src_stride ;
      }

      if (zeropad) v = 0 ;

      stop = filt_end - filt_begin + 1 ;
      while (filti > filt - stop) {
        c = *filti-- ;
        acc += v * c ;
      }

      if (transp) {
        *dst = acc ; dst += 1 ;
      } else {
        *dst = acc ; dst += dst_stride ;
      }
    } /* next y */
    if (transp) {
      dst += 1 * dst_stride - dheight * 1 ;
    } else {
      dst += 1 * 1 - dheight * dst_stride ;
    }
    x += 1 ;
  } /* next x */
}

/* ---------------------------------------------------------------- */
VL_EXPORT void
VL_XCAT(vl_imconvcoltri_v, SFX)
(T* dst, int dst_stride,
 T const* src,
 int src_width, int src_height, int src_stride,
 int filt_size,
 int step, unsigned int flags)
{
  int x = 0 ;
  int y ;
  int dheight = (src_height - 1) / step + 1 ;
  vl_bool transp = flags & VL_TRANSPOSE ;
  vl_bool zeropad = (flags & VL_PAD_MASK) == VL_PAD_BY_ZERO ;
#define fa ((double)(filt_size))
  T scale = ((T) (1.0/(fa*fa))) ;
  T * buff = vl_malloc(sizeof(T) * (src_height + filt_size)) ;
  buff += filt_size ;

  while (x < src_width) {
    T const *srci ;
    srci = src + x + src_stride * (src_height - 1) ;

    /* We decompose the convolution by a triangluar signal as the convolution
     * by two rectangular signals. The rectangular convolutions are computed
     * quickly by computing the integral signals. Each rectangular convolution
     * introduces a delay, which is compensated by convolving each in opposite
     * directions.
     */

    /* integrate backward the column */
    buff [src_height - 1] = *srci ;
    for (y = src_height-2 ; y >=  0 ; --y) {
      srci -= src_stride ;
      buff [y] = buff [y+1] + *srci ;
    }
    if (zeropad) {
      for ( ; y >= - filt_size ; --y) {
        buff [y] = buff [y+1] ;
      }
    } else {
      for ( ; y >= - filt_size ; --y) {
        buff [y] = buff[y+1] + *srci ;
      }
    }

    /* compute the filter forward */
    for (y = - filt_size ; y < src_height - filt_size ; ++y) {
      buff [y] = buff [y] - buff [y + filt_size] ;
    }
    if (! zeropad) {
      for (y = src_height - filt_size ; y < src_height ; ++y) {
        buff [y] = buff [y] - buff [src_height-1]  *
        (src_height - filt_size - y) ;
      }
    }

    /* integrate forward the column */
    for (y = - filt_size + 1 ; y < src_height ; ++y) {
      buff [y] += buff [y - 1] ;
    }

    /* compute the filter backward */
    {
      int stride = transp ? 1 : dst_stride ;
      dst += dheight * stride ;
      for (y = step * (dheight - 1) ; y >= 0 ; y -= step) {
        dst -= stride ;
        *dst = scale * (buff [y] - buff [y - filt_size]) ;
      }
    }
    x += 1 ;
    dst += transp ? dst_stride : 1 ;
  } /* next x */
  vl_free (buff - filt_size) ;
}

#undef FLT
#endif
