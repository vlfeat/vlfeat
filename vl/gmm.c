/** @file gmm.c
 ** @brief GMM - Declaration
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/
//
/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page gmm Gaussian mixture model estimation
@author David Novotny
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref gmm.h implements expectaton maximization algorithm (@cite{Dempster77maximumlikelihood})
for estimation of a gaussian mixture model of input data. The code supports
@c float or @c double data types, is parallelized and
offers several numerical stability protections so it
could be run on large datasets.

For detailed information on GMMs and the Expecation Maximization algorithm,
feel free to see the @subpage gmm-tech section.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section gmm-starting Getting started
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

To use @ref gmm.h to learn clusters from some training data,
instantiate a ::VlGMM object, set the configuration parameters
and run the training code. For instance, to learn @c numClusters
soft assignments from @c numData vectors of dimension @c dimension
and storage type @c float using at most 100 EM iterations use:

@code
float * means ;
float * sigmas ;
float * weights ;
float * posteriors ;

double loglikelyhood ;


// create a new instance of a GMM object, compute with float storage type
gmm = vl_gmm_new (VL_TYPE_FLOAT) ;

// set the maximum number of EM iterations to 100
vl_gmm_set_max_num_iterations (gmm, 100) ;

// set the initialization to random selection
vl_gmm_set_initialization	(gmm,VlGMMRand);

// set parallelized computation
vl_gmm_set_multithreading (gmm,VlGMMParallel);


// ---- obtain the mixture ----
vl_gmm_cluster ( gmm, data, dimension, numData, numClusters);


// get means of estimated gaussians
means = vl_gmm_get_means(gmm);

// get covariance matrices of estimated gaussians
sigmas = vl_gmm_get_sigmas(gmm);

// get weights of estimated gaussians
weights = vl_gmm_get_weights(gmm);

// get loglikelyhood of the estimated mixture
loglikelyhood = vl_gmm_get_loglikelyhood(gmm) ;

// get soft assignments of data points to each cluster
posteriors = vl_gmm_get_posteriors(gmm) ;

@endcode

@note Due to the increased speed of computations, the covariance matrices of estimated gaussians are forced to be diagonal.

Here we use ::vl_gmm_get_loglikelyhood to get the final
loglikelyhood of estimated mixture,
::vl_gmm_get_means and ::vl_gmm_get_sigmas to obtain
means and diagonals of covariance matrices of estimated gaussians.
::vl_gmm_get_posteriors is used to get probabilities of point
correspondences to each of the gaussians (= soft assignments).

There are several initialization methods which affect the
convergence speed and quality (= achieved loglikelyhood)
of the estimated mixture. These methods are selected by
setting the right parameter to ::vl_gmm_set_initialization
function. In the following table are shown implemented initialization
methods and their corresponding parameters to ::vl_gmm_set_initialization
function.

Method                | VlGMMInitialization parameter           | Description
----------------------|-----------------------------------------|-----------------------------------------------
Random initialization | ::VlGMMRand                             | Random initialization of mixture parameters
KMeans                | ::VlGMMKMeans                           | Initialization of mixture parameters using ::VlKMeans
Custom                | ::VlGMMCustom                           | User specification of initial mixture parameters

Note that in the case of ::VlGMMKMeans initialization, the user
has also to specify the customized ::VlKMeans object, which will
be later used for the initialization (please see @ref kmeans page
to see how to correctly set up the object).

When a user wants to use the ::VlGMMCustom method, the initial means,
sigmas and weights have to be specified using the ::vl_gmm_set_means,
::vl_gmm_set_sigmas and ::vl_gmm_set_weights methods

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-starting-multithreading OpenMP multithreading support
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref gmm.h supports parallel and serial implementation of the
GMM estimation. To enable/disable the parallel EM algorithm computation
use ::vl_gmm_set_multithreading function.
All the parallel sections of the algorithm are implemented using
<a href="http://openmp.org">OpenMP</a>.

By default the ::vl_gmm_cluster function is using the maximum amount
of availible threads. To disable multithreading, run the
::vl_gmm_set_multithreading function with ::VlGMMSerial parameter.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-starting-sse SSE instructions
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The implemented algorithm also uses the SSE2 instructions as
an optimization.

*/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page gmm-tech GMM fundamentals
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

For fitting a probabilistic model, consisting of a mixture
of several gaussians on a given dataset, the expectation
maximization @cite{Dempster77maximumlikelihood}
 algorithm (EM) is one of the most widely used methods.
The applications vary from the construction of vocabularies of
visual words to speech recognition or medical image
processing.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-usage-em EM algorithm
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref The EM algorithm consists of two succesive steps which
cyclically repeated until convergence:

- <b>Expectation</b>. Estimates probabilities of memberships of each
point to each of the clusters using current parameters (means,
covariances and weights) of gaussians in the mixture.

- <b>Maximization</b>. Given the estimates of posterior probabilities
computed in the E step, means, covariances and weights
of individual gaussians are computed such that the lower bound
of the loglikelyhood of the data is maximized in the next iteration.


@section gmm-tech-details Technical details

A gaussian mixture model is a representation of data points
@f$ x_1, \dots, x_n \in \mathbb{R}^d @f$, as a mixture of
@f$ K @f$ multivariate gaussian distributions each with the
weight @f$ \pi_k @f$, covariance matrix @f$ \Sigma_k @f$ and
mean @f$ \mu_k @f$. The overall probability of a point in mixture
is defined as:

@f[
  p(x_i) = \sum_{k=1}^{K} { \pi_k p( x_i |\mu_k,\Sigma_k) }
@f]

To obtain a "good" mixture one would like to maximize the
data loglikelyhood computed as:

@f[
  \sum_{i=1}^{n} { log { \sum_{k=1}^{K} p(x_i) } }
@f]

Since maximization of this term is a complicated problem
it is proven that maximizing the term

@f[
  \sum_{i=1}^{n} { \sum_{k=1}^{K} { q_{i,k} {log(p(x_i | \mu_k,\Sigma_k)) \over q_{i,k} } } }
@f]

effectively maximizes the lower bound of the data loglikelyhood
The @f$ q_{i,k} @f$ is an estimate of posterior probability @f$ p(C_k|x_i) @f$
that @f$ x_i @f$ belongs to cluster (i.e. gaussian) @f$ C_k @f$ represented by
estimates of covariance matrix @f$ \Sigma_k @f$, mean @f$ \mu_k @f$ and
weight @f$ \pi_k @f$.

