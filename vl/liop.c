/** @file liop.c
 ** @brief Local Intensity Order Pattern (LIOP) descriptor - Definition
 ** @author Hana Sarbortova
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Hana Sarbortova and Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page liop Local Intensity Order Pattern (LIOP) descriptor
@author Hana Sarbortova
@author Andrea Vedaldi
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref liop.h implements *Local Intensity Order Pattern descriptor*
(LIOP) of @cite{wang11local}. LIOP is a local image descriptor,
similarly to the @ref sift "SIFT descriptor".

@ref liop-starting demonstrates how to use the C API to compute the
LIOP descriptor of a patch. For further details refer to:

- @subpage liop-fundamentals - LIOP definition and parameters.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-starting Getting started with LIOP
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The following code fragment demonstrates how tow to use @ref liop.h in
a C program in order to compute the LIOP descriptor of an image patch.

@code
#include <vl/liop.h>

// Create a new object instance (these numbers coresponds to parameter
// values proposed by authors of the paper, except for 41)
vl_size sideLength = 41 ;
VlLiopDesc * liop = vl_liopdesc_new_basic (sideLength);

// allocate the descriptor array
vl_size dimension = vl_liopdesc_get_dimension(liop) ;
float * desc = vl_malloc(sizeof(float) * dimension) ;

// compute descriptor from a patch (an array of length sideLegnth *
// sideLength)
vl_liopdesc_process(liop, desc, patch) ;

// delete the object
vl_liopdesc_delete(liop) ;
@endcode

The image patch must be of odd side length and in single
precision. There are several parameters affecting the LIOP
descriptor. An example is the @ref liop-weighing "threshold" used to
discard low-contrast oder pattern in the computation of the
statistics. This is changed by using ::vl_liopdesc_set_threshold.

**/


/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page liop-fundamentals LIOP fundamentals
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The *Local Invariant Order Pattern* (LIOP) descriptor
@cite{wang11local} is a local feature descriptor. It is by design
invariant to monotonic intensity changes and image rotations and is
robust to several other geometric and photometric transformations.

LIOP is based on the concept of *orer pattern*. Consider a pixel $\bx$
and $n$ neighbours $\bx_1,\bx_2,\dots,\bx_n$. The order pattern is the
permutation $\sigma$ that sorts the neighbours by increasing intenstiy
$I(\bx_{\sigma(1)}) \leq I(\bx_{\sigma(2)}) \leq \dots \leq
I(\bx_{\sigma(2)})$.

LIOP builds a histograms of the order patterns in the patch. In order
to map permutations to histogram bins one considers a lexycogrpahic
ordering of the permutations. For example, for $n=4$ the
lexycographical ordering is

Permutation   | Lexycographical rank
--------------|----------------------
1 2 3 4       | 1
1 2 4 3       | 2
1 3 2 4       | 3
1 3 4 2       | 4
...           | ...
4 3 1 2       | 23
4 3 2 1       | 24

The order pattern $\sigma$ computed at location $\bx$ is then mapped
to the histogram bin of index $q(\bx) \in [1, n!]$, equal to the rank
of the permutation in this ordering (thus the histogram has
dimensionality $n!$).

In order to make the order patterns invariant to a rotation of the
patch, the samples $\bx_1,\dots,\bx_n$ are taken in a covariant
manner. The simplest way to do so is to sample points in a circle
around $\bx$, in anticlockwise order, using the radius connecting
$\bx$ to the patch center as a reference to identify the first sample:

@image html liop-neighbours-sampling.png "Sampling order of neighbouring points"

The sample points do not necessarily have integer coordinates; so
$I(\bx_i)$ is computed using bilinear interpolation.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-spatial-binning Intensity rank spatial binning
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Histograms of order patterns could be pooled from all pixels $bx$ in
the image patch. However, there are a few restrictions:

- Only pixels sufficiently far away from the boundary that their
  circular neighbours are included in the patch can be used.
- Only pixels within a cricular regions inside the patch can be used
  in orer to guaranteee rotation invariance.

