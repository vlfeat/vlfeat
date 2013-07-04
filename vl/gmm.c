/** @file gmm.c
 ** @brief Gaussian Mixture Models - Implementation
 ** @author David Novotny
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 David Novotny and Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page gmm Gaussian Mixture Models (GMM)
@author David Novotny
@author Andrea Vedaldi
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref gmm.h is an implementation of *Gaussian Mixture Models* (GMMs).
The main functionality provided by this module is learning GMMs from data.
Learning maximises the likelihood of a GMM using the
 expectation maximization
@cite{dempster77maximum}). The implementation supports @c float or @c
double data types, is parallelized, and is tuned to work reliably and
effectively on datasets of visual features. Stability is obtained in
part by regularizing and restricting the parameters of the GMM.

GMMs are at the basis of the computation of other useful features,
such as the Fisher vectors (@ref fisher).

To get started with the C API see @ref gmm-starting; for detailed
information on GMMs, their learning with the EM algorithm, and other
details of VLFeat implementation see @subpage gmm-fundamentals.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section gmm-starting Getting started
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

In order to use @ref gmm.h to learn a GMM from training data,
instantiate a ::VlGMM object, set the parameters as desired, and run
the training code. The following example learns @c numClusters
Gaussian components from @c numData vectors of dimension @c dimension
and storage class @c float using at most 100 EM iterations:

@code
float * means ;
float * sigmas ;
float * weights ;
float * posteriors ;

double loglikelihood ;

// create a new instance of a GMM object for float data
gmm = vl_gmm_new (VL_TYPE_FLOAT) ;

// set the maximum number of EM iterations to 100
vl_gmm_set_max_num_iterations (gmm, 100) ;

// set the initialization to random selection
vl_gmm_set_initialization (gmm,VlGMMRand);

// set parallelized computation
vl_gmm_set_multithreading (gmm,VlGMMParallel);

// cluster the data, i.e. learn the GMM
vl_gmm_cluster (gmm, data, dimension, numData, numClusters);

// get the means, covariances, and weights of the GMM
means = vl_gmm_get_means(gmm);
sigmas = vl_gmm_get_sigmas(gmm);
weights = vl_gmm_get_priors(gmm);

// get loglikelihood of the estimated GMM
loglikelihood = vl_gmm_get_loglikelihood(gmm) ;

// get the soft assignments of the data points to each cluster
posteriors = vl_gmm_get_posteriors(gmm) ;
@endcode

@note ::VlGMM assumes that the covariance matrices of the GMM are
diagonal. This reduces significantly the number of parameters to learn
and is usually an acceptable assumption. If the data is significantly
correlated, it can be beneficial to de-correlate it by PCA rotation or
projection in pre-processing.

::vl_gmm_get_loglikelihood is used to get the final loglikelihood of
the estimated mixture, ::vl_gmm_get_means and ::vl_gmm_get_sigmas to
obtain the means and the diagonals of the covariance matrices of the
estimated Gaussian modes, and ::vl_gmm_get_posteriors to get the
posterior probabilities that a given point is associated to each of
the modes (soft assignments).

The learning algorithm, which is an implementation of EM, finds a
local optimum of the objective function. Therefore the initialization
is crucial in obtaining a good model, measured in term of the final
loglikelihood. ::VlGMM supports a few methods (use
::vl_gmm_set_initialization to choose one) as follows:

Method                | ::VlGMMInitialization enumeration       | Description
----------------------|-----------------------------------------|-----------------------------------------------
Random initialization | ::VlGMMRand                             | Random initialization of the mixture parameters
KMeans                | ::VlGMMKMeans                           | Initialization of the mixture parameters using ::VlKMeans
Custom                | ::VlGMMCustom                           | User specified initialization

Note that in the case of ::VlGMMKMeans initialization, an object of
type ::VlKMeans object must be created and passed to the ::VlGMM
instance (see @ref kmeans to see how to correctly set up this object).

When a user wants to use the ::VlGMMCustom method, the initial means,
sigmas and weights have to be specified using the ::vl_gmm_set_means,
::vl_gmm_set_sigmas and ::vl_gmm_set_weights methods.

::VlGMM supports multi-core computations. This can controlled by using
::vl_gmm_set_multithreading function. Furthermore, the implementation
uses SSE2 vector instructions when available.

**/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page gmm-fundamentals GMM fundamentals
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