The expectation maximization algorithm maximizes the lower bound of
the data loglikelyhood in two cyclically repeating succesive steps:

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-tech-e Expectation
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The E step estimates the posteriors @f$ q_{i,k} @f$ using the
bayesan rule in the following equation:

@f[
 q_{i,k} = { { {\pi_k} {{(2 \pi)}^{-{K \over 2}}} {{| \Sigma_k |}^{-{1 \over 2}}} exp( {-{1 \over 2}} {(x_i - \mu_k)^T} {{ \Sigma_k }^{-1}} {(x_i - \mu_k)} ) } \over
              { \sum_{l=1}^{K} { {\pi_l} {(2 \pi)}^{-{K \over 2}} | \Sigma_l |^{-{1 \over 2}} exp( {-{1 \over 2}} {(x_i - \mu_l)^T} {{ \Sigma_l }^{-1}} {(x_i - \mu_l)} ) } } }
@f]

Since the exponent of the multivariate gaussian distribution could become
a very low negative number causing the @f$ exp @f$ function to underflow during
the computations, the @f$ q_{i,k} @f$ coefficients are at first computed in the
logarithm space. Also, as said above, the algorithm assumes that the
the covariance matrices are diagonal, so the computation
of the determinant of @f$ \Sigma_k @f$ reduces to computing the trace of
the matrix and the inversion of @f$ \Sigma_k @f$ could be obtained
by inverting the elements on the diagonal of the covariance matrix.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-tech-m Maximization
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The M step estimates the parameters of the gaussians given the
posterior probabilities computed in the E step. The parameters are
found such that they maximize the the term which expresses the lower
bound of the data log likelyhood thus giving these explicit formulas:

@f[
 \mu_k = { { \sum_{i=1}^n q_{i,k} x_{i} } \over { \sum_{i=1}^n q_{i,k} } }
@f]

@f[
 \Sigma_k = { { \sum_{i=1}^n { q_{i,k} (x_{i} - \mu_{k}) {(x_{i} - \mu_{k})}^T } } \over { \sum_{i=1}^n q_{i,k} } }
@f]

@f[
 \pi_k = { \sum_{i=1}^n { q_{i,k} } \over { \sum_{i=1}^n \sum_{k=1}^K q_{i,k} } }
@f]

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-tech-init Initialization algorithms
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Since the EM algorithm is a stochastic method, which converges to
one of the local minima of the criterion function, the speed of convergence
and also the quality of the achieved local minimum is highly dependent on
the initial configuration of the gaussians in the mixture.

@ref gmm.h supports the following cluster initialization algorithms:

- <b>Random data points</b> (::vl_gmm_rand_init_mixture)
  initializes the means from a random selection of the training data.
  The sigmas are initialized as the overall data covariance, weights
  of the gaussians are set equal and sum to one. This approach is the
  fastest, simplest and also the most likely to end in a bad local
  minimum.

- <b>Custom initialization</b> (::vl_gmm_custom_init_mixture)
  initialize the means, weights and variances according to the user input
  using funtions ::vl_gmm_set_means, ::vl_gmm_set_weights and
  ::vl_gmm_set_sigmas.

- <b>KMeans initialization</b> (::vl_gmm_kmeans_init_mixture)
  the means, covariances and weights are initialized as covariances, centers
  and relative masses of clusters obtained using the KMeans algorithm.
  A user has to specify a customized ::VlKMeans object using
  function ::vl_gmm_set_kmeans_init_object.
  If no ::VlKMeans object is specified at the start, a default object
  is used.

**/

#include "gmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#ifdef __SSE2__
  #include "mathop_sse2.h"
#endif

/* ================================================================ */
#ifndef VL_GMM_INSTANTIATING
#define VL_GAUSSIAN_PREFIX 2.506628275

/* ---------------------------------------------------------------- */
/*                                                      Constructor */
/* ---------------------------------------------------------------- */

#define VL_GMM_MIN_SIGMA 1e-6

