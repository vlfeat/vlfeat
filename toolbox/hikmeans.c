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

mxArray *  fill_subtree(VlHIKMNode * node, int M)
{
    int K = node->K;
    if(!node->children)
    {
        /* Base case */
    	mxArray * sub = mxCreateDoubleMatrix(1,K,mxREAL);
        memset(mxGetPr(sub), 0, K*sizeof(vl_double));
        return sub;
    }

    const char * field_names[] = {"centers", "sub"};
    mwSize dims[2] = {1,K};

    mxArray * subtree = mxCreateStructArray(2, dims, NFIELDS(field_names), field_names);

    /* add subtrees */
    int i;
    for(i=0; i<K; i++)
    {
        /* add centers */
        mxArray * centers = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL);
        memcpy(mxGetPr(centers), node->children[i]->centers, 
                sizeof(vl_int32)*M*K);
        mxSetField(subtree,i,"centers",centers);

        /* set sub[i] to result of fill_subtree */
        mxArray * child = fill_subtree(node->children[i], M);
    	mxSetField(subtree,i,"sub",child);
    }

    return subtree;
}

mxArray *  hkmtree_to_matlab(VlHIKMTree * tree)
{
    mexPrintf("Creating the matlab tree\n");
    mwSize dims[2] = {1,1};
    mxArray * mtree;
    const char * field_names[] = {"depth", "centers", "sub"};

    /* Create the main struct array */
    mtree = mxCreateStructArray(2, dims, NFIELDS(field_names), field_names);
    mxArray * field_value;

    /* Set the depth */
	field_value = mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(field_value) = tree->depth;
	mxSetField(mtree,0,"depth",field_value);

    int M = tree->M;
    int K = tree->root->K;
    /* Set the first tree centers */
    field_value = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(field_value), tree->root->centers, sizeof(vl_int32)*M*K);
    mxSetField(mtree,0,"centers",field_value);

    /* Set the subtrees recursively */
    field_value = fill_subtree(tree->root, M);
    if(field_value)
        mxSetField(mtree,0,"sub",field_value);

    return mtree;
}

/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
    vl_int32 *centers_pt;
    vl_uint8 *data_pt;

    /*  mxClassID data_class = mxINT8_CLASS ; */
    enum
    { IN_DATA = 0, IN_K, IN_NLEAVES };
    enum
    { OUT_TREE = 0, OUT_ASGN };
    int M, N;
    int K = 0;
    int nleaves;
    int verbose = 1;
    vl_uint npasses = 200;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
    if (nin != 3)
    {
        mexErrMsgTxt ("Three arguments required.");
    }
    else if (nout > 2)
    {
        mexErrMsgTxt ("Too many output arguments.");
    }

    if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS)
    {
        mexErrMsgTxt ("DATA must be of class uint8");
    }

    if (uIsRealScalar (in[IN_NLEAVES]))
        nleaves = (int) *mxGetPr (in[IN_NLEAVES]);
    else
        mexErrMsgTxt ("Nleaves must be a scalar\n");

    if (uIsRealScalar (in[IN_K]))
    {
        K = (int) *mxGetPr (in[IN_K]);
        if (K > 0)
            goto K_is_ok;
    }
    mexErrMsgTxt ("K must be a positive integer.");
  K_is_ok:

    M = mxGetM (in[IN_DATA]);   /* n of components */
    N = mxGetN (in[IN_DATA]);   /* n of elements */

    if (N < K)
    {
        mexErrMsgTxt ("Numeber of clusters cannot be less than data.");
    }

    data_pt = (vl_uint8 *) mxGetPr (in[IN_DATA]);

    /* K is the number of clusters */
    /* M is the dimension of each datapoint */
    /* N is the number of data points */
    VlHIKMTree * tree = vl_hikm(data_pt, M, N, K, nleaves);
    vl_uint * ids       = vl_hikm_push(tree, data_pt, N);
    int depth = tree->depth;

    /* copy the tree to matlab */
    out[OUT_TREE] = hkmtree_to_matlab(tree);
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
