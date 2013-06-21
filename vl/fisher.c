/** @file fisher.c
 ** @brief Fisher - Declaration
 ** @author David Novotny
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.
 
 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

/**
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
 @page fisher Fisher Vector encoding (FV)
 @author David Novotny
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
 
 @ref fisher.h implements the calculation of a fisher vector @cite{perronnin10improving}
 from the set of feature vectors with respect to the provided gaussian mixture model.
 The function can be applied to both @c float or @c double data types.
 The covariance matrices of individual gaussians are assumed to be diagonal.
 
 To get the fisher vector siply run the function (::vl_fisher_encode)
 with correct parameters.
 
 @section fisher-tech Technical details
 
 The fisher vector encoding @cite{perronnin10improving} is strongly bound to estimation of a gaussian
 mixture model (see @ref gmm.h). Using the obtained means, variances and
 weights of individual gaussians, one can encode a set of features with
 respect to the gaussian model using the fisher vector encoding technique.
 
 Having a set of @f$ D @f$ dimensional features @f$ x_1 ... x_N @f$,
 and a set of gaussian parameters
 @f$ \Sigma_1 ... \Sigma_K  @f$ (= covariance matrices of gaussians),
 @f$ \mu_1 ... \mu_K  @f$ (= means),
 @f$ \pi_1 ... \pi_K  @f$ (= weights),
 the components @f$ u_k @f$ and @f$ v_k @f$ of a final fisher vector
 are defined as:
 @f[
 u_k = {1 \over {N \sqrt{\pi_k}}} \sum_{i=1}^{N} { q_{i,k} \Sigma^{-{1 \over 2}} (x_i - \mu_k)}
 @f]
 @f[
 v_k = {1 \over {N \sqrt{2 \pi_k}}} \sum_{i=1}^{N} { q_{i,k} [ (x_i - \mu_k)^T \Sigma^{-1} (x_i - \mu_k) - 1 ] }
 @f]
 
 Where @f$ q_{i,k} @f$ is a soft assignment of data point @f$ x_i @f$ to cluster @f$ k @f$.
 
 The fisher vector of size @f$ 2KD @f$, which encodes
 the set of features is:
 @f[
 f = [ u_1^T, v_1^T, u_2^T, v_2^T, ... u_K^T, v_K^T ]
 @f]
 
 */

#include "fisher.h"
#include "mathop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VL_FISHER_INSTANTIATING

#endif

#ifdef VL_FISHER_INSTANTIATING

static void
VL_XCAT(_vl_fisher_encode_, SFX)
(TYPE const * data,
 TYPE const * means,
 TYPE const * sigmas,
 TYPE const * weights,
 TYPE * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  vl_size dim;
  vl_index i_cl, i_d;
  TYPE * invSigma;
  TYPE * sqrtInvSigma;
  TYPE * posteriors;
  TYPE * logSigmas;
  TYPE * logWeights;
  TYPE halfDimLog2Pi = (dimension/2.0)*log(2.0*VL_PI);
  
#if (FLT == VL_TYPE_FLOAT)
  VlFloatVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_f(VlDistanceMahal) ;
#else
  VlDoubleVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_d(VlDistanceMahal) ;
#endif
  
#if defined(_OPENMP)
  vl_size numThreads = vl_get_max_threads() ;
#else
  vl_size numThreads = 1 ;
#endif
  
  logSigmas = vl_malloc(sizeof(TYPE) * numClusters);
  logWeights = vl_malloc(sizeof(TYPE) * numClusters);
  invSigma = vl_malloc(dimension*sizeof(TYPE)*numClusters);
  sqrtInvSigma = vl_malloc(dimension*sizeof(TYPE)*numClusters);
  posteriors = vl_malloc(numData*numClusters*sizeof(TYPE));
  
  memset(enc, 0, sizeof(TYPE) * 2 * dimension * numClusters);
  
#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_cl,dim) num_threads(numThreads)
#endif
  for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
    TYPE logSigma = 0;
    logWeights[i_cl] = log(weights[i_cl]) ;
    
    for(dim = 0; dim < dimension; dim++) {
      logSigma += log(sigmas[i_cl*dimension + dim]);
      invSigma[i_cl*dimension + dim] = 1/(sigmas[i_cl*dimension + dim]);
      sqrtInvSigma[i_cl*dimension + dim] = sqrt(invSigma[i_cl*dimension + dim]);
    }
    
    logSigmas[i_cl] = logSigma;
  } /* end of parallel region */
  