A *Gaussian Mixture Model* (GMM) is a mixture of $K$ multivariate
Gaussian distributions. In order to sample from a GMM, one samples
first the component index $k \in \{1,\dots,K\}$ with probability
$\pi_k$, and then samples the vector $\bx \in \mathbb{R}^d$ from the
$k$-th Gaussian distribution $p(\bx|\mu_k,\Sigma_k)$. Here $\mu_k$ and
$\Sigma_k$ are respectively the mean and covariance matrix of the
distribution. The GMM is completely defined by the parameters
$\Theta=\{\pi_k,\mu_k,\Sigma_k; k = 1,\dots,K\}$

Marginalizing $k$ yields the probability density
\[
p(\bx|\Theta)
= \sum_{k=1}^{K} \pi_k p( \bx_i |\mu_k,\Sigma_k),
\qquad
p( \bx |\mu_k,\Sigma_k)
=
\frac{1}{\sqrt{(2\pi)^d\det\Sigma_k}}
\exp\left[
-\frac{1}{2} (\bx-\mu_k)^\top\Sigma_k^{-1}(\bx-\mu_k)
\right]
\]
Learning a GMM to fit a dataset $X=(\bx_1, \dots, \bx_n)$ is usually
done by maximizing the log-likelihood of the data:
@f[
 \ell(\Theta;X)=
 E_{\bx\sim\hat p} \log p(x|\Theta)
 = \frac{1}{n}\sum_{i=1}^{n} \log \sum_{k=1}^{K} \pi_k p(\bx_i|\mu_k, \Sigma_k).
@f]
where $\hat p$ is the empirical distribution of the data.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section gmm-em Learning a GMM by expectation maximization
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The direct maximization of the log-likelihood function of a GMM is
difficult due to the fact that the assignments of points to Gaussian
mode is not observable and, as such, must be treated as a latent
variable.

Usually, GMMs are learned by using the *Expectation Maximization* (EM)
algorithm @cite{dempster77maximum}. Consider in general the problem of
estimating to the maximum likelihood a distribution $p(x|\Theta) =
\int p(x,h|\Theta)\,dh$, where $x$ is a measurement, $h$ is a *latent
variable*, and $\Theta$ are the model parameters. By introducing an
auxiliary distribution $q(h|x)$ on the latent variable, one can use
Jensen inequality to obtain the following lower bound on the
log-likelihood:

@f{align*}
\ell(\Theta;X) =
E_{x\sim\hat p} \log p(x|\Theta)
&= E_{x\sim\hat p} \log \int p(x,h|\Theta) \,dh \\
&= E_{x\sim\hat p} \log \int \frac{p(x,h|\Theta)}{q(h|x)} q(h|x)\,dh \\
&\geq E_{x\sim\hat p} \int q(h) \log \frac{p(x,h|\Theta)}{q(h|x)}\,dh \\
&= E_{(x,q) \sim q(h|x) \hat p(x)} \log p(x,h|\Theta) -
   E_{(x,q) \sim q(h|x) \hat p(x)} \log q(h|x)
@f}

The first term of the last expression is the log-likelihood of the
model where both the $x$ and $h$ are observed and distributed as
$q(x|h)\hat p(x)$; the second term is the a average entropy of the
latent variable and does not depend on $\Theta$.  This lower bound is
maximized and becomes tight by setting $q(h|x) = p(h|x,\Theta)$ to be
the posterior distribution on the latent variable $h$ (given the
current estimate of the parameters $\Theta$). In fact:

