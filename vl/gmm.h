/** @file gmm.h
 ** @brief GMM (@ref gmm)
 ** @author Andrea Vedaldi, David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_GMM_H
#define VL_GMM_H

#include "kmeans.h"

/** @brief GMM initialization algorithms */

typedef enum _VlGMMInitialization
{
  VlGMMKMeans, /**< KMeans initialization */
  VlGMMRand,   /**< Random selection initialization */
  VlGMMCustom  /**< User specifies initialization parameters */
} VlGMMInitialization ;


/** @brief GMM multithreading setup */

typedef enum _VlGMMMultithreading
{
  VlGMMSerial,    /**< Compute using one thread */
  VlGMMParallel   /**< Turn multithreading on */
} VlGMMMultithreading ;

/** ------------------------------------------------------------------
 ** @brief GMM quantizer
 **/
typedef struct _VlGMM
{
  vl_type dataType ;                   /**< data type */
  vl_size dimension ;                  /**< data dimensionality */
  vl_size numClusters ;                 /**< number of clusters  */
  vl_size numData ;                     /**< number of last time clustered data points  */

  vl_size maxNumIterations ;             /**< Maximum number of refinement iterations */
  vl_size numRepetitions   ;             /**< Number of clustering repetitions */
  int verbosity ;                       /**< verbosity level */

  void * means;                         /**< Means of gaussians */
  void * weights;                       /**< Weights of gaussians */
  void * sigmas;                        /**< Diagonals of covariance matrices of gaussians */

  void * posteriors;                    /**< Probabilities of correspondences of points to clusters */

  double sigmaLowBound;

  VlGMMInitialization initialization;    /**< Initilaization option */
  VlGMMMultithreading multithreading;    /**< Multithreading option */

  VlKMeans * kmeansInit;                 /**< Kmeans object for initialization of gaussians */

  double LL;                            /**< current solution loglikelyhood */
} VlGMM ;

/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlGMM * vl_gmm_new (vl_type dataType) ;
VL_EXPORT VlGMM * vl_gmm_new_copy (VlGMM const * gmm) ;
VL_EXPORT void vl_gmm_delete (VlGMM * self) ;
VL_EXPORT void vl_gmm_reset (VlGMM * self);
/** @} */

/** @name Basic data processing
 ** @{
 **/
VL_EXPORT double
vl_gmm_get_mixture
(VlGMM * self,
 void const * data,
 vl_size numData);

VL_EXPORT double
vl_gmm_cluster
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numCenters);

VL_EXPORT void
vl_gmm_rand_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numMeans);

VL_EXPORT void
vl_gmm_kmeans_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters);

VL_EXPORT void
vl_gmm_init_mixture
(VlGMM * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters);

/** @} */

/** @name Advanced data processing
 ** @{
 **/
VL_EXPORT void
vl_gmm_set_means
(VlGMM * self,
 void * means,
 vl_size numClusters,
 vl_size dimension);

VL_EXPORT void
vl_gmm_set_sigmas
(VlGMM * self,
 void * sigmas,
 vl_size numClusters,
 vl_size dimension);

VL_EXPORT void
vl_gmm_set_weights
(VlGMM * self,
 void * weights,
 vl_size numClusters);

VL_EXPORT void
vl_gmm_set_posteriors
(VlGMM * self,
 void * posteriors,
 vl_size numClusters,
 vl_size numData);
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_gmm_set_dimension (VlGMM * self, vl_size dimension) ;
VL_INLINE void vl_gmm_set_num_repetitions (VlGMM * self, vl_size numRepetitions) ;
VL_INLINE void vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations) ;
VL_INLINE void vl_gmm_set_verbosity (VlGMM * self, int verbosity) ;
VL_INLINE void vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations);
VL_INLINE void vl_gmm_set_num_clusters (VlGMM * self, vl_size numClusters);
VL_INLINE void vl_gmm_set_initialization (VlGMM * self, VlGMMInitialization init);
VL_INLINE void vl_gmm_set_kmeans_init_object (VlGMM * self, VlKMeans * kmeans);
VL_INLINE void vl_gmm_set_multithreading (VlGMM * self, VlGMMMultithreading multithreading);
VL_INLINE void vl_gmm_set_sigma_lower_bound (VlGMM * self, double lowbound);
/** @} */

/** @name Get parameters
 ** @{
 **/
