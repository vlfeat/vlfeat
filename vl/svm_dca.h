/** @file svm_dca.h
 ** @brief Linear SVM Solver based on Dual Coordinate Descent Method
 ** @author Milan Sulc
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Milan Sulc, Andrea Vedaldi.

All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_SVM_DCA_H
#define VL_SVM_DCA_H

#include "svms.h"


/** @name Training Fuctions
 ** @{
 **/
VL_EXPORT void
vl_svm_dca_train (VlSvm * svm,
                    void * dataset,
                    vl_size numSamples,
                    VlSvmDatasetInnerProduct innerProduct,
                    VlSvmDatasetAccumulator accumulator,
                    VlSvmDatasetLengthSquare lengthSquare,
                    VlSvmLossFunction lossFunction,
                    VlSvmLossConjugateFunction lossConjugateFunction,
                    VlSvmDeltaAlpha deltaAlpha,
                    vl_int8 const * labels ) ;

VL_EXPORT void
vl_svm_dca_train_validation_data (VlSvm * svm,
                    void * dataset,
                    vl_size numSamples,
                    VlSvmDatasetInnerProduct innerProduct,
                    VlSvmDatasetAccumulator accumulator,
                    VlSvmDatasetLengthSquare lengthSquare,
                    VlSvmLossFunction lossFunction,
                    VlSvmLossConjugateFunction lossConjugateFunction,
                    VlSvmDeltaAlpha deltaAlpha,
                    vl_int8 const * labels,
                    void * validation,
                    vl_size validationNumSamples,
                    vl_int8 const * validationLabels) ;

/** @} */


/** @name Fuctions used in Training
 ** @{
 **/

VL_EXPORT
void vl_int_array_random_permute(vl_int *A, vl_size size, VlRand *random);

/** @} */

/* VL_SVM_DCA_H */
#endif
