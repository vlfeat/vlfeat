/** @file svm_dataset.h
 ** @brief Generic SVM Dataset
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#ifndef VL_SVM_DATASET_H
#define VL_SVM_DATASET_H

#include "generic.h"

/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @brief Feature map function type */
typedef void (*VlSvmDatasetFeatureMap)(const void* map, double * destination,
                                const vl_size stride, const double x) ;

/** @brief Inner product function type */
typedef double (*VlSvmDatasetInnerProduct)(const void* data,
                                            const vl_uindex element,
                                            const double* model) ;

/** @brief Accumulator function type */
typedef void (*VlSvmDatasetAccumulator)(const void* data,
                                         const vl_uindex element,
                                         double * model,
                                         const double multiplier) ;


/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @brief Svm Float Dataset */
typedef struct _VlSvmDataset {
  void * data ;
  vl_size dimension ;
  void* map ; /* Feature Map */
  VlSvmDatasetFeatureMap mapFunc ;
  vl_size order ;
} VlSvmDataset ;


VL_EXPORT
VlSvmDataset* vl_svmdataset_new (void * data, vl_size dimension) ;

VL_EXPORT
void vl_svmdataset_set_map (VlSvmDataset * data, void * map,VlSvmDatasetFeatureMap mapFunc, vl_size order) ;

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset) ;

/* ---------------------------------------------------------------- */
/*    Standard Atomic Functions                                     */
/* ---------------------------------------------------------------- */

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


/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE void*   vl_svmdataset_get_data           (VlSvmDataset const *self) ;
VL_INLINE vl_size vl_svmdataset_get_dimension      (VlSvmDataset const *self) ;
VL_INLINE void*   vl_svmdataset_get_map            (VlSvmDataset const *self) ;
VL_INLINE VlSvmDatasetFeatureMap vl_svmdataset_get_map_func (VlSvmDataset const *self) ;
VL_INLINE vl_size vl_svmdataset_get_order          (VlSvmDataset const *self) ;
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
/* VL_SVM_DATASET_H */
#endif


