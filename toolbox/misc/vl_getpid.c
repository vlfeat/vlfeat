/** @internal
 ** @file    vl_getpid.c
 ** @author  Andrea Vedaldi
 ** @brief   MEX implementation of VL_GETPID()
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <vl/generic.h>

#ifdef VL_OS_WIN
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <mexutils.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[] VL_UNUSED)
{
  double pid ;

  if (nin > 0) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

#ifdef VL_OS_WIN
  pid = (double) GetCurrentProcessId() ;
#else
  pid = (double) getpid() ;
#endif
  out[0] = vlmxCreatePlainScalar (pid) ;
}
