/** @file svms.h
 ** @brief Common functions and structures for SVMs
 ** @author Milan Sulc
 ** @author Daniele Perrone
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Milan Sulc.
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2011-13 Andrea Vedaldi.

All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_SVMS_H
#define VL_SVMS_H

#include "svmdataset.h"


#define VL_SVM_SGD  1
#define	VL_SVM_DCA  2


/** @brief Binary Svm Objective Function
 **
 ** This structure keeps some statistics on a specific classifier
 ** learned with the SVM Solver.
 **/
typedef struct _VlSvmObjective {
  double energy ;               /**< full energy value. */
  double regularizer ;          /**< regularization term. */
  double lossPos ;              /**< energy term due to false positives. */
  double lossNeg ;              /**< energy term due to false negatives. */
  double hardLossPos ;          /**< number of false positives. */
  double hardLossNeg ;          /**< number of false negatives. */
  /* for DCA : */
  double lossDual ;            /**< sum of conjugate (-alpha) losses */
  double energyDual ;          /**< the value of SVM dual objective */
  double dualityGap ;          /**< duality gap = primal - dual */

} VlSvmObjective ;


/** @typedef VlSvmDiagnostics
 ** @brief Pointer to a function called for
 ** diagnostics purposes.
 **
 ** @param svm is an instance of @ref VlSvm .
 **
 **/
typedef void (*VlSvmDiagnostics) (void *svm) ;


/** @brief Svm Solver
 **
 ** This structure represents the status of an SVM solver.
 **/
typedef struct _VlSvm {
  vl_uint8 type;                /**< svm type - VL_SVM_SGD or VL_SVM_DCA. */
  double * model ;             /**< svm model. */
  double bias ;                 /**< bias element. */
  vl_size dimension ;           /**< model length. */
  vl_size iterations ;          /**< number of iterations. */
  vl_size maxIterations ;       /**< maximum number of iterations. */
  double epsilon ;              /**< stopping criterion threshold */
  double lambda ;               /**< regularization parameter. */
  double biasMultiplier ;       /**< bias strength multiplier. */
  double elapsedTime ;          /**< elapsed time from algorithm start. */
  VlSvmObjective*  objective ;  /**< value and statistics of the objective function. */
  VlSvmDiagnostics diagnostic ; /**< diagnostic MEX function. */
  void * diagnosticFunction ; /**< reference to diagnostic MATLAB function */
  //vl_size diagnosticFreq;     /**< number of iterations between diagnostics */

// SGD specific:
  vl_size energyFrequency ;     /**< frequency of computation of svm energy */
  double  biasPreconditioner ;  /**< bias learning preconditioner.  */
  VlRand* randomGenerator ;     /**< random generator.  */
  vl_uint32 * permutation ;     /**< data permutation. */
  vl_size permutationSize ;     /**< permutation size. */

// DCA specific:
  vl_bool randomPermutation; /**< use Random Permutation of Sub-problems */
  vl_bool onlineSetting; /**< use an Online Setting */
  double * alpha;

} VlSvm;






/** @typedef VlSvmLossFunction
 ** @brief Pointer to a function that defines the loss function
 **/
typedef double (*VlSvmLossFunction)(vl_int8 label, double inner) ;

/** @typedef VlSvmLossConjugateFunction
 ** @brief Pointer to a function that defines the conjugate function of the loss function
 **/
typedef double (*VlSvmLossConjugateFunction)(vl_int8 label, double alpha);

/** @typedef VlSvmDeltaAlpha
 ** @brief Pointer to a function that defines the computation of alpha addition
 **/
typedef double (*VlSvmDeltaAlpha)(vl_int8 label, double inner, VlSvm * svm, double xi_square, double alpha, vl_size numSamples);





/** ------------------------------------------------------------------
** @brief Create a new @ref VlSvm structure
** @param dimension svm model dimension.
** @param lambda regularization parameter.
** @param type SVM type ( VL_SVM_SGD / VL_SVM_DCA )
**
** This function allocates and returns a new @ref VlSvm structure.
**
** @return the new @ref VlSvm structure.
** @sa ::vl_svm_delete
**/
VL_INLINE
VlSvm* vl_svm_new (vl_size dimension, double lambda, vl_uint8 type) {

  VlSvm* svm = (VlSvm*) vl_malloc(sizeof(VlSvm)) ;

  svm->type = type;

  svm->model = (double*) vl_calloc(dimension, sizeof(double)) ;
  svm->dimension = dimension ;
  svm->objective = (VlSvmObjective*) vl_malloc(sizeof(VlSvmObjective)) ;
  svm->iterations = 0 ;
  svm->maxIterations = 1000;
  svm->lambda = lambda ;
  svm->epsilon = 0 ;
  svm->biasMultiplier = 1 ;
  svm->bias = 0 ;
  svm->elapsedTime = 0 ;

  svm->diagnostic = NULL ;
  //svm->diagnosticFreq = 100 ;
  svm->diagnosticFunction = NULL ;


  /* SGD specific: */
  svm->biasPreconditioner = 1 ;
  svm->energyFrequency = 100 ;
  svm->randomGenerator = NULL ;
  svm->permutation = NULL ;


  /* DCA specific: */
  svm->randomPermutation = 1;
  svm->onlineSetting = 0;


  return svm ;
}

