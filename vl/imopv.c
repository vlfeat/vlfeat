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

#define FLT VL_TYPE_UINT32
#define VL_IMOPV_INSTANTIATING
#include "imopv.c"

#define FLT VL_TYPE_INT32
#define VL_IMOPV_INSTANTIATING
#include "imopv.c"

/* VL_IMOPV_INSTANTIATING */
#endif

#if defined(VL_IMOPV_INSTANTIATING) || defined(__DOXYGEN__)

#include "float.th"

/* ---------------------------------------------------------------- */
/*                                                Image Convolution */
/* ---------------------------------------------------------------- */

/** @fn  vl_imconvcol_vd(double*,int,double const*,int,int,int,double const*,int,int,int,unsigned int)
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

/** @fn vl_imconvcol_vf(float*,int,float const*,int,int,int,float const*,int,int,int,unsigned int)
 ** @see ::vl_imconvcol_vf()
 **/

#if (FLT == VL_TYPE_FLOAT || FLT == VL_TYPE_DOUBLE)

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
#ifdef VL_ENABLE_SSE2
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

/* VL_TYPE_FLOAT, VL_TYPE_DOUBLE */
#endif

/* ---------------------------------------------------------------- */
/*                         Image convolution by a triangular kernel */
/* ---------------------------------------------------------------- */

/** @fn vl_imconvcoltri_d(double*,vl_size,double const*,vl_size,vl_size,vl_size,vl_size,vl_size,int unsigned)
 ** @brief Convolve an image along the columns with a triangular kernel
 ** @param dest destination image.
 ** @param destStride destination image stride.
 ** @param image image to convolve.
 ** @param imageWidth width of the image.
 ** @param imageHeight height of the image.
 ** @param imageStride width of the image including padding.
 ** @param filterSize size of the triangular filter.
 ** @param step sub-sampling step.
 ** @param flags operation modes.
 **
 ** The function convolves the column of the image @a image with the
 ** discrete triangular kernel
 **
 ** @f[
 **   k(t) = \frac{1}{\Delta^2} \max\{ \Delta -  |t|, 0 \},
 **   \quad t \in \mathbb{Z}
 ** @f]
 **
 ** where @f$ \Delta @f$ is equal to the parameter @a filterSize. The
 ** filter normalization factor has been selected such that
 **
 ** @f[
 **   \sum_{t=-\infty}^{+\infty} k(t) = 1
 ** @f]
 **
 ** for all @f$ \Delta \in \mathbb{Z}_+ @f$. Notice that the support
 ** of @f$ k(x) @f$ as a continuous function is @f$ (-\Delta,\Delta)
 ** @f$, which has length @f$ 2\Delta @f$, and as a discrete function is
 ** @f$ [-\Delta + 1, \Delta-1] @f$, which has length @f$ 2 \Delta - 1
 ** @f$.
 **
 ** @see ::vl_imconvcol_vd for further details.
 **/

/** @fn vl_imconvcoltri_f(float*,vl_size,float const*,vl_size,vl_size,vl_size,vl_size,vl_size,int unsigned)
 ** @brief Convolve an image along the columns with a triangular kernel
 ** @see ::vl_imconvcoltri_d()
 **/

#if (FLT == VL_TYPE_FLOAT || FLT == VL_TYPE_DOUBLE)

VL_EXPORT void
VL_XCAT(vl_imconvcoltri_, SFX)
(T * dest, vl_size destStride,
 T const * image,
 vl_size imageWidth, vl_size imageHeight, vl_size imageStride,
 vl_size filterSize,
 vl_size step, unsigned int flags)
{
  vl_index x, y, dheight ;
  vl_bool transp = flags & VL_TRANSPOSE ;
  vl_bool zeropad = (flags & VL_PAD_MASK) == VL_PAD_BY_ZERO ;
  T scale = (T) (1.0 / ((double)filterSize * (double)filterSize)) ;
  T * buffer = vl_malloc (sizeof(T) * (imageHeight + filterSize)) ;
  buffer += filterSize ;

  if (imageHeight == 0) {
    return  ;
  }

  x = 0 ;
  dheight = (imageHeight - 1) / step + 1 ;

  while (x < (signed)imageWidth) {
    T const * imagei ;
    imagei = image + x + imageStride * (imageHeight - 1) ;

    /* We decompose the convolution by a triangluar signal as the convolution
     * by two rectangular signals. The rectangular convolutions are computed
     * quickly by computing the integral signals. Each rectangular convolution
     * introduces a delay, which is compensated by convolving each in opposite
     * directions.
     */

    /* integrate backward the column */
    buffer[imageHeight - 1] = *imagei ;
    for (y = (signed)imageHeight - 2 ; y >=  0 ; --y) {
      imagei -= imageStride ;
      buffer[y] = buffer[y + 1] + *imagei ;
    }
    if (zeropad) {
      for ( ; y >= - (signed)filterSize ; --y) {
        buffer[y] = buffer[y + 1] ;
      }
    } else {
      for ( ; y >= - (signed)filterSize ; --y) {
        buffer[y] = buffer[y + 1] + *imagei ;
      }
    }

    /* compute the filter forward */
    for (y = - (signed)filterSize ;
         y < (signed)imageHeight - (signed)filterSize ; ++y) {
      buffer[y] = buffer[y] - buffer[y + filterSize] ;
    }
    if (! zeropad) {
      for (y = (signed)imageHeight - (signed)filterSize ;
           y < (signed)imageHeight ;
           ++y) {
        buffer[y] = buffer[y] - buffer[imageHeight - 1]  *
        ((signed)imageHeight - (signed)filterSize - y) ;
      }
    }

    /* integrate forward the column */
    for (y = - (signed)filterSize + 1 ;
         y < (signed)imageHeight ; ++y) {
      buffer[y] += buffer[y - 1] ;
    }

    /* compute the filter backward */
    {
      vl_size stride = transp ? 1 : destStride ;
      dest += dheight * stride ;
      for (y = step * (dheight - 1) ; y >= 0 ; y -= step) {
        dest -= stride ;
        *dest = scale * (buffer[y] - buffer[y - (signed)filterSize]) ;
      }
      dest += transp ? destStride : 1 ;
    }
    x += 1 ;
  } /* next x */
  vl_free (buffer - filterSize) ;
}