Furthermore, in order to increase the discriminative power of these
histograms, LIOP breaks the spatial domain in a number of regions,
performing histogramming independently on each, and concatenating the
results.  Such regions need to be extracted in a rotation covariant
manner. This is obtained by considering level sets of the image patch:
\[
 R_t = \{\bx : \tau_{t} \leq I(\bx) < \tau_{t+1} \}.
\]
In order to be invariant to monotonic changes of the intensity, the
thresolds $\tau_t$ are selected such that each region contains the
same number of pixels. This is obtained quickly by sorting pixels by
increasing intesity and then dividing the list into $m$ equal parts.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-weighing Weighing and pooling
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The local intensity features $q(\bx)$ are then pooled in each spatial
bin, forming a histogram of size $!n$. However, contributions are
weighted to account for the stability of each elementary feature. The
latter, is assumed to be proportional to the number of sufficiently
dissimilar pairs forming the local neighbourhood. Two intensities are
sufficiently dissimilar if their absolute difference is above a
threshod $\Theta$:

@f[
w(\bx) = \sum_{ij} [ |I(\bx_{i}) - I(\bx_{j})| >  \Theta) ]
@f]

where $[\cdot]$ is the indicator function.

In VLFeat LIOP implementation, The threshold $\Theta$ is either set as
an absolute value, or as a faction of the difference between the
maximum or minimum intensity in the circular patch.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-normalization Normalization
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Consider the matrix of weighted histogram values

\[
  h_{qt} = \sum_{\bx : q(\bx) = q \ \wedge\  \bx \in R_t} w(\bx).
\]

The LIOP descriptor is obtained by stacking this matrix into a vector
$\mathbf{h}$ and then normalising it:

\[
  \Phi = \frac{\mathbf{h}}{\|\mathbf{h}\|_2}
\]

The dimensionality is therefore $m n!$, where $m$ is the @c
numSpatialBins number of spatial bins and $n$ is the @c numNeighbours
number of neighbours. The descriptor is approximately stored as a
vector of integers as

\[
 \operatorname{round}\left[ 255\, \times \Phi\right].
\]

*/

#include "liop.h"
#include "mathop.h"
#include "imopv.h"
#include <strings.h>

#define DEFAULT_INTENSITY_THRESHOLD -(5.0/255)
#define DEFAULT_RADIUS 6.0
#define DEFAULT_NUM_SPATIAL_BINS 6
#define DEFAULT_NUM_NEIGHBOURS 4

/* ---------------------------------------------------------------- */
/*                                                 Helper functions */
/* ---------------------------------------------------------------- */

static
vl_int factorial(vl_int num)
{
  vl_int result = 1;
  while(num > 1){
    result = num*result;
    num--;
  }
  return result ;
}

/** @internal @brief Compute permutation index.
 ** @param permutation array containing all values from 0 to (size - 1) (input/output).
 ** @param size size of the permutation array.
 ** @return permutation index.
 **
 ** Compute the position of @a permutation in the lexycographcial
 ** sorting of permutations of the given @a size.
 **
 ** For example, in the lexicographical ordering, permutations of four elements
 ** are listed as [1 2 3 4], [1 2 4 3], [1 3 2 4], [1 3 4 2], [1 4 2 3],
 ** [1 4 3 2], [2 1 3 4], ..., [4 3 2 1].
 **
 ** The index can be computed as follows. First pick the first digit
 ** perm[1]. This is either 1,2,...,n. For each
 ** choice of the first digits, there are (n-1)! other permutations, separated
 ** therefore by (n-1)! elements in lexicographical order.
 **
 ** Process then the second digit perm[2]. This can be though as finding
 ** the lexycotraphical index of perm[2], ..., perm[n], a permutation of
 ** n-1 elements. This can be explicitly obtained by taking out 1 from
 ** all elements perm[i] > perm[1]. */

VL_INLINE vl_index get_permutation_index(vl_uindex *permutation, vl_size size){
  vl_index index = 0 ;
  vl_index i ;
  vl_index j ;

  for (i = 0 ; i < (signed)size ; ++i) {
    index = index * ((signed)size - i) + permutation[i] ;
    for (j = i + 1 ; j < (signed)size ; ++j) {
      if (permutation[j] > permutation[i]) { permutation[j] -- ; }
    }
  }
  return index ;
}