/** ------------------------------------------------------------------
 ** @brief Create a new GMM object
 ** @param dataType type of data (::VL_TYPE_FLOAT or ::VL_TYPE_DOUBLE)
 ** @return new GMM object instance.
**/
VL_EXPORT VlGMM *
vl_gmm_new (vl_type dataType)
{

  VlGMM * self = vl_malloc(sizeof(VlGMM)) ;

  self->dataType = dataType;

  self->numClusters = 0;
  self->numData = 0;
  self->dimension = 0;
  self->initialization = VlGMMRand;

  self->multithreading = VlGMMParallel;

  self->verbosity = 0 ;
  self->maxNumIterations = 50;
  self->numRepetitions = 1;
  self->sigmaLowBound = 10e-6;

  self->weights = NULL ;
  self->sigmas = NULL ;
  self->means = NULL ;
  self->posteriors = NULL ;
  self->kmeansInit = NULL ;

  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Reset state
 **
 ** The function reset the state of the GMM object. It deletes
 ** any stored means, sigmas, weights and posterior probabilities
 ** releasing the corresponding memory. This
 ** cancels the effect of seeding or setting the means, sigmas and weights,
 ** but does not change the other configuration parameters.
 **/
VL_EXPORT void
vl_gmm_reset (VlGMM * self)
{
  self->numClusters = 0 ;
  self->dimension = 0 ;
  self->numData = 0 ;

  if (self->means) vl_free(self->means) ;
  if (self->sigmas) vl_free(self->sigmas) ;
  if (self->weights) vl_free(self->weights) ;
  if (self->posteriors) vl_free(self->posteriors) ;

  self->means = NULL ;
  self->sigmas = NULL ;
  self->weights = NULL ;
  self->posteriors = NULL ;
}

/** ------------------------------------------------------------------
 ** @brief Deletes a GMM object
 ** @param self GMM object instance.
 **
 ** The function deletes the GMM object instance created
 ** by ::vl_gmm_new.
 **/

VL_EXPORT void
vl_gmm_delete (VlGMM * self)
{
  if(self->means) vl_free(self->means);
  if(self->sigmas) vl_free(self->sigmas);
  if(self->weights) vl_free(self->weights);
  if(self->posteriors) vl_free(self->posteriors);
  vl_free(self);
}

/* ---------------------------------------------------------------- */
/* Instantiate shuffle algorithm */

#define VL_SHUFFLE_type vl_uindex
#define VL_SHUFFLE_prefix _vl_gmm
#include "shuffle-def.h"

/* #ifdef VL_GMM_INSTANTITATING */
#endif

/* ================================================================ */
#ifdef VL_GMM_INSTANTIATING

static void
VL_XCAT (_vl_gmm_print_,SFX)
(VlGMM* self)
{
  vl_size i_cl, dim;

//  VL_PRINT("posteriors:\n");
//  for(i_cl = 0; i_cl < self->numClusters; i_cl++){
//    for(i_d = 0; i_d < self->numData; i_d++){
//      VL_PRINT("%f ",*((TYPE*)self->posteriors + i_cl*self->numData + i_d));
//    }
//    VL_PRINT("\n");
//  }

  VL_PRINT("means:\n");
  for(i_cl = 0; i_cl < self->numClusters; i_cl++){
    for(dim = 0; dim < self->dimension; dim++){
      VL_PRINT("%f ",*((TYPE*)self->means + i_cl*self->dimension + dim));
    }
    VL_PRINT("\n");
  }

  VL_PRINT("sigmas:\n");
  for(i_cl = 0; i_cl < self->numClusters; i_cl++){
    for(dim = 0; dim < self->dimension; dim++){
      VL_PRINT("%f ",*((TYPE*)self->sigmas + i_cl*self->dimension + dim));
    }
    VL_PRINT("\n");
  }

  VL_PRINT("weights:\n");
  for(i_cl = 0; i_cl < self->numClusters; i_cl++){
      VL_PRINT("%f ",*((TYPE*)self->weights + i_cl));
  }
  VL_PRINT("\n");

}

/* ---------------------------------------------------------------- */
/*                                                 Copy constructor */
/* ---------------------------------------------------------------- */

static TYPE
VL_XCAT (_vl_gmm_pow2_,SFX)
(TYPE n)
{
  return (n*n);
}

static VlGMM *
VL_XCAT(_vl_gmm_new_copy_, SFX)
(VlGMM const * gmm)
{
  VlGMM * self = vl_gmm_new(gmm->dataType);

  self->dataType = gmm->dataType;
  self->dimension = gmm->dimension ;
  self->numClusters = gmm->numClusters;
  self->initialization = gmm->initialization;
  self->kmeansInit = gmm->kmeansInit;
  self->multithreading = gmm->multithreading;

  self->maxNumIterations = gmm->maxNumIterations;
  self->numRepetitions = gmm->numRepetitions;
  self->verbosity = gmm->verbosity;

  self->LL = gmm->LL;

  if (gmm->means) {
    memcpy(self->means, gmm->means, sizeof(TYPE)*gmm->numClusters*gmm->dimension);
  }
  if (gmm->sigmas) {
    memcpy(self->sigmas, gmm->sigmas, sizeof(TYPE)*gmm->numClusters*gmm->dimension);
  }
  if (gmm->posteriors) {
    memcpy(self->posteriors, gmm->posteriors, sizeof(TYPE)*gmm->numClusters*gmm->numData);
  }
  if (gmm->weights) {
    memcpy(self->weights, gmm->weights, sizeof(TYPE)*gmm->numClusters);
  }

  return self;
}

/* ---------------------------------------------------------------- */
/*                                Kmeans initialization of mixtures */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_gmm_compute_init_sigma_, SFX)
(VlGMM * self,
 TYPE const * data,
 TYPE * initSigma,
 vl_size dimension,
 vl_size numData)
{
  vl_size dim;
  vl_uindex i;

  TYPE * dataMean = vl_malloc(sizeof(TYPE)*dimension);

  memset(dataMean,0,sizeof(TYPE)*dimension);
  memset(initSigma,0,sizeof(TYPE)*dimension);

  /* find mean of the whole dataset */
  for(dim = 0; dim < dimension; dim++) {
    for(i = 0; i < numData; i++) {
      dataMean[dim] += data[i*dimension + dim];
    }
    dataMean[dim] /= numData;
  }

  /* compute variance of the whole dataset */
  for(dim = 0; dim < dimension; dim++) {
    for(i = 0; i < numData; i++) {
      initSigma[dim] += VL_XCAT (_vl_gmm_pow2_,SFX)
                        (data[i*self->dimension + dim] - dataMean[dim]);
    }
    initSigma[dim] /= numData-1;
  }

  vl_free(dataMean) ;

}

static void
VL_XCAT(_vl_gmm_compute_init_weights_, SFX)
(VlGMM * self,
 vl_size numClusters)
{
  vl_uindex k;
  TYPE initW = (TYPE)(1./numClusters);
  for (k = 0 ; k < numClusters ; ++ k) {
    *((TYPE*)self->weights + k) = initW;
  }
}