\[
E_{(x,h) \sim p(h|x,\Theta) \hat p(x)}\left[ \log \frac{p(x,h|\Theta)}{p(h|x,\Theta)}\right]
=
E_{(x,h) \sim p(h|x,\Theta) \hat p(x)\log p(x|\Theta)
=
E_{x\sim\hat p} \log p(x|\theta)
=
\ell(\Theta;X).
\]

EM alternates between updating the latent variable auxiliary
distribution $q(h|x) = p(h|x,\Theta_t)$ (*expectation step*) given the
current estimate of the parameters $\Theta_t$, and then updating the
model parameters $\Theta_{t+1}$ by maximizing the log-likelihood lower
bound derived (*maximization step*). The simplification is that in the
maximization step both $x$ and $h$ are now ``observed'' quantities.
This procedure converges to a local optimum of the model
log-likelihood.

@par "Expectation step"

In the case of a GMM, the latent variables are the point-to-cluster
assignments $k_i$, one for each of $n$ data points. The auxiliary
distribution $q(k_i|\bx_i) = q_{ik}$ is a matrix with $n \times K$
entries. Each row $q_{i,:}$ can be thought of as a vector of soft
assignments of the data points $\bx_i$ to each of the Gaussian
modes. Setting $q_{ik} = p(k_i | \bx_i, \Theta)$ yields

\[
 q_{ik} =
\frac
{\pi_k p(\bx_i|\mu_k,\Sigma_k)}
{\sum_{l=1}^K \pi_l p(\bx_i|\mu_l,\Sigma_l)}
\]

where the Gaussian density $p(\bx_i|\mu_k,\Sigma_k)$ was given above.

One important point to keep in mind when these probabilities are
computed is the fact that the Gaussian densities may attain very low
values and underflow in a vanilla implementation. Furthermore, VLFeat
GMM implementation restricts the covariance matrices to be
diagonal. In this case, the computation of the determinant of
$\Sigma_k$ reduces to computing the trace of the matrix and the
inversion of $\Sigma_k$ could be obtained by inverting the elements on
the diagonal of the covariance matrix.

@par "Maximization step"

The M step estimates the parameters of the Gaussian mixture components
and the prior probabilities $\pi_k$ given the auxiliary distribution
on the point-to-cluster assignments computed in the E step. Since all
the variables are now ``observed'', the estimate is quite simple. For
example, the mean $\mu_k$ of a Gaussian mode is obtained as the mean
of the data points assigned to it (accounting for the strength of the
soft assignments). The other quantities are obtained in a similar
manner, yielding to:

@f{align*}
 \mu_k &= { { \sum_{i=1}^n q_{i,k} \bx_{i} } \over { \sum_{i=1}^n q_{i,k} } },
\\
 \Sigma_k &= { { \sum_{i=1}^n { q_{i,k} (\bx_{i} - \mu_{k}) {(\bx_{i} - \mu_{k})}^T } } \over { \sum_{i=1}^n q_{i,k} } },
\\
 \pi_k &= { \sum_{i=1}^n { q_{i,k} } \over { \sum_{i=1}^n \sum_{k=1}^K q_{i,k} } }.
@f}

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection gmm-fundamentals-init Initialization algorithms
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The EM algorithm is a local optimization method. As such, the quality
of the solution strongly depends on the quality of the initial values
of the parameters (i.e.  of the locations and shapes of the Gaussian
modes).

@ref gmm.h supports the following cluster initialization algorithms:

- <b>Random data points.</b> (::vl_gmm_rand_init_mixture) This method
  initializes the means of the modes by sampling at random an equal
  number of data points, the covariance matrices of all the modes are
  set to be equal to the covariance of the entire dataset, and the
  prior probabilities of the Gaussian modes are initialized to be
  uniform. This initialization method is the fastest, simplest, as
  well as the one most likely to end in a bad local minimum.

- <b>Custom initialization</b> (::vl_gmm_custom_init_mixture) This
  allows the user choose the initial values of the means, the
  covariances, and the prior probabilities of the GMM. In order to set
  these parameters, use the functions ::vl_gmm_set_means,
  ::vl_gmm_set_weights, and ::vl_gmm_set_sigmas.

- <b>KMeans initialization</b> (::vl_gmm_kmeans_init_mixture) This
  method uses KMeans to pre-cluster the points. It then sets the means
  and covariances of the Gaussian distributions the sample means and
  covariances of each KMeans cluster. It also sets the prior
  probabilities to be proportional to the mass of each cluster. In
  order to use this initialization method, a user can specify an
  instance of ::VlKMeans by using the function
  ::vl_gmm_set_kmeans_init_object, or let ::VlGMM create one
  automatically.

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

