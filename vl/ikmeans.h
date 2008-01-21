/** @file   ikmeans.h
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Integer K-Means clustering
 **/

#ifndef VL_IKMEANS_H
#define VL_IKMEANS_H

#include "generic.h"

typedef vl_int32 vl_ikm_acc ;  /**< IKM accumulator data type */
typedef vl_uint8 vl_ikm_data ; /**< IKM data type */

/** ------------------------------------------------------------------
 ** @brief IKM algorithms 
 **/

enum VlIKMAlgorithms {
  VL_IKM_LLOYD,       /**< Lloyd algorithm */
  VL_IKM_ELKAN        /**< Elkan algorithm */
} ;
  
/** ------------------------------------------------------------------
 ** @brief Integer K-Means filter
 **/

typedef struct _VlIKMFilt
{
  vl_ikm_acc *centers ;    /**< centers             */
  int M ;                  /**< data dimensionality */
  int K ;                  /**< number of centers   */

  int method ;             /**< Learning method */
  int max_niters ;         /**< Lloyd: maximum number of iterations */
  int verb ;               /**< verbosity level */

  vl_ikm_acc *inter_dist ; /**< Centers inter-distances */
} VlIKMFilt ;

/** @name Create and destroy
 ** @{
 **/ 
VlIKMFilt *vl_ikm_new    (int method) ;
void       vl_ikm_delete (VlIKMFilt *f) ;
/** @} */

/** @name Process data
 ** @{
 **/
void vl_ikm_init           (VlIKMFilt *f, vl_ikm_acc const *centers, int M, int K) ;
void vl_ikm_init_rand      (VlIKMFilt *f, int M, int K) ;
void vl_ikm_init_rand_data (VlIKMFilt *f, vl_ikm_data const *data, int M, int N, int K) ;
int  vl_ikm_train          (VlIKMFilt *f, vl_ikm_data const *data, int N) ;
void vl_ikm_push           (VlIKMFilt *f, vl_uint *asgn, vl_ikm_data const *data, int N) ;

vl_uint  vl_ikm_push_one   (vl_ikm_acc const *centers, 
                            vl_ikm_data const *data,
                            int M, int K) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE int vl_ikm_get_ndims      (VlIKMFilt const *f) ;
VL_INLINE int vl_ikm_get_K          (VlIKMFilt const *f) ;
VL_INLINE int vl_ikm_get_verbosity  (VlIKMFilt const *f) ;
VL_INLINE int vl_ikm_get_max_niters (VlIKMFilt const *f) ;
VL_INLINE vl_ikm_acc const * vl_ikm_get_centers (VlIKMFilt const *f) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_ikm_set_verbosity  (VlIKMFilt *f, int verb) ;
VL_INLINE void vl_ikm_set_max_niters (VlIKMFilt *f, int max_niters) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get data dimensionality
 ** @param f IKM filter.
 ** @return data dimensionality.
 **/
 
VL_INLINE int
vl_ikm_get_ndims (VlIKMFilt const* f)
{
  return f-> M ;  
}

/** ------------------------------------------------------------------
 ** @brief Get the number of centers K
 ** @param f IKM filter.
 ** @return number of centers K.
 **/
 
VL_INLINE int
vl_ikm_get_K (VlIKMFilt const* f)
{
  return f-> K ;  
}

/** ------------------------------------------------------------------
 ** @brief Get verbosity level
 ** @param f IKM filter.
 ** @return verbosity level.
 **/
 
VL_INLINE int
vl_ikm_get_verbosity (VlIKMFilt const* f)
{
  return f-> verb ;  
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param f IKM filter.
 ** @return maximum number of iterations.
 **/
 
VL_INLINE int
vl_ikm_get_max_niters (VlIKMFilt const* f)
{
  return f-> max_niters ;  
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param f IKM filter.
 ** @return maximum number of iterations.
 **/
 
VL_INLINE vl_ikm_acc const *
vl_ikm_get_centers (VlIKMFilt const* f)
{
  return f-> centers ;  
}

/** ------------------------------------------------------------------
 ** @brief Set verbosity level
 ** @param f    IKM filter.
 ** @param verb verbosity level.
 **/
 
VL_INLINE void
vl_ikm_set_verbosity (VlIKMFilt *f, int verb)
{
  f-> verb = verb ;
}

/** ------------------------------------------------------------------
 ** @brief Set maximum number of iterations
 ** @param f          IKM filter.
 ** @param max_niters maximum number of iterations.
 **/
 
VL_INLINE void
vl_ikm_set_max_niters (VlIKMFilt *f, int max_niters)
{
  f-> max_niters = max_niters ;  
}

/* VL_IKMEANS_H */
#endif
