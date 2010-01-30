/** @file   kmeans.h
 ** @author Andrea Vedaldi
 ** @brief  K-means - Declaration
 **/

/* AUTORIGHTS */

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

/** ------------------------------------------------------------------
 ** @brief K-means quantizer
 **/

typedef struct _VlKMeans
{
  vl_type dataType ;                   /**< data type */
  vl_size dimension ;                  /**< data dimensionality */
  vl_size numCenters ;                 /**< number of centers   */

  VlKMeansAlgorithm algorithm ;        /**< Learning algorithm */
  VlVectorComparisonType distance ;    /**< Distance */
  vl_size maxNumIterations ;           /**< Lloyd: maximum number of iterations */
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
VL_EXPORT VlKMeans *vl_kmeans_new (VlKMeansAlgorithm algorithm,
                                   VlVectorComparisonType distance,
                                   vl_type dataType) ;
VL_EXPORT void vl_kmeans_delete (VlKMeans * self) ;
/** @} */

/** @name Process data
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
VL_EXPORT void vl_kmeans_train (VlKMeans * self,
                                void const *data,
                                vl_size numData) ;
VL_EXPORT void vl_kmeans_push (VlKMeans * f,
                               vl_uint32 * assignments,
                               void * distances,
                               void const * data,
                               vl_size numData) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE vl_type vl_kemans_get_data_type (VlKMeans const * self) ;
VL_INLINE vl_size vl_kemans_get_dimension (VlKMeans const * self) ;
VL_INLINE vl_size vl_kemans_get_num_centers (VlKMeans const * self) ;
VL_INLINE VlVectorComparisonType vl_kemans_get_distance (VlKMeans const * self) ;
VL_INLINE int vl_kemans_get_verbosity (VlKMeans const * self) ;
VL_INLINE vl_size vl_kemans_get_max_num_iterations (VlKMeans const * self) ;
VL_INLINE double vl_kmeans_get_energy (VlKMeans const * self) ;
VL_INLINE void const * vl_kmeans_get_centers (VlKMeans const * self) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_kemans_set_verbosity (VlKMeans * self, int verbosity) ;
VL_INLINE void vl_kemans_set_max_num_iterations (VlKMeans * self,
                                                 vl_size maxNumIterations) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get data dimension
 ** @param self KMeans object instance.
 ** @return data dimension.
 **/

VL_INLINE vl_size
vl_kmeans_get_dimension (VlKMeans const * self)
{
  return self->dimension ;
}

/** ------------------------------------------------------------------
 ** @brief Get the number of centers (K)
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
 ** @brief Get energy of current estimate
 ** @param self KMeans object instance.
 ** @return current estimate energy.
 **/

VL_INLINE double
vl_kmeans_get_energy (VlKMeans const * self)
{
  return self->energy ;
}

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

/** ------------------------------------------------------------------
 ** @brief Get data type
 ** @param self KMeans object instance.
 ** @return data type.
 **/

VL_INLINE VlVectorComparisonType
vl_kmeans_get_distance (VlKMeans const * self)
{
  return self->distance ;
}

/** ------------------------------------------------------------------
 ** @brief Set verbosity level
 ** @param self KMeans object instance.
 ** @param verbosity verbosity level.
 **/

VL_INLINE void
vl_kmeans_set_verbosity (VlKMeans * self, int verbosity)
{
  self->verbosity = verbosity ;
}

/** ------------------------------------------------------------------
 ** @brief Set maximum number of iterations
 ** @param self KMeans filter.
 ** @param maxNumIterations maximum number of iterations.
 **/

VL_INLINE void
vl_kmeans_set_max_num_iterations (VlKMeans * self, vl_size maxNumIterations)
{
  self->maxNumIterations = maxNumIterations ;
}

/* VL_IKMEANS_H */
#endif