static void
VL_XCAT(_vl_gmm_kmeans_init_mixture_, SFX)
(VlGMM * self,
 TYPE const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  vl_size i_d, i_cl, dim;

  vl_uint32 * assignments = vl_malloc(sizeof(vl_uint32) * numData);
  vl_size * clusterMasses = vl_malloc(sizeof(vl_size) * numClusters);
  TYPE * initSigma = vl_malloc(sizeof(TYPE) * dimension);

  self->numData = numData;
  self->dimension = dimension ;
  self->numClusters = numClusters;

  self->weights = vl_malloc (sizeof(TYPE) * numClusters) ;
  self->sigmas = vl_malloc (sizeof(TYPE) * numClusters * dimension) ;
  self->means = vl_malloc (sizeof(TYPE) * numClusters * dimension) ;

  memset(clusterMasses,0,sizeof(vl_size) * numClusters);
  memset(self->sigmas,0,sizeof(TYPE) * numClusters * dimension);
  memset(self->means,0,sizeof(TYPE) * numClusters * dimension);

  VL_XCAT(_vl_gmm_compute_init_sigma_, SFX) (self, data, initSigma, dimension, numData);

  /* if user did not provide kmeans object, create a default one */
  if(self->kmeansInit == NULL) {
    vl_size ncomparisons = numData/4;
    vl_size niter = 5;
    vl_size ntrees = 1;
    vl_size nrepetitions = 1;
    VlKMeansAlgorithm algorithm = VlKMeansANN;
    VlKMeansInitialization initialization = VlKMeansRandomSelection;

    VlKMeans * kmeansInit = vl_kmeans_new(self->dataType,VlDistanceL2);

    vl_kmeans_set_multithreading (kmeansInit, self->multithreading);
    vl_kmeans_set_initialization(kmeansInit, initialization);
    vl_kmeans_set_max_num_iterations (kmeansInit, niter) ;
    vl_kmeans_set_max_num_comparisons (kmeansInit, ncomparisons) ;
    vl_kmeans_set_num_trees (kmeansInit, ntrees);
    vl_kmeans_set_algorithm (kmeansInit, algorithm);
    vl_kmeans_set_num_repetitions(kmeansInit, nrepetitions);
    vl_kmeans_set_verbosity	(kmeansInit, self->verbosity);

    self->kmeansInit = kmeansInit;
  }

  vl_kmeans_cluster(self->kmeansInit, data, dimension, numData, numClusters);

  vl_kmeans_quantize (self->kmeansInit, assignments, NULL, data, numData) ;

  // compute sigmas, means and weights
  for(i_d = 0; i_d < numData; i_d++) {
    clusterMasses[assignments[i_d]]++;
    for(dim = 0; dim < dimension; dim++) {
      *((TYPE*)self->means + assignments[i_d] * dimension + dim) += data[i_d*dimension + dim];
    }
  }


  for(i_cl = 0; i_cl < numClusters; i_cl++) {
    *((TYPE*)self->weights + i_cl) = (TYPE)clusterMasses[i_cl]/(TYPE)numData;
    for(dim = 0; dim < dimension; dim++) {
      *((TYPE*)self->means + i_cl*dimension+dim) /= (TYPE)clusterMasses[i_cl];
    }
  }

  for(i_d = 0; i_d < numData; i_d++) {
    for(dim = 0; dim < dimension; dim++) {

      TYPE diffsq = VL_XCAT (_vl_gmm_pow2_,SFX)
                               (data[i_d*dimension + dim] -
                                *((TYPE*)self->means + assignments[i_d]*dimension+dim));

      *((TYPE*)self->sigmas + assignments[i_d]*dimension + dim) += diffsq;
    }
  }

  for(i_cl = 0; i_cl < numClusters; i_cl++) {
    if(clusterMasses[i_cl] != 0){
      for(dim = 0; dim < dimension; dim++) {
        *((TYPE*)self->sigmas + i_cl*dimension + dim) /= (TYPE)clusterMasses[i_cl];
      }
    } else {
      for(dim = 0; dim < dimension; dim++) {
        *((TYPE*)self->sigmas + i_cl*dimension + dim) = initSigma[dim];
      }
    }
  }

  vl_free(clusterMasses);
  vl_free(assignments);

}

/* ---------------------------------------------------------------- */
/*                                Random initialization of mixtures */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_gmm_rand_init_mixture_, SFX)
(VlGMM * self,
 TYPE const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  vl_uindex i, j, k, dim;
  VlRand * rand;

  vl_uindex * perm;
  TYPE * distances;
  TYPE * initSigma;

#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(VlDistanceL1) ;
#else
  VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(VlDistanceL1) ;
#endif

  self->numData = numData;
  self->dimension = dimension ;
  self->numClusters = numClusters;

  rand = vl_get_rand () ;

  self->weights = vl_malloc (sizeof(TYPE) * self->numClusters) ;
  self->sigmas = vl_malloc (sizeof(TYPE) * numClusters * dimension) ;
  self->means = vl_malloc (sizeof(TYPE) * numClusters * dimension) ;
  initSigma = vl_malloc (sizeof(TYPE) * numClusters * dimension) ;

  VL_XCAT(_vl_gmm_compute_init_sigma_, SFX)
  (self, data,  initSigma, dimension, numData);

  /* initilaize weights of gaussians so they are equal and sum to one */
  VL_XCAT(_vl_gmm_compute_init_weights_, SFX)
  (self, numClusters);

  /* initialize diagonals of covariance matrices to data variance */
  for (k = 0 ; k < numClusters ; ++ k) {
    for(dim = 0; dim < dimension; dim++) {
      *((TYPE*)self->sigmas + k*dimension + dim) = initSigma[dim];
    }
  }

  perm = vl_malloc (sizeof(vl_uindex) * numData) ;
  distances = vl_malloc (sizeof(TYPE) * numClusters) ;

  /* get a random permutation of the data point */
  for (i = 0 ; i < numData ; ++i) perm[i] = i ;
  _vl_gmm_shuffle (perm, numData, rand) ;

  /* sample means randomly picking points from the clustered data */
  for (k = 0, i = 0 ; k < numClusters ; ++ i) {
    /* compare the next data point to all centers collected so far
     to detect duplicates (if there are enough left)
     */
    if (numClusters - k < numData - i) {
      vl_bool duplicateDetected = VL_FALSE;
      VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)(distances,
          dimension,
          data + dimension * perm[i], 1,
          (TYPE*)self->means, k,
          distFn) ;
      for (j = 0 ; j < k ; ++j) {
        duplicateDetected |= (distances[j] == 0) ;
      }
      if (duplicateDetected) continue ;
    }

    /* ok, it is not a duplicate so we can accept it! */
    memcpy ((TYPE*)self->means + dimension * k,
            data + dimension * perm[i],
            sizeof(TYPE) * dimension) ;
    k ++ ;
  }

  vl_free(initSigma) ;
  vl_free(distances) ;
  vl_free(perm) ;
}

/* ---------------------------------------------------------------- */
/*                                Random initialization of mixtures */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_gmm_custom_init_mixture_, SFX)
(VlGMM * self,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{

  self->numData = numData;
  self->dimension = dimension ;
  self->numClusters = numClusters;

  if( !self->means || !self->weights || !self->sigmas ){
    VL_PRINT("VlGMM: Error: Custom initialization set, however not all initial parameters (weights,means,sigmas) were initialized!\n");
    abort();
  }

}

/* ---------------------------------------------------------------- */
/*             Calculation of log(weight_j * G(x_i|sigma_j,mean_j)) */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_gmm_logmultigaussian_, SFX)
(TYPE * posterior,
 TYPE * mean,
 TYPE * sigma,
 TYPE * weight,
 TYPE const * datum,
 vl_size dimension
)
{

  vl_uindex dim;
  TYPE dist = 0;

  (*posterior) = log(*weight);

  (*posterior) -= (dimension/2.0)*log(2.0*VL_PI);

  for(dim = 0; dim < dimension; dim++) {
    (*posterior) -= 0.5*log(sigma[dim]);
  }

  for(dim = 0; dim < dimension; dim++) {
    dist += (VL_XCAT (_vl_gmm_pow2_,SFX) (datum[dim]-mean[dim])) / sigma[dim];
    (*posterior) -= (0.5 * VL_XCAT (_vl_gmm_pow2_,SFX) (datum[dim]-mean[dim])) / sigma[dim];
  }

}

