/** @file homkermap.c
 ** @brief Homogeneous kernel map - Definition
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/** @file homkermap.h

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@page homkermap Homogeneous kernel map
@author Andrea Vedaldi
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

@ref homkermap.h implements the homogeneous kernel maps
introduced in @cite{vedaldi10efficient},@cite{vedaldi11efficient}.
Such maps are efficient linear representations of popular kernels
such as the intersection, @f$ \chij^2 @f$, and Jensen-Shannon ones.

- @ref homkermap-overview
  - @ref homkermap-overview-negative
  - @ref homkermap-overview-homogeneity
  - @ref homkermap-overview-window
- @ref homkermap-usage
- @ref homkermap-tech

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@section homkermap-overview Overview
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

The <em>homogeneous kernel map</em> is a finite dimensional linear
approximation of homgeneous kernels, including the intersection,
@f$ \chi^2 @f$, and Jensen-Shannon kernels. These kernels
 are ffrequently used in computer vision applications because they
are particular suitable for data in the format of histograms, which
 encompasses many visual descriptors used.

Let @f$ x,y \in \mathbb{R}_+ @f$ be non-negative scalars and let @f$
k(x,y) \in \mathbb{R} @f$ be an homogeneous kernel such as the
@f$ \chi^2 @f$ and or the intersection ones:

@f[
  k_{\mathrm{inters}}(x,y) = \min\{x, y\},
  \quad
  k_{\chi^2}(x,y) = 2 \frac{(x - y)^2}{x+y}.
@f]

For vectorial data @f$ \mathbf{x},\mathbf{y} \in \mathbb{R}_+^d @f$, the
homogeneous kernels in an <em>additive
combination</em> @f$ K(\mathbf{x},\mathbf{y}) = \sum_{i=1}^d k(x_i,y_i) @f$.

The <em>homogeneous kernel map</em> of order @f$ n @f$ is a vectorial
function @f$ \Psi(x) \in \mathbb{R}^{2n+1} @f$ such that, for any
choice of @f$ x, y \in \mathbb{R}_+ @f$, one has

@f[
  k(x,y) \approx \langle \Psi(x), \Psi(y) \rangle.
@f]

Given the feature map for the scalar case, the corresponding feature
 map @f$ \Psi(\mathbf{x}) @f$ for the vectorial case is obtained
 by stacking @f$ [\Psi(x_1), \dots, \Psi(x_n)] @f$.
Note that the combined feature @f$ \Psi(\mathbf{x}) @f$ has dimension @f$ d(2n+1) @f$.

Using linear analysis tools (e.g. a linear support vector machine)
on top of dataset that has been encoded by the homogeneous kernel map
is therefore approximately equivalent to using a method based
on the corresponding non-linear kernel.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@subsection homkermap-overview-negative Extension to the negative reals
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

Any positive (semi-)definite kernel @f$ k(x,y) @f$ defined on the
non-negative reals @f$ x,y \in \mathbb{R}_+ @f$ can be extended to the
entiere real line by using the definition:

@f[
  k_\pm(x,y) = \operatorname{sign}(x) \operatorname{sign}(y) k(|x|,|y|).
@f]

The homogeneous kernel map implements this extension by defining @f$
\Psi_\pm(x) = \operatorname{sign}(x) \Psi(|x|) @f$. Note that other
extensions are possible, such as

@f[
  k_\pm(x,y) = H(xy) \operatorname{sign}(y) k(|x|,|y|)
@f]

where @f$ H @f$ is the Heavyside function, but may require higher
dimensional feature maps.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@subsection homkermap-overview-homogeneity Homogeneity order
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

Any (1-)homogeneous kernel @f$ k_1(x,y) @f$ can be extended to a so
called gamma-homgeneous kernel @f$ k_\gamma(x,y) @f$ by the definition

@f[
  k_\gamma(x,y) = (xy)^{\frac{\gamma}{2}} \frac{k_1(x,y)}{\sqrt{xy}}
@f]

Smaller value of @f$ \gamma @f$ enhance the kernel non-linearity and
are sometimes beneficial in applications (see [1,2] for details).

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@subsection homkermap-overview-window Windowing and period
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

This section discusses aspects of the homogeneous kernel map which are
more technical and may be skipped. The homogeneous kernel map
approximation is based on periodicizing the kernel; given the kernel
signature

@f[
    \Kappa(\lambda) = k(e^{\frac{\lambda}{2}}, e^{-\frac{\lambda}{2}})
@f]

the homogeneous kerne map is a feature map for the windowed and
periodicized kernel whose signature is given by

@f[
   \hat{\mathcal{K}}(\lambda)
   =
   \sum_{i=-\infty}^{+\infty} \mathcal{K}(\lambda + k \Lambda) W(\lambda + k \Lambda)
@f]

where @f$ W(\lambda) @f$ is a windowing function and @f$ \Lambda @f$
is the period. This implementation of the homogeneous kernel map
supports the use of a <em>uniform window</em> (@f$ W(\lambda) = 1 @f$)
or of a <em>rectangular window</em> (@f$ W(\lambda) =
\operatorname{rect}(\lambda/\Lambda) @f$). Note that @f$ \lambda =
\log(y/x) @f$ is equal to the logarithmic ratio of the arguments of
the kernel. Empirically, the rectangular window seems to have a slight
edge in applications.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@section homkermap-usage Usage
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

The homogeneous kernel map is implemented as an object of type
::VlHomogeneousKernelMap. To use thois object, first create an
instance by ::vl_homogeneouskernelmap_new, then use
::vl_homogeneouskernelmap_evaluate_d or
::vl_homogeneouskernelmap_evaluate_f (depdening on whether the data is
@c double or @c float) to compute the feature map @f$ \Psi(x)
@f$. When done, dispose of the object by calling
::vl_homogeneouskernelmap_delete.

The constructor ::vl_homogeneouskernelmap_new requires the kernel type
@c kernel (see ::VlHomogeneousKernelType), the homogeneity order @c
gamma (use one for the standard kernels), the approximation order @c
order (usually order one is enough), the period @a period (use a
negative value to use the default period), and a window type @c window
(use ::VlHomogeneousKernelMapWindowRectangular if unsure). The
approximation order trades off the quality and dimensionality of the
approximation. The resulting feature map @f$ \Psi(x) @f$, computed by
::vl_homogeneouskernelmap_evaluate_d or
::vl_homogeneouskernelmap_evaluate_f , is <code>2*order+1</code>
dimensional.

The code pre-computes the map @f$ \Psi(x) @f$ for efficient
evaluation. The table spans values of @f$ x @f$ in the range
@f$[2^{-20}, 2^{8}) @f$. In particular, values smaller than @f$
2^{-20} @f$ are treated as zeroes (which result in a null feature).

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
@section homkermap-tech Technical details
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->

The code uses the expressions given in @cite{vedaldi10efficient},@cite{vedaldi11efficient}
to compute in closed form the maps @f$ \Psi(x) @f$ for the suppoerted kernel types.  For
efficiency reasons, it tabulates @f$ \Psi(x) @f$ when the homogeneous
kernel map object is created.

The interal table stores @f$ \Psi(x) \in \mathbb{R}^{2n+1} @f$ for
@f$ x \geq 0 @f$ by sampling this variable. In particular, @c x is
decomposed as
<pre>
  x = mantissa * (2**exponent),
  minExponent <= exponent <= maxExponent,
  1 <= matnissa < 2.
</pre>
Each octave is further subdivided in @c numSubdivisions sublevels.

When the map @f$ \Psi(x) @f$ is evaluated, @c x is decomposed back
into exponent and mantissa, and the result is computed by bilinear
interpolation from the appropriate table entries.

*/

