/** @file   vl_version.c
 ** @brief  vl_version MEX definition
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS */

#include "mexutils.h"
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
      assert(0) ;
    }
  }

  if (verbose) {
    snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]),
             "%s (%s)", VL_VERSION_STRING, __DATE__) ;
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