/* instantiate two quick sort algorithms */
VL_INLINE float patch_cmp (VlLiopDesc * liop, vl_index i, vl_index j)
{
  vl_index ii = liop->patchPermutation[i] ;
  vl_index jj = liop->patchPermutation[j] ;
  return liop->patchIntensities[ii] - liop->patchIntensities[jj] ;
}

VL_INLINE void patch_swap (VlLiopDesc * liop, vl_index i, vl_index j)
{
  vl_index tmp = liop->patchPermutation[i] ;
  liop->patchPermutation[i] = liop->patchPermutation[j] ;
  liop->patchPermutation[j] = tmp ;
}

#define VL_QSORT_prefix patch
#define VL_QSORT_array VlLiopDesc*
#define VL_QSORT_cmp patch_cmp
#define VL_QSORT_swap patch_swap
#include "qsort-def.h"

VL_INLINE float neigh_cmp (VlLiopDesc * liop, vl_index i, vl_index j)
{
  vl_index ii = liop->neighPermutation[i] ;
  vl_index jj = liop->neighPermutation[j] ;
  return liop->neighIntensities[ii] - liop->neighIntensities[jj] ;
}

VL_INLINE void neigh_swap (VlLiopDesc * liop, vl_index i, vl_index j)
{
  vl_index tmp = liop->neighPermutation[i] ;
  liop->neighPermutation[i] = liop->neighPermutation[j] ;
  liop->neighPermutation[j] = tmp ;
}

#define VL_QSORT_prefix neigh
#define VL_QSORT_array VlLiopDesc*
#define VL_QSORT_cmp neigh_cmp
#define VL_QSORT_swap neigh_swap
#include "qsort-def.h"

/* ---------------------------------------------------------------- */
/*                                            Construct and destroy */
/* ---------------------------------------------------------------- */

/** @brief Create a new LIOP object instance.
 ** @param numNeighbours number of neighbours.
 ** @param numSpatialBins number of bins.
 ** @param radius radius of the cirucal sample neighbourhoods.
 ** @param sideLength width of the input image patch (the patch is square).
 ** @return new object instance.
 **
 ** The value of @a radius should be at least less than half the @a
 ** sideLength of the patch.
 **/

VlLiopDesc *
vl_liopdesc_new (vl_int numNeighbours, vl_int numSpatialBins,
                 float radius, vl_size sideLength)
{
  vl_index i, t ;
  VlLiopDesc * self = vl_calloc(sizeof(VlLiopDesc), 1);

  assert(radius <= sideLength/2) ;

  self->numNeighbours = numNeighbours ;
  self->numSpatialBins = numSpatialBins ;
  self->neighRadius = radius ;
  self->intensityThreshold = DEFAULT_INTENSITY_THRESHOLD ;

  self->dimension = factorial(numNeighbours) * numSpatialBins ;

  /*
   Precompute a list of pixels within a circular patch inside
   the square image. Leave a suitable marging for sampling around
   these pixels.
   */

  self->patchSize = 0 ;
  self->patchPixels = vl_malloc(sizeof(vl_uindex)*sideLength*sideLength) ;
  self->patchSideLength = sideLength ;

  {
    vl_index x, y ;
    vl_index center = (sideLength - 1) / 2 ;
    double t = center - radius + 0.6 ;
    vl_index t2 = (vl_index) (t * t) ;
    for (y = 0 ; y < (signed)sideLength ; ++y) {
      for (x = 0 ; x < (signed)sideLength ; ++x) {
        vl_index dx = x - center ;
        vl_index dy = y - center ;
        if (x == 0 && y == 0) continue ;
        if (dx*dx + dy*dy <= t2) {
          self->patchPixels[self->patchSize++] = x + y * sideLength ;
        }
      }
    }
  }

  self->patchIntensities = vl_malloc(sizeof(vl_uindex)*self->patchSize) ;
  self->patchPermutation = vl_malloc(sizeof(vl_uindex)*self->patchSize) ;

  /*
   Precompute the samples in the circular neighbourhood of each
   measurement point.
   */

  self->neighPermutation = vl_malloc(sizeof(vl_uindex) * self->numNeighbours) ;
  self->neighIntensities = vl_malloc(sizeof(float) * self->numNeighbours) ;
  self->neighSamplesX = vl_calloc(sizeof(double), self->numNeighbours * self->patchSize) ;
  self->neighSamplesY = vl_calloc(sizeof(double), self->numNeighbours * self->patchSize) ;

  for (i = 0 ; i < (signed)self->patchSize ; ++i) {
    vl_index pixel ;
    double x, y ;
    double dangle = 2*VL_PI / (double)self->numNeighbours ;
    double angle0 ;
    vl_index center = (sideLength - 1) / 2 ;

    pixel = self->patchPixels[i] ;
    x = (pixel % (signed)self->patchSideLength) - center ;
    y = (pixel / (signed)self->patchSideLength) - center ;

    angle0 = atan2(y,x) ;

    for (t = 0 ; t < (signed)self->numNeighbours ; ++t) {
      double x1 = x + radius * cos(angle0 + dangle * t) + center ;
      double y1 = y + radius * sin(angle0 + dangle * t) + center ;
      self->neighSamplesX[t + (signed)self->numNeighbours * i] = x1 ;
      self->neighSamplesY[t + (signed)self->numNeighbours * i] = y1 ;
    }
  }
  return self ;
}

