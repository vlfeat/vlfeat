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
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref liop.h implements Local Intensity Order Pattern descriptor (LIOP)
as proposed in @cite{wang11local}

Please see @subpage liop-fundamentals for a technical description of
LIOP descriptor.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-starting Getting started
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The Following code demonstrates how tow to use @ref liop.h in the C
programming language in order to compute liop descriptor from a patch.

@code
#include <vl/liop.h>

// Create a new object instance (these numbers coresponds to parameter
// values proposed by authors of the paper, except for 41)
VlLiopDesc * liop = vl_liopdesc_new(4,6,6,5,41);

// allocate the descriptor array
float * desc = vl_malloc(sizeof(float)*liop->liopArraySize);

// compute descriptor from a patch (in this case, the patch must be
// an array of length 41*41=1681)
compute_liop_descriptor(liop, patch, desc);

// delete object instance
vl_liopdesc_delete(liop);
@endcode

Not all pixels from the patch will contribute to the descriptor as
sample points. The sample points belong to the area of the inner circle,
as all neighbours of sample points has to be computed from the patch.
Please see the image below for better understanding.

@image html liop-patch-layout.png "LIOP patch layout"

**/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page liop-fundamentals LIOP fundamentals
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Local Invariant Order Pattern (LIOP) descriptor, as proposed in
@cite{wang11local}, uses both local and overall intensity information
of the local patch for the descriptor computation. It is invariant to
monotonic intensity changes and image rotation and is robust to many
other geometric and photometric transformations.

The descriptor is computed from a normalized local patch. An image part that
belongs to a feature frame which can be generally of shape of an oriented
ellise is transformed to a circle.

In this implementation, we are computing the LIOP descriptor from a patch
which is represented by a square of an odd side length. Only points
belonging to the area of a circle of a radius which is always smaller than
half of the patch side will be used as sample points.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section liop-computation Descriptor computation
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The descriptor computation has 2 steps based on intensity order.
First, every sample point is assigned to a particular bin according
to overall intensity. Second, intensities of neighbours of sample points
contribute to actuall values of the computed descriptor.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-bins Region division
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

All sample points are sorted according to their intensity in a nondescending
order. The points are than equally quantized into @c numberOfBins ordinal bins.

As a result, each bin contains the same number of points except for
the last one which is between 0 to @c numberOfBins - 1 points larger. The first
bin contains points with the lowest intensity values, similarly the
last bin has the points with the highest intensity values.

(parameter to set @c numberOfBins)

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection liop-computation-points Sample point contribution
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Each point is represented by its neighbours. Specifically,
@c numberOfNeighbours points are sampled from a circle of radius
@c pointToNeighbourRadius and centre in the particular point.
The neighbouring points are sampled in the same manner as shown on the
image below ($C$ is the patch center). Sampling order is always anticlokwise.

@image html liop-neighbours-sampling.png "Sampling order of neighbouring points"

Intensity values of these neighbouring points are computed by linear
interpolation.

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

/* ---------------------------------------------------------------- */
/*                                            Construct and destroy */
/* ---------------------------------------------------------------- */

/**
 * @brief Create a new object instance
 * @param neighbours number of neighbours
 * @param bins number of bins
 * @param radius radius between a point and it's neighbours
 * @param threshold weighting threshold
 * @param sideLength patch side length
 * @return new liop descriptor settings
 */
VlLiopDesc * vl_liopdesc_new(vl_int neighbours, vl_int bins, float radius, float threshold, vl_size sideLength){

    VlLiopDesc * self = vl_malloc(sizeof(VlLiopDesc));
    self->numberOfNeighbours = neighbours;
    self->numberOfBins = bins;
    self->pointToNeighbourRadius = radius;
    self->patchSideLength = sideLength;
    self->patchArraySize = sideLength*sideLength;
    self->patchRadius = (sideLength - 1)/2 - radius;
    self->weightThreshold = threshold;
    self->liopArraySize = factorial(neighbours)*bins;
    self->liopBins = vl_malloc(sizeof(VlLiopBin)*bins);
    compute_inner_patch_indexes(self, sideLength, self->patchRadius);
    return self;
}

/**
 * @brief Create a new object instance
 * @param binIndexes patch indexes of points belonging to the bin
 * @param binSize number of points belonging to the bin
 * @param binLiop bin liop descriptor
 * @param binLiopSize size of the bin liop descriptor
 * @return
 */
VlLiopBin * vl_liopbin_new(vl_uindex * binIndexes, vl_size binSize, float * binLiop, vl_size binLiopSize){
    VlLiopBin * self = vl_malloc(sizeof(VlLiopBin));
    self->binIndexes = binIndexes;
    self->binSize = binSize;
    self->binLiopSize = binLiopSize;
    self->binLiop = binLiop;
    self->numOfUsedPoints = 0;
    return self;
}

/**
 * @brief Delete object instance
 * @param self object instance
 */
void vl_liopdesc_delete (VlLiopDesc * self){
    vl_int i;

    vl_free(self->innerPatchIndexes);

    for(i = 0; i < self->numberOfBins; i++){
        vl_liopbin_delete(self->liopBins[i]);
    }
    vl_free(self->liopBins);
    vl_free(self);
}

/**
 * @brief Delete object instance
 * @param bin object instance
 */
void vl_liopbin_delete(VlLiopBin * bin){
    vl_free(bin->binIndexes);
    vl_free(bin->binLiop);
    vl_free(bin);
}

/* ---------------------------------------------------------------- */
/*                                          Compute LIOP descriptor */
/* ---------------------------------------------------------------- */

/**
 * @brief Compute liop descriptor for a patch
 * @param self object instance
 * @param patch patch to process
 * @param desc descriptor to be computed
 */
