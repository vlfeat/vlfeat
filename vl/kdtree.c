/** @file   kdtree.c
 ** @brief  KD-tree - Definition
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include "kdtree.h"
#include "generic.h"
#include "random.h"
#include "mathop.h"

#include <stdlib.h>

/** @file kdtree.h
 **
 ** ::VlKDTree implements a KD-tree object data structure useful to
 ** index moderately dimensional vector spaces. It can be used to
 ** quickly match two groups of feature descriptors.
 **
 ** - @ref kdtree-overview
 ** - @ref kdtree-tech
 **
 ** @section kdtree-overview Overview
 **
 ** To create a ::VlKDForest object use ::vl_kdforest_new specifying
 ** the dimensionality of the data and the number of trees in the
 ** forest.  With one tree only, the algorithm is analogous to [1]
 ** (best-bin KDTree). Multiple trees correspond to the randomized
 ** KDTree forest as in [2,3].
 **
 ** To let the KD-tree index some data use ::vl_kdforest_build. Note
 ** that for efficiency KD-tree does not copy the data but retains a
 ** pointer to it. Therefore the data must exist (and not change)
 ** until the KD-tree is deleted. To delete the KD-tree object, use
 ** ::vl_kdforest_delete.
 **
 ** To find the N nearest neighbors to a query point use
 ** ::vl_kdforest_query. To set a maximum number of comparisons per
 ** query and calculate approximate nearest neighbors use
 ** ::vl_kdforest_set_max_num_comparisons.
 **
 ** @section kdtree-tech Technical details
 ** @sa @ref kdtree-references
 **
 ** KDTree implements the best-bin-first kd-tree of [1]. Given a set
 ** of points @f$ x_1,\dots,x_n \in \mathbb{R}^d @f$, the algorithm
 ** recursively partitions the @e d dimensional Euclidean space @f$
 ** \mathbb{R}^d @f$ into (hyper-) rectangles.
 **
 ** Partitions are organized into a binary tree with the root
 ** corresponding to the whole space @f$ \mathbb{R}^d @f$. The
 ** algorithm refines each partition by dividing it into two halves by
 ** thresholding along a given dimension. Both the splitting dimension
 ** and the threshold are determined as a statistic of the data points
 ** contained in the partition. The splitting dimension is the one
 ** which has largest sample variance and the splitting threshold is
 ** either the sample mean or the median. Leaves are atomic partitions
 ** and they contain a list of zero or more data points (typically
 ** one).
 **
 ** Querying amounts to finding the N data points closer to a given
 ** query point @f$ x_q \in \mathbb{R}^d @f$. This is done by
 ** branch-and-bound. A search state is an active partition (initially
 ** the root) and it is weighed by the lower bound on the distance of
 ** any point in the partition and the query point. Such a lower bound
 ** is trivial to compute because partitions are hyper-rectangles.
 **
 ** @section kdtree-references References
 **
 ** [1] J. S. Beis and D. G. Lowe. Shape indexing using approximate
 ** nearest-neighbour search in high-dimensional spaces. In
 ** Proc. CVPR, 1997.
 **
 ** [2] C. Silpa-Anan and R. Hartley. Optimised KD-trees for fast
 ** image descriptor matching. In Proc. CVPR, 2008.
 **
 ** [3] M. Muja and D. G. Lowe. Fast approximate nearest neighbors
 ** with automatic algorithmic configuration. In Proc. VISAPP, 2009.
 **/

#define NAME vl_kdforest_search_heap
#define T VlKDForestSearchState
#define CMP(x,y) (x->distanceLowerBound - y->distanceLowerBound)
#include "heap-t.h"
#undef CMP
#undef T
#undef NAME

#define NAME vl_kdtree_split_heap
#define T VlKDTreeSplitDimension
#define CMP(x,y) (x->variance - y->variance)
#include "heap-t.h"
#undef CMP
#undef T
#undef NAME