/* ---------------------------------------------------------------- */
/*                                 Restarts zero-weighted gaussians */
/* ---------------------------------------------------------------- */

static vl_size
VL_XCAT(_vl_gmm_zero_weights_disposal_, SFX)
(VlGMM * self,
 TYPE * weights,
 TYPE * sigmas,
 TYPE * means)
{
  vl_size dimension = self->dimension;
  vl_size numClusters = self->numClusters;
  vl_uindex i_cl, i_cl2, d;
  vl_size zeroWNum = 0;
  vl_int8 * nullWeights = NULL;

  /* detect zero weigths */
  for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
    if(weights[i_cl] < 0.00001/numClusters) {
      if(!nullWeights) {
        nullWeights = vl_malloc(sizeof(vl_int8) * numClusters);
        memset(nullWeights,0,sizeof(vl_int8) * numClusters);
      }
      nullWeights[i_cl] = (vl_int8)1;
      zeroWNum++;
    }
  }

  /* restart zero-weighted gaussians */
  if(zeroWNum > 0) {
    for(i_cl = 0; i_cl< numClusters; i_cl++) {
      if(nullWeights[i_cl] == 1) {

        TYPE maxDimSigma = 0;
        TYPE maxl2 = 0;
        vl_uindex maxClusterDim = 0;
        vl_uindex maxCluster = 0;

        /* find cluster with largest l2 norm of its sigma diagonal */
        for(i_cl2 = 0; i_cl2 < numClusters; i_cl2++) {
          TYPE l2 = 0;
          if(i_cl2 == i_cl) {
            continue;
          }
          for(d = 0; d < dimension; d++) {
            l2 += sigmas[i_cl2*dimension + d] * sigmas[i_cl2*dimension + d];
          }
          if(l2 > maxl2) {
            maxCluster = i_cl2;
            maxl2 = l2;
          }
        }

        /* find the dimension of the largest variance of the largest gaussian */
        for(d = 0; d < dimension; d++) {
          if(sigmas[maxCluster * dimension + d] > maxDimSigma) {
            maxClusterDim = d;
            maxDimSigma = sigmas[maxCluster * dimension + d];
          }
        }

        /* split the largest gaussian in the falf of its largest dimension */
        for(d = 0; d < dimension; d++) {
          if(d == maxClusterDim) {
            means[i_cl*dimension + d] = means[maxCluster*dimension + d] - maxDimSigma;
            means[maxCluster*dimension + d] += maxDimSigma;
            sigmas[i_cl       * dimension + d] = maxDimSigma / 2;
            sigmas[maxCluster * dimension + d] = maxDimSigma / 2;
          } else {
            means[i_cl*dimension + d] = means[maxCluster*dimension + d];
            sigmas[i_cl*dimension + d] = sigmas[maxCluster*dimension + d];
          }
        }
      }
    }
    vl_free(nullWeights);
  }
  return zeroWNum;
}

/* ---------------------------------------------------------------- */
/*                                           EM - Maximization step */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_gmm_maximization_, SFX)
(VlGMM * self,
 TYPE * posteriors,
 TYPE * weights,
 TYPE * sigmas,
 TYPE * means,
 TYPE const * data,
 vl_size numData)
{
  vl_size numClusters = self->numClusters;
  vl_uindex i_d, i_cl;
  vl_size dim;
  vl_int t;
  vl_size lowSigmas = 0;
  int chunkSize = 1;
  TYPE posteriorSum = 0;
  TYPE * oldMeans;

  TYPE ** clusterPosteriorSums;
  TYPE ** chunkMeans;
  TYPE ** chunkSigmas;

  /* parallel computation consts*/
  vl_int numChunks = 1;

  switch(self->multithreading) {
    case(VlGMMParallel):
#if defined(_OPENMP)
      numChunks = (vl_int)vl_get_num_threads();
#else
      numChunks = 1;
#endif
      break;
    case(VlGMMSerial):
      numChunks = 1;
      break;
    default:
      VL_PRINT("Bad multithreading value.\n");
      abort();
  }

  oldMeans = vl_malloc(sizeof(TYPE) * self->dimension * numClusters);
  memcpy(oldMeans, means, sizeof(TYPE) * self->dimension * numClusters);

  memset(sigmas, 0, sizeof(TYPE) * numClusters * self->dimension);
  memset(means, 0, sizeof(TYPE) * self->dimension * numClusters);
  memset(weights, 0, sizeof(TYPE) * numClusters);

  /* chunks initialization */
  clusterPosteriorSums = vl_malloc(sizeof(TYPE*) * numChunks);
  chunkMeans = vl_malloc(sizeof(TYPE*) * numChunks);
  chunkSigmas = vl_malloc(sizeof(TYPE*) * numChunks);

  for(t = 0; t < numChunks; t++) {

    chunkMeans[t]  = vl_malloc(sizeof(TYPE) * self->dimension * numClusters);
    chunkSigmas[t] = vl_malloc(sizeof(TYPE) * self->dimension * numClusters);
    clusterPosteriorSums[t] = vl_malloc(sizeof(TYPE) * numClusters);

    memset(chunkMeans[t], 0, sizeof(TYPE) * self->dimension * numClusters);
    memset(chunkSigmas[t], 0, sizeof(TYPE) * self->dimension * numClusters);
    memset(clusterPosteriorSums[t], 0, sizeof(TYPE) * numClusters);

  }

//compute sigmas
#if defined(_OPENMP)
#pragma omp parallel for private(t,i_d,i_cl,dim) schedule(static,chunkSize) reduction(+:posteriorSum)
#endif
  for(t=0; t < numChunks; t++) {
    for (i_d = (vl_size)t ; i_d < numData ; i_d += numChunks) {
      for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {

        clusterPosteriorSums[t][i_cl] += posteriors[i_cl * numData + i_d];
        posteriorSum += posteriors[i_cl * numData + i_d];

#ifdef __SSE2__
        VL_XCAT(_vl_weighted_mean_sse2_, SFX)
        (self->dimension,
         chunkMeans[t] + i_cl * self->dimension,
         data + i_d * self->dimension,
         posteriors + i_cl*numData + i_d);

        VL_XCAT(_vl_weighted_sigma_sse2_, SFX)
        (self->dimension,
         chunkSigmas[t] + i_cl * self->dimension ,
         data + i_d * self->dimension,
         oldMeans + i_cl * self->dimension,
         posteriors + i_cl*numData + i_d);
#else
        for (dim = 0 ; dim < self->dimension ; ++dim) {
          TYPE diff = data[i_d * self->dimension + dim] - oldMeans[i_cl*self->dimension + dim];
          chunkSigmas[t][i_cl * self->dimension + dim] += posteriors[i_cl*numData + i_d] * diff*diff;
        }

        for (dim = 0 ; dim < self->dimension ; ++dim) {
          chunkMeans[t][self->dimension * i_cl + dim] += data[self->dimension * i_d + dim] * posteriors[i_cl * numData + i_d];
        }
#endif
      }
    }
  } /* end of parallel region */

  //accumulate after aprallel section
  for(t = 0; t < numChunks; t++) {
    for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
      weights[i_cl] += clusterPosteriorSums[t][i_cl];
      for (dim = 0 ; dim < self->dimension ; ++dim) {
        sigmas[i_cl * self->dimension + dim] += chunkSigmas[t][i_cl * self->dimension + dim];
        means[i_cl * self->dimension + dim]  += chunkMeans[t][i_cl * self->dimension + dim];
      }
    }
  }

  // normalize
  for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
    vl_bool low = VL_FALSE;
    for (dim = 0 ; dim < self->dimension ; ++dim) {
      sigmas[i_cl * self->dimension + dim] /= weights[i_cl];
      means[i_cl * self->dimension + dim]  /= weights[i_cl];
      if ( !(sigmas[i_cl * self->dimension + dim] > (TYPE) self->sigmaLowBound) ) {
        low = VL_TRUE;
        sigmas[i_cl * self->dimension + dim] = (TYPE) self->sigmaLowBound;
      }
    }
    if(low == VL_TRUE) {
      lowSigmas++;
    }
  }

  if(lowSigmas > 0 && self->verbosity == 1) {
    VL_PRINT("Detected %d low sigmas -> set to lower bound.\n",lowSigmas);
  }

  for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
    weights[i_cl] /= posteriorSum;
  }

  for(t = 0; t < numChunks; t++) {
    vl_free(clusterPosteriorSums[t]);
    vl_free(chunkMeans[t]);
    vl_free(chunkSigmas[t]);
  }

  vl_free(chunkMeans);
  vl_free(chunkSigmas);
  vl_free(clusterPosteriorSums);
  vl_free(oldMeans);
}

