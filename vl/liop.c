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
@subsection liop-computation-regions Region division
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
@subsection liop-computation-pooling Sample point contribution
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

After regions are computed, an operator is applied to each pixel $\bx$
in the patch in order to characterize its local appearance. This
operator start by considering the intensities $I(\bx_1), I(\bx_2),
\dots$ of @c numberOfNeighbours image pixels in a small circle of
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

Each bin forms a factorial(@numberOfNeighbours) length descriptor.
Each point increments the element of the bin descriptor at position
of its permutation index. How much the element will be incremented is
determined by a weighting function.

@par Weighting function

The bigger difference is between intensities of all neighbours of
a particular point, the bigger weight is assigned. (In the following
equation the $Th$ stands for @c weightThreshold.)

@f[ 
w = \sum \limits_{i,j} sgn( | I( N_{i} )- I( N_{j} )| - Th) + 1
@f]

If $w > 0$ the particular element is increased by w, by 1 otherwise.

(parameters to set @c numberOfNeighbours, @c pointToNeighbourRadius
and @c weightThreshold)

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-concatenation Concatenation
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

As the last step, descriptors of all bins are concatenated together.
Therefore, the first group of factorial(@numberOfNeighbours) elements
of the descriptor come from the first bin, the second group of
factorial(@numberOfNeighbours) elements represents the second bin ect.

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
vl_uindex get_vector_position(vl_int a,
                              float x,
                              float y);

