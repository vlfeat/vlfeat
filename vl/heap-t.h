/** @file   heap-t.h
 ** @brief  Heap template
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include "generic.h"
#include "heap.h"

#define VL_HEAP_DEF_FUNC(NAME, SUFFIX) NAME ## SUFFIX
#define VL_HEAP_SWAP(NAME)   VL_HEAP_DEF_FUNC(NAME,_swap)
#define VL_HEAP_UP(NAME)     VL_HEAP_DEF_FUNC(NAME,_up)
#define VL_HEAP_DOWN(NAME)   VL_HEAP_DEF_FUNC(NAME,_down)
#define VL_HEAP_PUSH(NAME)   VL_HEAP_DEF_FUNC(NAME,_push)
#define VL_HEAP_POP(NAME)    VL_HEAP_DEF_FUNC(NAME,_pop)
#define VL_HEAP_UPDATE(NAME) VL_HEAP_DEF_FUNC(NAME,_update)

/** --------------------------------------------------------------- */

VL_INLINE void
VL_HEAP_SWAP(NAME) (T * a, T * b)
{
  T t = *a ;
  *a = *b ;
  *b = t ;
}

/** --------------------------------------------------------------- */

VL_INLINE void
VL_HEAP_UP(NAME)
(T * nodeArray, int unsigned numNodes, int unsigned nodeIndex)
{
  int unsigned leftIndex  = vl_heap_left_child (nodeIndex) ;
  int unsigned rightIndex = vl_heap_right_child (nodeIndex) ;

  T * node  = nodeArray + nodeIndex ;
  T * left  = nodeArray + leftIndex ;
  T * right = nodeArray + rightIndex ;

  /* no childer: stop */
  if (leftIndex >= numNodes) return ;

  /* only left childer: easy */
  if (rightIndex >= numNodes) {
    if (CMP(node, left) > 0) {
      VL_HEAP_SWAP(NAME) (node, left) ;
    }
    return ;
  }

  /* both childern */
  {
    if (CMP(left, right) < 0) {
      /* swap with left */
      if (CMP(node, left) > 0) {
        VL_HEAP_SWAP(NAME) (node, left) ;
        VL_HEAP_UP(NAME) (nodeArray, numNodes, leftIndex) ;
      }
    } else {
      /* swap with right */
      if (CMP(node, right) > 0) {
        VL_HEAP_SWAP(NAME) (node, right) ;
        VL_HEAP_UP(NAME) (nodeArray, numNodes, rightIndex) ;
      }
    }
  }
}

/** --------------------------------------------------------------- */

VL_INLINE void
VL_HEAP_DOWN(NAME)
(T * nodeArray, int unsigned nodeIndex)
{
  T * node ;
  T * parent ;
  int unsigned parentIndex ;

  if (nodeIndex == 0) return  ;
  parentIndex = vl_heap_parent (nodeIndex) ;

  node   = nodeArray + nodeIndex  ;
  parent = nodeArray + parentIndex ;

  if (CMP(node, parent) < 0) {
    VL_HEAP_SWAP(NAME) (node, parent) ;
    VL_HEAP_DOWN(NAME) (nodeArray, parentIndex) ;
  }
}

/** --------------------------------------------------------------- */

VL_INLINE void
VL_HEAP_PUSH(NAME)
(T * nodeArray, int unsigned *numNodes)
{
  VL_HEAP_DOWN(NAME) (nodeArray, *numNodes) ;
  *numNodes += 1 ;
}

/** --------------------------------------------------------------- */

VL_INLINE T *
VL_HEAP_POP(NAME)
(T * nodeArray, int unsigned *numNodes)
{
  T * root ;
  T * last ;

  if (*numNodes == 0) return NULL ;

  *numNodes -= 1 ;
  root = nodeArray + 0 ;
  last = nodeArray + *numNodes ;

  VL_HEAP_SWAP(NAME) (root, last) ;

  if (*numNodes > 1) {
    VL_HEAP_UP(NAME) (nodeArray, *numNodes, 0) ;
  }
  return last ;
}

/** --------------------------------------------------------------- */

VL_INLINE void
VL_HEAP_UPDATE(NAME)
(T * nodeArray,
 int unsigned numNodes,
 int unsigned nodeIndex)
{
  VL_HEAP_UP(NAME) (nodeArray, numNodes, nodeIndex) ;
  VL_HEAP_DOWN(NAME) (nodeArray, nodeIndex) ;
}