/* ---------------------------------------------------------------- */
/*                                            EM - Expectation step */
/* ---------------------------------------------------------------- */

static double
VL_XCAT(_vl_gmm_expectation_, SFX)
(VlGMM * self,
 TYPE * posteriors,
 TYPE * weights,
 TYPE * sigmas,
 TYPE * means,
 TYPE const * data,
 vl_size numData)
{
  vl_int numChunks = 1;
  vl_size numClusters = self->numClusters;
  vl_uindex i_d, i_cl;
  vl_size dim;
  double LL = 0;
  int chunkSize = 1;
  vl_int t;
  //TYPE halfDimLog2Pi = (self->dimension/2.0)*log(2.0*VL_PI);

  TYPE * logSigmas;
  TYPE * logWeights;
  TYPE * invSigmas;

#if (FLT == VL_TYPE_FLOAT)
  VlFloatVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_f(VlDistanceMahal) ;
#else
  VlDoubleVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_d(VlDistanceMahal) ;
#endif

  logSigmas = vl_malloc(sizeof(TYPE) * numClusters);
  invSigmas = vl_malloc(sizeof(TYPE) * numClusters * self->dimension);
  logWeights = vl_malloc(numClusters * sizeof(TYPE));

  /* parallel computation consts*/


  switch(self->multithreading) {
    case(VlGMMParallel):
#ifdef _OPENMP
      numChunks = (vl_int)vl_get_num_threads();
#else
      numChunks = 1;
#endif
      break;
    case(VlGMMSerial):
      numChunks = 1;
      break;
    default:
      VL_PRINT("Bad multithreading value.\n");
      abort();
  }

  //VL_XCAT(_vl_gmm_print_, SFX) (self);
  //getchar();


#if defined(_OPENMP)
#pragma omp parallel for private(t,i_cl,dim) schedule(static,chunkSize)
#endif
  for(t=0; t < numChunks; t++) {
    for (i_cl = (vl_size) t ; i_cl < numClusters ; i_cl += numChunks) {
      TYPE logSigma = 0;
      logWeights[i_cl] = log(weights[i_cl]);
      for(dim = 0; dim < self->dimension; dim++) {
        logSigma += log(sigmas[i_cl*self->dimension + dim]);
        invSigmas[i_cl*self->dimension + dim] = 1/sigmas[i_cl*self->dimension + dim];
      }
      logSigmas[i_cl] = logSigma;
    }
  } /* end of parallel region */

#if defined(_OPENMP)
#pragma omp parallel for private(t,i_cl,i_d) schedule(static,chunkSize) reduction(+:LL)
#endif
  for(t=0; t < numChunks; t++) {
    for (i_d = (vl_size) t ; i_d < numData ; i_d += numChunks) {
      TYPE clusterPosteriorsSum = 0;
      TYPE maxPosterior = (TYPE)(-VL_INFINITY_D);

      for (i_cl = 0 ; i_cl < numClusters ; i_cl++) {
        posteriors[i_cl * numData + i_d] = logWeights[i_cl];
        //posteriors[i_cl * numData + i_d] -= halfDimLog2Pi;
        posteriors[i_cl * numData + i_d] -= 0.5*logSigmas[i_cl];
        posteriors[i_cl * numData + i_d] -= 0.5 * distFn (self->dimension,
                                            data + i_d * self->dimension,
                                            means + i_cl * self->dimension,
                                            invSigmas + i_cl * self->dimension);
        if(posteriors[i_cl * numData + i_d] > maxPosterior) {
          maxPosterior = posteriors[i_cl * numData + i_d];
        }
      }

      for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
        posteriors[i_cl * numData + i_d] -= maxPosterior;
        posteriors[i_cl * numData + i_d] = exp(posteriors[i_cl * numData + i_d]);
        clusterPosteriorsSum += posteriors[i_cl * numData + i_d];
      }

      LL += (double)log(clusterPosteriorsSum)+maxPosterior;

      for (i_cl = 0 ; i_cl < numClusters ; ++i_cl) {
        posteriors[i_cl * numData + i_d] /= clusterPosteriorsSum;
      }
    }
  } /* end of parallel region */

  vl_free(logSigmas);
  vl_free(logWeights);
  vl_free(invSigmas);

  return LL;
}


