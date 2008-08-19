/** @internal
 ** @file   imsmooth.c
 ** @author Andrea Vedaldi
 ** @brief  Smooth an image - MEX definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/imopv.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* option codes */
enum {
  opt_padding = 0,
  opt_no_simd,
  opt_subsample,
  opt_verbose 
} ;

/* options */
uMexOption options [] = {
{"Padding",      1,   opt_padding       },
{"Verbose",      0,   opt_verbose       },
{"Subsample",    1,   opt_subsample     },
{0,              0,   0                 }
} ;

void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_S, IN_END} ;
  enum {OUT_J=0} ;
  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  int padding = VL_PAD_BY_CONTINUITY ;
  int flags ;
  vl_bool no_simd = 0 ;
  int subsample = 1 ;
  int verb = 0 ;  
  double sigma ;
  mxClassID classid ;
  
  int const *dims ;  

  int M,N,K,W,j,k,ndims ;
  int M_, N_, dims_[3] ;
  
  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */ 
  
  if (nin < 2) {
    mexErrMsgTxt("At least two input arguments required.");
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }
    
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_padding :
      {
        enum {buflen = 32} ;
        char buf [buflen] ;
        if (!uIsString(optarg, -1)) {
          mexErrMsgTxt("PADDING argument must be a string") ;
        }
        mxGetString(optarg, buf, buflen) ;
        buf [buflen - 1] = 0 ;
        if (uStrICmp("zero", buf)) {
          padding = VL_PAD_BY_ZERO ;
        } else if (uStrICmp("continuity", buf)) {
          padding = VL_PAD_BY_CONTINUITY ;
        } else {
          mexErrMsgTxt("PADDING can be either ZERO or CONTINUITY") ;
        }
        break ;
      }
        
      case opt_subsample :
        if (!uIsRealScalar(optarg)) {
          mexErrMsgTxt("SUBSAMPLE must be a scalar") ;
        }
        subsample = *mxGetPr(optarg) ;
        if (subsample < 1) {
          mexErrMsgTxt("SUBSAMPLE must be not less than one") ;
        }
        break ;

      case opt_verbose :
        ++ verb ;
        break ;
        
      case opt_no_simd :
        no_simd = 1 ;
        break ;
        
      default: 
        assert(0) ;
    }
  }
    
  if (!uIsRealScalar(in[IN_S])) {
    mexErrMsgTxt("S must be a real scalar") ;
  }
  
  classid = mxGetClassID(in[IN_I]) ;
  
  if (classid != mxDOUBLE_CLASS &&
      classid != mxSINGLE_CLASS) {
    mexErrMsgTxt("I must be either DOUBLE or SINGLE.") ;
  }
  
  if (mxGetNumberOfDimensions(in[IN_I]) > 3) {
    mexErrMsgTxt("I must be either a two or three dimensional array.") ;
  }
  
  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims = mxGetDimensions(in[IN_I]) ;
  M = dims[0] ;
  N = dims[1] ;
  K = (ndims > 2) ? dims[2] : 1 ;

  sigma = *mxGetPr(in[IN_S]) ;
  if ((sigma < 0.01) && (subsample == 1)) {    
    out[OUT_J] = mxDuplicateArray(in[IN_I]) ;
    return ;
  }
  
  M_ = (M - 1) / subsample + 1 ;
  N_ = (N - 1) / subsample + 1 ;
  dims_ [0] = M_ ;
  dims_ [1] = N_ ;
  if (ndims > 2) dims_ [2] = ndims ;
  
  out[OUT_J] = mxCreateNumericArray(ndims, dims_, classid, mxREAL) ;

  if (verb) {    
    char const * msg = 0 ;
    mexPrintf("%dx%d -> %dx%d, ", N, M, N_, M_) ;
    switch (padding) {
      case VL_PAD_BY_ZERO       : msg = "with zeroes" ; break ;
      case VL_PAD_BY_CONTINUITY : msg = "by continuity" ; break ;
      default: assert (0) ; break ;
    }
    mexPrintf("imsmooth: padding %s, ", msg) ;
    switch (classid) {
      case mxDOUBLE_CLASS: msg = "DOUBLE" ; break ;
      case mxSINGLE_CLASS: msg = "SINGLE" ; break ;
      default: assert (0) ; break ;
    }
    mexPrintf("data is %s, ", msg) ;
    if (no_simd) mexPrintf("no SIMD accel,") ;
    else mexPrintf("SIMD accel, ") ;    
    mexPrintf("subsample: %d\n", subsample) ;
  }
  
  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */ 
  W = ceil (4 * sigma) ;
  flags  = padding ;
  flags |= VL_TRANSPOSE ;
  
  switch (classid) {
    case mxSINGLE_CLASS: 
#undef FLT
#undef VL_IMCONVCOL
#define FLT float
#define VL_IMCONVCOL vl_imconvcol_vf
#include "imsmooth.tc"
    case mxDOUBLE_CLASS:
#undef FLT
#undef VL_IMCONVCOL
#define FLT double 
#define VL_IMCONVCOL vl_imconvcol_vd
#include "imsmooth.tc"
    default: assert (0) ; break ;
  }
}
