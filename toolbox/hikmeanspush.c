/** @internal
 ** @file    hikmeanspush.c
 ** @brief   vl_hikm_push - MEX driver
 ** @author  Brian Fulkerson
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS */

#include"mexutils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/hikmeans.h>
#include <vl/generic.h>

#define NFIELDS(field_names) (sizeof(field_names)/sizeof(*field_names))

/* ---------------------------------------------------------------- */
/** @brief Delete HIKM tree
 **/
void
xdel_tree (VlHIKMNode *node) 
{
  if (node) {
    int c ;
    if (node -> children) {
      for (c = 0 ; c < node-> K ; ++c) {
        xdel_tree (node -> children [c]) ;        
      }
      mxFree (node-> children) ;
    }
    
    if (node-> centers) {
      mxFree (node -> centers) ;
    }    
    mxFree(node) ;
  }
}

/* ---------------------------------------------------------------- */
/** @brief Delete HIKM tree node
 **/
void
del_tree (VlHIKMTree *tree)
{
  if(tree) {
    xdel_tree (tree->root) ;
    mxFree(tree) ;
  }  
}

/* ---------------------------------------------------------------- */
/** @brief Convert MATLAB structure to HIKM node
 **/
VlHIKMNode * 
matlab_to_hkmnode (VlHIKMTree *tree, mxArray const *mnode, int i)
{
  mxArray *mcenters, *msub ;
  VlHIKMNode * node ;
  int M, K ;

  /* sanity checks */
  mcenters = mxGetField(mnode, i, "centers") ;
  msub     = mxGetField(mnode, i, "sub") ;
  
  if (!mcenters                                 ||
      mxGetClassID (mcenters) != mxINT32_CLASS  ||
      !uIsMatrix (mcenters, -1, -1)             ) {
    mexErrMsgTxt("NODE.CENTERS must be a INT32 matrix.") ;
  }

  M = mxGetM (mcenters) ;
  K = mxGetN (mcenters) ;
  
  if (K > tree->K) {
    mexErrMsgTxt("A node has more clusters than TREE.K.") ;
  }

  if (tree->M < 0) {
    tree->M = M ;
  } else if (M != tree->M) {
    mexErrMsgTxt("A node centers have inconsistent dimensionality.") ;
  }
  
  node          = mxMalloc (sizeof(VlHIKMNode)) ;  
  node->K       = K ;
  node->centers = mxMalloc (sizeof(vl_int32) * M * K) ;
  node->children= 0 ;
  memcpy (node->centers, mxGetPr(mcenters), sizeof(vl_int32) * M * K) ; 
  
  /* has any childer? */
  if (msub) {
    /* sanity checks */

    if (mxGetClassID (msub) != mxSTRUCT_CLASS) {
      mexErrMsgTxt("NODE.SUB must be a MATLAB structure array.") ;
    }    
    if (mxGetNumberOfElements (msub) != K) {
      mexErrMsgTxt("NODE.SUB size must correspond to NODE.CENTERS.") ;
    }
    
    node-> children = mxMalloc (sizeof(VlHIKMNode *) * K) ;
    for(i = 0 ; i < K ; i++) {
      node-> children [i] = matlab_to_hkmnode (tree, msub, i) ;
    }    
  }

  return node ;
}

/* ---------------------------------------------------------------- */
/** @brief Convert MATLAB structure to HIKM tree */
VlHIKMTree*  
matlab_to_hkmtree (mxArray const *mtree)
{
  VlHIKMTree *tree ;
  mxArray *mK, *mdepth ;
  int K = 0, depth = 0;

  if (mxGetClassID (mtree) != mxSTRUCT_CLASS) {
    mexErrMsgTxt("TREE must be a MATLAB structure.") ;
  }

  mK       = mxGetField(mtree, 0, "K") ;
  mdepth   = mxGetField(mtree, 0, "depth") ;
  
  if (!mK                        ||
      !uIsRealScalar (mK)        ||
      (K = (int) *mxGetPr (mK)) < 1) {
    mexErrMsgTxt("TREE.K must be a DOUBLE not smaller than one.") ;
  }
  
  if (!mdepth                    ||
      !uIsRealScalar (mdepth)    ||
      (depth = (int) *mxGetPr (mdepth)) < 1) {
    mexErrMsgTxt("TREE.DEPTH must be a DOUBLE not smaller than one.") ;
  }
  
  tree         = mxMalloc (sizeof(VlHIKMTree)) ;  
  tree-> depth = depth ;
  tree-> K     = K ;
  tree-> M     = -1 ; /* to be initialized later */
  tree-> root  = matlab_to_hkmnode (tree, mtree, 0) ;
  return tree;
}

/* ---------------------------------------------------------------- */
/** @brief MEX driver entry point 
 **/
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_TREE = 0, IN_DATA} ;
  enum {OUT_ASGN = 0} ;
  vl_uint8 *data_pt; 
  int N = 0 ;
  
  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  if (nin != 2)
    mexErrMsgTxt ("Two arguments required.");
  else if (nout > 1)
    mexErrMsgTxt ("Too many output arguments.");
  
  if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS)
    {
      mexErrMsgTxt ("DATA must be of class UINT8");
    }
  
  N = mxGetN (in[IN_DATA]);   /* n of elements */

  data_pt = (vl_uint8 *) mxGetPr (in[IN_DATA]);

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  /* K is the number of clusters */
  /* M is the dimension of each datapoint */
  /* N is the number of data points */
  {
    VlHIKMTree *tree = matlab_to_hkmtree (in[IN_TREE]) ;
    vl_uint  *ids    = vl_hikm_push (tree, data_pt, N) ;
    int depth        = tree -> depth ;
    
    del_tree (tree) ;
    
    /* copy the assignments to matlab */
    out[OUT_ASGN] = mxCreateNumericMatrix (depth, N, mxUINT32_CLASS, mxREAL);
    {
      int j;
      for (j = 0 ; j < N*depth ; j++) ids[j]++;
    }
    memcpy (mxGetPr(out[OUT_ASGN]), ids, sizeof(vl_uint) * depth * N);
    free (ids);
  }
}