/* ---------------------------------------------------------------- */
#ifndef VL_GMM_INSTANTIATING
/* ---------------------------------------------------------------- */

#define VL_GAUSSIAN_PREFIX 2.506628275
#define VL_GMM_MIN_SIGMA 1e-6

struct _VlGMM
{
  vl_type dataType ;                  /**< Data type. */
  vl_size dimension ;                 /**< Data dimensionality. */
  vl_size numClusters ;               /**< Number of clusters  */
  vl_size numData ;                   /**< Number of last time clustered data points.  */
  vl_size maxNumIterations ;          /**< Maximum number of refinement iterations. */
  vl_size numRepetitions   ;          /**< Number of clustering repetitions. */
  int     verbosity ;                 /**< Verbosity level. */
  void *  means;                      /**< Means of Gaussian modes. */
  void *  sigmas;                     /**< Diagonals of covariance matrices of Gaussian modes. */
  void *  weights;                    /**< Weights of Gaussian modes. */
  void *  posteriors;                 /**< Probabilities of correspondences of points to clusters. */
  double sigmaLowBound ;              /**< Lower bound on the diagonal covariance values. */
  VlGMMInitialization initialization; /**< Initialization option */
  VlKMeans * kmeansInit;              /**< Kmeans object for initialization of gaussians */
  double LL ;                         /**< current solution loglikelihood */
} ;

/* ---------------------------------------------------------------- */
/*                                                       Life-cycle */
/* ---------------------------------------------------------------- */

/** @brief Create a new GMM object
 ** @param dataType type of data (::VL_TYPE_FLOAT or ::VL_TYPE_DOUBLE)
 ** @return new GMM object instance.
 **/

VlGMM *
vl_gmm_new (vl_type dataType)
{

  VlGMM * self = vl_calloc(1, sizeof(VlGMM)) ;
  self->dataType = dataType;
  self->numClusters = 0;
  self->numData = 0;
  self->dimension = 0;
  self->initialization = VlGMMRand;
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

/** @brief Reset state
 ** @param self object.
 **
 ** The function reset the state of the GMM object. It deletes
 ** any stored means, sigmas, weights and posterior probabilities
 ** releasing the corresponding memory. This
 ** cancels the effect of seeding or setting the means, sigmas and weights,
 ** but does not change the other configuration parameters.
 **/

void
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

/** @brief Deletes a GMM object
 ** @param self GMM object instance.
 **
 ** The function deletes the GMM object instance created
 ** by ::vl_gmm_new.
 **/

void
vl_gmm_delete (VlGMM * self)
{
  if(self->means) vl_free(self->means);
  if(self->sigmas) vl_free(self->sigmas);
  if(self->weights) vl_free(self->weights);
  if(self->posteriors) vl_free(self->posteriors);
  vl_free(self);
}

/* ---------------------------------------------------------------- */
/*                                              Getters and setters */
/* ---------------------------------------------------------------- */

/** @brief Get data type
 ** @param self object
 ** @return data type.
 **/

vl_type
vl_gmm_get_data_type (VlGMM const * self)
{
  return self->dataType ;
}

/** @brief Get the number of clusters
 ** @param self object
 ** @return number of clusters.
 **/

vl_size
vl_gmm_get_num_clusters (VlGMM const * self)
{
  return self->numClusters ;
}

/** @brief Get the number of data points
 ** @param self object
 ** @return number of data points.
 **/

vl_size
vl_gmm_get_num_data (VlGMM const * self)
{
  return self->numData ;
}

/** @brief Set the number of clusters
 ** @param self object
 ** @param numClusters number of clusters..
 **/

void
vl_gmm_set_num_clusters (VlGMM * self, vl_size numClusters)
{
  self->numClusters = numClusters;
}

/** @brief Get the log likelihood of the current mixture
 ** @param self object
 ** @return loglikelihood.
 **/

double
vl_gmm_get_loglikelihood (VlGMM const * self)
{
  return self->LL ;
}

/** @brief Get verbosity level
 ** @param self object
 ** @return verbosity level.
 **/

int
vl_gmm_get_verbosity (VlGMM const * self)
{
  return self->verbosity ;
}

/** @brief Set verbosity level
 ** @param self object
 ** @param verbosity verbosity level.
 **/

void
vl_gmm_set_verbosity (VlGMM * self, int verbosity)
{
  self->verbosity = verbosity ;
}

/** @brief Get means
 ** @param self object
 ** @return cluster means.
 **/

void const *
vl_gmm_get_means (VlGMM const * self)
{
  return self->means ;
}

/** @brief Get sigmas
 ** @param self object
 ** @return diagonals of cluster covariance matrices.
 **/

void const *
vl_gmm_get_sigmas (VlGMM const * self)
{
  return self->sigmas ;
}

/** @brief Get weights
 ** @param self object
 ** @return weights of cluster gaussians.
 **/

void const *
vl_gmm_get_priors (VlGMM const * self)
{
  return self->weights ;
}

/** @brief Get posteriors
 ** @param self object
 ** @return posterior probabilities of cluster memberships.
 **/

void const *
vl_gmm_get_posteriors (VlGMM const * self)
{
  return self->posteriors ;
}

/** @brief Get maximum number of iterations
 ** @param self object
 ** @return maximum number of iterations.
 **/

vl_size
vl_gmm_get_max_num_iterations (VlGMM const * self)
{
  return self->maxNumIterations ;
}

/** @brief Set maximum number of iterations
 ** @param self VlGMM filter.
 ** @param maxNumIterations maximum number of iterations.
 **/

void
vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations)
{
  self->maxNumIterations = maxNumIterations ;
}

