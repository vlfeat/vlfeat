/** @file   kmeans.h
 ** @author Andrea Vedaldi
 ** @brief  K-means - Declaration
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_KMEANS_H
#define VL_KMEANS_H

#include "generic.h"
#include "random.h"
#include "mathop.h"

/* ---------------------------------------------------------------- */

/** @brief K-means algorithms */

typedef enum _VlKMeansAlgorithm {
  VlKMeansLLoyd,       /**< Lloyd algorithm */
  VlKMeansElkan,       /**< Elkan algorithm */
  VlKMeansANN          /**< Approximate nearest neighbors */
} VlKMeansAlgorithm ;

/** @brief K-means algorithms */

typedef enum _VlKMeansInitialization {
  VlKMeansRandomSelection,  /**< Randomized selection */
  VlKMeansPlusPlus          /**< Plus plus raondomized selection */
} VlKMeansInitialization ;


/** ------------------------------------------------------------------
 ** @brief K-means quantizer
 **/

typedef struct _VlKMeans
{
  vl_type dataType ;                   /**< data type */
  vl_size dimension ;                  /**< data dimensionality */
  vl_size numCenters ;                 /**< number of centers   */

  VlKMeansInitialization initialization ; /** < Initalization algorithm */
  VlKMeansAlgorithm algorithm ;        /**< Clustring algorithm */
  VlVectorComparisonType distance ;    /**< Distance */
  vl_size maxNumIterations ;           /**< Maximum number of refinement iterations */
  vl_size numRepetitions   ;           /**< Number of clustering repetitions */
  int verbosity ;                      /**< verbosity level */

  void * centers ;                     /**< centers */
  void * centerDistances ;             /**< centers inter-distances */

  double energy ;                      /**< current solution energy */
  VlFloatVectorComparisonFunction floatVectorComparisonFn ;
  VlDoubleVectorComparisonFunction doubleVectorComparisonFn ;
} VlKMeans ;

/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlKMeans * vl_kmeans_new (vl_type dataType, VlVectorComparisonType distance) ;
VL_EXPORT VlKMeans * vl_kmeans_new_copy (VlKMeans const * kmeans) ;
VL_EXPORT void vl_kmeans_delete (VlKMeans * self) ;
/** @} */

/** @name Basic data processing
 ** @{
 **/
VL_EXPORT void vl_kmeans_reset (VlKMeans * self) ;

VL_EXPORT double vl_kmeans_cluster (VlKMeans * self,
                                    void const * data,
                                    vl_size dimension,
                                    vl_size numData,
                                    vl_size numCenters) ;

VL_EXPORT void vl_kmeans_quantize (VlKMeans * self,
                                   vl_uint32 * assignments,
                                   void * distances,
                                   void const * data,
                                   vl_size numData) ;
/** @} */

/** @name Advanced data processing
 ** @{
 **/
VL_EXPORT void vl_kmeans_set_centers (VlKMeans * self,
                                      void const * centers,
                                      vl_size dimension,
                                      vl_size numCenters) ;

VL_EXPORT void vl_kmeans_seed_centers_with_rand_data
                  (VlKMeans * self,
                   void const * data,
                   vl_size dimensions,
                   vl_size numData,
                   vl_size numCenters) ;

VL_EXPORT void vl_kmeans_seed_centers_plus_plus
                  (VlKMeans * self,
                   void const * data,
                   vl_size dimensions,
                   vl_size numData,
                   vl_size numCenters) ;

VL_EXPORT double vl_kmeans_refine_centers (VlKMeans * self,
                                           void const * data,
                                           vl_size numData) ;

/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE vl_type vl_kmeans_get_data_type (VlKMeans const * self) ;
VL_INLINE VlVectorComparisonType vl_kmeans_get_distance (VlKMeans const * self) ;

VL_INLINE VlKMeansAlgorithm vl_kmeans_get_algorithm (VlKMeans const * self) ;
VL_INLINE VlKMeansInitialization vl_kmeans_get_initialization (VlKMeans const * self) ;
VL_INLINE vl_size vl_kmeans_get_num_repetitions (VlKMeans const * self) ;

VL_INLINE vl_size vl_kmeans_get_dimension (VlKMeans const * self) ;
VL_INLINE vl_size vl_kmeans_get_num_centers (VlKMeans const * self) ;

