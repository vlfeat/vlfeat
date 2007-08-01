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
    acc_t * centers;
    struct _VlHIKMNode ** children;
} VlHIKMNode;

typedef struct _VlHIKMTree {
    int M; /* dimension of data */
    int depth;
    VlHIKMNode * tree;
} VlHIKMTree;

VlHIKMTree * vl_hikm(data_t * data_pt, int N, int M, int K, int nleaves);
void vl_hikm_delete(VlHIKMTree * hikm);

idx_t * vl_hikm_push(VlHIKMTree * hikm, data_t * data_pt, int N);

/* VL_HIKMEANS_H */
#endif
