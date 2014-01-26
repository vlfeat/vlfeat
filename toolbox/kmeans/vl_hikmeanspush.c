/** @internal
 ** @file    hikmeanspush.c
 ** @brief   vl_hikm_push - MEX driver
 ** @author  Brian Fulkerson
 ** @author  Andrea Vedaldi
 **/

/*
Copyright (C) 2014 Andrea Vedaldi.
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include<mexutils.h>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/hikmeans.h>
#include <vl/generic.h>

enum {
  opt_method,
  opt_verbose
} ;

vlmxOption  options [] = {
  {"Method",       1,   opt_method      },
  {"Verbose",      0,   opt_verbose     },
  {0,              0,   0               }
} ;

#define NFIELDS(field_names) (sizeof(field_names)/sizeof(*field_names))


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Convert MATLAB structure to HIKM node
 **/

static VlHIKMNode *
xcreate (VlHIKMTree *tree, mxArray const *mnode, int i)
{
  mxArray const *mcenters, *msub ;
  VlHIKMNode *node ;
  mwSize M ;
  mwSize node_K ;
  vl_uindex k ;

  /* sanity checks */
  mcenters = mxGetField(mnode, i, "centers") ;
  msub     = mxGetField(mnode, i, "sub") ;

  if (!mcenters ||
      mxGetClassID (mcenters) != mxINT32_CLASS  ||
      !vlmxIsMatrix (mcenters, -1, -1)) {
    mexErrMsgTxt("A NODE.CENTERS is not a INT32 matrix.") ;
  }

  M = mxGetM (mcenters) ;
  node_K = mxGetN (mcenters) ;

  if (M == 0) {
    mexErrMsgTxt("A NODE.CENTERS has zero rows.") ;
  }
  if ((vl_size)node_K > tree->K) {
    mexErrMsgTxt("A NODE.CENTERS has more columns than overall clusters TREE.K.") ;
  }
  if (tree->M == 0) {
    tree->M = M ;
  } else if (M != tree->M) {
    mexErrMsgTxt("A NODE.CENTERS field has inconsistent dimensionality.") ;
  }

  node = mxMalloc (sizeof(VlHIKMNode)) ;
  node->filter = vl_ikm_new (tree->method) ;
  node->children = 0 ;

  vl_ikm_init (node->filter, mxGetData(mcenters), M, node_K) ;

  /* has any childer? */
  if (msub) {

    /* sanity checks */
    if (mxGetClassID (msub) != mxSTRUCT_CLASS) {
      mexErrMsgTxt("A NODE.SUB is not a MATLAB structure array.") ;
    }
    if (mxGetNumberOfElements (msub) != node_K) {
      mexErrMsgTxt("A NODE.SUB does not correspond to NODE.CENTERS.") ;
    }

    node->children = mxMalloc (sizeof(VlHIKMNode *) * node_K) ;
    for(k = 0 ; k < node_K ; ++ k) {
      node->children[k] = xcreate (tree, msub, k) ;
    }
  }
  return node ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Convert MATLAB structure to HIKM tree
 **/

static VlHIKMTree*
matlab_to_hikm (mxArray const *mtree, int method_type)
{
  VlHIKMTree *tree ;
  mxArray *mK ;
  mxArray *mdepth ;
  vl_index K = 0 ;
  vl_index depth = 0 ;

  VL_USE_MATLAB_ENV ;

  if (mxGetClassID (mtree) != mxSTRUCT_CLASS) {
    mexErrMsgTxt("TREE must be a MATLAB structure.") ;
  }

  mK = mxGetField(mtree, 0, "K") ;
  mdepth = mxGetField(mtree, 0, "depth") ;

  if (!mK ||
      !vlmxIsPlainScalar(mK) ||
      (K = (int) *mxGetPr(mK)) < 1) {
    mexErrMsgTxt("TREE.K must be a DOUBLE not smaller than one.") ;
  }

  if (!mdepth                    ||
      !vlmxIsPlainScalar (mdepth)    ||
      (depth = (int) *mxGetPr (mdepth)) < 1) {
    mexErrMsgTxt("TREE.DEPTH must be a DOUBLE not smaller than one.") ;
  }

  tree = mxMalloc (sizeof(VlHIKMTree)) ;
  tree->depth = (vl_size)depth ;
  tree->K = (vl_size)K ;
  tree->M = 0 ; /* to be initialized later */
  tree->method = method_type ;
  tree->root = xcreate (tree, mtree, 0) ;
  return tree ;
}

/* ---------------------------------------------------------------- */
/** @brief MEX driver entry point
 **/
void mexFunction (int nout, mxArray * out[],
                  int nin, const mxArray * in[])
{
  enum {IN_TREE = 0, IN_DATA, IN_END} ;
  enum {OUT_ASGN = 0} ;
  vl_uint8 const *data;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  mwSize N = 0 ;
  int method_type = VL_IKM_LLOYD ;
  int verb = 0 ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  if (nin < 2)
    mexErrMsgTxt ("At least two arguments required.");
  else if (nout > 1)
    mexErrMsgTxt ("Too many output arguments.");

  if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS) {
    mexErrMsgTxt ("DATA must be of class UINT8");
  }

  N = mxGetN(in[IN_DATA]) ;
  data = (vl_uint8 *) mxGetPr (in[IN_DATA]);

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;

    switch (opt) {

    case opt_verbose :
      ++ verb ;
      break ;

    case opt_method :
      if (!vlmxIsString (optarg, -1)) {
        mexErrMsgTxt("'Method' must be a string.") ;
      }
      if (mxGetString (optarg, buf, sizeof(buf))) {
        mexErrMsgTxt("Option argument too long.") ;
      }
      if (strcmp("lloyd", buf) == 0) {
        method_type = VL_IKM_LLOYD ;
      } else if (strcmp("elkan", buf) == 0) {
        method_type = VL_IKM_ELKAN ;
      } else {
        mexErrMsgTxt("Unknown 'Method' type.") ;
      }
      break ;

    default :
      abort() ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  {
    VlHIKMTree *tree ;
    vl_uint32 *ids  ;
    vl_uindex j;
    vl_size depth ;

    tree = matlab_to_hikm (in[IN_TREE], method_type) ;
    depth = vl_hikm_get_depth (tree) ;

    if (verb) {
      mexPrintf("vl_hikmeanspush: ndims: %d K: %d depth: %d\n",
                vl_hikm_get_ndims (tree),
                vl_hikm_get_K (tree),
                depth) ;
    }

    out[OUT_ASGN] = mxCreateNumericMatrix (depth, N, mxUINT32_CLASS, mxREAL) ;
    ids = mxGetData (out[OUT_ASGN]) ;

    vl_hikm_push (tree, ids, data, N) ;
    vl_hikm_delete (tree) ;

    for (j = 0 ; j < N * depth ; j++) ids[j] ++ ;
  }
}
