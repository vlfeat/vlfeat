/** @file     quickshift.h
 ** @brief    Quick shift image segmentation
 ** @author   Andrea Vedaldi
 ** @author   Brian Fulkerson
 **/
 
/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_QUICKSHIFT_H
#define VL_QUICKSHIFT_H

#include "generic.h"
#include "mathop.h"

/** @brief quick shift datatype */
typedef double vl_qs_type ;

/** @brief quick shift infinity constant */
#define VL_QS_INF VL_INFINITY_D /* Change to _F for float math */

/** ------------------------------------------------------------------
 ** @brief quick shift results
 **
 ** This implements quick shift mode seeking.
 **/

typedef struct _VlQS
{
  vl_qs_type *image ;   /**< height x width x channels feature image */
  int height;           /**< height of the image */
  int width;            /**< width of the image */
  int channels;         /**< number of channels in the image */

  vl_bool medoid;
  vl_qs_type sigma;
  vl_qs_type tau;
 
  int *parents ;
  vl_qs_type *dists ;
  vl_qs_type *density ;
} VlQS ;

/** @name Create and destroy
 ** @{
 **/
VL_EXPORT
VlQS*  vl_quickshift_new (vl_qs_type const * im, int height, int width,
                          int channels);

VL_EXPORT
void   vl_quickshift_delete (VlQS *q) ;
/** @} */

/** @name Process data
 ** @{
 **/

VL_EXPORT
void   vl_quickshift_process (VlQS *q) ;

/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE vl_qs_type    vl_quickshift_get_max_dist      (VlQS const *q) ;
VL_INLINE vl_qs_type    vl_quickshift_get_kernel_size    (VlQS const *q) ;
VL_INLINE vl_bool       vl_quickshift_get_medoid   (VlQS const *q) ;

VL_INLINE int *        vl_quickshift_get_parents  (VlQS const *q) ;
VL_INLINE vl_qs_type * vl_quickshift_get_dists    (VlQS const *q) ;
VL_INLINE vl_qs_type * vl_quickshift_get_density  (VlQS const *q) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
VL_INLINE void vl_quickshift_set_max_dist    (VlQS *f, vl_qs_type tau) ;
VL_INLINE void vl_quickshift_set_kernel_size  (VlQS *f, vl_qs_type sigma) ;
VL_INLINE void vl_quickshift_set_medoid (VlQS *f, vl_bool medoid) ;
/** @} */

/* -------------------------------------------------------------------
 *                                     Inline functions implementation
 * ---------------------------------------------------------------- */

/** ------------------------------------------------------------------
 ** @brief Get tau.
 ** @param q quick shift object.
 ** @return the maximum distance in the feature space between nodes in the
 **         quick shift tree.  
 **/

VL_INLINE vl_qs_type
vl_quickshift_get_max_dist (VlQS const *q) 
{
  return q->tau ;
}

/** ------------------------------------------------------------------
 ** @brief Get sigma.
 ** @param q quick shift object.
 ** @return the standard deviation of the kernel used in the Parzen density
 **         estimate.  
 **/

VL_INLINE vl_qs_type
vl_quickshift_get_kernel_size (VlQS const *q) 
{
  return q->sigma ;
}

/** ------------------------------------------------------------------
 ** @brief Get medoid.
 ** @param q quick Shift object.
 ** @return @c true if medoid shift is used instead of quick shift.
 **/

VL_INLINE vl_bool
vl_quickshift_get_medoid (VlQS const *q) 
{
  return q->medoid ;
}

/** ------------------------------------------------------------------
 ** @brief Get parents.
 ** @param q quick shift object.
 ** @return a height x width matrix containing linearized links to the
 **         parents in the quick shift tree. If there is no parent, the
 **         parent will be set to the index of the node.
 **/

VL_INLINE int *
vl_quickshift_get_parents (VlQS const *q) 
{
  return q->parents ;
}

/** ------------------------------------------------------------------
 ** @brief Get dists.
 ** @param q quick shift object.
 ** @return for each pixel, the distance in feature space to the pixel
 **         that is its parent in the quick shift tree.
 **/

VL_INLINE vl_qs_type *
vl_quickshift_get_dists (VlQS const *q) 
{
  return q->dists ;
}

/** ------------------------------------------------------------------
 ** @brief Get density.
 ** @param q quick shift object.
 ** @return the estimate of the density at each pixel.
 **/

VL_INLINE vl_qs_type *
vl_quickshift_get_density (VlQS const *q) 
{
  return q->density ;
}

/** ------------------------------------------------------------------
 ** @brief Set sigma
 ** @param q quick shift object.
 ** @param sigma standard deviation of the kernel used in the Parzen density
 **        estimate.  
 **/

VL_INLINE void
vl_quickshift_set_kernel_size (VlQS *q, vl_qs_type sigma) 
{
  q -> sigma = sigma ;
}

/** ------------------------------------------------------------------
 ** @brief Set max distance 
 ** @param q quick shift object.
 ** @param tau the maximum distance in the feature space between nodes in the
 **            quick shift tree.  
 **/

VL_INLINE void
vl_quickshift_set_max_dist (VlQS *q, vl_qs_type tau) 
{
  q -> tau = tau ;
}

/** ------------------------------------------------------------------
 ** @brief Set medoid
 ** @param q quick shift object.
 ** @param medoid @c true to use kernelized medoid shift, @c false (default) uses
 **        quick shift.
 **/

VL_INLINE void
vl_quickshift_set_medoid (VlQS *q, vl_bool medoid) 
{
  q -> medoid = medoid ;
}


#endif