VL_INLINE void const * vl_gmm_get_means (VlGMM const * self);
VL_INLINE void const * vl_gmm_get_sigmas (VlGMM const * self);
VL_INLINE void const * vl_gmm_get_weights (VlGMM const * self);
VL_INLINE void const * vl_gmm_get_posteriors (VlGMM const * self);
VL_INLINE vl_type vl_gmm_get_data_type (VlGMM const * self);
VL_INLINE vl_size vl_gmm_get_dimension (VlGMM const * self);
VL_INLINE vl_size vl_gmm_get_num_clusters (VlGMM const * self);
VL_INLINE vl_size vl_gmm_get_num_data (VlGMM const * self);
VL_INLINE double vl_gmm_get_loglikelyhood (VlGMM const * self);
VL_INLINE int vl_gmm_get_verbosity (VlGMM const * self);
VL_INLINE vl_size vl_gmm_get_max_num_iterations (VlGMM const * self);
VL_INLINE vl_size vl_gmm_get_num_repetitions (VlGMM const * self);
VL_INLINE VlGMMInitialization vl_gmm_get_initialization (VlGMM const * self);
VL_INLINE VlKMeans * vl_gmm_get_kmeans_init_object (VlGMM const * self);
VL_INLINE VlGMMMultithreading vl_gmm_get_multithreading (VlGMM const * self);
VL_INLINE double vl_gmm_get_sigma_lower_bound (VlGMM * self);
/** @} */

/** ------------------------------------------------------------------
** @brief Get data type
** @param self VlGMM object instance.
** @return data type.
**/

VL_INLINE vl_type
vl_gmm_get_data_type (VlGMM const * self)
{
  return self->dataType ;
}

/** ------------------------------------------------------------------
 ** @brief Get the number of clusters
 ** @param self VlGMM object instance.
 ** @return number of clusters.
 **/

VL_INLINE vl_size
vl_gmm_get_num_clusters (VlGMM const * self)
{
  return self->numClusters ;
}

/** ------------------------------------------------------------------
 ** @brief Get the number of data points
 ** @param self VlGMM object instance.
 ** @return number of data points.
 **/

VL_INLINE vl_size
vl_gmm_get_num_data (VlGMM const * self)
{
  return self->numData ;
}

/** @brief Set the number of clusters
 ** @param self VlGMM object instance.
 ** @param numClusters number of clusters..
 **/

VL_INLINE void
vl_gmm_set_num_clusters (VlGMM * self, vl_size numClusters)
{
  self->numClusters = numClusters;
}

/** ------------------------------------------------------------------
 ** @brief Get the log likelyhood of the current mixture
 ** @param self VlGMM object instance.
 ** @return loglikelyhood.
 **/

VL_INLINE double
vl_gmm_get_loglikelyhood (VlGMM const * self)
{
  return self->LL ;
}

/** ------------------------------------------------------------------
 ** @brief Get verbosity level
 ** @param self VlGMM object instance.
 ** @return verbosity level.
 **/

VL_INLINE int
vl_gmm_get_verbosity (VlGMM const * self)
{
  return self->verbosity ;
}

/** @brief Set verbosity level
 ** @param self VlGMM object instance.
 ** @param verbosity verbosity level.
 **/

VL_INLINE void
vl_gmm_set_verbosity (VlGMM * self, int verbosity)
{
  self->verbosity = verbosity ;
}

/** ------------------------------------------------------------------
 ** @brief Get means
 ** @param self VlGMM object instance.
 ** @return cluster means.
 **/

VL_INLINE void const *
vl_gmm_get_means (VlGMM const * self)
{
  return self->means ;
}

/** ------------------------------------------------------------------
 ** @brief Get sigmas
 ** @param self VlGMM object instance.
 ** @return diagonals of cluster covariance matrices.
 **/

VL_INLINE void const *
vl_gmm_get_sigmas (VlGMM const * self)
{
  return self->sigmas ;
}

/** ------------------------------------------------------------------
 ** @brief Get weights
 ** @param self VlGMM object instance.
 ** @return weights of cluster gaussians.
 **/

VL_INLINE void const *
vl_gmm_get_weights (VlGMM const * self)
{
  return self->weights ;
}

/** ------------------------------------------------------------------
 ** @brief Get posteriors
 ** @param self VlGMM object instance.
 ** @return posterior probabilities of cluster memberships.
 **/