static
void rotate_vector(float alpha,
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

/* ---------------------------------------------------------------- */
/*                                        Reshape patch to a circle */
/* ---------------------------------------------------------------- */

/**
 * @brief Compute circle look up table
 * @param radius circle radius
 * @param sideLength patch side length
 * @return array of length <code>sideLength</code> with number of pixels
 *         that are outside of the circle bu`t still in the patch.
 *         Each alement is for one column of a half of the patch.
 */
vl_int * compute_circle_look_up_table(vl_int radius, vl_int sideLength){
    vl_int * table = vl_malloc(sideLength*sizeof(vl_int));
    float y = (float)radius - 0.4;
    vl_index i;
    vl_int value;
    for(i = 0; i < radius; i++){
        value = radius - (vl_int)vl_round_f(vl_fast_sqrt_f((float)(radius*radius) - y*y));
        table[i] = value;
        table[sideLength - 1 - i] = value;
        y = y - 1;
        }
    table[radius] = 0;

    return table;
}

/**
 * @brief Get indexes that will be used for descriptor computation (inner circle of the patch)
 * @param length patch side length
 * @param radius radius of the inner circle
 */
void compute_inner_patch_indexes(VlLiopDesc * self ,vl_int length, vl_int radius)
{
  vl_int * circleLookUpTable = compute_circle_look_up_table(radius,radius*2 + 1);
  vl_int i,j;
  vl_int outerArea = 0;
  vl_uindex * indexes;
  vl_uindex innerIndex, pos;
  //sum area out of the circle
  for(i = 0; i < radius; i++){
    outerArea += circleLookUpTable[i];
  }
  outerArea = outerArea*4 + length*length - pow((2*radius + 1),2);
  self->innerPatchIndexesSize = length*length - outerArea;

  indexes = vl_malloc(sizeof(vl_uindex)*self->innerPatchIndexesSize);

  innerIndex = ((length - 1)/2 - radius)*(length + 1);
  pos = 0;
  for(i = 0; i < (2*radius + 1); i++){
    for(j = circleLookUpTable[i]; j < (2*radius - circleLookUpTable[i] + 1); j++){
      indexes[pos++] = innerIndex + j;
    }
    innerIndex += length;
  }
  self->innerPatchIndexes = indexes;
  vl_free(circleLookUpTable);
}

/* ---------------------------------------------------------------- */
/*                                            Construct and destroy */
/* ---------------------------------------------------------------- */

/**
 ** @brief Create a new object instance.
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
  VlLiopDesc * self = vl_malloc(sizeof(VlLiopDesc));
  self->numberOfNeighbours = neighbours ;
  self->numberOfBins = bins ;
  self->pointToNeighbourRadius = radius ;
  self->patchSideLength = sideLength ;
  self->patchArraySize = sideLength*sideLength;
  self->patchRadius = (sideLength - 1)/2 - radius ;
  self->weightThreshold = threshold ;
  self->liopArraySize = factorial(neighbours) * bins ;

  self->innerPatchIndexesSize = 0 ;
  self->innerPatchIndexes = vl_malloc(sizeof(vl_uindex)*sideLength*sideLength) ;
  {
    vl_index x, y ;
    vl_index center = (sideLength - 1) / 2 ;
    double t = center - radius + 0.6 ;
    vl_index t2 = (vl_index) (t * t) ;
    for (y = 0 ; y < (signed)sideLength ; ++y) {
      for (x = 0 ; x < (signed)sideLength ; ++x) {
        vl_index dx = x - center ;
        vl_index dy = y - center ;
        if (dx*dx + dy*dy <= t2) {
          self->innerPatchIndexes[self->innerPatchIndexesSize++] = x + y * sideLength ;
        }
      }
    }    
  }

  VL_PRINTF("bla %d\n", self->innerPatchIndexesSize) ;

  {
    int i ;
    for(i= 0 ; i < self->innerPatchIndexesSize ; i++) {VL_PRINTF("%d ", self->innerPatchIndexes[i]) ; if (i%30==0) VL_PRINTF("\n") ;}
    VL_PRINTF("\n\n") ;
  }

  
  compute_inner_patch_indexes(self, sideLength, self->patchRadius);
  
  VL_PRINTF("bla %d\n", self->innerPatchIndexesSize) ;
  
  {
    int i ;
    for(i= 0 ; i < self->innerPatchIndexesSize ; i++) {VL_PRINTF("%d ", self->innerPatchIndexes[i]) ; if (i%30==0) VL_PRINTF("\n") ;}
    VL_PRINTF("\n\n") ;
  }
  
  self->permutation = vl_malloc(sizeof(vl_uindex) * self->numberOfNeighbours);
  self->intensities = vl_malloc(sizeof(float) * self->numberOfNeighbours);

  return self ;
}

/** @brief Delete object instance.
 ** @param self object instance.
 **/

void
vl_liopdesc_delete (VlLiopDesc * self){
  vl_free(self->innerPatchIndexes);
  vl_free(self);
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

/**
 ** @brief Compute liop descriptor for a patch
 ** @param self object instance
 ** @param patch patch to process
 ** @param desc descriptor to be computed
 */

void vl_liopdesc_process (VlLiopDesc * self, float * patch, float *desc)
{
  vl_int i,j,offset,numPermutations ;
  vl_int spatialBinArea, spatialBinEnd, spatialBinIndex ;
  vl_int center = (self->patchSideLength - 1) / 2 ;
    
  if(self->weightThreshold == NO_VALUE){
    self->weightThreshold = find_weight_threshold(patch, self->patchArraySize);
  }
  
  
  /* sort patc pixels by increasing intensity */
  stable_indexes_qsort(patch, self->innerPatchIndexes, 0, self->innerPatchIndexesSize - 1) ;

  /* process pixels in order of increasing intenisity, dividing them into
   spatial bins on the fly */
  numPermutations = factorial(self->numberOfNeighbours) ;
  spatialBinArea = self->innerPatchIndexesSize / self->numberOfBins ;
  spatialBinEnd = spatialBinArea ;
  spatialBinIndex = 0 ;
  offset = 0 ;
  
  memset(desc, 0, self->liopArraySize) ;

  for (i = 0 ; i < (signed)self->innerPatchIndexesSize ; ++i) {
    vl_index x, y ;
    vl_index pixel ;
    vl_index permIndex ;
    float weight ;
    
    if (i >= (signed)spatialBinEnd && spatialBinIndex < (signed)self->numberOfBins - 1) {
      spatialBinEnd += spatialBinArea ;
      spatialBinIndex ++ ;
      offset += numPermutations ;
    }
    
    /* get pixel coordinates */
    pixel = self->innerPatchIndexes[i] ;
    x = (pixel % self->patchSideLength) - center ;
    y = (pixel / self->patchSideLength) - center ;
    
    /* 
     the pixel in the center cannot be used as we need a radius
     from the patch center to the pixel in order to fix the rotation
     */
    if (x == 0 && y == 0) continue ;

    /* extract intensities of neighbours and get their permutation */
    get_intensities_of_neighbours(self->intensities,patch,self,x,y);
    for(j = 0; j < (signed)self->numberOfNeighbours; j++) {
      self->permutation[j] = j;
    }
    stable_indexes_qsort(self->intensities,
                         self->permutation,
                         0, self->numberOfNeighbours - 1);
    
    /* get permutation inde */
    permIndex = get_permutation_index(self->permutation, self->numberOfNeighbours);
    
    /* compute weight according to difference in intensity values */
    weight = get_weight(self->intensities, self->numberOfNeighbours, self->weightThreshold);

    /* accumulate to the descriptor */
    desc[permIndex + offset] += weight;
  }
  
  /* normalization */
  {
    float norm = 0;
    for(i = 0; i < (signed)self->liopArraySize; i++) {
      norm += desc[i]*desc[i];
    }
    norm = sqrt(norm) ;
    for(i = 0; i < (signed)self->liopArraySize; i++){
      desc[i] = (float)((desc[i]/norm) * 255) ;
    }
  }
}

/**
 * @brief Get intensities of neighbours
 * @param intensities array of neighbours' intensities
 * @param patch patch to process
 * @param self object instance
 * @param x x coordinate of the point X to be processed
 * @param y y coordinate of the point X to be processed
 */
void get_intensities_of_neighbours(float * intensities, float * patch, VlLiopDesc * self, float x, float y){

    float x1, y1, alpha,dist;
    vl_int i;

    /* distance between centre of coordinates and the point X*/
    dist = vl_fast_sqrt_f(x*x + y*y);

    /* create unit vector (x1,y1) */
    x1 = x/dist;
    y1 = y/dist;

    /*
     first neighbouring point belongs to the radial direction (from the center to the point X) and is distant
     pointToNeighbourRadius from the point X. As this condition is satisfied by two points, the farther
     point form the center is selected.
    */
    intensities[0] = interpolate(patch,self->patchSideLength, x1*self->pointToNeighbourRadius + x,
                                 y1*self->pointToNeighbourRadius + y);

    alpha = 2*VL_PI/(float)self->numberOfNeighbours;

    /*
     other neighbouring points are sampled on the circle (center in X, radius pointToNeighbourRadius)
     in the anticlokwise direction
    */
    for(i = 1; i < self->numberOfNeighbours; i++){
        rotate_vector(alpha,&x1,&y1);
        intensities[i] = interpolate(patch,self->patchSideLength,x1*self->pointToNeighbourRadius + x,
                                     y1*self->pointToNeighbourRadius + y);

    }

}

/* ---------------------------------------------------------------- */
/*                                                          Sorting */
/* ---------------------------------------------------------------- */

/**
 * @brief Stable quick sort (sort only indexes, not the array of values)
 * @param array array of values
 * @param indexes indexes to values of array that should be used for sorting
 * @param begin first index of the array of indexes
 * @param end last index of the array of indexes
 */
void stable_indexes_qsort(float * array, vl_uindex * indexes, vl_uindex begin, vl_uindex end){


    vl_uindex pivot, lowPart, i;

    pivot = (end + begin) / 2 ;

    /* swap pivot with last */
    qsort_swap(indexes, pivot, end) ;
    pivot = end ;

    /*
     Now scan from left to right, moving all element smaller
     or equal than the pivot to the low part
     array[0], array[1], ..., array[lowPart - 1].
    */
    lowPart = begin ;
    for (i = begin; i < end ; ++i) { //one less
        float diff = array[indexes[i]] - array[indexes[pivot]];
        if ((diff < 0) || ((diff == 0) && (indexes[pivot] > indexes[i])) ) {
            /* array[i] must be moved into the low part */
            qsort_swap (indexes, lowPart, i) ;
            lowPart ++ ;
       }
    }

    /* the pivot should also go into the low part */
    qsort_swap (indexes, lowPart, pivot) ;
    pivot = lowPart ;

    /* do recursion */
    if (pivot > begin) {
      /* note that pivot-1 stays non-negative */
      stable_indexes_qsort(array,indexes, begin, pivot - 1) ;
    }
    if (pivot < end) {
      stable_indexes_qsort(array,indexes, pivot + 1, end) ;
    }
}

/**
 * @brief Quick sort swap (swap only indexes, not array values)
 * @param array array of values
 * @param index1 first index to be swaped
 * @param index2 second index to be swaped
 */
void qsort_swap(vl_uindex * array,
                   vl_uindex index1,
                   vl_uindex index2){
    vl_uindex t = array[index1];
    array[index1] = array[index2];
    array[index2] = t;
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

/**
 * @brief Compute 4 point linear interpolation
 * @param patch patch to process
 * @param sideLength patch side length
 * @param x x coordinate
 * @param y y coordinate
 * @return interpolated intensity value
 */
float interpolate(float * patch, vl_int sideLength, float x, float y){
    float value, a, b, xM, yM;
    vl_uindex pos, pos1, pos2, pos3, pos4;
    float limit = (sideLength - 1)/2;
    if((x <= -limit) || (y <=- limit) || (x >= limit) || (y >= limit)){
        pos = get_vector_position(sideLength, (float)vl_round_f(x), (float)vl_round_f(y));
        value = patch[pos];
    }else{
        xM = (float)vl_ceil_f(x);
        yM = (float)vl_ceil_f(y);
        pos1 = get_vector_position(sideLength,xM,yM);
        pos2 = get_vector_position(sideLength,xM,(yM - 1));
        pos3 = get_vector_position(sideLength,(xM - 1),yM);
        pos4 = get_vector_position(sideLength,(xM - 1),(yM - 1));
        a = yM - y;
        b = xM - x;

        value = a*b*patch[pos4] + (1 - a)*b*patch[pos3] + (1 - a)*(1 - b)*patch[pos1] + a*(1 - b)*patch[pos2];

    }
    return value;
}



/**
 * @brief Get position of a point in the patch vector
 * @param sideLength patch side length
 * @param x x coordinate
 * @param y y coordinate
 * @return position in the patch vector
 */
vl_uindex get_vector_position(vl_int sideLength, float x, float y){
    float shift;
    vl_uindex pos;
    shift = (sideLength - 1)/2;
    x = x + shift;
    y = y + shift;
    pos = (vl_uindex)(y*sideLength + x);
    return pos;
}

/**
 * @brief Rotate vector
 * @param alpha angle of rotation
 * @param x first element of the vector
 * @param y second element of the vector
 */
void rotate_vector(float alpha, float * x, float * y){
    float x1, y1;
    x1 = *x;
    y1 = *y;
    (*x) = x1*cosf(alpha) - y1*sinf(alpha);
    (*y) = x1*sinf(alpha) + y1*cosf(alpha);

}

/**
 * @brief Get weight of a particular group of intensities
 * @param intensities array of intensities
 * @param size size of the intensities array
 * @param threshold threshold (minimum difference between intensities that should be considered
 *                             as large enough for increasing weight)
 * @return weight
 */
float get_weight(float * intensities, vl_size size, float threshold){
    float weight = 0;
    float value = 0;
    vl_uindex i,j;

    for(i = 0; i < size; i++){
        for(j = i + 1; j < size; j++){
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








