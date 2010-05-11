/** @file    homkermap.c
 ** @author  Andrea Vedaldi
 ** @brief   Homogeneous kernel map - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file homkermap.h

 @section homkermap Homogeneous kernel map

 @ref homkermap.h is an implementation of the homogeneous kernel maps
 introduced in [1]. Such maps allow linearizing the intersection, Chi2
 and other popular additive kernels.

 - [1] A. Vedaldi and A. Zisserman, <b>Efficient Additive Kernels via
 Explicit Feature Maps</b>, in <em>Proc. CVPR</em>, 2010.

 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
 @section homkermap-overview Overview
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

 An <em>homogeneous kernel map</em> computes a finite dimensional
 linear approximation of an homgeneous kernel such as the intersection
 kernel or the Chi2 kernel.

 Formally, let @f$ x,y \in \mathbb{R}_+ @f$ be non-negative scalars.
 Let @f$ K(x,y) @f$ be an homogeneous kernel like the intersection
 kernel or Chi2 kernels

 @f[
   k_{\mathrm{inters}}(x,y) = \min\{x, y\},
   \quad
   k_{\chi^2}(x,y) = 2 \frac{(x - y)^2}{x+y}.
 @f]
 The <em>kernel map</em> @f$ \Psi(x) @f$ is a vectorial function such that
 @f[
   k(x,y) \approx \langle \Psi(x), \Psi(y) \rangle.
 @f]

 Note that feature maps for the additive versions @f$
 K(\mathbb{x},\mathbb{y}) = \sum_{i=1}^d k(\mathbb{x}_i,\mahtbb{y}_i)
 @f$ of such kernels are obtained by stacking the vectors @f$
 (\Psi(\mathbb{x}_1), \dots, \Psi(\mathbb{x}_n)) @f$.

 @section homkermap-usage Usage

 First, create ::VlHomogeneousKernelMap object instance by
 ::vl_homogeneouskernelmap_new, then use
 ::vl_homogeneouskernelmap_evaluate_d or
 ::vl_homogeneouskernelmap_evaluate_f to compute @f$ \Psi(x) @f$ for
 each scalar @a x that needs to be expanded. Finally, call
 ::vl_homogeneouskernelmap_delete to delete the map object.

 ::vl_homogeneouskernelmap_new requires the kernel type (see
 ::VlHomogeneousKernelType), th approximation order @c order and the
 sampling step @a L. The approximation order trades off the quality
 and dimensionality of the approximation. The resulting feature map
 @f$ \Psi(x) @f$ is <code>d*(2*order+1)</code> dimensional.

 The sampling step @c step should be tuned to optimize the
 representation for the range of logarithmic ratios @f$ | \log(x / y)
 | @f$ for which the kernel @f$ K(x,y) @f$ needs to be accurately
 represented (note that for large values of the ratio the kernel is
 very small in any case, so the approximation quality becomes less
 important). While the step should be tuned by numerical evaluation,
 values in the range (.25, .5) usually work well in applications.  See
 [1] for details.

 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
 @section homkermap-tech Technical details
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

 The code uses the expressions given in [1] to compute in closed form
 the maps @f$ \Psi(x) @f$ for the Chi2 and intersection kernels.  For
 efficiency reasons, it tabulates @f$ \Psi(x) @f$ when the map object
 is created.

 The table stores @f$ \Psi(x) \in \mathbb{R}^{2n+1} @f$ for @f$ x \geq
 0 @f$ sampled logarithmically. In particular, @c x is decomposed as
 <pre>
   x = mantissa * (2**exponent),
   minExponent <= exponent <= maxExponent,
   1 <= matnissa < 2.
 </pre>
 Each octave is further subdivided in @c numSubdivisions sublevels.

 When the map is evaluated, @c x is decomposed in exponent and mantissa,
 and the result is computed by bilinear interpolation from the appropriate
 table entries.

 */

#ifndef VL_HOMKERMAP_INSTANTIATING

#include "homkermap.h"
#include "mathop.h"

#include <math.h>

/** ------------------------------------------------------------------
 ** @brief Create new map object
 ** @param kernelType type of homogeneous kernel.
 ** @param order approximation order.
 ** @param step sampling step.
 ** @return the new kernel map, or NULL if memory is exhausted.
 **
 ** The function intializes a new homogeneous kernel map for the
 ** specified kernel type, approximation order, and sampling step.
 ** See @ref homkermap-overview for details.
 **/

