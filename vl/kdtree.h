/** @file   kdtree.h
 ** @brief  KD-tree
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_KDTREE_H
#define VL_KDTREE_H

#include "generic.h"

#define VL_KDTREE_SPLIT_HEALP_SIZE 5

typedef struct _VlKDTreeNode VlKDTreeNode ;
typedef struct _VlKDTreeSplitDimension VlKDTreeSplitDimension ;
typedef struct _VlKDTreeDataIndexEntry VlKDTreeDataIndexEntry ;
typedef struct _VlKDForestSearchState VlKDForestSearchState ;

struct _VlKDTreeNode
{
  int unsigned parent ;
  int lowerChild ;
  int upperChild ;
  int unsigned splitDimension ;
  float splitThreshold ;
  float lowerBound ;
  float upperBound ;
} ;

struct _VlKDTreeSplitDimension
{
  int unsigned dimension ;
  float mean ;
  float variance ;
} ;

struct _VlKDTreeDataIndexEntry
{
  int index ;
  float value ;
} ;

/** @brief Thresholding method */
typedef enum _VlKDTreeThresholdingMethod
{
  VL_KDTREE_MEDIAN,
  VL_KDTREE_MEAN
} VlKDTreeThresholdingMethod ;

/** @brief Neighbor of a query point */
typedef struct _VlKDForestNeighbor {
  float distance ;     /**< distance to the query point */
  unsigned int index ; /**< index of the neighbor in the KDTree data */
} VlKDForestNeighbor ;

typedef struct _VlKDTree
{
  VlKDTreeNode * nodes ;
  int unsigned numUsedNodes ;
  int unsigned numAllocatedNodes ;
  VlKDTreeDataIndexEntry * dataIndex ;
  int depth ;
} VlKDTree ;

struct _VlKDForestSearchState
{
  VlKDTree * tree ;
  unsigned int nodeIndex ;
  float distanceLowerBound ;
} ;

/** @brief KDForest object */
typedef struct _VlKDForest
{
  int numDimensions ;

  /* indexed data */
  float const * data ;
  int numData ;

  /* tree structure */
  VlKDTree ** trees ;
  unsigned int numTrees ;

  /* build */
  VlKDTreeThresholdingMethod thresholdingMethod ;
  VlKDTreeSplitDimension splitHeapArray [VL_KDTREE_SPLIT_HEALP_SIZE] ;
  int unsigned splitHeapNumNodes ;
  int unsigned splitHeapSize ;

  /* querying */
  VlKDForestSearchState * searchHeapArray ;
  int unsigned searchHeapNumNodes ;
  int unsigned searchId ;
  int unsigned * searchIdBook ;

  int unsigned searchMaxNumComparisons ;
  int unsigned searchNumComparisons;
  int unsigned searchNumRecursions ;
  int unsigned searchNumSimplifications ;
} VlKDForest ;

/** @name Creatind and disposing
 ** @{ */
VL_EXPORT VlKDForest * vl_kdforest_new (int unsigned numDimensions, int unsigned numTrees) ;
VL_EXPORT void vl_kdforest_delete (VlKDForest * self) ;
/** @} */


/** @name Building and querying
 ** @{ */
VL_EXPORT void vl_kdforest_build (VlKDForest * self, int numData, float const * data) ;
VL_EXPORT int vl_kdforest_query (VlKDForest * self,
                                 VlKDForestNeighbor * neighbors,
                                 int unsigned numNeighbors,
                                 float const * query) ;
/** @} */

/** @name Retrieving and setting parameters
 ** @{ */
VL_INLINE int unsigned vl_kdforest_get_depth_of_tree (VlKDForest const * self, int unsigned treeIndex) ;
VL_INLINE int unsigned vl_kdforest_get_num_nodes_of_tree (VlKDForest const * self, int unsigned treeIndex) ;
VL_INLINE int unsigned vl_kdforest_get_num_trees (VlKDForest const * self) ;
VL_INLINE void vl_kdforest_set_max_num_comparisons (VlKDForest * self, int unsigned n) ;
VL_INLINE int unsigned vl_kdforest_get_max_num_comparisons (VlKDForest * self) ;
VL_INLINE void vl_kdforest_set_thresholding_method (VlKDForest * self, VlKDTreeThresholdingMethod method) ;
VL_INLINE VlKDTreeThresholdingMethod vl_kdforest_get_thresholding_method (VlKDForest const * self) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get the number of nodes of a given tree
 ** @param self KDForest object.
 ** @param treeIndex index of the tree.
 ** @return number of trees.
 **/

int unsigned
vl_kdforest_get_num_nodes_of_tree (VlKDForest const * self, int unsigned treeIndex)
{
  assert (treeIndex < self->numTrees) ;
  return self->trees[treeIndex]->numUsedNodes ;
}

/** ------------------------------------------------------------------
 ** @brief Get the detph of a given tree
 ** @param self KDForest object.
 ** @param treeIndex index of the tree.
 ** @return number of trees.
 **/

int unsigned
vl_kdforest_get_depth_of_tree (VlKDForest const * self, int unsigned treeIndex)
{
  assert (treeIndex < self->numTrees) ;
  return self->trees[treeIndex]->depth ;
}

/** ------------------------------------------------------------------
 ** @brief Get the number of trees in the forest
 **
 ** @param self KDForest object.
 ** @return number of trees.
 **/

int unsigned
vl_kdforest_get_num_trees (VlKDForest const * self)
{
  return self->numTrees ;
}

/** ------------------------------------------------------------------
 ** @brief Set the maximum number of comparisons for a search
 **
 ** @param self KDForest object.
 ** @param n maximum number of leaves.
 **
 ** This function sets the maximum number of comparisons for a
 ** nearest neighbor search. Setting it to 0 means unbounded comparisons.
 **
 ** @sa ::vl_kdforest_query, ::vl_kdforest_get_max_num_comparisons.
 **/

void
vl_kdforest_set_max_num_comparisons (VlKDForest * self, int unsigned n)
{
  self->searchMaxNumComparisons = n ;
}

/** ------------------------------------------------------------------
 ** @brief Get the maximum number of comparisons for a search
 **
 ** @param self KDForest object.
 ** @return maximum number of leaves.
 **
 ** @sa ::vl_kdforest_set_max_num_comparisons.
 **/

int unsigned
vl_kdforest_get_max_num_comparisons (VlKDForest * self)
{
  return self->searchMaxNumComparisons ;
}

/** ------------------------------------------------------------------
 ** @brief Set the thresholding method
 ** @param self KDForest object.
 ** @param method one of ::VlKDTreeThresholdingMethod.
 **
 ** @sa ::vl_kdforest_get_thresholding_method
 **/

VL_INLINE void
vl_kdforest_set_thresholding_method (VlKDForest * self, VlKDTreeThresholdingMethod method)
{
  assert(VL_KDTREE_MEDIAN <= method && method <= VL_KDTREE_MEAN) ;
  self->thresholdingMethod = method ;
}

/** ------------------------------------------------------------------
 ** @brief Get the thresholding method
 **
 ** @param self KDForest object.
 ** @return thresholding method.
 **
 ** @sa ::vl_kdforest_set_thresholding_method
 **/

VL_INLINE VlKDTreeThresholdingMethod
vl_kdforest_get_thresholding_method (VlKDForest const * self)
{
  return self->thresholdingMethod ;
}

/* VL_KDTREE_H */
#endif