void compute_liop_descriptor(VlLiopDesc * self, float * patch, float *desc){
    vl_int i,descShift,binShift,binLiopSize;
    vl_size residue, binSize;
    float norm;

    if(self->weightThreshold == NO_VALUE){
        self->weightThreshold = find_weight_threshold(patch, self->patchArraySize);
    }

    /* set all values of descriptor to zero */
    for(i = 0; i < (signed)self->liopArraySize; i++){
        desc[i] = 0;
    }

    /* sort indexes */
    stable_indexes_qsort(patch, self->innerPatchIndexes, 0, self->innerPatchIndexesSize - 1);

    /* create bins */
    descShift = 0;
    binShift = 0;
    binLiopSize = factorial(self->numberOfNeighbours);
    residue = self->innerPatchIndexesSize%self->numberOfBins;
    binSize = (self->innerPatchIndexesSize - residue) / self->numberOfBins;

    for(i = 0; i < (self->numberOfBins - 1); i++){

        self->liopBins[i] = vl_liopbin_new(self->innerPatchIndexes + binShift, binSize,
                                            desc + descShift, binLiopSize);
         binShift += binSize;
         descShift += binLiopSize;
    }
    /* the last bin may have more points - number of points is not generally devidable by number of bins*/
    self->liopBins[self->numberOfBins - 1] = vl_liopbin_new(self->innerPatchIndexes + binShift, binSize + residue,
                                        desc + descShift, binLiopSize);

    /* compute descriptor for each bin */
    for(i = 0; i < (self->numberOfBins); i++){
        compute_bin_liop_descriptor(self ,self->liopBins[i], patch);
    }

    /* normalization */
    norm = 0;

    for(i = 0; i < (signed)self->liopArraySize; i++){
       norm += desc[i]*desc[i];
    }

    norm = sqrt(norm);

    for(i = 0; i < (signed)self->liopArraySize; i++){
        desc[i] = (float)vl_ceil_f((desc[i]/norm)*255);
    }
}

/**
 * @brief Compute one bin of LIOP descriptor
 * @param self object instance
 * @param bin object instance
 * @param patch patch to process
 */
void compute_bin_liop_descriptor(VlLiopDesc *self, VlLiopBin * bin, float * patch){
    vl_uindex i,j;
    float x,y;
    vl_int permIndex;
    float weight = 0;

    /* array for intensity values of neighbours */
    float * intensities = vl_malloc(sizeof(float) * self->numberOfNeighbours);

    /* array with indexes to intensities array - used for getting index of a particular permutation */
    vl_uindex * permutation = vl_malloc(sizeof(vl_uindex) * self->numberOfNeighbours);

    /* compute liop for each point of the bin*/
    for(i = 0; i < bin->binSize; i++){
        get_coordinates(self->patchSideLength, bin->binIndexes[i], &x, &y);

        /* process only points which have all neighbours inside of the patch circle */
        if(x != 0 || y != 0){

            get_intensities_of_neighbours(intensities,patch,self,x,y);

            /* reset indexes of permutation array to increasing sequence 1,2,...,N */
            for(j = 0; j < (unsigned)self->numberOfNeighbours; j++){
                permutation[j] = j;
            }

            /* sort permutation indexes according to intensities array*/
            stable_indexes_qsort(intensities, permutation, 0, self->numberOfNeighbours - 1);

            /* get permutation index */
            permIndex = get_permutation_index(permutation, self->numberOfNeighbours);

            /* compute weight according to difference in intensity values */
            weight = get_weight(intensities, self->numberOfNeighbours, self->weightThreshold);
            //mexPrintf("weight %f\n",weight);
            /* increase value at the position of permutation index*/
            bin->binLiop[permIndex] += weight;

            /* keep the number of points which were used in each bin (debug information)*/
            bin->numOfUsedPoints++;
        }
    }
    vl_free(intensities);
    vl_free(permutation);
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

/**
 * @brief Compute permutation index
 * @param permutation array containing all values from 0 to (size - 1)
 * @param size size of the permutation array
 * @return permutation index
 *
 * Compute a unique index of a particular permutation without comparing
 * arrays. Let's consider permutation array containing numbers from 1 to 4.
 * The lowest index @b 0 has permutation <code>[1 2 3 4]</code>,
 * @b 1 <code>[1 2 4 3]</code>, @b 2 <code>[1 3 2 4]</code>, @b 3 <code>[1 3 4 2]</code>
 * ,... and the highest <B> (size - 1) </B> <code>[4 3 2 1]</code>
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
 * @brief Compute factorial of a number
 * @param num number
 * @return factorial of the number
 */
vl_int factorial(vl_int num){
    vl_int result = 1;
    while(num > 1){
        result = num*result;
        num--;
    }
    return result;
}

/**
 * @brief Get coordinates (the patch center has coordinates [0,0])
 * @param sideLength patch side length
 * @param pos position in the patch vector
 * @param x x coordinate
 * @param y y coordinate
 */
void get_coordinates(vl_int sideLength, vl_int pos, float * x, float * y){
    vl_int x1, y1, shift;
    x1 = pos % sideLength;
    y1 = (pos - x1)/sideLength;
    shift = (sideLength - 1)/2;
    (*x) = x1 - shift;
    (*y) = y1 - shift;
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

/* ---------------------------------------------------------------- */
/*                                        Reshape patch to a circle */
/* ---------------------------------------------------------------- */

/**
 * @brief Compute circle look up table
 * @param radius circle radius
 * @param sideLength patch side length
 * @return array of length <code>sideLength</code> with number of pixels
 *         that are outside of the circle but still in the patch.
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
void compute_inner_patch_indexes(VlLiopDesc * self ,vl_int length, vl_int radius){

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







