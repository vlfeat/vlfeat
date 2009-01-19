/** @internal
 ** @file    vl_getpid.c
 ** @author  Andrea Vedaldi
 ** @brief   MEX implementation of VL_GETPID()
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include <mexutils.h>
#include <vl/generic.h>

#ifdef VL_OS_WIN
#include <Windows.h>
#else
#include <unistd.h>
#endif

void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  double pid ;
#ifdef VL_OS_WIN
  pid = (double) GetCurrentProcessId() ;
#else
  pid = (double) getpid() ;
#endif
  out[0] = uCreateScalar(pid) ;
}
