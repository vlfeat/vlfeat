/** @file pegasos.h
 ** @brief PEGASOS (@ref pegasos)
 ** @author Andrea Vedaldi
 ** @author Daniele Perrone 2012
 **/

/*
Copyright
*/

#ifndef VL_PEGASOS_H
#define VL_PEGASOS_H

#include "svmdataset.h"


/** @brief Binary Svm Objective Function */
typedef struct _VlSvmObjective {
  double energy ;
  double regularizer ;
  double lossPos ;
  double lossNeg ;
  double hardLossPos ;
  double hardLossNeg ;
} VlSvmObjective ;


/** @brief Pegasos Svm Solver  */
typedef struct _VlSvmPegasos {
  double *  model ;
  double bias ;
  vl_size dimension ;
  vl_size iterations ;
  vl_size maxIterations ;
  double epsilon ;
  double lambda ;
  double biasMultiplier ;
  double elapsedTime ;
  vl_size energyFrequency ;
  double  biasLearningRate ;
  VlSvmObjective*  objective ;
  VlRand* randomGenerator ;
  vl_uint32 * permutation ;
  vl_size permutationSize ;
} VlSvmPegasos ;

/* Diagnostic function */
typedef void (*VlSvmDiagnostics) (void * diagnosticCallerRef, VlSvmPegasos *svm) ;

VL_EXPORT
VlSvmPegasos* vl_svmpegasos_new (vl_size dimension,
                                 double lambda) ;

VL_EXPORT
void vl_svmpegasos_delete (VlSvmPegasos* svm) ;


VL_EXPORT
void vl_svmpegasos_train (VlSvmPegasos * svm,
                          void * data,
                          vl_size numSamples,
                          vl_int8 const * labels,
                          VlSvmInnerProductFunction innerProduct,
                          VlSvmAccumulatorFunction accumulator,
                          void * validation,
                          vl_size validationNumSamples,
                          vl_int8 const * validationLabels,
                          VlSvmDiagnostics diagnostics,
                          void * diagnosticCallerRef) ;