VL_INLINE void const *
vl_gmm_get_posteriors (VlGMM const * self)
{
  return self->posteriors ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param self VlGMM object instance.
 ** @return maximum number of iterations.
 **/

VL_INLINE vl_size
vl_gmm_get_max_num_iterations (VlGMM const * self)
{
  return self->maxNumIterations ;
}

/** @brief Set maximum number of iterations
 ** @param self VlGMM filter.
 ** @param maxNumIterations maximum number of iterations.
 **/

VL_INLINE void
vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations)
{
  self->maxNumIterations = maxNumIterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of repetitions.
 ** @param self VlGMM object instance.
 ** @return current number of repretitions for quantization.
 **/

VL_INLINE vl_size
vl_gmm_get_num_repetitions (VlGMM const * self)
{
  return self->numRepetitions ;
}

/** @brief Set maximum number of repetitions
 ** @param self VlGMM object instance.
 ** @param numRepetitions maximum number of repetitions.
 ** The number of repetitions cannot be smaller than 1.
 **/

VL_INLINE void
vl_gmm_set_num_repetitions (VlGMM * self, vl_size numRepetitions)
{
  assert (numRepetitions >= 1) ;
  self->numRepetitions = numRepetitions ;
}

/** ------------------------------------------------------------------
 ** @brief Get data dimension
 ** @param self VlGMM object instance.
 ** @return data dimension.
 **/

VL_INLINE vl_size
vl_gmm_get_dimension (VlGMM const * self)
{
  return self->dimension ;
}

/** @brief Set dimensionality of the data.
 ** @param self VlGMM object instance.
 ** @param dimension dimensionality of the data.
  ** The number of repetitions cannot be smaller than 1.
 **/
VL_INLINE void vl_gmm_set_dimension (VlGMM * self, vl_size dimension)
{
  assert(dimension >= 1);
  self->dimension = dimension;
}

/** ------------------------------------------------------------------
 ** @brief Get initialization algorithm
 ** @param self VlGMM object instance.
 ** @return initialization algorithm.
 **/

VL_INLINE VlGMMInitialization
vl_gmm_get_initialization (VlGMM const * self)
{
  return self->initialization ;
}

/** @brief Set initialization algorithm.
 ** @param self VlGMM object instance.
 ** @param init initialization algorithm.
 **/
VL_INLINE void
vl_gmm_set_initialization (VlGMM * self, VlGMMInitialization init)
{
  self->initialization = init;
}

/** ------------------------------------------------------------------
 ** @brief Get KMeans initialization object.
 ** @param self VlGMM object instance.
 ** @return kmeans initialization object.
 **/
VL_INLINE VlKMeans * vl_gmm_get_kmeans_init_object (VlGMM const * self)
{
  return self->kmeansInit;
}

/** @brief Set KMeans initialization object.
 ** @param self VlGMM object instance.
 ** @param kmeans initialization KMeans object.
 **/
VL_INLINE void vl_gmm_set_kmeans_init_object (VlGMM * self, VlKMeans * kmeans)
{
  if(self->kmeansInit)
  {
    vl_kmeans_delete(self->kmeansInit);
  }
  self->kmeansInit = kmeans;
}

/** ------------------------------------------------------------------
 ** @brief Get GMM multithreading otion
 ** @param self VlGMM object instance.
 ** @return VlGMM multithreading option.
 **/
VL_INLINE VlGMMMultithreading vl_gmm_get_multithreading (VlGMM const * self)
{
  return self->multithreading;
}

/** @brief Set GMM multithreading option.
 ** @param self VlGMM object instance.
 ** @param multithreading multithreading option.
 **/
VL_INLINE void vl_gmm_set_multithreading (VlGMM * self, VlGMMMultithreading multithreading)
{
  self->multithreading = multithreading;
}

/** ------------------------------------------------------------------
 ** @brief Get lower bound on sigma diagonals.
 ** @param self VlGMM object instance.
 ** @return lower bound on sigmas.
 **/
VL_INLINE double vl_gmm_get_sigma_lower_bound (VlGMM * self)
{
  return self->sigmaLowBound;
}

/** @brief Set lower bound on sigma diagonals.
 ** @param self VlGMM object instance.
 ** @param lowbound lower bound on sigma diagonals.
 **/
VL_INLINE void vl_gmm_set_sigma_lower_bound (VlGMM * self, double lowbound)
{
  self->sigmaLowBound = lowbound;
}


/* VL_IKMEANS_H */
#endif