/** @brief Create a new object with default parameters
 ** @param patchSideLength size of the patches to be processed.
 ** @return new object.
 **
 ** @see ::vl_liopdesc_new. */

VlLiopDesc * vl_liopdesc_new_basic (vl_size sideLength)
{
  return vl_liopdesc_new(DEFAULT_NUM_NEIGHBOURS,
                         DEFAULT_NUM_SPATIAL_BINS,
                         DEFAULT_RADIUS,
                         sideLength) ;
}

/** @brief Delete object instance.
 ** @param self object instance. */

void
vl_liopdesc_delete (VlLiopDesc * self)
{
  vl_free (self->patchPixels) ;
  vl_free (self->patchIntensities) ;
  vl_free (self->patchPermutation) ;
  vl_free (self->neighSamplesX) ;
  vl_free (self->neighSamplesY) ;
  vl_free (self) ;
}

/* ---------------------------------------------------------------- */
/*                                          Compute LIOP descriptor */
/* ---------------------------------------------------------------- */

/** @brief Compute liop descriptor for a patch
 ** @param self object instance
 ** @param desc descriptor to be computed (output).
 ** @param patch patch to process
 **
 ** Use ::vl_liopdesc_get_dimension to get the size of the descriptor
 ** @desc. */

void
vl_liopdesc_process (VlLiopDesc * self, float * desc, float const * patch)
{
  vl_index i,t ;
  vl_index offset,numPermutations ;
  vl_index spatialBinArea, spatialBinEnd, spatialBinIndex ;
  float threshold ;

  memset(desc, 0, self->dimension) ;

  /*
   * Sort pixels in the patch by increasing intensity.
   */

  for (i = 0 ; i < (signed)self->patchSize ; ++i) {
    vl_index pixel = self->patchPixels[i] ;
    self->patchIntensities[i] = patch[pixel] ;
    self->patchPermutation[i] = i ;
  }
  patch_sort(self, self->patchSize) ;

  /*
   * Tune the threshold if needed
   */

  if (self->intensityThreshold < 0) {
    i = self->patchPermutation[0] ;
    t = self->patchPermutation[self->patchSize-1] ;
    threshold = - self->intensityThreshold
    * (self->patchIntensities[t] - self->patchIntensities[i]);
  } else {
    threshold = self->intensityThreshold ;
  }

  /*
   * Process pixels in order of increasing intenisity, dividing them into
   * spatial bins on the fly.
   */

  numPermutations = factorial(self->numNeighbours) ;
  spatialBinArea = self->patchSize / self->numSpatialBins ;
  spatialBinEnd = spatialBinArea ;
  spatialBinIndex = 0 ;
  offset = 0 ;

  for (i = 0 ; i < (signed)self->patchSize ; ++i) {
    vl_index permIndex ;
    double *sx, *sy ;

    /* advance to the next spatial bin if needed */
    if (i >= (signed)spatialBinEnd && spatialBinIndex < (signed)self->numSpatialBins - 1) {
      spatialBinEnd += spatialBinArea ;
      spatialBinIndex ++ ;
      offset += numPermutations ;
    }

    /* get intensities of neighbours of the current patch element and sor them */
    sx = self->neighSamplesX + self->numNeighbours * self->patchPermutation[i] ;
    sy = self->neighSamplesY + self->numNeighbours * self->patchPermutation[i] ;
    for (t = 0 ; t < self->numNeighbours ; ++t) {
      double x = *sx++ ;
      double y = *sy++ ;

      /* bilinear interpolation */
      vl_index ix = vl_floor_d(x) ;
      vl_index iy = vl_floor_d(y) ;

      double wx = x - ix ;
      double wy = y - iy ;

      double a = 0, b = 0, c = 0, d = 0 ;

      int L = (int) self->patchSideLength ;

      if (ix >= 0 && iy >= 0) { a = patch[ix   + iy * L] ; }
      if (ix <  L && iy >= 0) { b = patch[ix+1 + iy * L] ; }
      if (ix >= 0 && iy <  L) { c = patch[ix   + (iy+1) * L] ; }
      if (ix <  L && iy <  L) { d = patch[ix+1 + (iy+1) * L] ; }

      self->neighPermutation[t] = t;
      self->neighIntensities[t] = (1 - wy) * (a + (b - a) * wx) + wy * (c + (d - c) * wx) ;
    }
    neigh_sort (self, self->numNeighbours) ;

    /* get permutation index */
    permIndex = get_permutation_index(self->neighPermutation, self->numNeighbours);

    /*
     * Compute weight according to difference in intensity values and
     * accumulate.
     */
    {
      int k, t ;
      float weight = 0 ;
      for(k = 0; k < self->numNeighbours ; ++k) {
        for(t = k + 1; t < self->numNeighbours; ++t){
          double a = self->neighIntensities[k] ;
          double b = self->neighIntensities[t] ;
          weight += (a > b + threshold || b > a + threshold) ;
        }
      }
      desc[permIndex + offset] += weight ;
    }
  }

  /* normalization */
  {
    float norm = 0;
    for(i = 0; i < (signed)self->dimension; i++) {
      norm += desc[i]*desc[i];
    }
    norm = VL_MAX(sqrt(norm), 1e-12) ;
    for(i = 0; i < (signed)self->dimension; i++){
      desc[i] /= norm ;
    }
  }
}


