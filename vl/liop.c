/** @file liop.c
 ** @brief Local Intensity Order Pattern (LIOP) descriptor
 ** @author Hana Sarbortova
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page liop Local Intensity Order Pattern descriptor
@author Hana Sarbortova
@author Andrea Vedaldi
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref liop.h implements *Local Intensity Order Pattern descriptor* (LIOP)
of @cite{wang11local}

@ref liop-starting shows how to compute LIOP using the C API.  Refer
to @subpage liop-fundamentals for a technical description of LIOP
descriptor.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-starting Getting started with LIOP
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The following code fragment demonstrates how tow to use @ref liop.h in
a C program in order to compute the LIOP descriptor of an image patch.

@code
#include <vl/liop.h>

// Create a new object instance (these numbers coresponds to parameter
// values proposed by authors of the paper, except for 41)
VlLiopDesc * liop = vl_liopdesc_new(4,6,6,5,41);

// allocate the descriptor array
float * desc = vl_malloc(sizeof(float)*liop->liopArraySize);

// compute descriptor from a patch (in this case, the patch must be
// an array of length 41*41=1681)
vl_liopdesc_process(liop, patch, desc);

// delete object instance
vl_liopdesc_delete(liop);
@endcode

Not all pixels from the patch will contribute to the descriptor as
sample points. The sample points belonging to the area of the inner
circle, as all neighbours of sample points has to be computed from the
patch. Please see the image below for better understanding.

@image html liop-patch-layout.png "LIOP patch layout"
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

For convenience, LIOP is computed from a square image patch, obtained
for example by normalizing a feature frame (keypoint) obtained from a
covariant feature detector. However, LIOP is rotation invariant. On
one side, this means that rotation detection and normalization is
unnecessary with this descriptor. On the other side, this implies that
only pixels withing a cricle inscribe in the square image patch are
actually used to compute the descriptor.

The descriptor computation can be divided into two steps. In the @ref
liop-computation-regions first step, the image is partitioned into a
number of regions basd on the intensity values of the pixels. In the
@ref liop-computation-pooling second step, a local difference operator
is applied to each image pixel, and a statistic of the output of this
operator is pooled independenlty in each of the regions.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-regions Spatial binning
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The image patch is divided into a number @c numSpatialBins of sub-regions.
Each region is defined as the set of pixels between two intensity
values:
\[
 R_t = \{\bx : \tau_{t} \leq I(\bx) < \tau_{t+1} \}.
\]
In this manner, regions $R_t,$ $t=1,\dots,$ @c numSpatialBins are
covariant with any image warping, including translation, rotation, and
scaling.

In order to make the selection of regions invariant to affine changes
of the intensity, the thresholds $\tau_t$ are selected such that each
region contains the same number of pixels. This is obtained quickly by
sorting pixels by increasing intesity and then dividing the list into
@c numSpatialBins equal parts.
 
When @c numSpatialBins does not divide exactly the number of pixels,
all the regions have the same area except the last one, which can be
larger.

(parameter to set @c numberOfBins)

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-pooling Rank features
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

After regions are computed, an operator is applied to each pixel $\bx$
in the patch in order to characterize its local appearance. This
operator start by considering the intensities $I(\bx_1), I(\bx_2),
\dots$ of @c numNeighbours image pixels in a small circle of
radius @c pointToNeighbourRadius around point $\bx$.

The layout of the sample points $\bx_1,\by_2,\dots$ is show in the
image below. The sampling order is always anticlokwise and uses the
radious connecting the patch center to $\bx$ as a reference. In this
manner, the set of samples is also co-variant with the image rotation.

@image html liop-neighbours-sampling.png "Sampling order of neighbouring points"

The sample points do not necessarily have integer coordinates; so
$I(\bx_i)$ is computed using bilinear interpolation.

The intensity values thus collected are sorted by increasing intesnity
$I(\bx_{\sigma_1}) < I(\bx_{\sigma_2}) < \dots$ where $\sigma_i$ is a
permuation.

The set of local
samples is then mapped to a discrete symbol by comparing pixel
intensities.

This is obtained as follows.


We sort the neighbours according their intensities and take the indexes
that points from the sorted array into the unsorted one. Then the
permutation index is computed.

@f[
[ I( N_{1}) \; I( N_{2} ) \; I(N_{3}) \; I( N_{4}) ] =
[ 86 \; 217 \; 152 \; 101]\rightarrow ( 1 \; 4 \; 3 \; 2 )\rightarrow 6 
@f]

The permutation index is computed so that it fits the table below.

Array indexes | Permutation index
--------------|-----------------------
1 2 3 4       | 0
1 2 4 3       | 1
1 3 2 4       | 2
1 3 4 2       | 3
1 4 2 3       | 4
1 4 3 2       | 5
2 1 3 4       | 6
2 1 4 3       | 7
...           | ...
4 3 1 2       | 22
4 3 2 1       | 23

Each bin forms a factorial(@numNeighbours) length descriptor.
Each point increments the element of the bin descriptor at position
of its permutation index. How much the element will be incremented is
determined by a weighting function.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-weighting Weighting
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
 
The bigger difference is between intensities of all neighbours of
a particular point, the bigger weight is assigned. (In the following
equation the $Th$ stands for @c weightThreshold.)

@f[ 
w = \sum \limits_{i,j} sgn( | I( N_{i} )- I( N_{j} )| - Th) + 1
@f]

If $w > 0$ the particular element is increased by w, by 1 otherwise.

(parameters to set @c numNeighbours, @c pointToNeighbourRadius
and @c weightThreshold)

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-concatenation Concatenation
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

As the last step, descriptors of all bins are concatenated together.
Therefore, the first group of factorial(@numNeighbours) elements
of the descriptor come from the first bin, the second group of
factorial(@numNeighbours) elements represents the second bin ect.

The final step is normalization of the descriptor $\phi$.

@f[ 
\frac{\phi}{\|\phi\|}\times255
@f]

*/