/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE double* vl_svmpegasos_get_model             (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_bias              (VlSvmPegasos const *self) ;
VL_INLINE vl_size vl_svmpegasos_get_dimension         (VlSvmPegasos const *self) ;
VL_INLINE vl_size vl_svmpegasos_get_iterations        (VlSvmPegasos const *self) ;
VL_INLINE vl_size vl_svmpegasos_get_maxiterations     (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_epsilon           (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_lambda            (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_bias_multiplier   (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_elapsed_time      (VlSvmPegasos const *self) ;
VL_INLINE vl_size vl_svmpegasos_get_energy_frequency  (VlSvmPegasos const *self) ;
VL_INLINE double  vl_svmpegasos_get_bias_learningrate (VlSvmPegasos const *self) ;
VL_INLINE VlSvmObjective* vl_svmpegasos_get_objective (VlSvmPegasos const *self) ;
VL_INLINE VlRand* vl_svmpegasos_get_random_generator  (VlSvmPegasos const *self) ;
VL_INLINE vl_uint32* vl_svmpegasos_get_permutation    (VlSvmPegasos const *self) ;
VL_INLINE vl_size vl_svmpegasos_get_permutation_size  (VlSvmPegasos const *self) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_svmpegasos_set_maxiterations   (VlSvmPegasos *self, vl_size i) ;
VL_INLINE void vl_svmpegasos_set_iterations      (VlSvmPegasos *self, vl_size i) ;
VL_INLINE void vl_svmpegasos_set_epsilon         (VlSvmPegasos *self, double e) ;
VL_INLINE void vl_svmpegasos_set_lambda          (VlSvmPegasos *self, double l) ;
VL_INLINE void vl_svmpegasos_set_bias_multiplier (VlSvmPegasos *self, double b) ;
VL_INLINE void vl_svmpegasos_set_energy_frequency(VlSvmPegasos *self, vl_size e) ;
VL_INLINE void vl_svmpegasos_set_bias_learningrate (VlSvmPegasos *self, double  blr) ;
VL_INLINE void vl_svmpegasos_set_random_generator(VlSvmPegasos *self, VlRand * r) ;
VL_INLINE void vl_svmpegasos_set_permutation     (VlSvmPegasos *self, vl_uint32* p,
                                                  vl_size psize) ;
/** @} */

/* -------------------------------------------------------------------
 *                                     Inline functions implementation
 * ---------------------------------------------------------------- */

/** ------------------------------------------------------------------
 ** @brief Get svm model.
 ** @param self Pegasos Svm Solver.
 ** @return pointer to model.
 **/

VL_INLINE double*
vl_svmpegasos_get_model (VlSvmPegasos const *self)
{
  return self->model ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias.
 ** @param self Pegasos Svm Solver.
 ** @return bias value.
 **/

VL_INLINE double
vl_svmpegasos_get_bias (VlSvmPegasos const *self)
{
  return self->bias ;
}

/** ------------------------------------------------------------------
 ** @brief Get model dimension.
 ** @param self Pegasos Svm Solver.
 ** @return model dimension without considering the bias value.
 **/

VL_INLINE vl_size
vl_svmpegasos_get_dimension (VlSvmPegasos const *self)
{
  return self->dimension ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of Iterations performed so far.
 ** @param self Pegasos Svm Solver.
 ** @return number of iterations performed so far.
 **/

VL_INLINE vl_size
vl_svmpegasos_get_iterations (VlSvmPegasos const *self)
{
  return self->iterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations.
 ** @param self Pegasos Svm Solver.
 ** @return maximum number of iterations.
 **/

VL_INLINE vl_size
vl_svmpegasos_get_maxiterations (VlSvmPegasos const *self)
{
  return self->maxIterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get stopping criterion threshold.
 ** @param self Pegasos Svm Solver.
 ** @return stopping criterion threshold.
 **/

VL_INLINE double
vl_svmpegasos_get_epsilon (VlSvmPegasos const *self)
{
  return self->epsilon ;
}

/** ------------------------------------------------------------------
 ** @brief Get regularization parameter lambda.
 ** @param self Pegasos Svm Solver.
 ** @return lambda.
 **/

VL_INLINE double
vl_svmpegasos_get_lambda (VlSvmPegasos const *self)
{
  return self->lambda ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias multiplier.
 ** @param self Pegasos Svm Solver.
 ** @return bias multiplier.
 **/

VL_INLINE double
vl_svmpegasos_get_bias_multiplier (VlSvmPegasos const *self)
{
  return self->biasMultiplier ;
}

/** ------------------------------------------------------------------
 ** @brief Get the execution time in seconds.
 ** @param self Pegasos Svm Solver.
 ** @return elapsed time.
 **/

VL_INLINE double
vl_svmpegasos_get_elapsed_time (VlSvmPegasos const *self)
{
  return self->elapsedTime ;
}

/** ------------------------------------------------------------------
 ** @brief Get how frequent the full energy is computed.
 ** @param self Pegasos Svm Solver.
 ** @return energy computation frequency.
 **/

VL_INLINE vl_size
vl_svmpegasos_get_energy_frequency (VlSvmPegasos const *self)
{
  return self->energyFrequency ;
}

/** ------------------------------------------------------------------
 ** @brief Get bias learning rate.
 ** @param self Pegasos Svm Solver.
 ** @return bias learning rate.
 **/

VL_INLINE double
vl_svmpegasos_get_bias_learningrate (VlSvmPegasos const *self)
{
  return self->biasLearningRate ;
}

/** ------------------------------------------------------------------
 ** @brief Get svm objective energy.
 ** @param self Pegasos Svm Solver.
 ** @return objective energy.
 **/

VL_INLINE VlSvmObjective*
vl_svmpegasos_get_objective (VlSvmPegasos const *self)
{
  return self->objective ;
}

/** ------------------------------------------------------------------
 ** @brief Get random generator.
 ** @param self Pegasos Svm Solver.
 ** @return random generator.
 **/

VL_INLINE VlRand*
vl_svmpegasos_get_random_generator (VlSvmPegasos const *self)
{
  return self->randomGenerator ;
}

/** ------------------------------------------------------------------
 ** @brief Get permutation.
 ** @param self Pegasos Svm Solver.
 ** @return permutation.
 **/

VL_INLINE vl_uint32*
vl_svmpegasos_get_permutation (VlSvmPegasos const *self)
{
  return self->permutation ;
}

/** ------------------------------------------------------------------
 ** @brief Get permutation size.
 ** @param self Pegasos Svm Solver.
 ** @return permutation size.
 **/

VL_INLINE vl_size
vl_svmpegasos_get_permutationsize (VlSvmPegasos const *self)
{
  return self->permutationSize ;
}

/** ------------------------------------------------------------------
 ** @brief Set pegasos max iterations.
 ** @param self Pegasos Svm Solver.
 ** @param i pegasos max iterations
 **/

VL_INLINE void
vl_svmpegasos_set_maxiterations (VlSvmPegasos *self, vl_size i)
{
  self->maxIterations = i ;
}

/** ------------------------------------------------------------------
 ** @brief Set pegasos performed iterations.
 ** @param self Pegasos Svm Solver.
 ** @param i pegasos performed iterations.
 **/

VL_INLINE void
vl_svmpegasos_set_iterations (VlSvmPegasos *self, vl_size i)
{
  self->iterations = i ;
}

/** ------------------------------------------------------------------
 ** @brief Set pegasos stopping criterion threshold.
 ** @param self Pegasos Svm Solver.
 ** @param e stopping criterion threshold.
 **/

VL_INLINE void
vl_svmpegasos_set_epsilon (VlSvmPegasos *self, double e)
{
  self->epsilon = e ;
}

/** ------------------------------------------------------------------
 ** @brief Set lambda.
 ** @param self Pegasos Svm Solver.
 ** @param l lambda.
 **/

VL_INLINE void
vl_svmpegasos_set_lambda (VlSvmPegasos *self, double l)
{
  self->lambda = l ;
}

/** ------------------------------------------------------------------
 ** @brief Set bias multiplier.
 ** @param self Pegasos Svm Solver.
 ** @param b bias multiplier.
 **/

VL_INLINE void
vl_svmpegasos_set_bias_multiplier (VlSvmPegasos *self, double b)
{
  self->biasMultiplier = b ;
}

/** ------------------------------------------------------------------
 ** @brief Set how frequent the full energy is computed.
 ** @param self Pegasos Svm Solver.
 ** @param e energy compuation frequency.
 **/

VL_INLINE void
vl_svmpegasos_set_energy_frequency (VlSvmPegasos *self, vl_size e)
{
  self->energyFrequency = e ;
}

/** ------------------------------------------------------------------
 ** @brief Set bias learning rate.
 ** @param self Pegasos Svm Solver.
 ** @param blr bias learning rate.
 **/

VL_INLINE void
vl_svmpegasos_set_bias_learningrate (VlSvmPegasos *self, double blr)
{
  self->biasLearningRate = blr ;
}

/** ------------------------------------------------------------------
 ** @brief Set random generator.
 ** @param self Pegasos Svm Solver.
 ** @param r random generator.
 **/

VL_INLINE void
vl_svmpegasos_set_random_generator(VlSvmPegasos *self, VlRand * r)
{
  self->randomGenerator = r ;
}

/** ------------------------------------------------------------------
 ** @brief Set permutation.
 ** @param self Pegasos Svm Solver.
 ** @param p permutation.
 ** @param psize permutation size.
 **/

VL_INLINE void
vl_svmpegasos_set_permutation(VlSvmPegasos *self, vl_uint32* p,
                                   vl_size psize)
{
  self->permutation = p ;
  self->permutationSize = psize ;
}

/* VL_PEGASOS_H */
#endif