/** @brief Get maximum number of repetitions.
 ** @param self object
 ** @return current number of repretitions for quantization.
 **/

vl_size
vl_gmm_get_num_repetitions (VlGMM const * self)
{
  return self->numRepetitions ;
}

/** @brief Set maximum number of repetitions
 ** @param self object
 ** @param numRepetitions maximum number of repetitions.
 ** The number of repetitions cannot be smaller than 1.
 **/

void
vl_gmm_set_num_repetitions (VlGMM * self, vl_size numRepetitions)
{
  assert (numRepetitions >= 1) ;
  self->numRepetitions = numRepetitions ;
}

/** @brief Get data dimension
 ** @param self object
 ** @return data dimension.
 **/

vl_size
vl_gmm_get_dimension (VlGMM const * self)
{
  return self->dimension ;
}

/** @brief Set dimensionality of the data.
 ** @param self object
 ** @param dimension dimensionality of the data.
 ** The number of repetitions cannot be smaller than 1.
 **/
void vl_gmm_set_dimension (VlGMM * self, vl_size dimension)
{
  assert(dimension >= 1);
  self->dimension = dimension;
}

/** @brief Get initialization algorithm
 ** @param self object
 ** @return initialization algorithm.
 **/

VlGMMInitialization
vl_gmm_get_initialization (VlGMM const * self)
{
  return self->initialization ;
}

/** @brief Set initialization algorithm.
 ** @param self object
 ** @param init initialization algorithm.
 **/
void
vl_gmm_set_initialization (VlGMM * self, VlGMMInitialization init)
{
  self->initialization = init;
}

/** @brief Get KMeans initialization object.
 ** @param self object
 ** @return kmeans initialization object.
 **/
VlKMeans * vl_gmm_get_kmeans_init_object (VlGMM const * self)
{
  return self->kmeansInit;
}

/** @brief Set KMeans initialization object.
 ** @param self object
 ** @param kmeans initialization KMeans object.
 **/
void vl_gmm_set_kmeans_init_object (VlGMM * self, VlKMeans * kmeans)
{
  if(self->kmeansInit)
  {
    vl_kmeans_delete(self->kmeansInit);
  }
  self->kmeansInit = kmeans;
}

/** @brief Get lower bound on sigma diagonals.
 ** @param self object
 ** @return lower bound on sigmas.
 **/
double vl_gmm_get_sigma_lower_bound (VlGMM * self)
{
  return self->sigmaLowBound;
}

/** @brief Set lower bound on sigma diagonals.
 ** @param self object
 ** @param lowbound lower bound on sigma diagonals.
 **/
void vl_gmm_set_sigma_lower_bound (VlGMM * self, double lowbound)
{
  self->sigmaLowBound = lowbound;
}

