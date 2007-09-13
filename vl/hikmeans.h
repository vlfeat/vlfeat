/** @file   hikmeans.h
 ** @author Brian Fulkerson
 ** @brief  Heirachical Integer K-Means Clustering - Declaration
 **/
#ifndef VL_HIKMEANS_H
#define VL_HIKMEANS_H

#include "generic.h"
#include "ikmeans.h"

typedef struct _VlHIKMNode
{
  int K; /* size of tree at this level */
  vl_int32 *centers;
  struct _VlHIKMNode ** children;
} VlHIKMNode;

typedef struct _VlHIKMTree {
  int M; /* dimension of data */
  int depth;
  VlHIKMNode * root;
} VlHIKMTree;

VlHIKMTree * vl_hikm(vl_uint8 *data_pt, int M, int N, int K, int nleaves);
void vl_hikm_delete(VlHIKMTree *hikm);

vl_uint *vl_hikm_push(VlHIKMTree *hikm, vl_uint8 *data_pt, int N);

/* VL_HIKMEANS_H */
#endif