/* VL_TYPE_FLOAT, VL_TYPE_DOUBLE */
#endif

/* ---------------------------------------------------------------- */
/*                                                   Integral Image */
/* ---------------------------------------------------------------- */

/** @fn vl_imintegral_d(double*,vl_size,double const*,vl_size,vl_size,vl_size)
 ** @brief Compute integral image
 **
 ** @param integral integral image.
 ** @param integralStride integral image stride.
 ** @param image source image.
 ** @param imageWidth source image width.
 ** @param imageHeight source image height.
 ** @param imageStride source image stride.
 **
 ** Let @f$ I(x,y), (x,y) \in [0, W-1] \times [0, H-1] @f$. The
 ** function computes the integral image @f$ J(x,y) @f$ of @f$ I(x,g)
 ** @f$:
 **
 ** @f[
 **   J(x,y) = \sum_{x'=0}^{x} \sum_{y'=0}^{y} I(x',y')
 ** @f]
 **
 ** The integral image @f$ J(x,y) @f$ can be used to compute quickly
 ** the integral of of @f$ I(x,y) @f$ in a rectangular region @f$ R =
 ** [x',x'']\times[y',y''] @f$:
 **
 ** @f[
 **  \sum_{(x,y)\in[x',x'']\times[y',y'']} I(x,y) =
 **  (J(x'',y'') - J(x'-1, y'')) - (J(x'',y'-1) - J(x'-1,y'-1)).
 ** @f]
 **
 ** Note that the order of operations is important when the integral image
 ** has an unsigned data type (e.g. ::vl_uint32). The formula
 ** is easily derived as follows:
 **
 ** @f{eqnarray*}
 **   \sum_{(x,y)\in R} I(x,y)
 **   &=& \sum_{x=x'}^{x''} \sum_{y=y'}^{y''} I(x,y)\\
 **   &=& \sum_{x=0}^{x''}  \sum_{y=y'}^{y''} I(x,y)
 **     - \sum_{x=0}^{x'-1} \sum_{y=y'}^{y''} I(x,y)\\
 **   &=& \sum_{x=0}^{x''}  \sum_{y=0}^{y''}  I(x,y)
 **     - \sum_{x=0}^{x''}  \sum_{y=0}^{y'-1} I(x,y)
 **     - \sum_{x=0}^{x'-1} \sum_{y=0}^{y''}  I(x,y)
 **     + \sum_{x=0}^{x'-1} \sum_{y=0}^{y'-1} I(x,y)\\
 **   &=& J(x'',y'') - J(x'-1,y'') - J(x'',y'-1) + J(x'-1,y'-1).
 ** @f}
 **/

/** @fn vl_imintegral_f(float*,vl_size,float const*,vl_size,vl_size,vl_size)
 ** @brief Compute integral image
 ** @see ::vl_imintegral_d.
 **/

/** @fn vl_imintegral_ui32(vl_uint32*,vl_size,vl_uint32 const*,vl_size,vl_size,vl_size)
 ** @brief Compute integral image
 ** @see ::vl_imintegral_d.
 **/

/** @fn vl_imintegral_i32(vl_int32*,vl_size,vl_int32 const*,vl_size,vl_size,vl_size)
 ** @brief Compute integral image
 ** @see ::vl_imintegral_d.
 **/

VL_EXPORT void
VL_XCAT(vl_imintegral_, SFX)
(T * integral, vl_size integralStride,
 T const * image,
 vl_size imageWidth, vl_size imageHeight, vl_size imageStride)
{
  vl_uindex x, y ;
  T temp  = 0 ;

  if (imageHeight > 0) {
    for (x = 0 ; x < imageWidth ; ++ x) {
      temp += *image++ ;
      *integral++ = temp ;
    }
  }

  for (y = 1 ; y < imageHeight ; ++ y) {
    integral += integralStride - imageWidth ;
    image += imageStride - imageWidth ;
    T * integralPrev = integral - integralStride ;

    temp = 0 ;
    for (x = 0 ; x < imageWidth ; ++ x) {
      temp += *image++ ;
      *integral++ = *integralPrev++ + temp ;
    }
  }
}

#undef FLT

/* VL_IMOPV_INSTANTIATING */
#undef VL_IMOPV_INSTANTIATING
#endif
