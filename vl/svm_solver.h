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

/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @brief Feature map function type */
typedef void (*VlSvmFeatureMap)(const void* map, double * destination,vl_size stride,double x) ;

/** @brief Inner product function type */
typedef double (*VlSvmInnerProductFunction)(const VlSvmSolver* model, const void* data,
					 const vl_size dataDimension, const vl_uindex element) ;

/** @brief Accumulator function type */
typedef void (*VlSvmAccumulatorFunction)(VlSvmSolver* model, const void* data,
					 const vl_size dataDimension, const vl_uindex element,
                                         const double multiplier) ;


/* ---------------------------------------------------------------- */
/*                                                                  */
/* ---------------------------------------------------------------- */

/** @brief Binary Svm Objective Function */
typedef struct _VlSvmObjective {
  double energy ;
  double regularizer ;
  double lossPos ;
  double lossNeg ;
  double hardLossPos ;
  double hardLossNeg ;
} VlSvmObjective ;

/** @brief Svm General Solver  */
typedef struct _VlSvmSolver {
  double * model ;
  double b ;
  vl_size dimension ;

  /* Feature Map */
  void* map ;
  VlSvmFeatureMap mapFunc ;

  double elapsedTime ;
} VlSvmSolver ;


VL_EXPORT
VlSvmSolver* vl_svmsolver_new (vl_size dimension, const void * map,
			       const VlSvmFeatureMap mapFunc) ;

VL_EXPORT
void vl_svmSolver_delete (VlSvmSolver* svm) ;

/* ---------------------------------------------------------------- */
/*    Standard Atomic Functions                                     */
/* ---------------------------------------------------------------- */

VL_EXPORT
double vl_svm_innerproduct_d(const VlSvmSolver* model, const void* data,
			     const vl_size dataDimension, const vl_uindex element) ;

VL_EXPORT
void vl_Svm_accumulator_d(VlSvmSolver* model, const void* data,
			  const vl_size dataDimension, const vl_uindex element,
			  const double multiplier) ;

VL_EXPORT
double vl_svm_innerproduct_f(const VlSvmSolver* model, const void* data,
			     const vl_size dataDimension, const vl_uindex element) ;

VL_EXPORT
void vl_Svm_accumulator_f(VlSvmSolver* model, const void* data,
			  const vl_size dataDimension, const vl_uindex element,
			  const double multiplier) ;

/* VL_SVM_SOLVER_H */
#endif