/* ---------------------------------------------------------------- */
/* Instantiate shuffle algorithm */

#define VL_SHUFFLE_type vl_uindex
#define VL_SHUFFLE_prefix _vl_gmm
#include "shuffle-def.h"

/* #ifdef VL_GMM_INSTANTITATING */
#endif

/* ---------------------------------------------------------------- */
#ifdef VL_GMM_INSTANTIATING
/* ---------------------------------------------------------------- */


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

  VL_PRINT("covariances:\n");
  for(i_cl = 0; i_cl < self->numClusters; i_cl++){
    for(dim = 0; dim < self->dimension; dim++){
      VL_PRINT("%f ",*((TYPE*)self->sigmas + i_cl*self->dimension + dim));
    }
    VL_PRINT("\n");
  }

  VL_PRINT("priors:\n");
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
  self->maxNumIterations = gmm->maxNumIterations;
  self->numRepetitions = gmm->numRepetitions;
  self->verbosity = gmm->verbosity;
  self->LL = gmm->LL;

  /* TODO: check if this memory is allocated properly */
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

  if(!self->means || !self->weights || !self->sigmas){
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
  vl_index i_d, i_cl;
  vl_size dim ;
  vl_size lowSigmas = 0;
  TYPE posteriorSum = 0;
  TYPE * oldMeans;

  oldMeans = vl_malloc(sizeof(TYPE) * self->dimension * numClusters);
  memcpy(oldMeans, means, sizeof(TYPE) * self->dimension * numClusters);

  memset(sigmas, 0, sizeof(TYPE) * numClusters * self->dimension);
  memset(means, 0, sizeof(TYPE) * self->dimension * numClusters);
  memset(weights, 0, sizeof(TYPE) * numClusters);

  /* compute covariance */
#if defined(_OPENMP)
#pragma omp parallel default(shared) private(i_d, i_cl, dim) \
                     reduction(+:posteriorSum) \
                     num_threads(vl_get_max_threads())
#endif
  {
    TYPE * clusterPosteriorSum_, * means_, * sigmas_ ;

#if defined(_OPENMP)
#pragma omp critical
#endif
    {
      clusterPosteriorSum_ = vl_calloc(sizeof(TYPE), numClusters) ;
      means_ = vl_calloc(sizeof(TYPE), self->dimension * numClusters) ;
      sigmas_ = vl_calloc(sizeof(TYPE), self->dimension * numClusters) ;
    }

#if defined(_OPENMP)
#pragma omp for
#endif
    for (i_d = 0 ; i_d < (signed)numData ; ++i_d) {
      for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
        
        clusterPosteriorSum_ [i_cl] += posteriors[i_cl * numData + i_d] ;
        posteriorSum += posteriors[i_cl * numData + i_d] ;
        
        /* TODO: this section must be conditional on the architecture at run time */
#ifdef __AVX__
        VL_XCAT(_vl_weighted_mean_avx_, SFX)
        (self->dimension,
         means_ + i_cl * self->dimension,
         data + i_d * self->dimension,
         posteriors + i_cl * numData + i_d);
        
        VL_XCAT(_vl_weighted_sigma_avx_, SFX)
        (self->dimension,
         sigmas_ + i_cl * self->dimension ,
         data + i_d * self->dimension,
         oldMeans + i_cl * self->dimension,
         posteriors + i_cl*numData + i_d);
#else
#ifdef __SSE2__
        VL_XCAT(_vl_weighted_mean_sse2_, SFX)
        (self->dimension,
         means_+ i_cl * self->dimension,
         data + i_d * self->dimension,
         posteriors + i_cl*numData + i_d);
        
        VL_XCAT(_vl_weighted_sigma_sse2_, SFX)
        (self->dimension,
         sigmas_ + i_cl * self->dimension ,
         data + i_d * self->dimension,
         oldMeans + i_cl * self->dimension,
         posteriors + i_cl*numData + i_d);
#else
        for (dim = 0 ; dim < self->dimension ; ++dim) {
          TYPE diff = data[i_d * self->dimension + dim] - oldMeans[i_cl*self->dimension + dim];
          sigmas_ [i_cl * self->dimension + dim] += posteriors[i_cl*numData + i_d] * diff*diff;
        }
        for (dim = 0 ; dim < self->dimension ; ++dim) {
          means_ [self->dimension * i_cl + dim] += data[self->dimension * i_d + dim] * posteriors[i_cl * numData + i_d];
        }
#endif
#endif
      }
    }

    /* accumulate */
#if defined(_OPENMP)
#pragma omp critical
#endif
    {
      for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
        weights[i_cl] += clusterPosteriorSum_ [i_cl];
        for (dim = 0 ; dim < self->dimension ; ++dim) {
          sigmas[i_cl * self->dimension + dim] += sigmas_ [i_cl * self->dimension + dim];
          means[i_cl * self->dimension + dim]  += means_ [i_cl * self->dimension + dim];
        }
      }
      vl_free(means_);
      vl_free(sigmas_);
      vl_free(clusterPosteriorSum_);
    }
  } /* parallel section */

  /* normalize */
  for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
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

  for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
    weights[i_cl] /= posteriorSum;
  }

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
  vl_size numClusters = self->numClusters ;
  vl_index i_d, i_cl;
  vl_size dim;
  double LL = 0;
  
  // TODO : check this statement
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

