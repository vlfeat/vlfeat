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

VL_EXPORT
void vl_pegasos_train_binary_svm (VlSvm * svm,
				  void const * data,
				  vl_size dataDimension,
				  vl_size numSamples,
				  vl_int8 const * labels,
				  vlSvmInnerProductFunction innerProduct,
				  vlSvmAccumulatorFunction accumulator,
				  VlRand* randomGenerator,
				  vl_uint32 const * permutation,
				  vl_size permutationSize,
				  vlSvmFeatureMap mapFunc, 
				  const void * map) ;

VL_EXPORT
void vl_pegasos_train_binary_svm_diagnostics (VlSvm * svm,
					      void const * data,
					      vl_size dataDimension,
					      vl_size numSamples,
					      vl_int8 const * labels,
					      vlSvmInnerProductFunction innerProduct,
					      vlSvmAccumulatorFunction accumulator,
					      VlRand* randomGenerator,
					      vl_uint32 const * permutation,
					      vl_size permutationSize,
					      vlSvmFeatureMap mapFunc, 
					      const void * map,
					      vlSvmDiagnostics diagnostics,
					      vl_size diagnosticsFrequency) ;




/* VL_PEGASOS_H */
#endif