#ifndef VL_HOMKERMAP_INSTANTIATING

#include "homkermap.h"
#include "mathop.h"

#include <math.h>

/** ------------------------------------------------------------------
 ** @brief Sample the kernel specturm
 ** @param self homogeneous kernel map.
 ** @param omega sampling frequency.
 ** @return the spectrum sampled at @a omega.
 **/

VL_INLINE double
vl_homogeneouskernelmap_get_spectrum (VlHomogeneousKernelMap const * self, double omega)
{
  assert (self) ;
  switch (self->kernelType) {
    case VlHomogeneousKernelIntersection:
      return (2.0 / VL_PI) / (1 + 4 * omega*omega) ;
    case VlHomogeneousKernelChi2:
      return 2.0 / (exp(VL_PI * omega) + exp(-VL_PI * omega)) ;
    case VlHomogeneousKernelJS:
      return (2.0 / log(4.0)) *
      2.0 / (exp(VL_PI * omega) + exp(-VL_PI * omega)) /
      (1 + 4 * omega*omega) ;
    default:
      abort() ;
  }
}

/* helper */
VL_INLINE double sinc(double x)
{
  if (x == 0.0) return 1.0 ;
  return sin(x) / x ;
}

/** ------------------------------------------------------------------
 ** @brief Sample the smoothed kernel spectrum
 ** @param self homogeneous kernel map.
 ** @param omega sampling frequency.
 ** @return the spectrum sampled at @a omega after smoothing.
 **/

