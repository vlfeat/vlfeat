/** @file liop.h
 ** @brief Local Intensity Order Pattern descriptor (@ref liop)
 ** @author Hana Sarbortova
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#ifndef VL_LIOP_H
#define VL_LIOP_H

#include "generic.h"
#include "mathop.h"
#include "imopv.h"

#define NO_VALUE -1
#define THRESHOLD_MULTIPLIER 0.0196
#define MAX_FLOAT_VALUE 1E+37
#define MIN_FLOAT_VALUE 1E-37
#define MAX_UINT8_VALUE 255

/** @brief LIOP bin
 ** A bin of a LIOP descriptor
 **/
typedef struct _VlLiopBin
{
  vl_size binSize ; /**< Number of points beloging to the bin. */
  vl_uindex * binIndexes ; /**< Patch indexes of points belonging to the bin. */
  vl_size binLiopSize ; /**< Size of the bin LIOP descriptor. */
  float * binLiop ; /**< Bin LIOP descriptor. */
  vl_uint numOfUsedPoints ; /**< Number of points used for computation of bin LIOP. */
} VlLiopBin ;

/** @brief LIOP Descriptor settings
 *
 *  LIOP descriptor settings necessary for computing of discriptor from ma patch
 */
typedef struct _VlLiopDesc
{
    vl_int numberOfNeighbours; /**< Number of neighbours. */
    vl_int numberOfBins; /**< Number of bins. */
    float pointToNeighbourRadius; /**< Point to neighbour radius (distance). */
    vl_size patchSideLength;/**< Length of a patch side. */
    vl_size patchArraySize; /**< Length of the whole path array. */
    vl_int patchRadius; /**< Radius of circle inside of the patch (defining are of points to be used for computation). */
    float weightThreshold; /**< Weight threshold. */
    vl_uindex * innerPatchIndexes; /**< Indexes of patch array that are used for descriptor computation. */
    vl_size innerPatchIndexesSize; /**< Number of innerPatchIndexes. */
    vl_size liopArraySize; /**< Liop descriptor size. */
    VlLiopBin ** liopBins;/**< Array of liop bins. */
} VlLiopDesc;

/** @name Construct and destroy
 *  @{
 */
VL_EXPORT
VlLiopDesc * vl_liopdesc_new (vl_int neighbours,
                              vl_int bins,
                              float radius,
                              float threshold,
                              vl_size patchSideLength
                              );

VL_EXPORT
VlLiopBin * vl_liopbin_new(vl_uindex * binIndexes,
                           vl_size binSize,
                           float * binLiop,
                           vl_size binLiopSize
                           );

VL_EXPORT
void vl_liopdesc_delete (VlLiopDesc * self);

VL_EXPORT
void vl_liopbin_delete(VlLiopBin * bin);

/**
 * @}
 *
 *  @name Compute LIOP descriptor
 *  @{
 */

VL_EXPORT
void compute_liop_descriptor(VlLiopDesc * liop,
                             float * patch,
                             float * desc);

VL_EXPORT
void compute_bin_liop_descriptor(VlLiopDesc * liop,
                                 VlLiopBin * bin,
                                 float * patch);

VL_EXPORT
void get_intensities_of_neighbours(float *intensities,
                                   float * patch,
                                   VlLiopDesc * liop,
                                   float x,
                                   float y);

/**
 * @}
 *
 * @name Sorting
 * @{
 *
 **/

VL_EXPORT
void stable_indexes_qsort(float * array,
                 vl_uindex *indexes,
                 vl_uindex begin,
                 vl_uindex end);

VL_EXPORT
void qsort_swap(vl_uindex * array,
                   vl_uindex index1,
                   vl_uindex index2);

/**
 * @}
 *
 * @name Other functions
 * @{
 */

VL_EXPORT
vl_int get_permutation_index(vl_uindex * permutation, vl_size size);

VL_EXPORT
float interpolate(float * patch,
                  vl_int sideLength,
                  float x,
                  float y);

VL_EXPORT
vl_int factorial(int num);

VL_EXPORT
vl_int factorial(vl_int num);

VL_EXPORT
void get_coordinates(vl_int a,
                     vl_int pos,
                     float * x,
                     float * y);

VL_EXPORT
vl_uindex get_vector_position(vl_int a,
                              float x,
                              float y);

VL_EXPORT
void rotate_vector(float alpha,
                   float * x,
                   float * y);

VL_EXPORT
float get_weight(float * intensities,
                  vl_size size,
                  float threshold);

VL_EXPORT
float find_weight_threshold(float * data,
                            vl_size length);

/**
 * @}
 *
 * @name Reshape patch to a circle
 * @{
 *
 */

VL_EXPORT
vl_int * compute_circle_look_up_table(vl_int radius,
                                      vl_int sideLength);

VL_EXPORT
void compute_inner_patch_indexes( VlLiopDesc * self,
                                  vl_int length,
                                  vl_int radius);
/**
 * @}
 */


#endif