VL_INLINE int vl_kmeans_get_verbosity (VlKMeans const * self) ;
VL_INLINE vl_size vl_kmeans_get_max_num_iterations (VlKMeans const * self) ;
VL_INLINE double vl_kmeans_get_energy (VlKMeans const * self) ;
VL_INLINE void const * vl_kmeans_get_centers (VlKMeans const * self) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_kmeans_set_algorithm (VlKMeans * self, VlKMeansAlgorithm algorithm) ;
VL_INLINE void vl_kmeans_set_initialization (VlKMeans * self, VlKMeansInitialization initialization) ;
VL_INLINE void vl_kmeans_set_num_repetitions (VlKMeans * self, vl_size numRepetitions) ;
VL_INLINE void vl_kmeans_set_max_num_iterations (VlKMeans * self, vl_size maxNumIterations) ;
VL_INLINE void vl_kmeans_set_verbosity (VlKMeans * self, int verbosity) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get data type
 ** @param self KMeans object instance.
 ** @return data type.
 **/

VL_INLINE vl_type
vl_kmeans_get_data_type (VlKMeans const * self)
{
  return self->dataType ;
}

/** @brief Get data dimension
 ** @param self KMeans object instance.
 ** @return data dimension.
 **/

VL_INLINE vl_size
vl_kmeans_get_dimension (VlKMeans const * self)
{
  return self->dimension ;
}

/** @brief Get data type
 ** @param self KMeans object instance.
 ** @return data type.
 **/

VL_INLINE VlVectorComparisonType
vl_kmeans_get_distance (VlKMeans const * self)
{
  return self->distance ;
}

/** @brief Get the number of centers (K)
 ** @param self KMeans object instance.
 ** @return number of centers.
 **/

VL_INLINE vl_size
vl_kmeans_get_num_centers (VlKMeans const * self)
{
  return self->numCenters ;
}

/** ------------------------------------------------------------------
 ** @brief Get verbosity level
 ** @param self KMeans object instance.
 ** @return verbosity level.
 **/

VL_INLINE int
vl_kmeans_get_verbosity (VlKMeans const * self)
{
  return self->verbosity ;
}

/** @brief Set verbosity level
 ** @param self KMeans object instance.
 ** @param verbosity verbosity level.
 **/

VL_INLINE void
vl_kmeans_set_verbosity (VlKMeans * self, int verbosity)
{
  self->verbosity = verbosity ;
}

/** ------------------------------------------------------------------
 ** @brief Get centers
 ** @param self KMeans object instance.
 ** @return cluster centers.
 **/

VL_INLINE void const *
vl_kmeans_get_centers (VlKMeans const * self)
{
  return self->centers ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param self KMeans object instance.
 ** @return maximum number of iterations.
 **/

VL_INLINE vl_size
vl_kmeans_get_max_num_iterations (VlKMeans const * self)
{
  return self->maxNumIterations ;
}

/** @brief Set maximum number of iterations
 ** @param self KMeans filter.
 ** @param maxNumIterations maximum number of iterations.
 **/

VL_INLINE void
vl_kmeans_set_max_num_iterations (VlKMeans * self, vl_size maxNumIterations)
{
  self->maxNumIterations = maxNumIterations ;
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of repetitions.
 ** @param self KMeans object instance.
 ** @return current number of repretitions for quantization.
 **/

VL_INLINE vl_size
vl_kmeans_get_num_repetitions (VlKMeans const * self)
{
  return self->numRepetitions ;
}

/** @brief Set maximum number of repetitions
 ** @param self KMeans object instance.
 ** @param numRepetitions maximum number of repetitions.
 ** The number of repetitions cannot be smaller than 1.
 **/

VL_INLINE void
vl_kmeans_set_num_repetitions (VlKMeans * self,
                               vl_size numRepetitions)
{
  assert (numRepetitions >= 1) ;
  self->numRepetitions = numRepetitions ;
}

/** ------------------------------------------------------------------
 ** @brief Get K-means algorithm
 ** @param self KMeans object.
 ** @return algorithm.
 **/

VL_INLINE VlKMeansAlgorithm
vl_kmeans_get_algorithm (VlKMeans const * self)
{
  return self->algorithm ;
}

/** @brief Set K-means algorithm
 ** @param self KMeans object.
 ** @param algorithm K-means algorithm.
 **/

VL_INLINE void
vl_kmeans_set_algorithm (VlKMeans * self, VlKMeansAlgorithm algorithm)
{
  self->algorithm = algorithm ;
}

/** ------------------------------------------------------------------
 ** @brief Get K-means initialization algorithm
 ** @param self KMeans object.
 ** @return algorithm.
 **/

VL_INLINE VlKMeansInitialization
vl_kmeans_get_initialization (VlKMeans const * self)
{
  return self->initialization ;
}

/** @brief Set K-means initialization algorithm
 ** @param self KMeans object.
 ** @param initialization initialization.
 **/

VL_INLINE void
vl_kmeans_set_initialization (VlKMeans * self,
                              VlKMeansInitialization initialization)
{
  self->initialization = initialization ;
}


/* VL_IKMEANS_H */
#endif
