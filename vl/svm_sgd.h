/** @file svm_sgd.h
 ** @brief SGD SVM Solver (@ref SGD)
 ** @author Daniele Perrone
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2011-12 Andrea Vedaldi.

All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_SGD_H
#define VL_SGD_H

#include "svms.h"


/** @name Training Fuctions
 ** @{
 **/
VL_EXPORT
void vl_svm_sgd_train (VlSvm * svm,
                          void * dataset,
                          vl_size numSamples,
                          VlSvmDatasetInnerProduct innerProduct,
                          VlSvmDatasetAccumulator accumulator,
                          vl_int8 const * labels) ;

VL_EXPORT
void vl_svm_sgd_train_validation_data (VlSvm * svm,
                                          void * dataset,
                                          vl_size numSamples,
                                          VlSvmDatasetInnerProduct innerProduct,
                                          VlSvmDatasetAccumulator accumulator,
                                          vl_int8 const * labels,
                                          void * validation,
                                          vl_size validationNumSamples,
                                          vl_int8 const * validationLabels) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE double* vl_svm_sgd_get_model             (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_bias              (VlSvm const *self) ;
VL_INLINE vl_size vl_svm_sgd_get_dimension         (VlSvm const *self) ;
VL_INLINE vl_size vl_svm_sgd_get_iterations        (VlSvm const *self) ;
VL_INLINE vl_size vl_svm_sgd_get_maxiterations     (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_epsilon           (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_lambda            (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_bias_multiplier   (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_elapsed_time      (VlSvm const *self) ;
VL_INLINE vl_size vl_svm_sgd_get_energy_frequency  (VlSvm const *self) ;
VL_INLINE double  vl_svm_sgd_get_bias_preconditioner (VlSvm const *self) ;
VL_INLINE VlSvmObjective* vl_svm_sgd_get_objective (VlSvm const *self) ;
VL_INLINE VlRand* vl_svm_sgd_get_random_generator  (VlSvm const *self) ;
VL_INLINE vl_uint32* vl_svm_sgd_get_permutation    (VlSvm const *self) ;
VL_INLINE vl_size vl_svm_sgd_get_permutation_size  (VlSvm const *self) ;
VL_INLINE VlSvmDiagnostics vl_svm_sgd_get_diagnostic (VlSvm const *self) ;
VL_INLINE void* vl_svm_sgd_get_diagnostic_caller_ref (VlSvm const *self) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_svm_sgd_set_model           (VlSvm *self, double * m) ;
VL_INLINE void vl_svm_sgd_set_bias            (VlSvm *self, double  b) ;
VL_INLINE void vl_svm_sgd_set_maxiterations   (VlSvm *self, vl_size i) ;
VL_INLINE void vl_svm_sgd_set_iterations      (VlSvm *self, vl_size i) ;
VL_INLINE void vl_svm_sgd_set_epsilon         (VlSvm *self, double e) ;
VL_INLINE void vl_svm_sgd_set_lambda          (VlSvm *self, double l) ;
VL_INLINE void vl_svm_sgd_set_bias_multiplier (VlSvm *self, double b) ;
VL_INLINE void vl_svm_sgd_set_energy_frequency(VlSvm *self, vl_size e) ;
VL_INLINE void vl_svm_sgd_set_bias_preconditioner (VlSvm *self, double  blr) ;
VL_INLINE void vl_svm_sgd_set_random_generator(VlSvm *self, VlRand * r) ;
VL_INLINE void vl_svm_sgd_set_permutation     (VlSvm *self, vl_uint32* p,
                                                  vl_size psize) ;
VL_INLINE void vl_svm_sgd_set_diagnostic      (VlSvm *self,
                                                  VlSvmDiagnostics d) ;
/** @} */

/* -------------------------------------------------------------------
 *                                     Inline functions implementation
 * ---------------------------------------------------------------- */

/** ------------------------------------------------------------------
 ** @brief Get svm model.
 ** @param self SGD Svm Solver.
 ** @return pointer to model.
 **/

VL_INLINE double*
vl_svm_sgd_get_model (VlSvm const *self)
{
  return self->model ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias.
 ** @param self SGD Svm Solver.
 ** @return bias value.
 **/

VL_INLINE double
vl_svm_sgd_get_bias (VlSvm const *self)
{
  return self->bias ;
}

/** ------------------------------------------------------------------
 ** @brief Get model dimension.
 ** @param self SGD Svm Solver.
 ** @return model dimension without considering the bias value.
 **/

VL_INLINE vl_size
vl_svm_sgd_get_dimension (VlSvm const *self)
{
  return self->dimension ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of Iterations performed so far.
 ** @param self SGD Svm Solver.
 ** @return number of iterations performed so far.
 **/

VL_INLINE vl_size
vl_svm_sgd_get_iterations (VlSvm const *self)
{
  return self->iterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations.
 ** @param self SGD Svm Solver.
 ** @return maximum number of iterations.
 **/

VL_INLINE vl_size
vl_svm_sgd_get_maxiterations (VlSvm const *self)
{
  return self->maxIterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get stopping criterion threshold.
 ** @param self SGD Svm Solver.
 ** @return stopping criterion threshold.
 **/

VL_INLINE double
vl_svm_sgd_get_epsilon (VlSvm const *self)
{
  return self->epsilon ;
}

/** ------------------------------------------------------------------
 ** @brief Get regularization parameter lambda.
 ** @param self SGD Svm Solver.
 ** @return lambda.
 **/

VL_INLINE double
vl_svm_sgd_get_lambda (VlSvm const *self)
{
  return self->lambda ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias multiplier.
 ** @param self SGD Svm Solver.
 ** @return bias multiplier.
 **/

VL_INLINE double
vl_svm_sgd_get_bias_multiplier (VlSvm const *self)
{
  return self->biasMultiplier ;
}

/** ------------------------------------------------------------------
 ** @brief Get the execution time in seconds.
 ** @param self SGD Svm Solver.
 ** @return elapsed time.
 **/

VL_INLINE double
vl_svm_sgd_get_elapsed_time (VlSvm const *self)
{
  return self->elapsedTime ;
}

/** ------------------------------------------------------------------
 ** @brief Get how frequent the full energy is computed.
 ** @param self SGD Svm Solver.
 ** @return energy computation frequency.
 **/

VL_INLINE vl_size
vl_svm_sgd_get_energy_frequency (VlSvm const *self)
{
  return self->energyFrequency ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias pre-conditioner.
 ** @param self SGD Svm Solver.
 ** @return bias pre-conditioner.
 **/

VL_INLINE double
vl_svm_sgd_get_bias_preconditioner (VlSvm const *self)
{
  return self->biasPreconditioner ;
}

/** ------------------------------------------------------------------
 ** @brief Get svm objective energy.
 ** @param self SGD Svm Solver.
 ** @return objective energy.
 **/

VL_INLINE VlSvmObjective*
vl_svm_sgd_get_objective (VlSvm const *self)
{
  return self->objective ;
}

/** ------------------------------------------------------------------
 ** @brief Get random generator.
 ** @param self SGD Svm Solver.
 ** @return random generator.
 **/

VL_INLINE VlRand*
vl_svm_sgd_get_random_generator (VlSvm const *self)
{
  return self->randomGenerator ;
}

/** ------------------------------------------------------------------
 ** @brief Get permutation.
 ** @param self SGD Svm Solver.
 ** @return permutation.
 **/

VL_INLINE vl_uint32*
vl_svm_sgd_get_permutation (VlSvm const *self)
{
  return self->permutation ;
}

/** ------------------------------------------------------------------
 ** @brief Get permutation size.
 ** @param self SGD Svm Solver.
 ** @return permutation size.
 **/

VL_INLINE vl_size
vl_svm_sgd_get_permutationsize (VlSvm const *self)
{
  return self->permutationSize ;
}

/** ------------------------------------------------------------------
 ** @brief Get Diagnostic Function.
 ** @param self SGD Svm Solver.
 ** @return pointer to diagnostic function.
 **/

VL_INLINE VlSvmDiagnostics
vl_svm_sgd_get_diagnostic (VlSvm const *self)
{
  return self->diagnostic ;
}


/** ------------------------------------------------------------------
 ** @brief Set SVM model.
 ** @param self SGD SVM Solver.
 ** @param m svm model.
 **
 ** If there is a model already allocated, this method frees it
 ** before substituting it with the new one.
 **/

VL_INLINE void
vl_svm_sgd_set_model (VlSvm *self, double* m)
{
  if(self->model)
    vl_free(self->model) ;
  self->model = m ;
}

/** ------------------------------------------------------------------
 ** @brief Set SVM bias.
 ** @param self SGD SVM Solver.
 ** @param b bias.
 **/

VL_INLINE void
vl_svm_sgd_set_bias (VlSvm *self, double b)
{
  self->bias = b ;
}

/** ------------------------------------------------------------------
 ** @brief Set SGD SVM max iterations.
 ** @param self SGD SVM Solver.
 ** @param i SGD max iterations.
 **/

VL_INLINE void
vl_svm_sgd_set_maxiterations (VlSvm *self, vl_size i)
{
  self->maxIterations = i ;
}

/** ------------------------------------------------------------------
 ** @brief Set SGD SVM performed iterations.
 ** @param self SGD SVM Solver.
 ** @param i SGD performed iterations.
 **/

VL_INLINE void
vl_svm_sgd_set_iterations (VlSvm *self, vl_size i)
{
  self->iterations = i ;
}

/** ------------------------------------------------------------------
 ** @brief Set SGD SVM stopping criterion threshold.
 ** @param self SGD SVM Solver.
 ** @param e stopping criterion threshold.
 **/

VL_INLINE void
vl_svm_sgd_set_epsilon (VlSvm *self, double e)
{
  self->epsilon = e ;
}

/** ------------------------------------------------------------------
 ** @brief Set lambda.
 ** @param self SGD SVM Solver.
 ** @param l lambda.
 **/

VL_INLINE void
vl_svm_sgd_set_lambda (VlSvm *self, double l)
{
  self->lambda = l ;
}

/** ------------------------------------------------------------------
 ** @brief Set bias multiplier.
 ** @param self SGD SVM Solver.
 ** @param b bias multiplier.
 **/

VL_INLINE void
vl_svm_sgd_set_bias_multiplier (VlSvm *self, double b)
{
  self->biasMultiplier = b ;
}

/** ------------------------------------------------------------------
 ** @brief Set how frequent the full energy is computed.
 ** @param self SGD SVM Solver.
 ** @param e energy compuation frequency.
 **/

VL_INLINE void
vl_svm_sgd_set_energy_frequency (VlSvm *self, vl_size e)
{
  self->energyFrequency = e ;
}

/** ------------------------------------------------------------------
 ** @brief Set bias learning rate.
 ** @param self SGD SVM Solver.
 ** @param blr bias learning rate.
 **/

VL_INLINE void
vl_svm_sgd_set_bias_preconditioner (VlSvm *self, double blr)
{
  self->biasPreconditioner = blr ;
}

/** ------------------------------------------------------------------
 ** @brief Set random generator.
 ** @param self SGD SVM Solver.
 ** @param r random generator.
 **/

VL_INLINE void
vl_svm_sgd_set_random_generator(VlSvm *self, VlRand * r)
{
  self->randomGenerator = r ;
}

/** ------------------------------------------------------------------
 ** @brief Set permutation.
 ** @param self SGD SVM Solver.
 ** @param p permutation.
 ** @param psize permutation size.
 **/

VL_INLINE void
vl_svm_sgd_set_permutation(VlSvm *self, vl_uint32* p,
                              vl_size psize)
{
  self->permutation = p ;
  self->permutationSize = psize ;
}

/** ------------------------------------------------------------------
 ** @brief Set Diagnostic Function.
 ** @param self SGD SVM Solver.
 ** @param d Diagnostic Function.
 ** @param cr Diagnostic Function Caller Reference.
 **/


VL_INLINE void
vl_svm_sgd_set_diagnostic(VlSvm *self, VlSvmDiagnostics d)
{
  self->diagnostic = d ;
}

/* VL_SGD_H */
#endif