#if defined(_OPENMP)
#pragma omp parallel for private(i_cl,dim) num_threads(vl_get_max_threads())
#endif
  for (i_cl = 0 ; i_cl < (signed)numClusters ; ++ i_cl) {
    TYPE logSigma = 0;
    logWeights[i_cl] = log(weights[i_cl]);
    for(dim = 0; dim < self->dimension; dim++) {
      logSigma += log(sigmas[i_cl*self->dimension + dim]);
      invSigmas [i_cl*self->dimension + dim] = (TYPE) 1.0 / sigmas[i_cl*self->dimension + dim];
    }
    logSigmas[i_cl] = logSigma;
  } /* end of parallel region */

#if defined(_OPENMP)
#pragma omp parallel for private(i_cl,i_d) reduction(+:LL) \
                         num_threads(vl_get_max_threads())
#endif
  for (i_d = 0 ; i_d < (signed)numData ; ++ i_d) {
    TYPE clusterPosteriorsSum = 0;
    TYPE maxPosterior = (TYPE)(-VL_INFINITY_D);
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; i_cl++) {
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
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      posteriors[i_cl * numData + i_d] -= maxPosterior;
      posteriors[i_cl * numData + i_d] = exp(posteriors[i_cl * numData + i_d]);
      clusterPosteriorsSum += posteriors[i_cl * numData + i_d];
    }
    
    LL += (double)log(clusterPosteriorsSum)+maxPosterior;
    
    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      posteriors[i_cl * numData + i_d] /= clusterPosteriorsSum;
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

  if ((restarted > 0) & (self->verbosity > 0)) {
    VL_PRINT("Warning: %d Gaussian modes reinitialized because they had become empty.\n",restarted);
  }

  return LL ;
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

  for (iteration = 0 ; 1 ; ++ iteration) {
    double eps;

    /* assign data to cluters */
    LL = VL_XCAT(_vl_gmm_EM_, SFX)(self, self->posteriors, self->weights, self->sigmas, self->means, data, numData) ;

    if (self->verbosity) {
      VL_PRINTF("GMM: GMM-EM iter %d: loglikelihood = %f\n", iteration, LL) ;
    }

    /* check termination conditions */
    if (iteration >= self->maxNumIterations) {
      if (self->verbosity) {
        VL_PRINTF("GMM: GMM-EM terminating because maximum number of iterations reached\n") ;
      }
      break ;
    }

    eps = vl_abs_d ((LL - previousLL) / (LL));
    if ((iteration > 0) && (eps < 0.00001)) {
      if (self->verbosity) {
        VL_PRINTF("GMM: GMM-EM terminating because the algorithm fully converged\n") ;
      }
      break ;
    }

    previousLL = LL;
  }
  return LL;
}

/* ---------------------------------------------------------------- */
#else /* VL_GMM_INSTANTIATING */
/* ---------------------------------------------------------------- */

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

/* ---------------------------------------------------------------- */
#ifndef VL_GMM_INSTANTIATING
/* ---------------------------------------------------------------- */