VL_INLINE double
vl_homogeneouskernelmap_get_smooth_spectrum (VlHomogeneousKernelMap const * self, double omega)
{
  double kappa_hat = 0 ;
  double omegap ;
  double epsilon = 1e-2 ;
  double const omegaRange = 2.0 / (self->period * epsilon) ;
  double const domega = 2 * omegaRange / (2 * 1024.0 + 1) ;
  assert (self) ;
  switch (self->windowType) {
    case VlHomogeneousKernelMapWindowUniform:
      kappa_hat = vl_homogeneouskernelmap_get_spectrum(self, omega) ;
      break ;
    case VlHomogeneousKernelMapWindowRectangular:
      for (omegap = - omegaRange ; omegap <= omegaRange ; omegap += domega) {
        double win = sinc((self->period/2.0) * omegap) ;
        win *= (self->period/(2.0*VL_PI)) ;
        kappa_hat += win * vl_homogeneouskernelmap_get_spectrum(self, omegap + omega) ;
      }
      kappa_hat *= domega ;
      /* project on the postivie orthant (see PAMI) */
      kappa_hat = VL_MAX(kappa_hat, 0.0) ;
      break ;
    default:
      abort() ;
  }
  return kappa_hat ;
}

/** ------------------------------------------------------------------
 ** @brief Create a new homgeneous kernel map
 ** @param kernelType type of homogeneous kernel.
 ** @param gamma kernel homogeneity degree.
 ** @param order approximation order.
 ** @param period kernel period.
 ** @param windowType type of window used to truncate the kernel.
 ** @return the new homogeneous kernel map.
 **
 ** The function intializes a new homogeneous kernel map for the
 ** specified kernel type, homogeneity degree, approximation order,
 ** period, and truncation window. See @ref homkermap-overview for
 ** details.
 **
 ** The homogeneity degree @c gamma must be positive (the standard
 ** kernels are obtained by setting @c gamma to 1). When unsure, set
 ** @c windowType to ::VlHomogeneousKernelMapWindowRectangular. The @c
 ** period should be non-negative; specifying a negative or null value
 ** causes the function to switch to a default value.
 **
 ** The function returns @c NULL if there is not enough free memory.
 **/

