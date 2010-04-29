#ifndef VL_PEGASOS_H
#define VL_PEGASOS_H

#include "generic.h"

void vl_pegasos_train_binary_svm_d (double * model,
                                    double const * data,
                                    vl_size dimension,
                                    vl_size numData,
                                    vl_int8 const * labels,
                                    double regularizer,
                                    vl_size numIterations,
                                    double biasMultiplier) ;

void vl_pegasos_train_binary_svm_f (float * model,
                                    float const * data,
                                    vl_size dimension,
                                    vl_size numData,
                                    vl_int8 const * labels,
                                    double regularizer,
                                    vl_size numIterations,
                                    double biasMultiplier) ;

/* VL_PEGASOS_H */
#endif