/* ---------------------------------------------------------------- */
/*                               Expectation maximization algorithm */
/* ---------------------------------------------------------------- */

static double
VL_XCAT(_vl_gmm_EM_, SFX)
(VlGMM * self,
 TYPE * posteriors,
 TYPE * weights,
 TYPE * sigmas,
 TYPE * means,
 TYPE const * data,
 vl_size numData)
{
  vl_size restarted;
  vl_size i_cl,dim;
  double LL;

  for(i_cl = 0; i_cl < self->numClusters; i_cl++) {
    for(dim = 0; dim < self->dimension; dim++) {
      if ( !(sigmas[i_cl * self->dimension + dim] > (TYPE) self->sigmaLowBound) ) {
        sigmas[i_cl * self->dimension + dim] = (TYPE) self->sigmaLowBound;
      }
    }
  }

  LL = VL_XCAT(_vl_gmm_expectation_, SFX) (self,posteriors,weights,sigmas,means,data,numData);

  VL_XCAT(_vl_gmm_maximization_, SFX) (self,posteriors,weights,sigmas,means,data,numData);

  restarted = VL_XCAT(_vl_gmm_zero_weights_disposal_, SFX) (self, weights, sigmas, means);

  if(restarted > 0) {
    VL_PRINT("Warning: %d gaussians had to be restarted because of their zero weight.\n",restarted);
  }

  //VL_XCAT(_vl_gmm_print_, SFX)(self);
  //getchar();

  return LL;

}

/* ---------------------------------------------------------------- */
/*                                                    EM iterations */
/* ---------------------------------------------------------------- */

static double
VL_XCAT(_vl_gmm_get_mixture_, SFX)
(VlGMM * self,
 TYPE const * data,
 vl_size numData)
{
  vl_size iteration ;
  double previousLL = (TYPE)(-VL_INFINITY_D);
  double LL = (TYPE)(-VL_INFINITY_D);

  self->posteriors = (TYPE*)vl_malloc(sizeof(TYPE) * numData * self->numClusters);

#if ! defined(_OPENMP)
  if(self->multithreading == VlGMMParallel) {
    VL_PRINT("VlGMM: Warning: OpenMP not included, continuing with serial computation.")
  }
#endif

  for (  iteration = 0 ;
         1 ;
         ++ iteration) {
    double eps;

    /* assign data to cluters */
    LL = VL_XCAT(_vl_gmm_EM_, SFX)(self, self->posteriors, self->weights, self->sigmas, self->means, data, numData) ;

    if (self->verbosity) {
      VL_PRINTF("GMM: GMM-EM iter %d: loglikelyhood = %f\n", iteration, LL) ;
    }

    /* check termination conditions */
    if (iteration >= self->maxNumIterations) {
      if (self->verbosity) {
        VL_PRINTF("GMM: GMM-EM terminating because maximum number of iterations reached\n") ;
      }
      break ;
    }

    eps = vl_abs_d ((LL - previousLL) / (LL));
    if (iteration>0 && (eps < 0.00001)) {
      if (self->verbosity) {
        VL_PRINTF("GMM: GMM-EM terminating because the algorithm fully converged\n") ;
      }
      break ;
    }

    previousLL = LL;
  }

  return (double)LL;
}

/* VL_GMM_INSTANTIATING */
#else

#ifndef __DOXYGEN__
#define FLT VL_TYPE_FLOAT
#define TYPE float
#define SFX f
#define VL_GMM_INSTANTIATING
#include "gmm.c"

#define FLT VL_TYPE_DOUBLE
#define TYPE double
#define SFX d
#define VL_GMM_INSTANTIATING
#include "gmm.c"
#endif

/* VL_GMM_INSTANTIATING */
#endif

/* ================================================================ */
#ifndef VL_GMM_INSTANTIATING

/** ------------------------------------------------------------------
 ** @brief Create a new GMM object by copy
 ** @param gmm GMM object to copy.
 ** @return new copy.
 **/

VL_EXPORT VlGMM *
vl_gmm_new_copy (VlGMM const * gmm)
{
  switch(gmm->dataType) {
    case VL_TYPE_FLOAT:
      return _vl_gmm_new_copy_f (gmm);
      break;
    case VL_TYPE_DOUBLE:
      return _vl_gmm_new_copy_d (gmm);
      break;
    default:
      abort();
  }
}

/** ------------------------------------------------------------------
 ** @brief Initialize mixture before EM takes place using random initialization
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

VL_EXPORT void
vl_gmm_rand_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  switch (self->dataType) {

    case VL_TYPE_FLOAT :
      _vl_gmm_rand_init_mixture_f
      (self, (float const *)data, dimension, numData, numClusters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_gmm_rand_init_mixture_d
      (self, (double const *)data, dimension, numData, numClusters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Initialize mixture before EM takes place using kmeans
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

VL_EXPORT void
vl_gmm_kmeans_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  switch (self->dataType) {

    case VL_TYPE_FLOAT :
      _vl_gmm_kmeans_init_mixture_f
      (self, (float const *)data, dimension, numData, numClusters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_gmm_kmeans_init_mixture_d
      (self, (double const *)data, dimension, numData, numClusters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Initialize mixture before EM takes place using the custom parameters
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

VL_EXPORT void
vl_gmm_custom_init_mixture
(VlGMM * self,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  switch (self->dataType) {

    case VL_TYPE_FLOAT :
      _vl_gmm_custom_init_mixture_f
      (self, dimension, numData, numClusters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_gmm_custom_init_mixture_d
      (self, dimension, numData, numClusters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Initialize mixture before EM takes place
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

VL_EXPORT void
vl_gmm_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters)
{
  switch (self->initialization) {
    case VlGMMKMeans :
      vl_gmm_reset (self) ;
      vl_gmm_kmeans_init_mixture
      (self, data, dimension, numData, numClusters) ;
      break ;
    case VlGMMRand :
      vl_gmm_reset (self) ;
      vl_gmm_rand_init_mixture
      (self, data, dimension, numData, numClusters) ;
      break ;
    case VlGMMCustom:
      vl_gmm_custom_init_mixture
      (self, dimension, numData, numClusters) ;
      break;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Run GMM clustering - includes initialization and EM
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

VL_EXPORT double vl_gmm_cluster (VlGMM * self,
                                 void const * data,
                                 vl_size dimension,
                                 vl_size numData,
                                 vl_size numClusters)
{
  void * bestMeans = NULL;
  void * bestSigmas = NULL;
  void * bestPosteriors = NULL;
  double bestLL = -VL_INFINITY_D;
  vl_uindex repetition;

  self->numData = numData;
  self->numClusters = numClusters;


  for (repetition = 0 ; repetition < self->numRepetitions ; ++ repetition) {
    double LL ;
    double timeRef ;

    if (self->verbosity) {
      VL_PRINTF("GMM: repetition %d of %d\n", repetition + 1, self->numRepetitions) ;
    }

    timeRef = vl_get_cpu_time() ;

    vl_gmm_init_mixture (self, data, dimension, numData, numClusters) ;

    if (self->verbosity) {
      VL_PRINTF("GMM: GMM initialized in %.2f s\n",
                vl_get_cpu_time() - timeRef) ;
    }



    timeRef = vl_get_cpu_time () ;
    LL = vl_gmm_get_mixture (self, data, numData) ;

    if (self->verbosity) {
      VL_PRINTF("GMM: GMM terminated in %.2f s with loglikelyhood %f\n",
                vl_get_cpu_time() - timeRef, LL) ;
    }



    if (LL > bestLL || repetition == 0) {
      void * temp ;
      bestLL = LL;


      if (bestMeans == NULL) {
        bestMeans = vl_malloc(vl_get_type_size(self->dataType) *
                              self->dimension *
                              self->numClusters) ;
        bestSigmas = vl_malloc(vl_get_type_size(self->dataType) *
                               self->numClusters) ;
        bestPosteriors = vl_malloc(vl_get_type_size(self->dataType) *
                                   numData *
                                   self->numClusters) ;
      }

      /* swap buffers */
      temp = bestMeans ;
      bestMeans = self->means ;
      self->means = temp ;

      temp = bestSigmas ;
      bestSigmas = self->sigmas ;
      self->sigmas = temp ;

      temp = bestPosteriors ;
      bestPosteriors = self->posteriors ;
      self->posteriors = temp ;
    }
  }

  vl_free (self->posteriors) ;
  vl_free (self->means) ;
  vl_free (self->sigmas) ;
  self->posteriors = bestPosteriors ;
  self->means = bestMeans ;
  self->sigmas = bestSigmas ;
  self->LL = bestLL;
  return bestLL ;

}

