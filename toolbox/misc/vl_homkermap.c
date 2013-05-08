/** @file   vl_homkmap.c
 ** @brief  vl_homkmap - MEX implementation
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include "mexutils.h"
#include <vl/homkermap.h>
#include <vl/mathop.h>
#include <vl/stringop.h>

enum {
  opt_KCHI2,
  opt_KL1,
  opt_KJS,
  opt_KINTERS,
  opt_kernel,
  opt_gamma,
  opt_period,
  opt_window
} ;

vlmxOption options [] = {
  {"kl1",          0,   opt_KL1           },
  {"kchi2",        0,   opt_KCHI2         },
  {"kjs",          0,   opt_KJS           },
  {"kinters",      0,   opt_KINTERS       },
  {"kernel",       1,   opt_kernel        },
  {"gamma",        1,   opt_gamma         },
  {"period",       1,   opt_period        },
  {"window",       1,   opt_window        },
  {0,              0,   0                 }
} ;

/* -------------------------------------------------------------------
 *                                                              Driver
 * ---------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{

  typedef int unsigned data_t ;

  enum {IN_X = 0, IN_N, IN_END} ;
  enum {OUT_V = 0} ;
  VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
  VlHomogeneousKernelMapWindowType windowType = VlHomogeneousKernelMapWindowRectangular ;
  mwSize numDimensions ;
  mwSize const * dimensions ;
  mxClassID dataClassId ;
  double gamma = 1.0 ;
  int n ;
  double period = -1 ;

  /* for option parsing */
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }
  if (nin < IN_END) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }

  dataClassId = mxGetClassID(IN(X)) ;
  if (dataClassId != mxDOUBLE_CLASS &&
      dataClassId != mxSINGLE_CLASS) {
    vlmxError(vlmxErrInvalidArgument, "X is neither DOUBLE nor SINGLE.") ;
  }

  numDimensions = mxGetNumberOfDimensions(IN(X)) ;
  dimensions = mxGetDimensions(IN(X)) ;

  if (! vlmxIsPlainScalar(IN(N))) {
    vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
  }
  n = *mxGetPr(IN(N)) ;
  if (n < 0) {
    vlmxError(vlmxErrInvalidArgument, "N is negative.") ;
  }

  while ((opt = vlmxNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_KINTERS:
      case opt_KL1:
        kernelType = VlHomogeneousKernelIntersection ;
        break ;
      case opt_KCHI2:
        kernelType = VlHomogeneousKernelChi2 ;
        break ;
      case opt_KJS:
        kernelType = VlHomogeneousKernelJS ;
        break ;
      case opt_kernel:
        /* make the kernel option argument to be the next option, and
         hence processed by the code above */
        next-- ;
        break ;
      case opt_period:
        if (! vlmxIsPlainScalar(optarg)){
          vlmxError(vlmxErrInvalidArgument, "PERIOD is not a scalar.") ;
        }
        period = *mxGetPr(optarg) ;
        if (period <= 0) {
          vlmxError(vlmxErrInvalidArgument, "PERIOD is not positive.") ;
        }
        break ;
      case opt_gamma:
        if (! vlmxIsPlainScalar(optarg)){
          vlmxError(vlmxErrInvalidArgument, "GAMMA is not a scalar.") ;
        }
        gamma = *mxGetPr(optarg) ;
        if (gamma <= 0) {
          vlmxError(vlmxErrInvalidArgument, "GAMMA is not positive.") ;
        }
        break ;
      case opt_window:
        if (! vlmxIsString(optarg,-1)){
          vlmxError(vlmxErrInvalidArgument, "WINDOW is not a string.") ;
        } else {
          char buffer [1024] ;
          mxGetString(optarg, buffer, sizeof(buffer) / sizeof(char)) ;
          if (vl_string_casei_cmp("uniform", buffer) == 0) {
            windowType = VlHomogeneousKernelMapWindowUniform ;
          } else if (vl_string_casei_cmp("rectangular", buffer) == 0) {
            windowType = VlHomogeneousKernelMapWindowRectangular ;
          } else {
            vlmxError(vlmxErrInvalidArgument, "WINDOW=%s is not recognized.", buffer) ;
          }
        }
        break ;
      default:
        abort() ;
    }
  }

  /* -----------------------------------------------------------------
   *                                               Evaluate kernel map
   * -------------------------------------------------------------- */

  {
    vl_uindex j ;
    vl_size numElements = mxGetNumberOfElements(IN(X)) ;
    VlHomogeneousKernelMap * map = vl_homogeneouskernelmap_new (kernelType, gamma, n, period, windowType) ;
    mwSize extDimensions [20] ;
    for (j = 0 ; j < numDimensions ; ++j) extDimensions[j] = dimensions[j] ;
    extDimensions[0] *= 2*n+1 ;
    OUT(V) = mxCreateNumericArray(numDimensions, extDimensions, dataClassId, mxREAL) ;
    switch (dataClassId) {
      case mxDOUBLE_CLASS :
      {
        double * X = mxGetData(IN(X)) ;
        double * V = mxGetData(OUT(V)) ;
        for (j = 0 ; j < numElements ; ++j) {
          vl_homogeneouskernelmap_evaluate_d(map, V, 1, *X++) ;
          V += 2*n+1 ;
        }
        break ;
      }
      case mxSINGLE_CLASS :
      {
        float * X = mxGetData(IN(X)) ;
        float * V = mxGetData(OUT(V)) ;
        for (j = 0 ; j < numElements ; ++j) {
          vl_homogeneouskernelmap_evaluate_f(map, V, 1, *X++) ;
          V += 2*n+1 ;
        }
        break ;
      }
      default:
        abort() ;
    }
    vl_homogeneouskernelmap_delete (map) ;
  }
}
