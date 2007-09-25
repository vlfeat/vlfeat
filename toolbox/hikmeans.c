/** file:        hikmeans.c
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

/** @brief Copy HIKM tree node to a MATLAB structure */
mxArray *  
fill_subtree (VlHIKMNode * node, int M)
{
  int i, K = node->K ;
  mwSize dims [2] ;
  const char * field_names[] = {"centers", "sub" } ;
  mxArray *subtree, *child ;

  dims[0] = 1 ;
  dims[1] = K ;

  /* Base case: this node does not have any childern (leaf). 
   * In this case the node is empty.
   */
  if (! node->children) {
    return 0 ;
  }

  /* Base case */
  
  /*  mxArray * sub = mxCreateDoubleMatrix(1,K,mxREAL);
    memset (mxGetPr(sub), 0, K * sizeof(vl_double));
    return sub;
    }
  */
  
  /* Recursive case: the node is a structure. */
  subtree = mxCreateStructArray
    (2, dims, NFIELDS(field_names), field_names);
  
  /* Add subtrees */
  for(i = 0 ; i < K ; ++ i) {
    int K_           = node->children[i]->K ;
    mxArray *centers ;

    /* .centers field */
    centers = mxCreateNumericMatrix (M, K_, mxINT32_CLASS, mxREAL);
    memcpy (mxGetPr(centers), 
            node->children[i]->centers, 
            sizeof(vl_int32) * M * K_) ;
    mxSetField (subtree, i, "centers", centers) ;
    
    /* .sub field */
    child = fill_subtree (node->children[i], M) ;
    mxSetField (subtree, i, "sub", child) ;
  }
  
  return subtree ;
}

/** @brief Copy HIKM tree to a MATLAB structure */
mxArray *  
hkmtree_to_matlab(VlHIKMTree * tree)
{
  int M = tree->M;
  int K = tree->root->K;
  mwSize dims [2] = {1, 1} ;
  mxArray *mtree, *field_value;
  const char *field_names[] = {"K", "depth", "centers", "sub"} ;
  
  /* Create the main struct array */
  mtree = mxCreateStructArray 
    (2, dims, NFIELDS(field_names), field_names) ;

  /* Set K */
  field_value = mxCreateDoubleMatrix(1,1,mxREAL) ;
  *mxGetPr(field_value) = tree->K ;
  mxSetField (mtree, 0, "K", field_value) ;
  
  /* Set the depth */
  field_value = mxCreateDoubleMatrix(1,1,mxREAL) ;
  *mxGetPr(field_value) = tree->depth ;
  mxSetField (mtree, 0, "depth", field_value) ;
  
  /* Set the first tree centers */
  field_value = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL) ;
  memcpy (mxGetPr(field_value), 
          tree->root->centers, 
          sizeof(vl_int32) * M * K) ;
  mxSetField (mtree, 0, "centers", field_value) ;

  /* Set the subtrees recursively */
  field_value = fill_subtree (tree->root, M) ;
  if(field_value)
    mxSetField(mtree,0,"sub",field_value) ;
  
  return mtree;
}

/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
    vl_uint8 *data ;
    enum { IN_DATA = 0, IN_K, IN_NLEAVES };
    enum { OUT_TREE = 0, OUT_ASGN } ;    
    int M, N, K = 2 ;
    int nleaves = 1 ;
    int verbose = 1 ;

  /* ------------------------------------------------------------------
   *                                                Check the arguments
   * --------------------------------------------------------------- */

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
        mexErrMsgTxt ("DATA must be of class UINT8.");
    }

    if (! uIsRealScalar (in[IN_NLEAVES])           ||
        (nleaves = (int) *mxGetPr (in[IN_NLEAVES])) < 1) {
      mexErrMsgTxt ("NLEAVES must be a scalar not smaller than 2.") ;
    }
    
    M = mxGetM (in[IN_DATA]);   /* n of components */
    N = mxGetN (in[IN_DATA]);   /* n of elements */
    
    if (! uIsRealScalar (in[IN_K])         ||
        (K = (int) *mxGetPr (in[IN_K])) > N  ) {
      mexErrMsgTxt ("Cannot have more clusters than data.") ;
    }
    
    data = (vl_uint8 *) mxGetPr (in[IN_DATA]) ;
    
    if (verbose) {
      mexPrintf("hikmeans: data dimension: %d\n", M) ;
      mexPrintf("hikmeans: data size: %d\n", N) ;
      mexPrintf("hikmeans: K: %d\n", K) ;
      mexPrintf("hikmeans: minimum number of leaves: %d\n", nleaves) ;
    }

    /* ---------------------------------------------------------------
     *                                                      Do the job
     * ------------------------------------------------------------ */

    /* K is the number of clusters */
    /* M is the dimension of each datapoint */
    /* N is the number of data points */
    {
      VlHIKMTree * tree = vl_hikm (data, M, N, K, nleaves) ;
      vl_uint * ids     = vl_hikm_push (tree, data, N) ;
      int depth         = tree->depth;
      
      /* copy the tree to matlab */
      out[OUT_TREE] = hkmtree_to_matlab (tree) ;
      vl_hikm_delete (tree) ;
      
      /* copy the assignments to matlab */
      out [OUT_ASGN] = mxCreateNumericMatrix 
        (depth, N, mxUINT32_CLASS, mxREAL) ;
      {
        int j ;
        for (j = 0 ; j < N * depth ; ++j) ids[j]++ ;
      }
      memcpy(mxGetPr(out[OUT_ASGN]), ids, sizeof(vl_uint) * depth * N) ;
      free(ids) ;
      
      if (verbose) {
        mexPrintf("hikmeans: done.\n") ;
      }
    }
}

