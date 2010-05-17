/** @file     pegasos.c
 ** @brief    PEGASOS SVM - Definition
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available under the terms of the
 GNU GPLv2, or (at your option) any later version.
 */

/** @file pegasos.h

::pegasos.h provides basic impelmentation of the PEGASOS [1] SVM solver.

PEGASOS solves the binary SVM problem

@f[
 \min_{w} \frac{\lambda}{2} \|w\|^2 + \frac{1}{m} \sum_{i=1}^n
 \max\{0, 1 - y_i \langle w,x_i\rangle\}.
@f]

Here @f$ x_i @f$ are vectors in @f$ \mathbb{R}^d @f$, @f$ y_i \in \{-1,1\}
@f$ are binary labels, and @f$ \lambda > 0 @f$ is the regularization
parameter. This a binary SVM without bias, corresponding to the
decision function @f$ F(x) = \operatorname{sign} \langle w, x\rangle @f$.

To learn an SVM with bias, the vector @f$ x @f$ can be optionally
extended by a constant component @f$ B @f$. In this case, @f$ w @f$ is
@f$d + 1 @f$ dimensional and the decision function is @f$ F(x) = \operatorname{sign}
(\langle w_{1:d}, x\rangle+ w_{d+1} B) @f$.

PEGASOS is accessed by calling ::vl_pegasos_train_binary_svm_d or
 ::vl_pegasos_train_binary_svm_f.


@section pegasos-references

[1] S. Shalev-Shwartz, Y. Singer, and N. Srebro.
    <em>Pegasos: Primal estimated sub-GrAdient SOlver for SVM.</em>
    In Proc. ICML, 2007.
*/

/** @fn vl_pegasos_train_binary_svm_d(double*,double const*,vl_size,vl_size,vl_int8 const*,double,vl_size,double,VlRand*)
 ** @param model (out) the learned model.
 ** @param data training vectors.
 ** @param dimension data dimension.
 ** @param numSamples number of training data vectors.
 ** @param labels lables of the training vetctors.
 ** @param regularizer value of @f$ lambda @f$.
 ** @param numIterations number of PEGASOS iterations.
 ** @param biasMultiplier value of @f$ B @f$.
 ** @param randomGenerator random number generator.
 **
 ** The function runs PEGASOS on the specified data. The vector @a
 ** model must have either dimension equal to @a dimension if @a
 ** biasMultiplier is zero, or @a dimension + 1 if @a biasMultiplier
 ** is larger than zero.
 **
 ** If @a randomGenerator is @c NULL, the default random generator (as
 ** returned by ::vl_get_rand()) is used.
 **/

#ifndef VL_PEGASOS_INSTANTIATING

#include "pegasos.h"
#include "mathop.h"
#include <math.h>

#define FLT VL_TYPE_FLOAT
#define VL_PEGASOS_INSTANTIATING
#include "pegasos.c"

#define FLT VL_TYPE_DOUBLE
#define VL_PEGASOS_INSTANTIATING
#include "pegasos.c"

/* VL_PEGAOS_INSTANTIATING */
#else

#include "float.th"

VL_EXPORT void
VL_XCAT(vl_pegasos_train_binary_svm_,SFX)(T *  model,
                                          T const * data,
                                          vl_size dimension,
                                          vl_size numSamples,
                                          vl_int8 const * labels,
                                          double regularizer,
                                          vl_size numIterations,
                                          double biasMultiplier,
                                          VlRand * randomGenerator)
{
  vl_uindex iteration ;
  vl_uindex i ;
  T const * x ;
  T acc, eta, y, scale ;
  double lambda = regularizer ;
  double sqrtLambda = sqrt(lambda) ;
  if (randomGenerator == NULL) randomGenerator = vl_get_rand() ;

  for (iteration = 0 ; iteration < numIterations ; ++ iteration) {
    /* pick a sample  */
    vl_uindex k = vl_rand_uindex(randomGenerator, numSamples) ;
    y = labels[k] ;
    x = data + dimension * k ;

    /* project on the weight vector */
    acc = 0 ;
    for (i = 0 ; i < dimension ; ++i) {
      acc += x[i] * model[i] ;
    }
    acc += biasMultiplier * model[dimension] ;

    eta = 1.0 / ((iteration + 1) * lambda) ;

    /* compare to true label */
    if (y * acc < 1.0) {
      acc = 0 ;
      for (i = 0 ; i < dimension ; ++i) {
        model[i] = model[i] - eta * (lambda * model[i] - y * x[i]) ;
        acc += model[i] * model[i] ;
      }
      if (biasMultiplier) {
        model[dimension] = model[dimension] - eta * (lambda * model[dimension] - y * biasMultiplier) ;
        acc += model[dimension] * model[dimension] ;
      }
    } else {
      for (i = 0 ; i < dimension ; ++i) {
        model[i] = model[i] - eta * lambda * model[i] ;
        acc += model[i] * model[i] ;
      }
      if (biasMultiplier) {
        model[dimension] = model[dimension] - eta * lambda * model[dimension]  ;
        acc += model[dimension] * model[dimension] ;
      }
    }

    scale = (T) 1.0 / (sqrtLambda * sqrt(acc + VL_EPSILON_D)) ;
    if (scale < 1) {
      for (i = 0 ; i < dimension ; ++i) model[i] *= scale ;
      if (biasMultiplier) {
        model[dimension] *= scale ;
      }
    }
  }
}

/* VL_PEGAOS_INSTANTIATING */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif

