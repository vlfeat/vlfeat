/** @file   vl_alldist.c
 ** @brief  vl_alldist - MEX implementation
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include "mexutils.h"
#include <vl/mathop.h>

enum {
  opt_LINF,
  opt_L2,
  opt_L1,
  opt_L0,
  opt_CHI2,
  opt_HELL,

  opt_KL2,
  opt_KL1,
  opt_KCHI2,
  opt_KHELL
} ;

uMexOption options [] = {
{"linf",         0,   opt_LINF          },
{"l2",           0,   opt_L2            },
{"l1",           0,   opt_L1            },
{"l0",           0,   opt_L0            },
{"chi2",         0,   opt_CHI2          },
{"hell",         0,   opt_HELL          },

{"kl2",          0,   opt_KL2           },
{"kl1",          0,   opt_KL1           },
{"kchi2",        0,   opt_KCHI2         },
{"khell",        0,   opt_KHELL         },

{0,              0,   0                 }
} ;

/* driver */
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{

  typedef int  unsigned data_t ;

  vl_bool autoComparison = VL_TRUE ;
  VlVectorComparisonType comparisonType = VlDistanceL2 ;

  enum {IN_X = 0, IN_Y} ;
  enum {OUT_D = 0} ;
  mwSize numDataX = 0 ;
  mwSize numDataY = 0 ;
  mwSize numDimensions ;
  mxClassID classId ;

  /* for option parsing */
  int opt ;
  int next ;
  mxArray const *optarg ;

  VL_USE_MATLAB_ENV ;

  if (nin < 1) {
    VLMX_EIA("At least one argument required.") ;
  }

  if (! (vlmxIsMatrix (in[IN_X],-1,-1) && vlmxIsReal(in[IN_X]))) {
    VLMX_EIA("X must be a real matrix.") ;
  }
  next = 1 ;
  classId = mxGetClassID(in[IN_X]) ;
  numDimensions = mxGetM(in[IN_X]) ;
  numDataX = mxGetN(in[IN_X]) ;

  if (nin > 1 && vlmxIsMatrix (in[IN_Y],-1,-1) && vlmxIsReal(in[IN_Y])) {
    next = 2 ;
    autoComparison = VL_FALSE ;
    numDataY = mxGetN(in[IN_Y]) ;
    if (mxGetClassID(in[IN_Y]) != classId) {
      VLMX_EIA("X and Y must have the same class.") ;
    }
    if (numDimensions != mxGetM(in[IN_Y])) {
      VLMX_EIA("X and Y must have the same number of rows.") ;
    }
  }

  if (classId != mxSINGLE_CLASS && classId != mxDOUBLE_CLASS) {
    VLMX_EIA("X must be either of class SINGLE or DOUBLE.");
  }

  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_L2    : comparisonType = VlDistanceL2 ; break ;
      case opt_L1    : comparisonType = VlDistanceL1 ; break ;
      case opt_CHI2  : comparisonType = VlDistanceChi2 ; break ;
      case opt_HELL  : comparisonType = VlDistanceHellinger ; break ;
      case opt_KL2   : comparisonType = VlKernelL2 ; break ;
      case opt_KL1   : comparisonType = VlKernelL1 ; break ;
      case opt_KCHI2 : comparisonType = VlKernelChi2 ; break ;
      case opt_KHELL : comparisonType = VlKernelHellinger ; break ;
      default:
        assert(0) ;
    }
  }

  /* allocate output */
  {
    mwSize dims [2] ;
    dims[0] = numDataX ;
    dims[1] = autoComparison ? numDataX : numDataY ;
    out[OUT_D] = mxCreateNumericArray (2, dims, classId, mxREAL) ;
  }

  /* make calculation */
  switch (classId) {
  case mxSINGLE_CLASS:
    {
      VlFloatVectorComparisonFunction f = vl_get_vector_comparison_function_f (comparisonType) ;
      if (autoComparison) {
        vl_eval_vector_comparison_on_all_pairs_f ((float*)mxGetData(out[OUT_D]),
                                                  numDimensions,
                                                  (float*)mxGetData(in[IN_X]), numDataX,
                                                  0, 0,
                                                  f) ;
      } else {
        vl_eval_vector_comparison_on_all_pairs_f ((float*)mxGetData(out[OUT_D]),
                                                  numDimensions,
                                                  (float*)mxGetData(in[IN_X]), numDataX,
                                                  (float*)mxGetData(in[IN_Y]), numDataY,
                                                  f) ;
      }
    }
    break ;

    case mxDOUBLE_CLASS:
    {
      VlDoubleVectorComparisonFunction f = vl_get_vector_comparison_function_d (comparisonType) ;
      if (autoComparison) {
        vl_eval_vector_comparison_on_all_pairs_d ((double*)mxGetData(out[OUT_D]),
                                                  numDimensions,
                                                  (double*)mxGetData(in[IN_X]), numDataX,
                                                  0, 0,
                                                  f) ;
      } else {
        vl_eval_vector_comparison_on_all_pairs_d ((double*)mxGetData(out[OUT_D]),
                                                  numDimensions,
                                                  (double*)mxGetData(in[IN_X]), numDataX,
                                                  (double*)mxGetData(in[IN_Y]), numDataY,
                                                  f) ;
      }
    }
    break ;

  default:
    assert(0) ;
  }
}