VlHomogeneousKernelMap *
vl_homogeneouskernelmap_new (VlHomogeneousKernelType kernelType,
                             double gamma,
                             vl_size order,
                             double period,
                             VlHomogeneousKernelMapWindowType windowType)
{
  int tableWidth, tableHeight ;
  VlHomogeneousKernelMap * self = vl_malloc(sizeof(VlHomogeneousKernelMap)) ;
  if (! self) return NULL ;

  assert(gamma > 0) ;

  assert(kernelType == VlHomogeneousKernelIntersection ||
         kernelType == VlHomogeneousKernelChi2 ||
         kernelType == VlHomogeneousKernelJS) ;

  assert(windowType == VlHomogeneousKernelMapWindowUniform ||
         windowType == VlHomogeneousKernelMapWindowRectangular) ;

  if (period < 0) {
    switch (windowType) {
    case VlHomogeneousKernelMapWindowUniform:
      switch (kernelType) {
      case VlHomogeneousKernelChi2:         period = 5.86 * sqrt(order + 0)  + 3.65 ; break ;
      case VlHomogeneousKernelJS:           period = 6.64 * sqrt(order + 0)  + 7.24 ; break ;
      case VlHomogeneousKernelIntersection: period = 2.38 * log(order + 0.8) + 5.6 ; break ;
      }
      break ;
    case VlHomogeneousKernelMapWindowRectangular:
      switch (kernelType) {
      case VlHomogeneousKernelChi2:         period = 8.80 * sqrt(order + 4.44) - 12.6 ; break ;
      case VlHomogeneousKernelJS:           period = 9.63 * sqrt(order + 1.00) - 2.93;  break ;
      case VlHomogeneousKernelIntersection: period = 2.00 * log(order + 0.99)  + 3.52 ; break ;
      }
      break ;
    }
    period = VL_MAX(period, 1.0) ;
  }

  self->kernelType = kernelType ;
  self->windowType = windowType ;
  self->gamma = gamma ;
  self->order = order ;
  self->period = period ;
  self->numSubdivisions = 8 + 8*order ;
  self->subdivision = 1.0 / self->numSubdivisions ;
  self->minExponent = -20 ;
  self->maxExponent = 8 ;

  tableHeight = 2*self->order + 1 ;
  tableWidth = self->numSubdivisions * (self->maxExponent - self->minExponent + 1) ;
  self->table = vl_malloc (sizeof(double) *
                           (tableHeight * tableWidth + 2*(1+self->order))) ;
  if (! self->table) {
    vl_free(self) ;
    return NULL ;
  }

  {
    vl_int exponent ;
    vl_uindex i, j ;
    double * tablep = self->table ;
    double * kappa = self->table + tableHeight * tableWidth ;
    double * freq = kappa + (1+self->order) ;
    double L = 2.0 * VL_PI / self->period ;

    /* precompute the sampled periodicized spectrum */
    j = 0 ;
    i = 0 ;
    while (i <= self->order) {
      freq[i] = j ;
      kappa[i] = vl_homogeneouskernelmap_get_smooth_spectrum(self, j * L) ;
      ++ j ;
      if (kappa[i] > 0 || j >= 3*i) ++ i ;
    }

    /* fill table */
    for (exponent  = self->minExponent ;
         exponent <= self->maxExponent ; ++ exponent) {

      double x, Lxgamma, Llogx, xgamma ;
      double sqrt2kappaLxgamma ;
      double mantissa = 1.0 ;

      for (i = 0 ; i < self->numSubdivisions ;
           ++i, mantissa += self->subdivision) {
        x = ldexp(mantissa, exponent) ;
        xgamma = pow(x, self->gamma) ;
        Lxgamma = L * xgamma ;
        Llogx = L * log(x) ;

        *tablep++ = sqrt(Lxgamma * kappa[0]) ;
        for (j = 1 ; j <= self->order ; ++j) {
          sqrt2kappaLxgamma = sqrt(2.0 * Lxgamma * kappa[j]) ;
          *tablep++ = sqrt2kappaLxgamma * cos(freq[j] * Llogx) ;
          *tablep++ = sqrt2kappaLxgamma * sin(freq[j] * Llogx) ;
        }
      } /* next mantissa */
    } /* next exponent */
  }
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
  double sign = (mantissa >= 0.0) ? +1.0 : -1.0 ;
  mantissa *= 2*sign ;
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
      *destination = (T) sign * ((f2 - f1) * (self->numSubdivisions * mantissa) + f1) ;
      destination += stride ;
    }
  }
}

#undef FLT
#undef VL_HOMKERMAP_INSTANTIATING
#endif /* VL_HOMKERMAP_INSTANTIATING */
