/** @file   vl_version.c
 ** @brief  vl_version MEX definition
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include "mexutils.h"
#include <vl/stringop.h>

#include <assert.h>

/* option codes */
enum {
  opt_verbose
} ;

/* options */
vlmxOption options [] = {
{"Verbose",          0,   opt_verbose          },
{0,                  0,   0                    }
} ;

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  int verbose = 0 ;
  char buffer [1024] ;
  int unsigned const bufferSize = sizeof(buffer)/sizeof(buffer[0]) ;

  int            opt ;
  int            next = 0 ;
  mxArray const *optarg ;

  VL_USE_MATLAB_ENV ;

  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_verbose :
      ++ verbose ;
      break ;
    default:
      abort() ;
    }
  }

  if (verbose) {
    int offset = 0 ;
    char * string = vl_configuration_to_string_copy() ;
    offset = vl_string_copy(buffer, bufferSize, string) ;
    snprintf(buffer + offset, bufferSize - offset,
             "    SIMD enabled: %s\n", VL_YESNO(vl_get_simd_enabled())) ;
    if(string) vl_free(string) ;
  } else {
    snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]),
             "%s", VL_VERSION_STRING) ;
  }

  if (nout == 0) {
    mexPrintf("%s\n", buffer) ;
  } else {
    out[0] = mxCreateString(buffer) ;
  }
}

