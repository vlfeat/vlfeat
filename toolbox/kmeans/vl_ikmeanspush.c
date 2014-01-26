/* file:        ikmeanspush.c
** description: MEX weighted ikmeanspush function.
** author:      Andrea Vedaldi
**/

/*
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

#include <vl/generic.h>
#include <vl/ikmeans.h>

enum {
  opt_method,
  opt_verbose
} ;

vlmxOption  options [] = {
  {"Method",       1,   opt_method      },
  {"Verbose",      0,   opt_verbose     },
  {0,              0,   0               }
} ;

/* driver */
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_X=0,IN_C,IN_END} ;
  enum {OUT_ASGN=0} ;
  VlIKMFilt *ikmf ;
  vl_uint32* asgn ;
  vl_ikmacc_t* centers ;
  vl_uint8* data ;
  mwSize M,N,K ;
  vl_uindex j ;
  int method_type = VL_IKM_LLOYD ;
  int verb = 0 ;

  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  VL_USE_MATLAB_ENV ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */

  if (nin < 2) {
    mexErrMsgTxt("At least two arguments required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.") ;
  }

  if(mxGetClassID(in[IN_X]) != mxUINT8_CLASS) {
    mexErrMsgTxt("X is not of class UINT8.") ;
  }

  if(mxGetClassID(in[IN_C]) != mxINT32_CLASS) {
    mexErrMsgTxt("C is not of class INT32.") ;
  }

  M = mxGetM(in[IN_X]) ; /* n of components */
  N = mxGetN(in[IN_X]) ; /* n of elements */
  K = mxGetN(in[IN_C]) ; /* n of centers */

  if (mxGetM(in[IN_C]) != M ) {
    mexErrMsgTxt("DATA and CENTERS must have the same number of columns.") ;
  }

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
        mexErrMsgTxt("Unknown cost type.") ;
      }
      break ;

    default :
      abort() ;
    }
  }

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */

  if (verb) {
    char const * method_name = 0 ;
    switch (method_type) {
    case VL_IKM_LLOYD: method_name = "Lloyd" ; break ;
    case VL_IKM_ELKAN: method_name = "Elkan" ; break ;
    default :
      abort() ;
    }
    mexPrintf("ikmeanspush: Method = %s\n", method_name) ;
    mexPrintf("ikmeanspush: ndata  = %d\n", N) ;
  }

  out[OUT_ASGN] = mxCreateNumericMatrix (1, N, mxUINT32_CLASS, mxREAL) ;

  data = (vl_uint8*) mxGetData (in[IN_X]) ;
  centers = (vl_ikmacc_t*) mxGetData (in[IN_C]) ;
  asgn = (vl_uint32*) mxGetData (out[OUT_ASGN]) ;
  ikmf = vl_ikm_new (method_type) ;

  vl_ikm_set_verbosity (ikmf, verb) ;
  vl_ikm_init (ikmf, centers, M, K) ;
  vl_ikm_push (ikmf, asgn, data, N) ;

  /* adjust for MATLAB indexing */
  for(j = 0 ; j < N ; ++j) ++ asgn[j] ;

  vl_ikm_delete (ikmf) ;
}