/** ------------------------------------------------------------------
 ** @brief Invoke the EM algorithm.
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param numData number of data points.
 **/

VL_EXPORT double vl_gmm_get_mixture (VlGMM * self, void const * data, vl_size numData)
{
  switch (self->dataType) {
    case VL_TYPE_FLOAT:
      return
        _vl_gmm_get_mixture_f
        (self, (float const *)data, numData) ;

      break ;
    case VL_TYPE_DOUBLE:
      return
        _vl_gmm_get_mixture_d
        (self, (double const *)data, numData) ;
      break;
    default:
      abort() ;
  }

  return 0;
}

/** ------------------------------------------------------------------
 ** @brief Explicitly set the initial means for EM.
 ** @param self GMM object instance.
 ** @param means initial values of means.
 ** @param numClusters number of mean points.
 ** @param dimension dimensionality of the means.
 **/

VL_EXPORT void
vl_gmm_set_means (VlGMM * self, void * means, vl_size numClusters, vl_size dimension)
{
  vl_size typeSize = 4;
  self->dimension = dimension;
  self->numClusters = numClusters;

  switch(self->dataType) {
    case VL_TYPE_FLOAT:
      typeSize = sizeof(float);
      break;
    case VL_TYPE_DOUBLE:
      typeSize = sizeof(double);
      break;
    default:
      abort();
  }
  if(!self->means) {
    self->means = vl_malloc(numClusters*typeSize*dimension);
  }

  memcpy(self->means,means,numClusters*typeSize*dimension);
}

/** ------------------------------------------------------------------
 ** @brief Explicitly set the initial sigma diagonals for EM.
 ** @param self GMM object instance.
 ** @param sigmas initial values of covariance matrix diagonals.
 ** @param numClusters number of sigma matrices.
 ** @param dimension number of points on the diagonals of the covariance matrices.
 **/

VL_EXPORT void vl_gmm_set_sigmas (VlGMM * self, void * sigmas, vl_size numClusters, vl_size dimension)
{
  vl_size typeSize = 4;
  self->dimension = dimension;
  self->numClusters = numClusters;

  switch(self->dataType) {
    case VL_TYPE_FLOAT:
      typeSize = sizeof(float);
      break;
    case VL_TYPE_DOUBLE:
      typeSize = sizeof(double);
      break;
    default:
      abort();
  }
  if(!self->sigmas) {
    self->sigmas = vl_malloc(numClusters*typeSize*dimension);
  }

  memcpy(self->sigmas,sigmas,numClusters*typeSize*dimension);

}

/** ------------------------------------------------------------------
 ** @brief Explicitly set the initial weights of the gaussians.
 ** @param self GMM object instance.
 ** @param weights initial values of the gaussian weights.
 ** @param numClusters number of gaussians.
 **/

VL_EXPORT void vl_gmm_set_weights (VlGMM * self, void * weights, vl_size numClusters)
{
  vl_size typeSize = 4;
  self->numClusters = numClusters;

  switch(self->dataType) {
    case VL_TYPE_FLOAT:
      typeSize = sizeof(float);
      break;
    case VL_TYPE_DOUBLE:
      typeSize = sizeof(double);
      break;
    default:
      abort();
  }

  if(!self->weights) {
    self->weights = vl_malloc(numClusters*typeSize);
  }

  memcpy(self->weights,weights,numClusters*typeSize);
}

/** ------------------------------------------------------------------
 ** @brief Explicitly set the posterior probabilities.
 ** @param self GMM object instance.
 ** @param posteriors initial values of the posterior probabilities.
 ** @param numClusters number of gaussians.
 ** @param numClusters number of data points.
 **/

VL_EXPORT void vl_gmm_set_posteriors (VlGMM * self, void * posteriors, vl_size numClusters, vl_size numData)
{
  vl_size typeSize = 4;
  self->numClusters = numClusters;
  self->numData = numData;

  switch(self->dataType) {
    case VL_TYPE_FLOAT:
      typeSize = sizeof(float);
      break;
    case VL_TYPE_DOUBLE:
      typeSize = sizeof(double);
      break;
    default:
      abort();
  }
  memcpy(self->posteriors,posteriors,numClusters*typeSize*numData);
}




/* VL_GMM_INSTANTIATING */
#endif

#undef SFX
#undef TYPE
#undef FLT
#undef VL_GMM_INSTANTIATING

