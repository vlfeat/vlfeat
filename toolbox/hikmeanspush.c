/* file:        hikmeans.c
** description: MEX hierarchical ikmeans.
** author:      Brian Fulkerson
**/

#include"mexutils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/hikmeans.h>
#include <vl/generic.h>

#define NFIELDS(field_names) (sizeof(field_names)/sizeof(*field_names))

void dfs_hkm_print(VlHIKMNode * node, int spaces)
{
    char * buf = malloc(sizeof(char)*(spaces+1));
    memset(buf, ' ', sizeof(char)*spaces);
    buf[spaces] = '\0';

    mexPrintf("%sNode:\n", buf);
    mexPrintf("%s Centers: %d\n", buf, node->K);
    mexPrintf("%s Children:\n", buf);

    if(node->children)
    {
    spaces += 2;
    int i;
    for(i=0; i<node->K; i++)
        dfs_hkm_print(node->children[i], spaces);
    }

    free(buf);
}

VlHIKMNode *  matlab_to_hkmnode(mxArray * centers, mxArray * sub)
{
    VlHIKMNode * node = malloc(sizeof(VlHIKMNode));

    int M = mxGetM(centers);
    int K = mxGetN(centers);
    node->K = K;
    node->centers = malloc(sizeof(vl_int32)*M*K);
    memcpy(node->centers, mxGetPr(centers), sizeof(vl_int32)*M*K);

    node->children = malloc(sizeof(VlHIKMNode *)*K);
    int i;
    for(i=0; i<K; i++)
    {
        mxArray * childcenters = mxGetField(sub, i, "centers");
        mxArray * childsub     = mxGetField(sub, i, "sub");
        /* Base case, has no children */
        if(!childsub || !childcenters) 
        {
            free(node->children);
            node->children = NULL;
            break;
        }
        node->children[i] = matlab_to_hkmnode(childcenters, childsub);
    }

    return node;
}

VlHIKMTree *  matlab_to_hkmtree(mxArray * mtree)
{
    VlHIKMTree * tree = malloc(sizeof(VlHIKMTree));

    mxArray * centers = mxGetField(mtree, 0, "centers");
    mxArray * sub = mxGetField(mtree, 0, "sub");

    tree->depth = *mxGetPr(mxGetField(mtree, 0, "depth"));
    tree->M     = mxGetM(centers);

    tree->root  = matlab_to_hkmnode(centers, sub);
    /* dfs_hkm_print(tree->root, 1); */

    return tree;
}

/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
    vl_uint8 *data_pt;

    /*  mxClassID data_class = mxINT8_CLASS ; */
    enum
    { IN_TREE = 0, IN_DATA };
    enum
    { OUT_ASGN = 0 };
    int K = 0;
    int N = 0;
    int verbose = 1;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
    if (nin != 2)
        mexErrMsgTxt ("Two arguments required.");
    else if (nout > 1)
        mexErrMsgTxt ("Too many output arguments.");

    if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS)
    {
        mexErrMsgTxt ("DATA must be of class uint8");
    }

    N = mxGetN (in[IN_DATA]);   /* n of elements */

    data_pt = (vl_uint8 *) mxGetPr (in[IN_DATA]);

    /* K is the number of clusters */
    /* M is the dimension of each datapoint */
    /* N is the number of data points */
    VlHIKMTree * tree = matlab_to_hkmtree(in[IN_TREE]);
    vl_uint * ids       = vl_hikm_push(tree, data_pt, N);
    int depth = tree->depth;

    vl_hikm_delete(tree);

    /* copy the assignments to matlab */
    out[OUT_ASGN] = mxCreateNumericMatrix (depth, N, mxUINT32_CLASS, mxREAL);
    /* Matlab has 1 based indexing */
    {
        int j;
        for (j = 0 ; j < N*depth ; j++) ids[j]++;
    }
    memcpy(mxGetPr(out[OUT_ASGN]), ids, sizeof(vl_uint)*depth*N);
    free(ids);
    
}
