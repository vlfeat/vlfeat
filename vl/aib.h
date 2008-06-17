/** @file   aib.h
 ** @author Brian Fulkerson
 ** @brief  Agglomerative Information Bottleneck (AIB)
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_AIB_H
#define VL_AIB_H

#include "generic.h"

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief AIB algorithm data
 **
 ** The implementation is quite straightforward, but the way feature
 ** values are handled in order to support efficiently joins,
 ** deletions and re-arrangement needs to be explaiend. This is
 ** achieved by a layer of indrirection:
 ** - Call each feature value (either original or obtained by a join
 **   operation) <em>node</em>. Nodes are idenitfied by numbers.
 ** - Call the elements of various array (such as VlAIB::Px)
 **    <em>entry</em>.
 ** - Entry are dynamically associated to nodes as specified by
 **   VlAIB::nodes. So @c Px[i] actually refers to the node @c
 **   nodes[i].
 **/

typedef struct _VlAIB
{
  vl_uint   *nodes ;    /**< Entires to nodes */
  vl_uint    nentries ; /**< Total number of entries (= # active nodes) */
  double    *beta ;     /**< Minimum distance to an entry  */
  vl_uint   *bidx ;     /**< Closest entry */


  vl_uint   *which ;    /**< List of entries to update */
  vl_uint    nwhich ;   /**< Number of entries to update */
  
  double    *Pcx;       /**< Joint probability table */
  double    *Px;        /**< Marginal. */
  double    *Pc;        /**< Marginal. */
  vl_uint    nvalues;   /**< Number of feature values */
  vl_uint    nlabels;   /**< Number of labels */

  vl_uint   *parents;   /**< Array of parents */
  double    *costs;     /**< Cost of each merge */
} VlAIB;

/** @name Create and destroy
 ** @{
 **/

VL_EXPORT
VlAIB * vl_aib_new(double * Pcx, vl_uint nvalues, vl_uint nlabels);

VL_EXPORT 
void vl_aib_delete (VlAIB * aib);

/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT 
void vl_aib_process(VlAIB * aib);

/** @} */

/** @name Retrieve results
 ** @{
 **/
VL_INLINE vl_uint * vl_aib_get_parents(VlAIB const * aib);
VL_INLINE double  * vl_aib_get_costs(VlAIB const * aib);

/** @} */


/* -------------------------------------------------------------------
 *                                     Inline functions implementation
 * ---------------------------------------------------------------- */

/** ------------------------------------------------------------------
 ** @brief Get resulting list of parents
 ** @param aib AIB filter.
 ** @return An array of parents
 **/
VL_INLINE 
vl_uint * vl_aib_get_parents(VlAIB const * aib)
{
  return aib->parents;
}


/** ------------------------------------------------------------------
 ** @brief Get a list of merge costs
 ** @param aib AIB filter.
 ** @return An array of costs
 **/
VL_INLINE 
double * vl_aib_get_costs(VlAIB const * aib)
{
  return aib->costs;
}

/* VL_AIB_H */
#endif