#include "liop.h"
#include "mathop.h"
#include "imopv.h"
#include <strings.h>

#define NO_VALUE -1
#define THRESHOLD_MULTIPLIER 0.0196
#define MAX_FLOAT_VALUE 1E+37
#define MIN_FLOAT_VALUE 1E-37
#define MAX_UINT8_VALUE 255

static
void get_intensities_of_neighbours(float *intensities,
                                   float * patch,
                                   VlLiopDesc * liop,
                                   float x,
                                   float y);

static
void stable_indexes_qsort(float * array,
                 vl_uindex *indexes,
                 vl_uindex begin,
                 vl_uindex end);

static
void qsort_swap(vl_uindex * array,
                vl_uindex index1,
                vl_uindex index2);

static
vl_int get_permutation_index(vl_uindex * permutation, vl_size size);

static
float interpolate(float * patch,
                  vl_int sideLength,
                  float x,
                  float y);

static
vl_int factorial(vl_int num);

static
void get_coordinates(vl_int a,
                     vl_int pos,
                     float * x,
                     float * y);

static
float get_weight(float * intensities,
                  vl_size size,
                  float threshold);

static
float find_weight_threshold(float * data,
                            vl_size length);

static
vl_int * compute_circle_look_up_table(vl_int radius,
                                      vl_int sideLength);

static
void compute_inner_patch_indexes( VlLiopDesc * self,
                                  vl_int length,
                                  vl_int radius);


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


/** @internal @brief Get weight of a particular group of intensities
 ** @param intensities array of intensities.
 ** @param numIntensities size of the array.
 ** @param threshold threshold.
 ** @return weight */

static float
get_weight (float * intensities, vl_size numIntensities, float threshold){
  float weight = 0;
  float value = 0;
  vl_uindex i,j;
  
  for(i = 0; i < numIntensities; i++){
    for(j = i + 1; j < numIntensities; j++){
      value = vl_abs_f(intensities[i] - intensities[j]) - threshold;
      if(value > 0){
        weight += 2;
      }else if(value == 0){
        weight += 1;
      }
    }
  }
  if(weight == 0){
    return 1;
  }else{
    return weight;
  }
}

/**
 * @brief Find weight threshold according to data range
 * @param data intensity values
 * @param length length of the data array
 * @return threshold
 */
float find_weight_threshold(float * data, vl_size length){
  vl_uindex i;
  float threshold = 0;
  float maxVal = MIN_FLOAT_VALUE;
  
  /* find maximum intensity value in the data*/
  for(i = 0; i < length; i++){
    if(data[i] > maxVal){
      maxVal = data[i];
    }
  }
  
  if(maxVal <= 1){ /* range 0 - 1*/
    threshold = THRESHOLD_MULTIPLIER;
  }else if(maxVal < MAX_UINT8_VALUE){ /* range 0 - 255 */
    threshold = MAX_UINT8_VALUE*THRESHOLD_MULTIPLIER;
  }else{ /* other range */
    threshold = maxVal*THRESHOLD_MULTIPLIER;
  }
  
  return threshold;
}




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

/** @brief Create a new object instance.
 ** @param neighbours number of neighbours.
 ** @param bins number of bins.
 ** @param radius radius between a point and its neighbours.
 ** @param threshold weighting threshold.
 ** @param sideLength patch side length.
 ** @return new liop descriptor settings.
 **/