#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_d,i_cl) num_threads(numThreads)
#endif
  for (i_d = 0 ; i_d < (signed)numData ; ++i_d) {
    TYPE clusterPosteriorsSum = 0;
    TYPE maxPosterior = (TYPE)(-VL_INFINITY_D);
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; i_cl++) {
      posteriors[i_cl * numData + i_d] = logWeights[i_cl];
      posteriors[i_cl * numData + i_d] -= halfDimLog2Pi;
      posteriors[i_cl * numData + i_d] -= 0.5*logSigmas[i_cl];
      posteriors[i_cl * numData + i_d] -= 0.5 * distFn (dimension,
                                                        data + i_d * dimension,
                                                        means + i_cl * dimension,
                                                        invSigma + i_cl * dimension);
      if(posteriors[i_cl * numData + i_d] > maxPosterior) {
        maxPosterior = posteriors[i_cl * numData + i_d];
      }
    }
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      posteriors[i_cl * numData + i_d] -= maxPosterior;
      posteriors[i_cl * numData + i_d] = exp(posteriors[i_cl * numData + i_d]);
      clusterPosteriorsSum += posteriors[i_cl * numData + i_d];
    }
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      posteriors[i_cl * numData + i_d] /= clusterPosteriorsSum;
    }
  }
  
  
#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_cl) num_threads(numThreads)
#endif
  for(i_cl = 0; i_cl < (signed)numClusters; ++ i_cl) {
    TYPE uprefix;
    TYPE vprefix;
    
    TYPE * uk = enc + i_cl*dimension ;
    TYPE * vk = enc + i_cl*dimension + numClusters*dimension ;
    
    uprefix = 1/(numData*sqrt(weights[i_cl]));
    vprefix = 1/(numData*sqrt(2*weights[i_cl]));
    
    for(i_d = 0; i_d < (signed)numData; i_d++) {
      for(dim = 0; dim < dimension; dim++) {
        TYPE diff = (data[i_d*dimension + dim]-means[i_cl*dimension + dim]);
        
        *(uk + dim) += posteriors[i_cl*numData+i_d] * sqrtInvSigma[i_cl*dimension + dim] * diff;
        *(vk + dim) += posteriors[i_cl*numData+i_d] * (invSigma[i_cl*dimension + dim] * diff * diff - 1);
      }
    }
    
    for(dim = 0; dim < dimension; dim++) {
      *(uk + dim) = *(uk + dim) * uprefix;
      *(vk + dim) = *(vk + dim) * vprefix;
    }
  }
  vl_free(invSigma);
  vl_free(sqrtInvSigma);
  vl_free(logSigmas);
  vl_free(logWeights);
  vl_free(posteriors);
}

/* VL_FISHER_INSTANTIATING */
#else

#ifndef __DOXYGEN__
#define FLT VL_TYPE_FLOAT
#define TYPE float
#define SFX f
#define VL_FISHER_INSTANTIATING
#include "fisher.c"

#define FLT VL_TYPE_DOUBLE
#define TYPE double
#define SFX d
#define VL_FISHER_INSTANTIATING
#include "fisher.c"
#endif

#endif

/* ================================================================ */
#ifndef VL_FISHER_INSTANTIATING

/** @brief Calculates fisher vector for a given feature set
 ** @param dataType the type of the input data (::VL_TYPE_DOUBLE or ::VL_TYPE_FLOAT)
 ** @param data set of features
 ** @param means gaussian mixture means
 ** @param sigmas diagonals of gaussian mixture covariance matrices
 ** @param weights weights of individual gaussians in the mixture
 ** @param enc output fisher vector
 ** @param dimension dimensionality of the data
 ** @param numData number of data vectors
 ** @param numClusters number of gaussians in the mixture
 ** @param multithreading set to use serial or parallel computation
 **/

VL_EXPORT void
vl_fisher_encode
(vl_type dataType,
 void const * data,
 void const * means,
 void const * sigmas,
 void const * weights,
 void * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  switch(dataType) {
    case VL_TYPE_FLOAT:
      _vl_fisher_encode_f
      ((float const *) data,
       (float const *) means,
       (float const *) sigmas,
       (float const *) weights,
       (float *) enc,
       dimension,
       numData,
       numClusters);
      break;
    case VL_TYPE_DOUBLE:
      _vl_fisher_encode_d
      ((double const *) data,
       (double const *) means,
       (double const *) sigmas,
       (double const *) weights,
       (double *) enc,
       dimension,
       numData,
       numClusters);
      break;
    default:
      abort();
  }
}

#endif

#undef SFX
#undef TYPE
#undef FLT
#undef VL_FISHER_INSTANTIATING
