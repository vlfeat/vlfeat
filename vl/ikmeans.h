/** @file ikmeans.h
 ** @brief Integer K-Means clustering
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_IKMEANS_H
#define VL_IKMEANS_H

#include "generic.h"
#include "random.h"

typedef vl_int32 vl_ikm_acc ;  /**< IKM accumulator data type */

/** ------------------------------------------------------------------
 ** @brief IKM algorithms
 **/

enum VlIKMAlgorithms {
  VL_IKM_LLOYD,       /**< Lloyd algorithm */
  VL_IKM_ELKAN        /**< Elkan algorithm */
} ;

/** ------------------------------------------------------------------
 ** @brief IKM quantizer
 **/

typedef struct _VlIKMFilt
{
  int M ;                  /**< data dimensionality */
  int K ;                  /**< number of centers   */

  int method ;             /**< Learning method */
  int max_niters ;         /**< Lloyd: maximum number of iterations */
  int verb ;               /**< verbosity level */

  vl_ikm_acc *centers ;    /**< centers */
  vl_ikm_acc *inter_dist ; /**< centers inter-distances */
} VlIKMFilt ;

/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlIKMFilt *vl_ikm_new    (int method) ;
VL_EXPORT void       vl_ikm_delete (VlIKMFilt *f) ;
/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT void vl_ikm_init           (VlIKMFilt *f, vl_ikm_acc const *centers, int M, int K) ;
VL_EXPORT void vl_ikm_init_rand      (VlIKMFilt *f, int M, int K) ;
VL_EXPORT void vl_ikm_init_rand_data (VlIKMFilt *f, vl_uint8 const *data, int M, int N, int K) ;
VL_EXPORT int  vl_ikm_train          (VlIKMFilt *f, vl_uint8 const *data, int N) ;
VL_EXPORT void vl_ikm_push           (VlIKMFilt *f, vl_uint *asgn, vl_uint8 const *data, int N) ;

VL_EXPORT
vl_uint  vl_ikm_push_one   (vl_ikm_acc const *centers,
                            vl_uint8 const *data,
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
  f-> verb = VL_MAX(0,verb) ;
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
