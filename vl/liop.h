/** @file liop.h
 ** @brief Local Intensity Order Pattern (LIOP) descriptor (@ref liop)
 ** @author Hana Sarbortova
 ** @author Andrea Vedaldi
 ** @see @ref liop
 **/

/*
 Copyright (C) 2013 Hana Sarbortova and Andrea Vedaldi.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#ifndef VL_LIOP_H
#define VL_LIOP_H

#include "generic.h"

/** @brief LIOP descriptor extractor object */
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
  vl_size liopArraySize; /**< LIOP descriptor size. */

  float * intensities ;
  vl_uindex * permutation ;
} VlLiopDesc ;

/** @name Construct and destroy
 *  @{
 */
VL_EXPORT
VlLiopDesc * vl_liopdesc_new (vl_int neighbours,
                              vl_int bins,
                              float radius,
                              float threshold,
                              vl_size patchSideLength) ;

VL_EXPORT
void vl_liopdesc_delete (VlLiopDesc * self) ;
/** @} */


/**  @name Get data and parameters
 **  @{ */
VL_EXPORT
vl_size vl_liopdesc_get_dimension (VlLiopDesc * self) ;
/** @} */

/**  @name Compute LIOP descriptor
 **  @{ */
VL_EXPORT
void vl_liopdesc_process (VlLiopDesc * liop, float * patch, float * desc);
/** @} */

/* VL_LIOP_H */
#endif