VlHomogeneousKernelMap *
vl_homogeneouskernelmap_new (VlHomogeneousKernelType kernelType, vl_size order, double step)
{
  VlHomogeneousKernelMap * self = vl_malloc(sizeof(VlHomogeneousKernelMap)) ;
  if (! self) return NULL ;
  self->kernelType = kernelType ;
  self->order = order ;
  self->step = step ;
  self->numSubdivisions = 8  ;
  self->subdivision = 1.0 / self->numSubdivisions ;
  self->minExponent = -20 ;
  self->maxExponent = 8 ;

  assert(kernelType == VlHomogeneousKernelIntersection ||
         kernelType == VlHomogeneousKernelChi2 ||
         kernelType == VlHomogeneousKernelJS) ;

  self->table = vl_malloc (sizeof(double) *
                           (2*self->order+1) *
                           self->numSubdivisions *
                           (self->maxExponent - self->minExponent + 1)) ;
  if (! self->table) {
    vl_free(self) ;
    return NULL ;
  }

  {
    vl_int exponent ;
    double * tablep  = self->table ;
    for (exponent  = self->minExponent ;
         exponent <= self->maxExponent ; ++ exponent) {

      double L = self->step ;
      double x, Lx, sqrtLx, Llogx, lambda ;
      double kappa, sqrtkappa0, sqrt2kappa ;
      double mantissa = 1.0 ;
      vl_uint i, j ;

      for (i = 0 ; i < self->numSubdivisions ;
           ++i, mantissa += self->subdivision) {
        x = ldexp(mantissa, exponent) ;

        Lx = self-> step * x ;
        sqrtLx = sqrt(Lx) ;

        switch (self->kernelType) {
          case VlHomogeneousKernelIntersection:
            sqrtkappa0 = sqrt(2.0 / VL_PI) ;
            break ;
          case VlHomogeneousKernelChi2:
            sqrtkappa0 = 1.0 ;
            break ;
          case VlHomogeneousKernelJS:
            sqrtkappa0 = sqrt(2.0 / log(4.0)) ;
            break ;
          default:
            abort() ;
        }
        *tablep++ = sqrtkappa0 * sqrtLx ;

        for (j = 1 ; j <= self->order ; ++j) {
          lambda = j * L ;
          Llogx = L * log(x) ;
          switch (kernelType) {
            case VlHomogeneousKernelIntersection:
              kappa = (2.0 / VL_PI) / (1 + 4 * lambda*lambda) ;
              break ;
            case VlHomogeneousKernelChi2:
              kappa = 2.0 / (exp(VL_PI * lambda) + exp(-VL_PI * lambda)) ;
              break ;
            case VlHomogeneousKernelJS:
              kappa = (2.0 / log(4.0)) *
              2.0 / (exp(VL_PI * lambda) + exp(-VL_PI * lambda)) /
              (1 + 4 * lambda*lambda) ;
              break ;
            default:
              abort() ;
          }
          sqrt2kappa = sqrt(2.0 * kappa) ;
          *tablep++ = sqrt2kappa * sqrtLx * cos(j * Llogx) ;
          *tablep++ = sqrt2kappa * sqrtLx * sin(j * Llogx) ;
        }
      } /* next mantissa */
    } /* next exponent */
  } /* initialize table */
  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Delete a map object
 ** @param self map object.
 ** The function deletes the specified map object.
 **/

VL_EXPORT void
vl_homogeneouskernelmap_delete (VlHomogeneousKernelMap * self)
{
  vl_free(self->table) ;
  self->table = NULL ;
  vl_free(self) ;
}

/** ------------------------------------------------------------------
 ** @fn ::vl_homogeneouskernelmap_evaluate_d(VlHomogeneousKernelMap const*,double*,vl_size,double)
 ** @brief Evaluate map
 ** @param self map object.
 ** @param destination output buffer.
 ** @param stride stride of the output buffer.
 ** @param x value to expand.
 **
 ** The function evaluates the feature map on @a x and stores the
 ** resulting <code>2*order+1</code> dimensional vector to
 ** @a destination[0], @a destination[stride], @a destination[2*stride], ....
 **/

/** ------------------------------------------------------------------
 ** @fn ::vl_homogeneouskernelmap_evaluate_f(VlHomogeneousKernelMap const*,float*,vl_size,double)
 ** @brief Evaluate map
 ** @param self map object.
 ** @param destination output buffer.
 ** @param stride stride of the output buffer.
 ** @param x value to expand.
 **
 ** The function evaluates the feature map on @a x and stores the
 ** resulting <code>2*order+1</code> dimensional vector to
 ** @a destination[0], @a destination[stride], @a destination[2*stride], ....
 **/

#define FLT VL_TYPE_FLOAT
#define VL_HOMKERMAP_INSTANTIATING
#include "homkermap.c"

#define FLT VL_TYPE_DOUBLE
#define VL_HOMKERMAP_INSTANTIATING
#include "homkermap.c"

#else

#include "float.th"

VL_EXPORT void
VL_XCAT(vl_homogeneouskernelmap_evaluate_,SFX)
(VlHomogeneousKernelMap const * self,
 T * destination,
 vl_size stride,
 double x)
{
  /* break value into exponent and mantissa */
  int exponent ;
  int unsigned j ;
  double mantissa = frexp(x, &exponent) ;
  mantissa *= 2 ;
  exponent -- ;

  if (mantissa == 0 ||
      exponent <= self->minExponent ||
      exponent >= self->maxExponent) {
    for (j = 0 ; j <= self->order ; ++j) {
      *destination = (T) 0.0 ;
      destination += stride ;
    }
    return  ;
  }
  {
    vl_size featureDimension = 2*self->order + 1 ;
    double const * v1 = self->table +
    (exponent - self->minExponent) * self->numSubdivisions * featureDimension ;
    double const * v2 ;
    double f1, f2 ;

    mantissa -= 1.0 ;
    while (mantissa >= self->subdivision) {
      mantissa -= self->subdivision ;
      v1 += featureDimension ;
    }
    v2 = v1 + featureDimension ;
    for (j = 0 ; j < featureDimension ; ++j) {
      f1 = *v1++ ;
      f2 = *v2++ ;
      *destination = (T) ((f2 - f1) * (self->numSubdivisions * mantissa) + f1) ;
      destination += stride ;
    }
  }
}

#undef FLT
#undef VL_HOMKERMAP_INSTANTIATING
#endif /* VL_HOMKERMAP_INSTANTIATING */