/** @brief Create a new GMM object by copy
 ** @param gmm GMM object to copy.
 ** @return new copy.
 **/

VlGMM *
vl_gmm_new_copy (VlGMM const * gmm)
{
  switch(gmm->dataType) {
    case VL_TYPE_FLOAT:
      return _vl_gmm_new_copy_f (gmm);
    case VL_TYPE_DOUBLE:
      return _vl_gmm_new_copy_d (gmm);
    default:
      abort() ;
      return NULL ;
  }
}

/** @brief Initialize mixture before EM takes place using random initialization
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

void
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

/** @brief Initialize mixture before EM takes place using kmeans
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

void
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

/** @brief Initialize mixture before EM takes place using the custom parameters
 ** @param self GMM object instance.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

void
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

/** @brief Initialize mixture before EM takes place
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

void
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

/** @brief Run GMM clustering - includes initialization and EM
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param dimension dimensionality of the data points.
 ** @param numData number of data points.
 ** @param numClusters number of gaussians which should be estimated.
 **/

double vl_gmm_cluster (VlGMM * self,
                       void const * data,
                       vl_size dimension,
                       vl_size numData,
                       vl_size numClusters)
{
  void * bestMeans = NULL ;
  void * bestSigmas = NULL ;
  void * bestPosteriors = NULL ;
  double bestLL = -VL_INFINITY_D ;
  vl_uindex repetition;

  assert(self->numRepetitions >=1) ;

  self->numData = numData;
  self->numClusters = numClusters;

  for (repetition = 0 ; repetition < self->numRepetitions ; ++ repetition) {
    double LL ;
    double timeRef ;

    if (self->verbosity) {
      VL_PRINTF("GMM: repetition %d of %d\n", repetition + 1, self->numRepetitions) ;
    }

    /* seed a new mixture model */
    timeRef = vl_get_cpu_time() ;
    vl_gmm_init_mixture (self, data, dimension, numData, numClusters) ;
    if (self->verbosity) {
      VL_PRINTF("GMM: GMM initialized in %.2f s\n",
                vl_get_cpu_time() - timeRef) ;
    }
    
    /* fit the model to data by running EM */
    timeRef = vl_get_cpu_time () ;
    LL = vl_gmm_get_mixture (self, data, numData) ;
    if (self->verbosity) {
      VL_PRINTF("GMM: GMM terminated in %.2f s with loglikelihood %f\n",
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

/** @brief Invoke the EM algorithm.
 ** @param self GMM object instance.
 ** @param data data points which should be clustered.
 ** @param numData number of data points.
 **/

double vl_gmm_get_mixture (VlGMM * self, void const * data, vl_size numData)
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

/** @brief Explicitly set the initial means for EM.
 ** @param self GMM object instance.
 ** @param means initial values of means.
 ** @param numClusters number of mean points.
 ** @param dimension dimensionality of the means.
 **/

void
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

/** @brief Explicitly set the initial sigma diagonals for EM.
 ** @param self GMM object instance.
 ** @param sigmas initial values of covariance matrix diagonals.
 ** @param numClusters number of sigma matrices.
 ** @param dimension number of points on the diagonals of the covariance matrices.
 **/

void vl_gmm_set_sigmas (VlGMM * self, void * sigmas, vl_size numClusters, vl_size dimension)
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
  if (self->sigmas == NULL) {
    self->sigmas = vl_malloc(numClusters*typeSize*dimension);
  }

  memcpy(self->sigmas,sigmas,numClusters*typeSize*dimension);
}

/** @brief Explicitly set the initial weights of the gaussians.
 ** @param self GMM object instance.
 ** @param weights initial values of the gaussian weights.
 ** @param numClusters number of gaussians.
 **/

void vl_gmm_set_weights (VlGMM * self, void * weights, vl_size numClusters)
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

/** @brief Explicitly set the posterior probabilities.
 ** @param self GMM object instance.
 ** @param posteriors initial values of the posterior probabilities.
 ** @param numClusters number of gaussians.
 ** @param numClusters number of data points.
 **/

void vl_gmm_set_posteriors (VlGMM * self, void * posteriors, vl_size numClusters, vl_size numData)
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
