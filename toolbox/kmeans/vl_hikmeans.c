/** file:        hikmeans.c
 ** description: MEX hierarchical ikmeans.
 ** author:      Brian Fulkerson
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include<mexutils.h>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/hikmeans.h>
#include <vl/generic.h>

#define NFIELDS(field_names) (sizeof(field_names)/sizeof(*field_names))

enum {
  opt_max_niters,
  opt_method,
  opt_verbose
} ;

vlmxOption  options [] = {
  {"MaxIters",     1,   opt_max_niters  },
  {"Method",       1,   opt_method      },
  {"Verbose",      0,   opt_verbose     },
  {0,              0,   0               }
} ;

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Copy HIKM tree node to a MATLAB structure
 **/

static void
xcreate (mxArray *mnode, int i, VlHIKMNode *node)
{
  int node_K                = vl_ikm_get_K (node->filter) ;
  int M                     = vl_ikm_get_ndims (node->filter) ;
  vl_ikm_acc const *centers = vl_ikm_get_centers (node->filter) ;

  mxArray *mcenters ;

  mcenters = mxCreateNumericMatrix (M, node_K, mxINT32_CLASS, mxREAL);
  memcpy (mxGetPr(mcenters), centers, sizeof(vl_ikm_acc) * M * node_K) ;
  mxSetField (mnode, i, "centers", mcenters) ;

  if (node->children) {
    mxArray * msub ;
    const char * field_names[] = {"centers", "sub" } ;
    mwSize dims [2] ;
    int k ;

    dims[0] = 1 ;
    dims[1] = node_K ;

    msub = mxCreateStructArray (2, dims, 2, field_names) ;

    for (k = 0 ; k < node_K ; ++k) {
      xcreate (msub, k, node -> children [k]) ;
    }

    mxSetField (mnode, i, "sub", msub) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Copy HIKM tree to a MATLAB structure
 **/

mxArray *
hikm_to_matlab (VlHIKMTree * tree)
{
  int K      = vl_hikm_get_K (tree) ;
  int depth  = vl_hikm_get_depth (tree) ;
  mwSize  dims [2] = {1, 1} ;
  mxArray *mtree ;
  const char *field_names[] = {"K", "depth", "centers", "sub"} ;

  /* Create the main struct array */
  mtree = mxCreateStructArray
    (2, dims, NFIELDS(field_names), field_names) ;
  mxSetField (mtree, 0, "K",      mxCreateDoubleScalar (K)) ;
  mxSetField (mtree, 0, "depth",  mxCreateDoubleScalar (depth)) ;
  if (tree->root) xcreate (mtree, 0, tree->root) ;
  return mtree;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief MEX driver
 **/
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  vl_uint8 *data ;
  enum {IN_DATA = 0, IN_K, IN_NLEAVES, IN_END} ;
  enum {OUT_TREE = 0, OUT_ASGN} ;
  int M, N, K = 2, depth = 0 ;

  int             opt ;
  int             next = IN_END ;
  mxArray const  *optarg ;

  int nleaves     = 1 ;
  int method_type = VL_IKM_LLOYD ;
  int max_niters  = 200 ;
  int verb        = 0 ;

  VlHIKMTree* tree ;

  VL_USE_MATLAB_ENV ;

  /* ------------------------------------------------------------------
   *                                                Check the arguments
   * --------------------------------------------------------------- */

  if (nin < 3)
    {
      mexErrMsgTxt ("At least three arguments required.");
    }
  else if (nout > 2)
    {
      mexErrMsgTxt ("Too many output arguments.");
    }

  if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS)
    {
      mexErrMsgTxt ("DATA must be of class UINT8.");
    }

  if (! vlmxIsPlainScalar (in[IN_NLEAVES])           ||
      (nleaves = (int) *mxGetPr (in[IN_NLEAVES])) < 1) {
    mexErrMsgTxt ("NLEAVES must be a scalar not smaller than 2.") ;
  }

  M = mxGetM (in[IN_DATA]);   /* n of components */
  N = mxGetN (in[IN_DATA]);   /* n of elements */

  if (! vlmxIsPlainScalar (in[IN_K])         ||
      (K = (int) *mxGetPr (in[IN_K])) > N  ) {
    mexErrMsgTxt ("Cannot have more clusters than data.") ;
  }

  data = (vl_uint8 *) mxGetPr (in[IN_DATA]) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;

    switch (opt) {

    case opt_verbose :
      ++ verb ;
      break ;

    case opt_max_niters :
      if (!vlmxIsPlainScalar(optarg) || (max_niters = (int) *mxGetPr(optarg)) < 1) {
        mexErrMsgTxt("MaxNiters must be not smaller than 1.") ;
      }
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
        mexErrMsgTxt("Unknown cost type.") ;
      }

      break ;

    default :
      abort() ;
      break ;
    }
  }

  /* ---------------------------------------------------------------
   *                                                      Do the job
   * ------------------------------------------------------------ */

  depth = VL_MAX(1, ceil (log (nleaves) / log(K))) ;
  tree  = vl_hikm_new  (method_type) ;

  if (verb) {
    mexPrintf("hikmeans: # dims: %d\n", M) ;
    mexPrintf("hikmeans: # data: %d\n", N) ;
    mexPrintf("hikmeans: K: %d\n", K) ;
    mexPrintf("hikmeans: depth: %d\n", depth) ;
  }

  vl_hikm_set_verbosity (tree, verb) ;
  vl_hikm_init          (tree, M, K, depth) ;
  vl_hikm_train         (tree, data, N) ;

  out[OUT_TREE] = hikm_to_matlab (tree) ;

  if (nout > 1) {
    vl_uint *asgn ;
    int j ;
    out [OUT_ASGN] = mxCreateNumericMatrix
      (vl_hikm_get_depth (tree), N, mxUINT32_CLASS, mxREAL) ;
    asgn = mxGetData(out[OUT_ASGN]) ;
    vl_hikm_push (tree, asgn, data, N) ;
    for (j = 0 ; j < N*depth ; ++ j) asgn [j] ++ ;
  }

  if (verb) {
    mexPrintf("hikmeans: done.\n") ;
  }

  /* vl_hikm_delete (tree) ; */
}
