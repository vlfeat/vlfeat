/* file:        ikmeans.c
** description: MEX weighted ikmeans function.
** author:      Andrea Vedaldi
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

#include <vl/ikmeans.h>
#include <vl/generic.h>

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


/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_X = 0, IN_K, IN_END} ;
  enum {OUT_C = 0, OUT_I} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  int M, N, K = 0 ;
  int err = 0 ;

  vl_uint     *asgn = 0 ;
  vl_ikm_acc  *centers = 0 ;
  vl_uint8 *data ;

  int method_type = VL_IKM_LLOYD ;
  int max_niters  = 200 ;
  int verb = 0 ;

  VlIKMFilt *ikmf ;

  VL_USE_MATLAB_ENV ;

  /* ------------------------------------------------------------------
   *                                                Check the arguments
   * --------------------------------------------------------------- */
  if (nin < 2) {
    mexErrMsgTxt ("At least two arguments required.");
  }
  else if (nout > 2) {
    mexErrMsgTxt ("Too many output arguments.");
  }
  if (mxGetClassID (in[IN_X]) != mxUINT8_CLASS) {
    mexErrMsgTxt ("X must be of class uint8");
  }

  M = mxGetM (in[IN_X]);   /* n of components */
  N = mxGetN (in[IN_X]);   /* n of elements */

  if (!vlmxIsPlainScalar (in[IN_K])          ||
      (K = (int) *mxGetPr(in[IN_K])) < 1 ||
      K > N                               ) {
    mexErrMsgTxt ("K must be a positive integer not greater than the number of data.");
  }

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
        mexErrMsgTxt("Unknown method type.") ;
      }
      break ;

    default :
      abort() ;
    }
  }

  /* ------------------------------------------------------------------
   *                                                         Do the job
   * --------------------------------------------------------------- */

  if (verb) {
    char const * method_name = 0 ;
    switch (method_type) {
    case VL_IKM_LLOYD: method_name = "Lloyd" ; break ;
    case VL_IKM_ELKAN: method_name = "Elkan" ; break ;
    default :
      abort() ;
    }
    mexPrintf("ikmeans: MaxInters = %d\n", max_niters) ;
    mexPrintf("ikmeans: Method    = %s\n", method_name) ;
  }

  data = (vl_uint8*) mxGetPr(in[IN_X]) ;
  ikmf = vl_ikm_new (method_type) ;

  vl_ikm_set_verbosity  (ikmf, verb) ;
  vl_ikm_set_max_niters (ikmf, max_niters) ;
  vl_ikm_init_rand_data (ikmf, data, M, N, K) ;

  err = vl_ikm_train (ikmf, data, N) ;
  if (err) mexWarnMsgTxt("ikmeans: possible overflow!") ;

  /* ------------------------------------------------------------------
 *                                                       Return results
   * --------------------------------------------------------------- */

  {
    out[OUT_C] = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL) ;
    centers    = mxGetData (out[OUT_C]) ;
    memcpy (centers, vl_ikm_get_centers (ikmf), sizeof(vl_ikm_acc) * M * K) ;
  }

  if (nout > 1) {
    int j ;
    out[OUT_I] = mxCreateNumericMatrix (1, N, mxUINT32_CLASS, mxREAL) ;
    asgn       = mxGetData (out[OUT_I]) ;

    vl_ikm_push (ikmf, asgn, data, N) ;

    for (j = 0 ; j < N ; ++j)
      ++ asgn [j] ;
  }

  vl_ikm_delete (ikmf) ;

  if (verb) {
    mexPrintf("ikmeans: done\n") ;
  }
}
