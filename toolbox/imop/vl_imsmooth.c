/** @internal
 ** @file   imsmooth.c
 ** @author Andrea Vedaldi
 ** @brief  Smooth an image - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/mathop.h>
#include <vl/imopv.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* option codes */
enum {
  opt_padding = 0,
  opt_subsample,
  opt_kernel,
  opt_verbose 
} ;

/* options */
uMexOption options [] = {
{"Padding",      1,   opt_padding       },
{"Verbose",      0,   opt_verbose       },
{"Subsample",    1,   opt_subsample     },
{"Kernel",       1,   opt_kernel        },
{0,              0,   0                 }
} ;

enum {GAUSSIAN, TRIANGULAR} ;

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
  int kernel = GAUSSIAN ;
  int flags ;
  int subsample = 1 ;
  int verb = 0 ;  
  double sigma ;
  mxClassID classid ;  
  
  int M,N,K,j,k,ndims ;
  int M_, N_ ;
  mwSize dims_[3] ;
  mwSize const *dims ;
  
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
        if (uStrICmp("zero", buf) == 0) {
          padding = VL_PAD_BY_ZERO ;
        } else if (uStrICmp("continuity", buf) == 0) {
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

      case opt_kernel :
      {
        enum {buflen = 32} ;
        char buf [buflen] ;
        if (!uIsString(optarg, -1)) {
          mexErrMsgTxt("KERNEL argument must be a string") ;
        }
        mxGetString(optarg, buf, buflen) ;
        buf [buflen - 1] = 0 ;
        if (uStrICmp("gaussian", buf) == 0) {
          kernel = GAUSSIAN ;
        } else if (uStrICmp("triangular", buf) == 0) {
          kernel = TRIANGULAR ;
        } else {
          mexErrMsgTxt("Unknown kernel type") ;
        }
        break ;
      }

      case opt_verbose :
        ++ verb ;
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
    char const *classid_str = 0, *kernel_str = 0, *padding_str = 0 ;
    switch (padding) {
      case VL_PAD_BY_ZERO       : padding_str = "with zeroes" ; break ;
      case VL_PAD_BY_CONTINUITY : padding_str = "by continuity" ; break ;
      default: assert (0) ; break ;
    }
    switch (classid) {
      case mxDOUBLE_CLASS: classid_str = "DOUBLE" ; break ;
      case mxSINGLE_CLASS: classid_str = "SINGLE" ; break ;
      default: assert (0) ; break ;
    }
    switch (kernel) {
      case GAUSSIAN:   kernel_str = "Gaussian" ; break ;
      case TRIANGULAR: kernel_str = "triangular" ; break ;
      default: assert (0) ; break ;
    }
    
    mexPrintf("imsmooth: [%dx%d] -> [%dx%d] (%s, sampling per. %d)\n", 
              N, M, N_, M_, classid_str, subsample) ;
    mexPrintf("          padding: %s\n", padding_str) ;
    mexPrintf("          kernel: %s\n", kernel_str) ;
    mexPrintf("          sigma: %g\n", sigma) ;
    mexPrintf("          SIMD enabled: %s\n", vl_get_simd_enabled() ? "yes" : "no") ;
  }
  
  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */ 
  flags  = padding ;
  flags |= VL_TRANSPOSE ;
  
  switch (classid) {
    case mxSINGLE_CLASS: 
#undef FLT
#undef VL_IMCONVCOL
#undef VL_IMCONVCOLTRI
#define FLT float
#define VL_IMCONVCOL vl_imconvcol_vf
#define VL_IMCONVCOLTRI vl_imconvcoltri_vf
#include "imsmooth.tc"
      break ;
    case mxDOUBLE_CLASS:
#undef FLT
#undef VL_IMCONVCOL
#undef VL_IMCONVCOLTRI
#define FLT double 
#define VL_IMCONVCOL vl_imconvcol_vd
#define VL_IMCONVCOLTRI vl_imconvcoltri_vd
#include "imsmooth.tc"
      break ;
    default: assert (0) ; break ;
  }
}
