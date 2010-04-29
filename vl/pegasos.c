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
                                          double biasMultiplier)
{
  vl_uindex iteration ;
  vl_uindex i ;
  T const * x ;
  T acc, eta, y, scale ;
  double lambda = regularizer ;
  double sqrtLambda = sqrt(lambda) ;
  for (iteration = 0 ; iteration < numIterations ; ++ iteration) {
    /* pick a sample  */
    vl_uindex k = iteration % numSamples ;
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

