/** @file svm_solver.h
 ** @brief Generic SVM solver
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#ifndef VL_SVM_SOLVER_H
#define VL_SVM_SOLVER_H

#include "generic.h"

// VlSvmObjective
typedef struct _VlSvmStatus {
  double energy ;
  double regularizer ;
  double lossPos ;
  double lossNeg ;
  double hardLossPos ;
  double hardLossNeg ;
} VlSvmStatus ;


// VlSvmSolverPegasos
// VlSvmSolverDualCoordiante

typedef struct _VlSvm {
  double * model ;
  vl_size dimension ;
  vl_size iterationsSoFar ;
  vl_size maxIterations ;
  double regularizer ;
  double biasMultiplier ;
  double * preConditioner ;

  double elapsedTime ;
  
  // VlSvmObjective objective ;
} VlSvm ;

/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/* Atomic Functions used by a svn solver */
typedef void (*vlSvmFeatureMap)(const void* map, double * destination,vl_size stride,double x) ;

/** @brief Inner product function type */
typedef double (*vlSvmInnerProductFunction)(const double* model, vl_size modelDimension, 
                                            const void* data, vl_size dataDimension, vl_uindex element, 
                                            vlSvmFeatureMap mapFunc,
                                            const void * map) ;

/** @brief Accumulator function type */
typedef void (*vlSvmAccumulatorFunction)(VlSvm* model,  vl_size modelDimension,
                                         const void* data, vl_size dataDimension, vl_uindex element,
                                         double multiplier,
                                         vlSvmFeatureMap mapFunc,
                                         const void * map) ;

/* Diagnostic function */
typedef void (*vlSvmDiagnostics) (VlSvm *svm, VlSvmStatus* status) ;

/* Standard Atomic Functions  */
VL_EXPORT
double vlSvmInnerProductFunction_d(const double* model, vl_size modelDimension, 
                                   const void* data, vl_size dataDimension, vl_uindex element,
                                   vlSvmFeatureMap mapFunc, const void * map) ;

VL_EXPORT
void vlSvmAccumulatorFunction_d(VlSvm* svm,  vl_size modelDimension,
                                const void* data, vl_size dataDimension, vl_uindex element,
                                double multiplier, vlSvmFeatureMap mapFunc, const void * map) ;


VL_EXPORT
double vlSvmInnerProductFunction_f(const double* model, vl_size modelDimension,
                                   const void* data, vl_size dataDimension, vl_uindex element,
                                   vlSvmFeatureMap mapFunc, const void * map) ;

VL_EXPORT
void vlSvmAccumulatorFunction_f(VlSvm* svm,  vl_size modelDimension, const void* data, vl_size dataDimension, vl_uindex element, double multiplier, vlSvmFeatureMap mapFunc, const void * map) ;

/* Utilities  */
VL_EXPORT
VlSvm* initialiseSvm (vl_size dimension, double regularizer, double biasMultiplier , double *preConditioner, vl_size maxIterations) ;

VL_EXPORT
void deleteSvm (VlSvm* svm) ;

VL_EXPORT
void vlSvmComputeDiagnostics(VlSvm *svm, VlSvmStatus* status, void const * data,vl_size dataDimension, vl_size numSamples, const vl_int8 * labels, vlSvmInnerProductFunction innerProduct, vlSvmFeatureMap mapFunc, const void * map ) ;

/* VL_SVM_SOLVER_H */
#endif