VlLiopDesc *
vl_liopdesc_new (vl_int neighbours, vl_int bins,
                 float radius, float threshold, vl_size sideLength)
{
  vl_index i, t ;
  
  VlLiopDesc * self = vl_calloc(sizeof(VlLiopDesc), 1);
  self->numNeighbours = neighbours ;
  self->numberOfBins = bins ;
  self->pointToNeighbourRadius = radius ;
  self->patchSideLength = sideLength ;
  self->patchArraySize = sideLength*sideLength;
  self->patchRadius = (sideLength - 1)/2 - radius ;
  self->weightThreshold = threshold ;
  self->liopArraySize = factorial(neighbours) * bins ;

  self->patchSize = 0 ;
  self->patchPixels = vl_malloc(sizeof(vl_uindex)*sideLength*sideLength) ;
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

/** @brief Delete object instance.
 ** @param self object instance. */

void
vl_liopdesc_delete (VlLiopDesc * self)
{
  vl_free (self->patchPixels);
  vl_free (self);
}

/** @brief Get the dimension of a LIOP descriptor.
 ** @return dimension. */

vl_size
vl_liopdesc_get_dimension (VlLiopDesc * self)
{
  return self->liopArraySize ;
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
vl_liopdesc_process (VlLiopDesc * self, float * patch, float *desc)
{
  vl_int i,t ;
  vl_int offset,numPermutations ;
  vl_int spatialBinArea, spatialBinEnd, spatialBinIndex ;
  
  memset(desc, 0, self->liopArraySize) ;
  
  if(self->weightThreshold == NO_VALUE){
    self->weightThreshold = find_weight_threshold(patch, self->patchArraySize);
  }
    
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
   * Process pixels in order of increasing intenisity, dividing them into
   * spatial bins on the fly.
   */
  
  numPermutations = factorial(self->numNeighbours) ;
  spatialBinArea = self->patchSize / self->numberOfBins ;
  spatialBinEnd = spatialBinArea ;
  spatialBinIndex = 0 ;
  offset = 0 ;

  for (i = 0 ; i < (signed)self->patchSize ; ++i) {
    vl_index permIndex ;
    float weight ;
    double *sx, *sy ;
    
    /* advance to the next spatial bin if needed */
    if (i >= (signed)spatialBinEnd && spatialBinIndex < (signed)self->numberOfBins - 1) {
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
      int ix = vl_floor_d(x) ;
      int iy = vl_floor_d(y) ;
      
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
    
    /* compute weight according to difference in intensity values */
    weight = get_weight(self->neighIntensities, self->numNeighbours, self->weightThreshold);

    /* accumulate to the descriptor */
    desc[permIndex + offset] += weight;
  }
  
  /* normalization */
  {
    float norm = 0;
    for(i = 0; i < (signed)self->liopArraySize; i++) {
      norm += desc[i]*desc[i];
    }
    norm = VL_MAX(sqrt(norm), 1e-12) ;
    for(i = 0; i < (signed)self->liopArraySize; i++){
      desc[i] = (float)((desc[i]/norm) * 255) ;
    }
  }
}

/* ---------------------------------------------------------------- */
/*                                                  Other functions */
/* ---------------------------------------------------------------- */

/** @internal @brief Compute permutation index.
 ** @param permutation array containing all values from 0 to (size - 1).
 ** @param size size of the permutation array.
 ** @return permutation index.
 **
 ** Compute a unique index of a particular permutation without
 ** comparing arrays. Let's consider permutation array containing
 ** numbers from 1 to 4.  The lowest index @b 0 has permutation
 ** <code>[1 2 3 4]</code>, @b 1 <code>[1 2 4 3]</code>, @b 2 <code>[1
 ** 3 2 4]</code>, @b 3 <code>[1 3 4 2]</code> ,... and the highest
 ** <B> (size - 1) </B> <code>[4 3 2 1]</code>.
 */

vl_int get_permutation_index(vl_uindex *permutation, vl_size size){
    vl_int * controlArray = vl_malloc(sizeof(vl_int)*size);
    vl_int permutationIndex = 0;

    vl_int i, j, f, value;
    for(i = 0; i < (signed)size; i++){
        controlArray[i] = i + 1;
    }

    f = factorial(size - 1);

    value = permutation[0];
    controlArray[value] = NO_VALUE;
    permutationIndex = value*f;

    for(i = 1; i < (signed)(size - 1); i++){
        value = permutation[i];
        controlArray[value] = NO_VALUE;

        for(j = (value - 1); j >= 0; j--){
            if(controlArray[j] == NO_VALUE){
                value--;
            }
        }
        f = f/(size - i);
        permutationIndex += value*f;
    }

    vl_free(controlArray);

    return permutationIndex;
}








