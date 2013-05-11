/** @file   vl_threads.c
 ** @brief  vl_threads MEX definition
 ** @author Andrea Vedaldi
 **/

/*
 Copyright (C) 2013 Andrea Vedaldi
 All rights reserved.
 
 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#include "mexutils.h"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_NUM = 0} ;
  enum {OUT_NUM = 0, OUT_MAXNUM} ;
  vl_size numThreads, maxNumThreads ;
    
  if (nout > 2) {
    vlmxError(vlmxErrInvalidArgument, "More than two output arguments requested.") ;
  }
  
  numThreads = vl_get_num_threads() ;
  maxNumThreads = vl_get_max_num_threads() ;
  
  OUT(NUM) = vlmxCreatePlainScalar (numThreads) ;
  OUT(MAXNUM) = vlmxCreatePlainScalar (maxNumThreads) ;
  
  if (nin == 0) {
    return ;
  }
  if (nin > 1) {
    vlmxError(vlmxErrInvalidArgument, "More than one input argument specified.") ;
  }
  if (!vlmxIsScalar(IN(NUM))) {
    vlmxError(vlmxErrInvalidArgument, "NUM is not a scalar.") ;
  }
  
  numThreads = mxGetScalar(IN(NUM)) ;
  numThreads = VL_MAX(numThreads, 0) ;
  numThreads = VL_MIN(numThreads, maxNumThreads) ;
  vl_set_num_threads (numThreads) ;
}
