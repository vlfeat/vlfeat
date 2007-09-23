/** @file   hikmeans.h
 ** @author Brian Fulkerson
 ** @brief  Heirachical Integer K-Means Clustering - Declaration
 **/
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
  int K ;                         /**< Number of childer  */
  vl_int32 *centers ;             /**< Centers */
  struct _VlHIKMNode **children ; /**< Children */
} VlHIKMNode ;


/** @brief HIKM tree */
typedef struct _VlHIKMTree {
  int M;                /**< Dimension of data */
  int K;                /**< K */
  int depth;            /**< Depth of the tree */
  VlHIKMNode * root;    /**< Tree root node */
  int miter ;           /**< Maximum K-means iterations per node */
} VlHIKMTree ;

VlHIKMTree *vl_hikm (vl_uint8 *data_pt, int M, int N, int K, int nleaves) ;
void vl_hikm_delete (VlHIKMTree *hikm) ;
vl_uint *vl_hikm_push (VlHIKMTree *hikm, vl_uint8 *data_pt, int N) ;

/* VL_HIKMEANS_H */
#endif
