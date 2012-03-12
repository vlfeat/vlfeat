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

#include "svm_solver.h"

/** @brief Pegasos Svm Solver  */
typedef struct _VlSvmSolverPegasos {
  VlSvmSolver * svmSolver ;
  vl_size iterationsSoFar ;
  vl_size maxIterations ;
  double regularizer ;
  double biasMultiplier ;
  double * preConditioner ;
} VlSvmSolverPegasos ;

VL_EXPORT
VlSvmSolver* vl_svmsolver_pegasos_new (vl_size dimension, const void * map,
				       const VlSvmFeatureMap mapFunc,
				       vl_size maxIterations,
				       double regularizer,
				       double biasMultiplier,
				       double * preConditioner) ;

VL_EXPORT
void vl_svmSolver_pegasos_delete (VlSvmSolverPegasos* svm) ;

/* Diagnostic function */
typedef void (*VlSvmDiagnostics) (VlSvm *svm, VlSvmObjective* status) ;

VL_EXPORT
void vl_svmsolver_pegasos_train (VlSvmSolverPegasos * svm,
				 void const * data,
				 vl_size dataDimension,
				 vl_size numSamples,
				 vl_int8 const * labels,
				 VlSvmInnerProductFunction innerProduct,
				 VlSvmAccumulatorFunction accumulator,
				 VlRand* randomGenerator,
				 vl_uint32 const * permutation,
				 vl_size permutationSize) ;

VL_EXPORT
void vl_svmsolver_pegasos_train_diagnostics (VlSvmSolverPegasos * svm,
					     void const * data,
					     vl_size dataDimension,
					     vl_size numSamples,
					     vl_int8 const * labels,
					     VlSvmInnerProductFunction innerProduct,
					     VlSvmAccumulatorFunction accumulator,
					     VlRand* randomGenerator,
					     vl_uint32 const * permutation,
					     vl_size permutationSize,
					     VlSvmDiagnostics diagnostics,
					     vl_size diagnosticsFrequency) ;





VL_EXPORT
void vl_svm_pegasos_compute_diagnostics(VlSvmSolverPegasos *svm,
					VlSvmObjective* status,
					void const * data,
					vl_size dataDimension,
					vl_size numSamples,
					const vl_int8 * labels,
					VlSvmInnerProductFunction innerProduct) ;

/* VL_PEGASOS_H */
#endif
