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
mexFunction(int nout VL_UNUSED, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_NUM = 0} ;
  enum {OUT_NUM = 0} ;
  vl_size numThreads ;

  numThreads = vl_get_max_threads() ;

  if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument, "More than one ouptut argumnets requested.") ;
  }
  if (nin > 1) {
    vlmxError(vlmxErrInvalidArgument, "More than one input argument specified.") ;
  }


  OUT(NUM) = vlmxCreatePlainScalar (numThreads) ;

  if (nin == 0) {
    return ;
  }

  if (!vlmxIsScalar(IN(NUM))) {
    vlmxError(vlmxErrInvalidArgument, "NUM is not a scalar.") ;
  }

  numThreads = (vl_size) mxGetScalar(IN(NUM)) ;
  vl_set_num_threads (numThreads) ;
}