/* ---------------------------------------------------------------- */
/*                                              Getters and setters */
/* ---------------------------------------------------------------- */

/** @brief Get the dimension of a LIOP descriptor.
 ** @return dimension. */

vl_size
vl_liopdesc_get_dimension (VlLiopDesc const * self)
{
  return self->dimension ;
}


/** @brief Get the number of neighbours.
 ** @param self object.
 ** @return number of neighbours.
 **/

vl_size
vl_liopdesc_get_num_neighbours (VlLiopDesc const * self)
{
  assert(self) ;
  return self->numNeighbours ;
}

/** @brief Get the intensity threshold
 ** @param self object.
 ** @return intensity threshold.
 ** @see liop-weighing
 **/

float
vl_liopdesc_get_intensity_threshold (VlLiopDesc const * self)
{
  assert(self) ;
  return self->intensityThreshold ;
}

/** @brief Set the intensity threshold
 ** @param self object.
 ** @param x intensity threshold.
 **
 ** If non-negative, the threshold as is is used when comparing
 ** intensities. If negative, the absolute value of the specified
 ** number is multipled by the maximum intensity difference inside a
 ** patch to obtain the threshold.
 **
 ** @see liop-weighing
 **/

void
vl_liopdesc_set_intensity_threshold (VlLiopDesc * self, float x)
{
  assert(self) ;
  self->intensityThreshold = x ;
}

/** @brief Get the neighbourhood radius.
 ** @param self object.
 ** @return neighbourhood radius.
 **/

double
vl_liopdesc_get_neighbourhood_radius (VlLiopDesc const * self)
{
  assert(self) ;
  return self->neighRadius ;
}

/** @brief Get the number of spatial bins.
 ** @param self object.
 ** @return number of spatial bins.
 **/

vl_size
vl_liopdesc_get_num_spatial_bins (VlLiopDesc const * self)
{
  assert(self) ;
  return self->numSpatialBins ;
}
