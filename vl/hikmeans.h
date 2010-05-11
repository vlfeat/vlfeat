/** @file   hikmeans.h
 ** @author Brian Fulkerson
 ** @brief  Hierarchical Integer K-Means Clustering
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_HIKMEANS_H
#define VL_HIKMEANS_H

#include "generic.h"
#include "ikmeans.h"

struct _VLHIKMTree ;
struct _VLHIKMNode ;

/** @brief HIKM tree node 
 **
 ** The number of children @a K is not bigger than the @a K parameter
 ** of the HIKM tree.
 **/
typedef struct _VlHIKMNode
{
  VlIKMFilt *filter ;             /**< IKM filter for this node*/
  struct _VlHIKMNode **children ; /**< Node children (if any) */
} VlHIKMNode ;

/** @brief HIKM tree */
typedef struct _VlHIKMTree {
  int M ;               /**< IKM: data dimensionality */
  int K ;               /**< IKM: K */
  int max_niters ;      /**< IKM: maximum # of iterations */
  int method ;          /**< IKM: method */
  int verb ;            /**< Verbosity level */
  
  int depth ;           /**< Depth of the tree */
  VlHIKMNode * root;    /**< Tree root node */
} VlHIKMTree ;

/** @name Create and destroy
 ** @{
 **/ 
VL_EXPORT VlHIKMTree  *vl_hikm_new    (int method) ;
VL_EXPORT void         vl_hikm_delete (VlHIKMTree *f) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE int vl_hikm_get_ndims      (VlHIKMTree const *f) ;
VL_INLINE int vl_hikm_get_K          (VlHIKMTree const *f) ;
VL_INLINE int vl_hikm_get_depth      (VlHIKMTree const *f) ;
VL_INLINE int vl_hikm_get_verbosity  (VlHIKMTree const *f) ;
VL_INLINE int vl_hikm_get_max_niters (VlHIKMTree const *f) ;
VL_INLINE VlHIKMNode const * vl_hikm_get_root (VlHIKMTree const *f) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_hikm_set_verbosity  (VlHIKMTree *f, int verb) ;
VL_INLINE void vl_hikm_set_max_niters (VlHIKMTree *f, int max_niters) ;
/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT void vl_hikm_init  (VlHIKMTree *f, int M, int K, int depth) ;
VL_EXPORT void vl_hikm_train (VlHIKMTree *f, vl_uint8 const *data, int N) ;
VL_EXPORT void vl_hikm_push  (VlHIKMTree *f, vl_uint *asgn, vl_uint8 const *data, int N) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get data dimensionality
 ** @param f HIKM tree.
 ** @return data dimensionality.
 **/
 
VL_INLINE int
vl_hikm_get_ndims (VlHIKMTree const* f)
{
  return f-> M ;  
}

/** ------------------------------------------------------------------
 ** @brief Get K
 ** @param f HIKM tree.
 ** @return K.
 **/
 
VL_INLINE int
vl_hikm_get_K (VlHIKMTree const* f)
{
  return f-> K ;  
}

/** ------------------------------------------------------------------
 ** @brief Get depth
 ** @param f HIKM tree.
 ** @return depth.
 **/
 
VL_INLINE int
vl_hikm_get_depth (VlHIKMTree const* f)
{
  return f-> depth ;  
}


/** ------------------------------------------------------------------
 ** @brief Get verbosity level
 ** @param f HIKM tree.
 ** @return verbosity level.
 **/
 
VL_INLINE int
vl_hikm_get_verbosity (VlHIKMTree const* f)
{
  return f-> verb ;  
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param f HIKM tree.
 ** @return maximum number of iterations.
 **/
 
VL_INLINE int
vl_hikm_get_max_niters (VlHIKMTree const* f)
{
  return f-> max_niters ;  
}

/** ------------------------------------------------------------------
 ** @brief Get maximum number of iterations
 ** @param f HIKM tree.
 ** @return maximum number of iterations.
 **/
 
VL_INLINE VlHIKMNode const *
vl_hikm_get_root (VlHIKMTree const* f)
{
  return f-> root ;  
}

/** ------------------------------------------------------------------
 ** @brief Set verbosity level
 ** @param f    HIKM tree.
 ** @param verb verbosity level.
 **/

VL_INLINE void
vl_hikm_set_verbosity (VlHIKMTree *f, int verb)
{
  f-> verb = verb ;
}

/** ------------------------------------------------------------------
 ** @brief Set maximum number of iterations
 ** @param f          HIKM tree.
 ** @param max_niters maximum number of iterations.
 **/
 
VL_INLINE void
vl_hikm_set_max_niters (VlHIKMTree *f, int max_niters)
{
  f-> max_niters = max_niters ;  
}

/* VL_HIKMEANS_H */
#endif
