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
    fprintf(stderr, "Count is %d\n", count);

    data_t * new_data = malloc(sizeof(data_t)*M*count);

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
    fprintf(stderr, "xmeans depth=%d N=%d\n", depth, N);
    VlHIKMNode * node = malloc(sizeof(VlHIKMNode));
    node->children = 0;
    /* get the center points */
    idx_t * ids = malloc(sizeof(idx_t)*N);
    fprintf(stderr, "ikmeans with M=%d N=%d K=%d ids=%p\n", hikm->M, N, K, ids);
    node->centers = vl_ikmeans(data, hikm->M, N, K, ids);
    node->K = K;
    fprintf(stderr, "Done with ikm\n");

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
            fprintf(stderr, "Creating some new data\n");
            data_t * new_data = vl_hikm_copy_subset(data, ids, N, hikm->M, c, 
                                &N2);
            fprintf(stderr, "Done creating subset\n");
            node->children[c] = (VlHIKMNode *)vl_hikm_xmeans(hikm, new_data, N2, K, depth-1);
            fprintf(stderr, "Freeing data\n");
            free(new_data);
        }
    }
    fprintf(stderr, "Freeing ids\n");
    free(ids);

    return node;
}

VlHIKMTree * vl_hikm(data_t * data, int M, int N, int K, int nleaves)
{
    VlHIKMTree * hikm = malloc(sizeof(VlHIKMTree));
    // First, figure out the depth
    hikm->depth = ceil(log(nleaves)/log(K));
    hikm->M = M;

    fprintf(stderr, "starting tree build, depth = %d\n", hikm->depth);
    // Now, build the tree recursively
    hikm->root = vl_hikm_xmeans(hikm, data, N, K, hikm->depth);

    fprintf(stderr, "Done building tree\n");
    return hikm;
}

void vl_hikm_delete(VlHIKMTree * hikm)
{

    /* free all the subtrees */
    vl_hikm_delete_node(hikm->root);
    
    free(hikm);
}

idx_t * vl_hikm_push(VlHIKMTree * hikm, data_t * data, int N)
{
    idx_t * ids = malloc(sizeof(idx_t)*hikm->depth*N);
    int i,d;
    int depth = hikm->depth;
    int M = hikm->M;

    /* For each datapoint */
    for(i=0; i<N; i++)
    {
        fprintf(stderr, "Pushing %d\n", i);
        VlHIKMNode * node = hikm->root;
        d=0;
        while(node->centers)
        {
            idx_t best = vl_ikmeans_push_one(node->centers, node->K, &data[i*M], M);
            ids[i*depth+d] = best;
            fprintf(stderr, "Level %d best is %d k was %d\n", d, best, node->K);
            d++;

            if(!node->children) break;

            node = node->children[best];
        }
    }

    return ids;
}
