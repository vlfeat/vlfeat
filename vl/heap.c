/** @file   heap.c
 ** @brief  Heap - Definition
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include "heap.h"
#include <string.h>

/** @file heap.h

 ::VlHeap implements a heap. The main use of a heap is to
 organize an array of objects (called nodes) in a priority queue.

 - @ref heap-overview
 - @ref heap-tech

 ::VlHeap behavior (node comparison and swapping functions) are set at run-time.
 This incurs in a small overhead, which in most cases is tolerable, but
 in thigt loops might be unacceptable. In such cases, @ref heap-t.h
 can be used instead.

 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
 @section heap-overview Overview
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

 To create a ::VlHeap objet to mange an array of nodes call ::vl_heap_new
 specifying

 - the array <code>nodes</code> of nodes,
 - the size (in bytes) of each node,
 - a function <code>compare</code> specifying an order relation between nodes.

 Notice that the number of nodes into the array is left unspecified. Initially, the
 heap is empty. Calling ::vl_heap_push includes into the heap
 the first node of the array <code>nodes[0]</code>. Further calls to
 ::vl_heap_push include successive nodes. Calling ::vl_heap_pop removes
 and returns
 the smallest node (as specified by <code>compare</code>) from the heap.
 ::vl_heap_clear empties the heap.

 ::VlHeap rearranges the nodes in the heap to maintain the proper
 ordering of the queue (heap property). By default,
 this is done by copying the memory occupied by each node.
 For efficiency nodes should be small objects (consider using pointers).

 ::VlHeap supports updaing node priorities. After changing the priority
 of a node (in the heap), call ::vl_heap_update to update the heap.
 Only one node can be changed at a time.

 To use ::vl_heap_update one needs to know the index of the updated
 node in the heap array. Since nodes are rearranged by the heap operations,
 this is in general different from the index of the node when it was first
 added to the array. This problem may be solved by customizing the node swapping function
 (::vl_heap_set_swap_func) to track the node indexes.

 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
 @section heap-tech Technical details
 <!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

 The heap is stored as a binary tree with the property (<em>heap property</em>)
 that any node is not larger than any of its children. In particular,
 the root is the smallest node.

 ::VlHeap uses the standard binary tree representation as an array
 of nodes. <code>node[0]</code> corresponds to the root, <code>node[1]</code>
 and <code>node[2]</code> to the root left and right children and so on.
 In this way, the tree structure is fully specified by the total
 numeber of nodes.

 Assuming that the heap has <code>N</code> nodes (from
 <code>node[0]</code> to <code>node[N-1]</code>), adding the node
 <code>node[N]</code> to the heap is done by a <em>push down</em> operation:
 if the node <code>node[N]</code> is smaller than its parent (violating the heap property)
 it is pushed down by swapping it with the parent, and so on recursively.

 Removing the smallest element <code>node[0]</code> with an heap of
 <code>N</code> nodes is done by swapping <code>node[0]</code> with
 <code>node[N-1]</code>. If then <code>node[0]</code> is larger
 than any of its children, it is swapped with the smallest of the two
 and so on recursively (<em>push up</em> operation).

 */