#define NAME vl_kdforest_neighbor_heap
#define T VlKDForestNeighbor
#define CMP(x,y) (y->distance - x->distance)
#include "heap-t.h"
#undef CMP
#undef T
#undef NAME

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Allocate a new node from the tree pool
 **
 **/

int unsigned
vl_kdtree_node_new (VlKDTree * tree, int unsigned parentIndex)
{
  VlKDTreeNode * node = NULL ;
  int unsigned nodeIndex = tree->numUsedNodes ;
  tree -> numUsedNodes += 1 ;

  assert (tree->numUsedNodes <= tree->numAllocatedNodes) ;

  node = tree->nodes + nodeIndex ;
  node -> parent = parentIndex ;
  node -> lowerChild = 0 ;
  node -> upperChild = 0 ;
  node -> splitDimension = 0 ;
  node -> splitThreshold = 0 ;
  return nodeIndex ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Compare KDTree index entries for sorting
 **/

static int
vl_kdtree_compare_index_entries (void const * a,
                                 void const * b)
{
  float delta =
   ((VlKDTreeDataIndexEntry const*)a) -> value -
   ((VlKDTreeDataIndexEntry const*)b) -> value ;
  if (delta < 0) return -1 ;
  if (delta > 0) return +1 ;
  return 0 ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Build KDTree recursively
 ** @param tree KDTree object.
 ** @param nodeIndex node to process.
 ** @param dataBegin begin of data for this node.
 ** @param dataEnd end of data for this node.
 ** @param depth depth of this node.
 **/

static void
vl_kdtree_build_recursively
(VlKDForest * forest,
 VlKDTree * tree, unsigned int nodeIndex,
 unsigned int dataBegin, unsigned int dataEnd,
 unsigned int depth)
{
  int d, i, medianIndex, splitIndex ;
  VlKDTreeNode * node = tree->nodes + nodeIndex ;
  VlKDTreeSplitDimension * splitDimension ;

  /* base case: there is only one data point */
  if (dataEnd - dataBegin <= 1) {
    if (tree->depth < depth) tree->depth = depth ;
    node->lowerChild = - dataBegin - 1;
    node->upperChild = - dataEnd - 1 ;
    return ;
  }

  /* compute the dimension with largest variance */
  forest->splitHeapNumNodes = 0 ;
  for (d = 0 ; d < forest->numDimensions ; ++ d) {
    float mean = 0 ; /* unnormalized */
    float secondMoment = 0 ;
    float variance = 0 ;
    for (i = dataBegin ; i < dataEnd ; ++ i) {
      int di = tree -> dataIndex [i] .index ;
      float const datum = forest->data [di * forest->numDimensions + d] ;
      mean += datum ;
      secondMoment += datum * datum ;
    }
    mean /= (dataEnd - dataBegin) ;
    secondMoment /= (dataEnd - dataBegin) ;
    variance = secondMoment - mean * mean ;

    /* keep splitHeapSize most varying dimensions */
    if (forest->splitHeapNumNodes < forest->splitHeapSize) {
      VlKDTreeSplitDimension * splitDimension
        = forest->splitHeapArray + forest->splitHeapNumNodes ;
      splitDimension->dimension = d ;
      splitDimension->mean = mean ;
      splitDimension->variance = variance ;
      vl_kdtree_split_heap_push (forest->splitHeapArray, &forest->splitHeapNumNodes) ;
    } else {
      VlKDTreeSplitDimension * splitDimension = forest->splitHeapArray + 0 ;
      if (splitDimension->variance < variance) {
        splitDimension->dimension = d ;
        splitDimension->mean = mean ;
        splitDimension->variance = variance ;
        vl_kdtree_split_heap_update (forest->splitHeapArray, forest->splitHeapNumNodes, 0) ;
      }
    }
  }

  /* toss a dice to decide the splitting dimension */
  splitDimension = forest->splitHeapArray
  + (vl_rand_uint32() % VL_MIN(forest->splitHeapSize, forest->splitHeapNumNodes)) ;

  /* additional base case: variance is equal to 0 (overlapping points) */
  if (splitDimension->variance == 0) {
    node->lowerChild = - dataBegin - 1 ;
    node->upperChild = - dataEnd - 1 ;
    return ;
  }
  node->splitDimension = splitDimension->dimension ;

  /* sort data along largest variance dimension */
  for (i = dataBegin ; i < dataEnd ; ++ i) {
    int di = tree->dataIndex [i] .index ;
    float datum = forest->data [di * forest->numDimensions + splitDimension->dimension] ;
    tree->dataIndex [i] .value = datum ;
  }
  qsort (tree->dataIndex + dataBegin,
         dataEnd - dataBegin,
         sizeof (VlKDTreeDataIndexEntry),
         vl_kdtree_compare_index_entries) ;

  /* determine split threshold */
  switch (forest->thresholdingMethod) {
    case VL_KDTREE_MEAN :
      node->splitThreshold = splitDimension->mean ;
      for (splitIndex = dataBegin ;
           splitIndex < dataEnd && tree->dataIndex[splitIndex].value <= node->splitThreshold ;
           ++ splitIndex) ;
      splitIndex -= 1 ;
      /* If the mean does not provide a proper partition, fall back to
       * median. This usually happens if all points have the same
       * value and the zero variance test fails for numerical accuracy
       * reasons. In this case, also due to numerical accuracy, the
       * mean value can be smaller, equal, or larger than all
       * points. */
      if (dataBegin <= splitIndex && splitIndex + 1 < dataEnd) break ;

    case VL_KDTREE_MEDIAN :
      medianIndex = (dataBegin + dataEnd - 1) / 2 ;
      splitIndex = medianIndex ;
      node -> splitThreshold = tree->dataIndex[medianIndex].value ;
      break ;

    default:
      assert(0) ;
  }

  /* divide subparts */
  node->lowerChild = vl_kdtree_node_new (tree, nodeIndex) ;
  vl_kdtree_build_recursively (forest, tree, node->lowerChild, dataBegin, splitIndex + 1, depth + 1) ;

  node->upperChild = vl_kdtree_node_new (tree, nodeIndex) ;
  vl_kdtree_build_recursively (forest, tree, node->upperChild, splitIndex + 1, dataEnd, depth + 1) ;
}

/** ------------------------------------------------------------------
 ** @brief Create new KDForest object
 ** @param numDimensions data dimensionality.
 ** @param numTrees number of trees in the forest.
 ** @return new KDForest.
 **/

VL_EXPORT
VlKDForest *
vl_kdforest_new (int unsigned numDimensions, int unsigned numTrees)
{
  VlKDForest * self = vl_malloc (sizeof(VlKDForest)) ;

  self -> numData = 0 ;
  self -> data = 0 ;
  self -> numDimensions = numDimensions ;
  self -> numTrees = numTrees ;
  self -> trees = 0 ;
  self -> thresholdingMethod = VL_KDTREE_MEDIAN ;
  self -> splitHeapSize = (numTrees == 1) ? 1 : VL_KDTREE_SPLIT_HEALP_SIZE ;
  self -> splitHeapNumNodes = 0 ;

  self -> searchHeapArray = 0 ;
  self -> searchHeapNumNodes = 0 ;


  self -> searchMaxNumComparisons = 0 ;
  self -> searchIdBook = 0 ;
  self -> searchId = 0 ;

  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Delete KDForest object
 ** @param tree KDForest object to delete
 ** @sa ::vl_kdforest_new
 **/

VL_EXPORT void
vl_kdforest_delete (VlKDForest * self)
{
  int ti ;
  if (self->searchIdBook) vl_free (self->searchIdBook) ;
  if (self->trees) {
    for (ti = 0 ; ti < self->numTrees ; ++ ti) {
      if (self->trees[ti]) {
        if (self->trees[ti]->nodes) vl_free (self->trees[ti]->nodes) ;
        if (self->trees[ti]->dataIndex) vl_free (self->trees[ti]->dataIndex) ;
      }
    }
    vl_free (self->trees) ;
  }
  vl_free (self) ;
}

/** ------------------------------------------------------------------
 ** @brief Build KDTree from data
 ** @param tree KDTree object
 ** @param numData number of data points.
 ** @param data pointer to the data.
 **
 ** The function builds the KDTree by processing the data @a data. For
 ** efficiency, KDTree does not copy the data, but retains a pointer to it.
 ** Therefore the data must survive (and not change) until the KDTree
 ** is deleted.
 **/

VL_EXPORT void
vl_kdforest_build (VlKDForest * self, int numData, float const * data)
{
  int di, ti ;

  /* need to check: if alredy built, clean first */
  self->data = data ;
  self->numData = numData ;
  self->trees = vl_malloc (sizeof(VlKDTree*) * self->numTrees) ;

  for (ti = 0 ; ti < self->numTrees ; ++ ti) {
    self->trees[ti] = vl_malloc (sizeof(VlKDTree)) ;
    self->trees[ti]->dataIndex = vl_malloc (sizeof(VlKDTreeDataIndexEntry) * self->numData) ;
    for (di = 0 ; di < self->numData ; ++ di) {
      self->trees[ti]->dataIndex[di].index = di ;
    }
    self->trees[ti]->numUsedNodes = 0 ;
    /* num. nodes of a complete binary tree with numData leaves */
    self->trees[ti]->numAllocatedNodes = 2 * self->numData - 1 ;
    self->trees[ti]->nodes = vl_malloc (sizeof(VlKDTreeNode) * self->trees[ti]->numAllocatedNodes) ;
    self->trees[ti]->depth = 0 ;
    vl_kdtree_build_recursively (self, self->trees[ti],
                                 vl_kdtree_node_new(self->trees[ti], 0), 0,
                                 self->numData, 0) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief
 **/

VL_INLINE
float calc_dist2 (float const * a, float const * b, int unsigned N)
{
  float acc = 0 ;
  float d0, d1, d2, d3 ;
  float const * age = a + N - 3 ;
  float const * ae = a + N ;

  while (a < age) {
    d0 = *a++ - *b++ ;
    d1 = *a++ - *b++ ;
    d2 = *a++ - *b++ ;
    d3 = *a++ - *b++ ;
    acc += d0 * d0 ;
    acc += d1 * d1 ;
    acc += d2 * d2 ;
    acc += d3 * d3 ;
  }
  while (a < ae) {
    d0 = *a++ - *b++ ;
    acc += d0 * d0 ;
  }
  return acc ;
}

/** ------------------------------------------------------------------
 ** @internal @brief
 **/

VL_EXPORT int
vl_kdforest_query_recursively (VlKDForest  * self,
                               VlKDTree * tree,
                               int unsigned nodeIndex,
                               VlKDForestNeighbor * neighbors,
                               int unsigned numNeighbors,
                               int unsigned * numAddedNeighbors,
                               float dist,
                               float const * query)
{
  VlKDTreeNode const * node = tree->nodes + nodeIndex ;
  int unsigned i = node->splitDimension ;
  int nextChild, saveChild ;
  float delta, saveDist ;
  float x = query [i] ;
  float x1 = node->lowerBound ;
  float x2 = node->splitThreshold ;
  float x3 = node->upperBound ;
  VlKDForestSearchState * searchState ;

  self->searchNumRecursions ++ ;

  /* base case: this is a leaf node */
  if (node->lowerChild < 0) {
    int begin = - node->lowerChild - 1 ;
    int end   = - node->upperChild - 1 ;
    int iter ;

    for (iter = begin ;
         iter < end &&
         (self->searchMaxNumComparisons == 0 ||
          self->searchNumComparisons < self->searchMaxNumComparisons) ;
         ++ iter) {

      int unsigned di = tree->dataIndex [iter].index ;
      float const * datum = self->data + di * self->numDimensions ;

      /* multiple KDTrees share the database points and we must avoid
       * adding the same point twice */
      if (self->searchIdBook [di] == self->searchId) continue ;
      self->searchIdBook [di] = self->searchId ;

      /* compare the query to this point */
      dist = calc_dist2 (query, datum, self->numDimensions) ;
      self->searchNumComparisons += 1 ;

      /* see if it should be added to the result set */
      if (*numAddedNeighbors < numNeighbors) {
        VlKDForestNeighbor * newNeighbor = neighbors + *numAddedNeighbors ;
        newNeighbor->index = di ;
        newNeighbor->distance = dist ;
        vl_kdforest_neighbor_heap_push (neighbors, numAddedNeighbors) ;
      } else {
        VlKDForestNeighbor * largestNeighbor = neighbors + 0 ;
        if (largestNeighbor->distance > dist) {
          largestNeighbor->index = di ;
          largestNeighbor->distance = dist ;
          vl_kdforest_neighbor_heap_update (neighbors, *numAddedNeighbors, 0) ;
        }
      }
    } /* next data point */

    return nodeIndex ;
  }

#if 0
  assert (x1 <= x2 && x2 <= x3) ;
  assert (node->lowerChild >= 0) ;
  assert (node->upperChild >= 0) ;
#endif

  /*
   *   x1  x2 x3
   * x (---|---]
   *   (--x|---]
   *   (---|x--]
   *   (---|---] x
   */

  delta = x - x2 ;
  saveDist = dist + delta*delta ;

  if (x <= x2) {
    nextChild = node->lowerChild ;
    saveChild = node->upperChild ;
    if (x <= x1) {
      delta = x - x1 ;
      saveDist -= delta*delta ;
    }
  } else {
    nextChild = node->upperChild ;
    saveChild = node->lowerChild ;
    if (x > x3) {
      delta = x - x3 ;
      saveDist -= delta*delta ;
    }
  }

  if (*numAddedNeighbors < numNeighbors || neighbors[0].distance > saveDist) {
    searchState = self->searchHeapArray + self->searchHeapNumNodes ;
    searchState->tree = tree ;
    searchState->nodeIndex = saveChild ;
    searchState->distanceLowerBound = saveDist ;
    vl_kdforest_search_heap_push (self->searchHeapArray,
                                  &self->searchHeapNumNodes) ;
  }

  return vl_kdforest_query_recursively (self,
                                        tree,
                                        nextChild,
                                        neighbors,
                                        numNeighbors,
                                        numAddedNeighbors,
                                        dist,
                                        query) ;
}

/** ------------------------------------------------------------------
 ** @internal @brief Compute tree bounds recursively
 ** @param self KDTree object instance.
 ** @param nodeIndex node index to start from.
 ** @param searchBounds 2 x numDimension array of bounds.
 **/

static void
vl_kdtree_calc_bounds_recursively (VlKDTree * tree,
                                   int unsigned nodeIndex, float * searchBounds)
{
  VlKDTreeNode * node = tree->nodes + nodeIndex ;
  int unsigned i = node->splitDimension ;
  float t = node->splitThreshold ;

  node->lowerBound = searchBounds [2 * i + 0] ;
  node->upperBound = searchBounds [2 * i + 1] ;

  if (node->lowerChild > 0) {
    searchBounds [2 * i + 1] = t ;
    vl_kdtree_calc_bounds_recursively (tree, node->lowerChild, searchBounds) ;
    searchBounds [2 * i + 1] = node->upperBound ;
  }
  if (node->upperChild > 0) {
    searchBounds [2 * i + 0] = t ;
    vl_kdtree_calc_bounds_recursively (tree, node->upperChild, searchBounds) ;
    searchBounds [2 * i + 0] = node->lowerBound ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Query operation
 ** @param self KDTree object instance.
 ** @param neighbors list of nearest neighbors found (output).
 ** @param numNeighbors number of nearest neighbors to find.
 ** @param query query point.
 ** @return number of tree leaves visited.
 **
 ** A neighbor is represented by an instance of the structure
 ** ::VlKDForestNeighbor. Each entry contains the index of the
 ** neighbor (this is an index into the KDTree data) and its distance
 ** to the query point. Neighbors are sorted by increasing distance.
 **/

VL_EXPORT int
vl_kdforest_query (VlKDForest * self,
                   VlKDForestNeighbor * neighbors,
                   int unsigned numNeighbors,
                   float const * query)
{
  int i, ti ;
  vl_bool exactSearch = (self->searchMaxNumComparisons == 0) ;
  VlKDForestSearchState * searchState  ;
  int unsigned numAddedNeighbors = 0 ;

  assert (neighbors) ;
  assert (numNeighbors > 0) ;
  assert (query) ;

  /* this number is used to differentiate a query from the next */
  self -> searchId += 1 ;
  self -> searchNumRecursions = 0 ;

  if (! self -> searchHeapArray) {
    /* count number of tree nodes */
    /* add support structures */
    int maxNumNodes = 0 ;
    for (ti = 0 ; ti < self->numTrees ; ++ti) {
      maxNumNodes += self->trees[ti]->numUsedNodes ;
    }
    self -> searchHeapArray = vl_malloc (sizeof(VlKDForestSearchState) * maxNumNodes) ;
    self -> searchIdBook = vl_calloc (sizeof(unsigned int), self->numData) ;

    for (ti = 0 ; ti < self->numTrees ; ++ti) {
      float * searchBounds = vl_malloc(sizeof(float) * 2 * self->numDimensions) ;
      float * iter = searchBounds  ;
      float * end = iter + 2 * self->numDimensions ;
      while (iter < end) {
        *iter++ = - VL_INFINITY_F ;
        *iter++ = + VL_INFINITY_F ;
      }
      vl_kdtree_calc_bounds_recursively (self->trees[ti], 0, searchBounds) ;
      vl_free (searchBounds) ;
    }
  }

  self->searchNumComparisons = 0 ;
  self->searchNumSimplifications = 0 ;

  /* put the root node into the search heap */
  self->searchHeapNumNodes = 0 ;
  for (ti = 0 ; ti < self->numTrees ; ++ ti) {
    searchState = self->searchHeapArray + self->searchHeapNumNodes ;
    searchState -> tree = self->trees[ti] ;
    searchState -> nodeIndex = 0 ;
    searchState -> distanceLowerBound = 0 ;
    vl_kdforest_search_heap_push (self->searchHeapArray, &self->searchHeapNumNodes) ;
  }

  /* branch and bound */
  while (exactSearch || self->searchNumComparisons < self->searchMaxNumComparisons)
  {
    /* pop the next optimal search node */
    VlKDForestSearchState * searchState = vl_kdforest_search_heap_pop
      (self->searchHeapArray, &self->searchHeapNumNodes) ;

    /* break if search space completed */
    if (searchState == NULL) {
      break ;
    }

    /* break if no better solution may exist */
    if (numAddedNeighbors == numNeighbors &&
        neighbors[0].distance < searchState->distanceLowerBound) {
      self->searchNumSimplifications ++ ;
      break ;
    }

    vl_kdforest_query_recursively (self,
                                   searchState->tree,
                                   searchState->nodeIndex,
                                   neighbors,
                                   numNeighbors,
                                   &numAddedNeighbors,
                                   searchState->distanceLowerBound,
                                   query) ;
  }

  /* sort neighbors by increasing distance */
  for (i = numAddedNeighbors ;  i < numNeighbors ; ++ i) {
    neighbors[i].index = -1 ;
    neighbors[i].distance = VL_NAN_F ;
  }
  while (numAddedNeighbors) {
    vl_kdforest_neighbor_heap_pop (neighbors, &numAddedNeighbors) ;
  }

  return self->searchNumComparisons ;
}
