/** @file   vl_simdctlr.c
 ** @brief  vl_simdctrl MEX definition
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include "mexutils.h"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_ENABLED = 0} ;
  enum {OUT_ENABLED = 0} ;

  vl_bool wasEnabled = vl_get_simd_enabled() ;

  if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument,
              "at most one output argument") ;
  }

  OUT(ENABLED) = vlmxCreatePlainScalar (wasEnabled) ;

  if (nin == 0) {
    return ;
  }
  if (nin > 1) {
    vlmxError(vlmxErrInvalidArgument,
      "At most one argument") ;
  }
  if (!vlmxIsScalar(IN(ENABLED))) {
    vlmxError(vlmxErrInvalidArgument,
      "ENABLED must be a scalar") ;
  }

  vl_set_simd_enabled ((vl_bool) mxGetScalar(IN(ENABLED))) ;
}