/** -------------------------------------------------------------------
 ** @brief Delete a @ref VlSvm structure
 ** @param *svm @ref VlSvm structure.
 **
 ** The function frees the resources allocated by
 ** ::vl_svm_new().
 **/

VL_INLINE
void vl_svm_delete (VlSvm * svm, vl_bool freeModel)
{
  if (svm->model && freeModel) vl_free(svm->model) ;
  if (svm->objective) vl_free(svm->objective) ;
  vl_free(svm) ;
}


/** -------------------------------------------------------------------
 ** @brief Compute diagnostics for an @ref VlSvm structure
 ** @param *svm @ref VlSvm structure.
 ** @param *dataset pointer to training dataset
 ** @param numSamples number of training samples
 ** @param labels labels for training data (+1/-1)
 ** @param innerProduct function defining the innerProduct between the model and a data point.
 **/

VL_INLINE
void
vl_svm_compute_diagnostic(VlSvm *svm,
                          void * dataset,
                          vl_size numSamples,
                          vl_int8 const * labels,
                          VlSvmDatasetInnerProduct innerProduct,
                          VlSvmLossFunction lossFunction,
                          VlSvmLossConjugateFunction lossConjugateFunction
                          )
{
  vl_size i, k ;

  svm->objective->regularizer = svm->bias * svm->bias;
  for (i = 0; i < svm->dimension; i++) {
    svm->objective->regularizer += svm->model[i] * svm->model[i] ;
  }
  svm->objective->regularizer *= svm->lambda * 0.5 ;

  svm->objective->lossPos = 0 ;
  svm->objective->lossNeg = 0 ;
  svm->objective->hardLossPos = 0 ;
  svm->objective->hardLossNeg = 0 ;

  svm->objective->lossDual = 0 ;



  for (k = 0; k < numSamples; k++) {

    double inner = innerProduct(dataset,k,svm->model) ;
    inner += svm->bias * svm->biasMultiplier;
    double loss = lossFunction(labels[k],inner) ;

    if (labels[k] < 0) {
      svm->objective->lossNeg += loss ;
      svm->objective->hardLossNeg += (loss > 0) ;
    } else {
      svm->objective->lossPos += loss ;
      svm->objective->hardLossPos += (loss > 0) ;
    }

    if (svm->type == VL_SVM_DCA) svm->objective->lossDual += lossConjugateFunction(labels[k], - svm->alpha[k]);
  }

  svm->objective->lossPos /= numSamples ;
  svm->objective->lossNeg /= numSamples ;
  svm->objective->hardLossPos /= numSamples ;
  svm->objective->hardLossNeg /= numSamples ;
  svm->objective->energy = svm->objective->regularizer + svm->objective->lossPos + svm->objective->lossNeg ;


  if (svm->type == VL_SVM_DCA) {
    svm->objective->lossDual /= numSamples ;
    svm->objective->energyDual = - svm->objective->regularizer - svm->objective->lossDual;
    svm->objective->dualityGap = svm->objective->energy - svm->objective->energyDual;
  }

}




/** @name Methods for different loss functions
 ** @{
 **/

VL_INLINE
double vl_L1_loss(vl_int8 label, double inner) {
  return VL_MAX(1 - label*inner, 0.0);
}

VL_INLINE
double vl_L2_loss(vl_int8 label, double inner) {
  double l = VL_MAX(1 - label*inner, 0.0);
  return l*l;
}

VL_INLINE
double vl_L1_lossConjugate (vl_int8 label, double alpha) {
  return label*alpha;
}

VL_INLINE
double vl_L2_lossConjugate (vl_int8 label, double alpha) {
  return (label-alpha/4)*alpha;
}

VL_INLINE
double vl_L1_deltaAlpha (vl_int8 label, double inner, VlSvm * svm, double xi_square, double alpha, vl_size numSamples) {
  //return svm->lambda*numSamples*(label-inner)/xi_square;
  return label * VL_MAX(0, VL_MIN(1,  svm->lambda*numSamples*(1-label*inner)/xi_square +alpha*label ) ) - alpha;
}

VL_INLINE
double vl_L2_deltaAlpha (vl_int8 label, double inner, VlSvm * svm, double xi_square, double alpha, vl_size numSamples) {
  //return svm->lambda*numSamples*(label-inner-alpha/2)/(xi_square + svm->lambda*numSamples/2);
  return label * VL_MAX(0, (svm->lambda*numSamples*(1-label*inner-label*alpha/2)/(xi_square + svm->lambda*numSamples/2) + alpha*label)) - alpha;

}



/* VL_SVMS_H */
#endif
