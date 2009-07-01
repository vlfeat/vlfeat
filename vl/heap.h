/** @file   heap.h
 ** @brief  Heap
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#ifndef VL_HEAP_H
#define VL_HEAP_H

#include <stdlib.h>
#include "generic.h"

/** @brief Heap object */
typedef struct _VlHeap
{
  size_t nodeSize ;
  size_t numNodes ;
  void * nodes ;
  void * tempNode ;
  int (*compareFunction)(void const *, void const *) ;
  void (*swapFunction)(void *, void *, void *) ;
  void * swapFunctionCustomData ;
} VlHeap ;

/** @name Creating and disposing
 ** @{ */
VL_EXPORT VlHeap * vl_heap_new  (void * nodes, int nodeSize,
                                 int (*compare)(void const * a, void const * b)) ;
VL_EXPORT void vl_heap_delete (VlHeap * self) ;
/** @} */

/** @name Manipulating nodes
 ** @{ */
VL_INLINE int unsigned vl_heap_get_node_num (VlHeap * self) ;
VL_INLINE void * vl_heap_get_node_with_index (VlHeap * self, int unsigned nodeIndex) ;
VL_INLINE void vl_heap_clear (VlHeap * self) ;
VL_EXPORT void vl_heap_push (VlHeap * self) ;
VL_EXPORT void * vl_heap_pop (VlHeap * self) ;
VL_EXPORT void vl_heap_update (VlHeap * self, int unsigned nodeIndex) ;
/** @} */

/** @name Retrieving and setting paramters
 ** @{ */
VL_EXPORT void vl_heap_set_swap_func (VlHeap *self,
                                      void (*swap)(void * custom , void * a, void * b),
                                      void * customData) ;
/** @} */

/** ------------------------------------------------------------------
 ** @brief Get node with given index
 ** @param self heap object instance.
 ** @param nodeIndex index of the heap node.
 ** @return pointer to the node.
 **/

void *
vl_heap_get_node_with_index (VlHeap * self, int unsigned nodeIndex)
{
  char * rawData = self->nodes ;
  return rawData + self->nodeSize * nodeIndex ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of nodes in the heap
 ** @param self heap object instance.
 ** @return number of nodes in the heap.
 **/

int unsigned
vl_heap_get_node_num (VlHeap * self)
{
  return self->numNodes ;
}

/** ------------------------------------------------------------------
 ** @brief Clear the heap
 ** @param self heap object instance.
 **/

void
vl_heap_clear (VlHeap * self)
{
  self->numNodes = 0 ;
}

/** @internal @name Moving in the heap
 ** @{ */

/** ------------------------------------------------------------------
 ** @internal @brief Get index of parent node
 ** @param nodeIndex a node index.
 ** @return index of the parent node.
 **/

VL_INLINE int unsigned
vl_heap_parent (int unsigned nodeIndex)
{
  if (nodeIndex == 0) return 0 ;
  return (nodeIndex - 1) / 2 ;
}

/** ------------------------------------------------------------------
 ** @internal @brief Get index of left child
 ** @param nodeIndex a node index.
 ** @return index of the left child.
 **/

VL_INLINE int unsigned
vl_heap_left_child (int unsigned nodeIndex)
{
  return 2 * nodeIndex + 1 ;
}

/** ------------------------------------------------------------------
 ** @internal @brief Get index of right child
 ** @param nodeIndex a node index.
 ** @return index of the right child.
 **/

VL_INLINE int unsigned
vl_heap_right_child (int unsigned nodeIndex)
{
  return vl_heap_left_child (nodeIndex) + 1 ;
}

/** @} */

/* VL_HEAP_H */
#endif