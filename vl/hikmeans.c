#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hikmeans.h"

/* recursive way */
void vl_hikm_delete_node(VlHIKMNode * node)
{
    if(node)
    {
        int c;
        if(node->children)
        {
            for(c=0; c<node->K; c++)
                vl_hikm_delete_node(node->children[c]);
            free(node->children);
        }

        if(node->centers) 
            free(node->centers);

        free(node);
    }
}

data_t * vl_hikm_copy_subset(data_t * data, idx_t * ids, int N, int M, idx_t id, int * N2)
{
    int i;
    int count = 0;

    for(i=0; i<N; i++)
        if(ids[i] == id)
            count++;
    *N2 = count;

    data_t * new_data = malloc(sizeof(data_t)*count);

    count = 0;
    for(i=0; i<N; i++)
        if(ids[i] == id)
        {
            memcpy(&new_data[count*M], &data[i*M], M*sizeof(data_t));
            count++;
        }

    *N2 = count;
    return new_data;
}

VlHIKMNode * vl_hikm_xmeans(VlHIKMTree * hikm, data_t * data, 
                       int N, int K, int depth)
{
    VlHIKMNode * node = malloc(sizeof(VlHIKMNode));
    node->children = 0;
    /* get the center points */
    idx_t * ids = malloc(sizeof(idx_t)*N);
    node->centers = vl_ikmeans(data, hikm->M, N, K, ids);
    node->K = K;

    if(depth != 1)
    {
        /* find data assignments to children */
        /* for each child
         *      partition the data
         *      recurse with this partition to get the node
         */
        node->children = malloc(sizeof(VlHIKMNode *)*K);
        int c;
        for(c=0; c < K; c++)
        {
            int N2;
            data_t * new_data = vl_hikm_copy_subset(data, ids, N, hikm->M, c, 
                                &N2);
            node->children[c] = (VlHIKMNode *)vl_hikm_xmeans(hikm, new_data, N2, K, depth-1);
            free(new_data);
        }
    }
    free(ids);

    return node;
}

VlHIKMTree * vl_hikm(data_t * data, int N, int M, int K, int nleaves)
{
    VlHIKMTree * hikm = malloc(sizeof(VlHIKMTree));
    // First, figure out the depth
    hikm->depth = ceil(log(nleaves)/log(K));
    hikm->M = M;

    // Now, build the tree recursively
    hikm->tree = vl_hikm_xmeans(hikm, data, N, K, hikm->depth);

    return hikm;
}

void vl_hikm_delete(VlHIKMTree * hikm)
{

    /* free all the subtrees */
    vl_hikm_delete_node(hikm->tree);
    
    free(hikm);
}

idx_t * vl_hikm_push(VlHIKMTree * hikm, data_t * data, int N)
{
    return 0;
}