VL_INLINE void
vl_heap_swap (VlHeap * self, void * a, void * b)
{
  if (self->swapFunction == NULL) {

    char * ac = a ;
    char * bc = b ;
    char * end = ac + self->nodeSize ;
    char * endg = end - sizeof(int) + 1 ;

    while (ac < endg) {
      int t = ((int*)ac)[0] ;
      ((int*)ac)[0] = ((int*)bc)[0]  ;
      ((int*)bc)[0] = t ;
      ac += 4 ;
      bc += 4 ;
    }
    while (ac < end) {
      char t = *ac ;
      *ac++ = *bc ;
      *bc++ = t ;
    }
  } else {
    (self->swapFunction)(self->swapFunctionCustomData, a, b) ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Set function used to swap nodes
 ** @param self heap object instance.
 ** @param swap swap function.
 ** @param customData pointer to data passed to the swap function.
 **
 ** The swap function @c swap is called whenever nodes are rearranged
 ** in the heap. @c swap must swap the node pointed by @c a with the node
 ** pointed by @c b.
 **
 ** You can customize the swap function to track the index of the nodes
 ** as they are rearranged in the heap.
 **/

VL_EXPORT void
vl_heap_set_swap_func (VlHeap *self,
                       void (*swap)(void * custom , void * a, void * b),
                       void * customData)
{
  self->swapFunction = swap ;
  self->swapFunctionCustomData = customData ;
}

/** ------------------------------------------------------------------
 ** @brief Create new heap object
 ** @param nodes array of nodes.
 ** @param nodeSize size in bytes of a node.
 ** @param compare node comparison function.
 **
 ** @return new heap object instance.
 **/

VL_EXPORT VlHeap * vl_heap_new
(void * nodes, int nodeSize, int (*compare)(void const * a, void const * b))
{
  VlHeap * self = vl_malloc (sizeof(VlHeap)) ;
  self->nodeSize = nodeSize ;
  self->numNodes = 0 ;
  self->nodes = nodes ;
  self->tempNode = vl_malloc (nodeSize) ;
  self->compareFunction = compare ;
  self->swapFunction = 0 ;/*vl_heap_standard_swap ;*/
  self->swapFunctionCustomData = 0 ;
  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Delete heap object
 ** @param self heap object to delete.
 **/

VL_EXPORT void
vl_heap_delete (VlHeap * self)
{
  if (self->tempNode) vl_free (self->tempNode) ;
  vl_free (self) ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Heap push-down operation
 ** @param self heap object instance.
 ** @param nodeIndex index of the node to push down.
 **/

VL_EXPORT void
vl_heap_down (VlHeap* self, int nodeIndex)
{
  void * node ;
  void * parent ;
  int unsigned parentIndex ;
  int sign ;

  if (nodeIndex == 0) return  ;
  parentIndex = vl_heap_parent (nodeIndex) ;

  node   = vl_heap_get_node_with_index (self, nodeIndex) ;
  parent = vl_heap_get_node_with_index (self, parentIndex) ;

  sign = (self->compareFunction)(node, parent) ;

  if (sign < 0) {
    vl_heap_swap(self, node, parent) ;
    vl_heap_down (self, parentIndex) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Heap push-up operation
 ** @param self heap object instance.
 ** @param nodeIndex index of the node to push up.
 **/

VL_EXPORT void
vl_heap_up (VlHeap * self, int nodeIndex)
{
  int leftIndex  = vl_heap_left_child  (nodeIndex) ;
  int rightIndex = vl_heap_right_child (nodeIndex) ;

  void * node  = vl_heap_get_node_with_index (self, nodeIndex) ;
  void * left  = vl_heap_get_node_with_index (self, leftIndex) ;
  void * right = vl_heap_get_node_with_index (self, rightIndex) ;

  /* no childer: stop */
  if (leftIndex >= self->numNodes) return ;

  /* only left childer: easy */
  if (rightIndex >= self->numNodes) {
    int sign = (self->compareFunction)(node, left) ;
    if (sign > 0) {
      vl_heap_swap (self, node, left) ;
    }
    return ;
  }

  /* both childern */
  {
    int sign = (self->compareFunction)(left, right) ;
    if (sign < 0) {
      /* swap with left */
      sign = (self->compareFunction)(node, left) ;
      if (sign > 0) {
        vl_heap_swap(self, node, left) ;
        vl_heap_up (self, leftIndex) ;
      }
    } else {
      /* swap with right */
      sign = (self->compareFunction)(node, right) ;
      if (sign > 0) {
        vl_heap_swap(self, node, right) ;
        vl_heap_up (self, rightIndex) ;
      }
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief Push a new node into the heap
 **
 ** @param self heap object instance.
 **
 ** The function adds to the heap the first node after the heap end.
 **
 ** @par Example
 ** To add a node of value <code>1.0</code> to an heap of floats:
 ** @code
 ** float * node = vl_heap_get_node_with_index (heap, vl_heap_get_node_num (heap)) ;
 ** *node = 1.0F ;
 ** vl_heap_push (self) ;
 ** @endcode
 **/

VL_EXPORT void
vl_heap_push (VlHeap * self)
{
  self->numNodes += 1 ;
  vl_heap_down (self, self->numNodes - 1) ;
}

/** ------------------------------------------------------------------
 ** @brief Pop the smallest node from the heap.
 **
 ** @param self heap object instance.
 ** @return pointer do extracted node (or @c NULL if the heap is empty).
 **
 ** The function removes the root node from the heap and moves it
 ** to the firs position after the heap end.
 ** It then returns a pointer to it.
 **
 ** @par Example
 ** To extract smallest value from an heap of floats:
 ** @code
 ** float * node = vl_heap_pop (heap) ;
 ** @endcode
 **/

VL_EXPORT void *
vl_heap_pop (VlHeap * self)
{
  void * root ;
  void * last ;
  if (self->numNodes == 0) return NULL ;
  root = vl_heap_get_node_with_index (self, 0) ;
  last = vl_heap_get_node_with_index (self, self->numNodes - 1) ;
  vl_heap_swap(self, root, last) ;
  self->numNodes -= 1 ;
  if (self->numNodes > 1) {
    vl_heap_up (self, 0) ;
  }
  return last ;
}

/** ------------------------------------------------------------------
 ** @brief Update node priority
 **
 ** @param self Heap object instance.
 ** @param nodeIndex node with changed priority.
 **
 ** The function restores the heap property after a node
 ** priority has been changed.
 **
 ** @par Example
 ** Updating the third node of an heap of floats to have value 2.0:
 ** @code
 ** float * node = vl_heap_get_node_with_index (heap, 2) ;
 ** *node = 2.0F ;
 ** vl_heap_update (heap, 2) ;
 ** @endcode
 **/

VL_EXPORT void
vl_heap_update (VlHeap * self, int unsigned nodeIndex)
{
  vl_heap_up (self, nodeIndex) ;
  vl_heap_down (self, nodeIndex) ;
}
