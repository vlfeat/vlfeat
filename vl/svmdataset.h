/** @file svmdataset.h
 ** @brief SVM Dataset
 ** @author Daniele Perrone
 **/

/*
Copyright (C) 2012 Daniele Perrone.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_SVMDATASET_H
#define VL_SVMDATASET_H

#include "generic.h"

/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @typedef VlSvmDatasetFeatureMap
 ** @brief Pointer to a function that maps the value in @x to
 ** @destination using the feature map @map
 **/
typedef void (*VlSvmDatasetFeatureMap)(const void* map, double * destination,
                                       const vl_size stride, const double x) ;

/** @typedef VlSvmDatasetInnerProduct
 ** @brief Pointer to a function that defines the inner product
 ** between the data point at position @element and the SVM model
 **/
typedef double (*VlSvmDatasetInnerProduct)(const void* data,
                                           const vl_uindex element,
                                           const double* model) ;

/** @typedef VlSvmDatasetAccumulator
 ** @brief Pointer to a function that adds to @model the data point at
 ** position @element multiplied by the constant @multiplier
 **/
typedef void (*VlSvmDatasetAccumulator)(const void* data,
                                        const vl_uindex element,
                                        double * model,
                                        const double multiplier) ;


/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @brief Svm Dataset
 **
 ** This structure is a wrapper for a generic SVM dataset. It should
 ** be provided with a set of functions of type
 ** @ref VlSvmDatasetInnerProduct and @ref VlSvmDatasetAccumulator
 **/
typedef struct _VlSvmDataset {
  void * data ;                 /**< pointer to data */
  vl_size dimension ;           /**< data point dimension */
  void* map ;                   /**< feature Map */
  VlSvmDatasetFeatureMap mapFunc ; /**< function that defines the feature map*/
  vl_size order ;                  /**< order of expansion of the feture map */
} VlSvmDataset ;

/** @name Create and destroy
 ** @{
 **/
VL_EXPORT
VlSvmDataset* vl_svmdataset_new (void * data, vl_size dimension) ;

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset) ;
/** @} */

/** @name Set map
 ** @{
 **/
VL_EXPORT
void vl_svmdataset_set_map (VlSvmDataset * data, void * map,
                            VlSvmDatasetFeatureMap mapFunc, vl_size order) ;
/** @} */

/* ---------------------------------------------------------------- */
/*    Standard Atomic Functions                                     */
/* ---------------------------------------------------------------- */

/** @name Standard dataset functions
 ** @{
 **/
VL_EXPORT
double vl_svmdataset_innerproduct_d(const void* data,
                                    const vl_uindex element,
                                    const double* model) ;

VL_EXPORT
void vl_svmdataset_accumulator_d(const void* data,
                                 const vl_uindex element,
                                 double * model,
                                 const double multiplier) ;

VL_EXPORT
double vl_svmdataset_innerproduct_f(const void* data,
                                    const vl_uindex element,
                                    const double* model) ;

VL_EXPORT
void vl_svmdataset_accumulator_f(const void* data,
                                 const vl_uindex element,
                                 double * model,
                                 const double multiplier) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE void*   vl_svmdataset_get_data (VlSvmDataset const *self) ;
VL_INLINE vl_size vl_svmdataset_get_dimension (VlSvmDataset const *self) ;
VL_INLINE void*   vl_svmdataset_get_map (VlSvmDataset const *self) ;
VL_INLINE VlSvmDatasetFeatureMap vl_svmdataset_get_map_func (VlSvmDataset const *self) ;
VL_INLINE vl_size vl_svmdataset_get_order (VlSvmDataset const *self) ;
/** @} */

/* -------------------------------------------------------------------
 *                                     Inline functions implementation
 * ---------------------------------------------------------------- */

/** ------------------------------------------------------------------
 ** @brief Get Data.
 ** @param self Svm Dataset.
 ** @return data pointer.
 **/

VL_INLINE void*
vl_svmdataset_get_data (VlSvmDataset const *self)
{
  return self->data ;
}

/** ------------------------------------------------------------------
 ** @brief Get Data Dimension.
 ** @param self Svm Dataset.
 ** @return data dimension.
 **/

VL_INLINE vl_size
vl_svmdataset_get_dimension (VlSvmDataset const *self)
{
  return self->dimension ;
}

/** ------------------------------------------------------------------
 ** @brief Get Feature Map Object.
 ** @param self Svm Dataset.
 ** @return feature map.
 **/

VL_INLINE void*
vl_svmdataset_get_map (VlSvmDataset const *self)
{
  return self->map ;
}

/** ------------------------------------------------------------------
 ** @brief Get Feature Map Function.
 ** @param self Svm Dataset.
 ** @return feature map function.
 **/

VL_INLINE VlSvmDatasetFeatureMap
vl_svmdataset_get_map_func (VlSvmDataset const *self)
{
  return self->mapFunc ;
}

/** ------------------------------------------------------------------
 ** @brief Get Feature Map Order.
 ** @param self Svm Dataset.
 ** @return feature map order.
 **/

VL_INLINE vl_size
vl_svmdataset_get_map_order (VlSvmDataset const *self)
{
  return self->order ;
}
/* VL_SVMDATASET_H */
#endif


