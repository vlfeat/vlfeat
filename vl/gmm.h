/** @file gmm.h
 ** @brief GMM (@ref gmm)
 ** @author David Novotny
 **/

/*
Copyright (C) 2013 David Novotny.
Copyright (C) 2013 Andrea Vedaldi.
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


#ifndef __DOXYGEN__
struct _VlGMM ;
typedef struct _VlGMM VlGMM ;
#else
/** @brief GMM quantizer */
typedef OPAQUE VlGMM ;
#endif 

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
VL_EXPORT void vl_gmm_set_dimension (VlGMM * self, vl_size dimension) ;
VL_EXPORT void vl_gmm_set_num_repetitions (VlGMM * self, vl_size numRepetitions) ;
VL_EXPORT void vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations) ;
VL_EXPORT void vl_gmm_set_verbosity (VlGMM * self, int verbosity) ;
VL_EXPORT void vl_gmm_set_max_num_iterations (VlGMM * self, vl_size maxNumIterations);
VL_EXPORT void vl_gmm_set_num_clusters (VlGMM * self, vl_size numClusters);
VL_EXPORT void vl_gmm_set_initialization (VlGMM * self, VlGMMInitialization init);
VL_EXPORT void vl_gmm_set_kmeans_init_object (VlGMM * self, VlKMeans * kmeans);
VL_EXPORT void vl_gmm_set_sigma_lower_bound (VlGMM * self, double lowbound);
/** @} */

/** @name Get parameters
 ** @{
 **/
VL_EXPORT void const * vl_gmm_get_means (VlGMM const * self);
VL_EXPORT void const * vl_gmm_get_sigmas (VlGMM const * self);
VL_EXPORT void const * vl_gmm_get_priors (VlGMM const * self);
VL_EXPORT void const * vl_gmm_get_posteriors (VlGMM const * self);
VL_EXPORT vl_type vl_gmm_get_data_type (VlGMM const * self);
VL_EXPORT vl_size vl_gmm_get_dimension (VlGMM const * self);
VL_EXPORT vl_size vl_gmm_get_num_clusters (VlGMM const * self);
VL_EXPORT vl_size vl_gmm_get_num_data (VlGMM const * self);
VL_EXPORT double vl_gmm_get_loglikelyhood (VlGMM const * self);
VL_EXPORT int vl_gmm_get_verbosity (VlGMM const * self);
VL_EXPORT vl_size vl_gmm_get_max_num_iterations (VlGMM const * self);
VL_EXPORT vl_size vl_gmm_get_num_repetitions (VlGMM const * self);
VL_EXPORT VlGMMInitialization vl_gmm_get_initialization (VlGMM const * self);
VL_EXPORT VlKMeans * vl_gmm_get_kmeans_init_object (VlGMM const * self);
VL_EXPORT double vl_gmm_get_sigma_lower_bound (VlGMM * self);
/** @} */

/* VL_GMM_H */
#endif
