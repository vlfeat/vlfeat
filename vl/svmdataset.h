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
typedef void (*VlSvmFeatureMap)(const void* map, double * destination,
                                const vl_size stride, const double x) ;

/** @brief Inner product function type */
typedef double (*VlSvmInnerProductFunction)(const void* data,
                                            const vl_uindex element,
                                            const double* model) ;

/** @brief Accumulator function type */
typedef void (*VlSvmAccumulatorFunction)(const void* data,
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

  /* Feature Map */
  void* map ;
  VlSvmFeatureMap mapFunc ;
  vl_size order ;
} VlSvmDataset ;


VL_EXPORT
VlSvmDataset* vl_svmdataset_new (void * data, vl_size dimension) ; 

VL_EXPORT
void vl_svmdataset_set_map (VlSvmDataset * data, void * map,VlSvmFeatureMap mapFunc, vl_size order) ;

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset) ;

/* ---------------------------------------------------------------- */
/*    Standard Atomic Functions                                     */
/* ---------------------------------------------------------------- */

VL_EXPORT
double vl_svmdataset_innerproduct_d(const void* data, const vl_uindex element,
                             const double* model) ;

VL_EXPORT
void vl_svm_accumulator_d(const void* data,
                          const vl_uindex element,
                          double * model,
                          const double multiplier) ;

VL_EXPORT
double vl_svm_innerproduct_f(const void* data, const vl_uindex element,
                             const double* model) ;

VL_EXPORT
void vl_svm_accumulator_f(const void* data,
                          const vl_uindex element,
                          double * model,
                          const double multiplier) ;

/* VL_SVM_DATASET_H */
#endif


