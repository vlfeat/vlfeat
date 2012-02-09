/** @file pegasos.h
 ** @brief PEGASOS (@ref pegasos)
 ** @author Andrea Vedaldi
 **/

/*
Copyright
*/

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
                                    VlRand* randomGenerator,
                                    vl_uint32 const * permutation,
                                    vl_size permutationSize,
                                    double const * preconditioner) ;

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
                                    VlRand* randomGenerator,
                                    vl_uint32 const * permutation,
                                    vl_size permutationSize,
                                    float const * preconditioner) ;


/* VL_PEGASOS_H */
#endif
