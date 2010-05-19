/** @file     pegasos.h
 ** @brief    PEGASOS SVM
 ** @author   Andrea Vedaldi
 **/

#ifndef VL_PEGASOS_H
#define VL_PEGASOS_H

#include "generic.h"

VL_EXPORT
void vl_pegasos_train_binary_svm_d (double * model,
                                    double const * data,
                                    vl_size dimension,
                                    vl_size numSamples,
                                    vl_int8 const * labels,
                                    double regularizer,
                                    double biasMultiplier,
                                    vl_uindex startingIteration,
                                    vl_size numIterations,
                                    VlRand* randomGenerator) ;

VL_EXPORT
void vl_pegasos_train_binary_svm_f (float * model,
                                    float const * data,
                                    vl_size dimension,
                                    vl_size numSamples,
                                    vl_int8 const * labels,
                                    double regularizer,
                                    double biasMultiplier,
                                    vl_uindex startingIteration,
                                    vl_size numIterations,
                                    VlRand* randomGenerator) ;

/* VL_PEGASOS_H */
#endif
