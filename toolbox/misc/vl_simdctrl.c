/*
 *  vl_simdctrl.c
 *  vlfeat
 *
 *  Created by Andrea Vedaldi on 12/07/2009.
 *  Copyright 2009 UCLA. All rights reserved.
 *
 */

#include "mexutils.h"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_ENABLED} ;
  enum {OUT_ENABLED} ;

  vl_bool wasEnabled = vl_get_simd_enabled() ;

  out[OUT_ENABLED] = uCreateScalar(wasEnabled) ;

  if (nin == 0) {
    return ;
  }

  if (nin > 1) {
    mexErrMsgTxt("At most one argument") ;
  }

  if (!uIsScalar(in[IN_ENABLED])) {
    mexErrMsgTxt("ENABLED must be a scalar") ;
  }

  vl_set_simd_enabled(mxGetScalar(in[IN_ENABLED])) ;
}

