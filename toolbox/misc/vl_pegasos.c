/** @internal
 ** @file     vl_pegasos.c
 ** @brief    vl_pegasos.c
 ** @author   Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/pegasos.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* option codes */
enum {
  opt_verbose, opt_bias_multiplier, opt_num_iterations,
  opt_starting_iteration, opt_starting_model, opt_permutation,
  opt_preconditioner
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"BiasMultiplier",    1,   opt_bias_multiplier     },
  {"NumIterations",     1,   opt_num_iterations      },
  {"StartingIteration", 1,   opt_starting_iteration  },
  {"StartingModel",     1,   opt_starting_model      },
  {"Permutation",       1,   opt_permutation         },
  {"Preconditioner",    1,   opt_preconditioner      },
  {0,                   0,   0                       }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATA, IN_LABELS, IN_LAMBDA, IN_END} ;
  enum {OUT_MODEL = 0} ;

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  double biasMultiplier = 0 ;
  double lambda ;
  void * data ;
  void * preconditioner = NULL ;
  vl_size preconditionerDimension = 0 ;
  mxClassID dataClass ;
  vl_type dataType ;
  vl_size numSamples ;
  vl_size dimension ;
  vl_size numIterations = 0 ;
  vl_uindex startingIteration = 1 ;
  vl_uint32 * permutation  = NULL ;
  vl_size permutationSize = 0 ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError(vlmxErrInvalidArgument,
              "At least three arguments are required.") ;
  } else if (nout > 2) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  dataClass = mxGetClassID(IN(DATA)) ;

  if (! vlmxIsMatrix (IN(DATA), -1, -1) ||
      ! vlmxIsReal (IN(DATA))) {
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be a real matrix.") ;
  }

  data = mxGetData (IN(DATA)) ;
  dimension = mxGetM(IN(DATA)) ;
  numSamples = mxGetN(IN(DATA)) ;

  switch (dataClass) {
  case mxSINGLE_CLASS : dataType = VL_TYPE_FLOAT ; break ;
  case mxDOUBLE_CLASS : dataType = VL_TYPE_DOUBLE ; break ;
  default:
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be either SINGLE or DOUBLE.") ;
  }

  if (mxGetClassID(IN(LABELS)) != mxINT8_CLASS) {
    vlmxError(vlmxErrInvalidArgument, "LABELS must be INT8.") ;
  }
  if (! vlmxIsVector(IN(LABELS), numSamples)) {
    vlmxError(vlmxErrInvalidArgument, "LABELS is not a vector of dimension compatible with DATA.") ;
  }

  if (! vlmxIsPlainScalar(IN(LAMBDA))) {
    vlmxError(vlmxErrInvalidArgument, "LAMBDA is not a plain scalar.") ;
  }
  lambda = *mxGetPr(IN(LAMBDA)) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_bias_multiplier :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "BIASMULTIPLIER is not a plain scalar.") ;
        }
        biasMultiplier = *mxGetPr(optarg) ;
        break ;

      case opt_num_iterations :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is not a plain scalar.") ;
        }
        if (*mxGetPr(optarg) < 0) {
          vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is negative.") ;
        }
        numIterations = (vl_size) *mxGetPr(optarg) ;
        break ;

      case opt_starting_iteration :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is not a plain scalar.") ;
        }
        if (*mxGetPr(optarg) < 1) {
          vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is smaller than 1.") ;
        }
        startingIteration = (vl_size) *mxGetPr(optarg) ;
        break ;

      case opt_starting_model :
        if (!vlmxIsVector(optarg, -1) ||
            mxIsComplex(optarg) ||
            !mxIsNumeric(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL is not a real vector.") ;
        }
        OUT(MODEL) = mxDuplicateArray(optarg) ;
        break ;

      case opt_permutation :
        if (!vlmxIsVector(optarg, -1) ||
            mxIsComplex(optarg) ||
            mxGetClassID(optarg) != mxUINT32_CLASS) {
          vlmxError(vlmxErrInvalidArgument, "PERMUTATION is not a UINT32 vector.") ;
        }
        permutationSize = mxGetNumberOfElements(optarg) ;
        permutation = mxMalloc(sizeof(vl_uint32) * permutationSize) ;
        {
          /* adjust indexing */
          vl_uint32 const * matlabPermutation = mxGetData(optarg) ;
          vl_uindex k ;
          for (k = 0 ; k < permutationSize ; ++k) {
            permutation[k] = matlabPermutation[k] - 1 ;
            if (permutation[k] >= numSamples) {
              vlmxError(vlmxErrInconsistentData,
                        "Permutation indexes out of bounds: PERMUTATION(%d) = %d > %d = number of data samples.",
                        k + 1, permutation[k] + 1, numSamples) ;
            }
          }
        }
        break ;

      case opt_preconditioner :
        if (!vlmxIsVector(optarg, -1) ||
            mxIsComplex(optarg) ||
            !mxIsNumeric(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "PRECONDITIONER is not a real vector.") ;
        }
        if (mxGetClassID(optarg) != dataClass) {
          vlmxError(vlmxErrInvalidArgument, "PRECODNITIONER storage class does not match the data.") ;
        }
        preconditioner = mxGetData(optarg) ;
        preconditionerDimension = mxGetNumberOfElements(optarg) ;
        break ;

      case opt_verbose :
        ++ verbose ;
        break ;
    }
  }

  if (preconditioner && preconditionerDimension != (dimension + (biasMultiplier > 0))) {
    vlmxError(vlmxErrInvalidArgument, "PRECONDITIONER has incompatible dimension.") ;
  }

  if (numIterations == 0) {
    numIterations = (vl_size) 10 / (lambda + 1) ;
  }

  if (! OUT(MODEL)) {
    OUT(MODEL) = mxCreateNumericMatrix(dimension + (biasMultiplier > 0),
                                       1,
                                       dataClass, mxREAL) ;
  } else {
    if (mxGetClassID(OUT(MODEL)) != dataClass) {
      vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL is not of the same class of DATA.") ;
    }
    if (mxGetNumberOfElements(OUT(MODEL)) != dimension + (biasMultiplier > 0)) {
      vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL has incompatible dimension.") ;
    }
  }

  if (verbose) {
    mexPrintf("vl_pegasos: Lambda = %g\n", lambda) ;
    mexPrintf("vl_pegasos: BiasMultiplier = %g\n", biasMultiplier) ;
    mexPrintf("vl_pegasos: NumIterations = %d\n", numIterations) ;
    mexPrintf("vl_pegasos: permutation size = %d\n", permutationSize) ;
    mexPrintf("vl_pegasos: using preconditioner = %s\n", VL_YESNO(preconditioner)) ;
  }

  switch (dataType) {
    case VL_TYPE_FLOAT :
      vl_pegasos_train_binary_svm_f((float *)mxGetData(OUT(MODEL)),
                                    (float const *)mxGetPr(IN(DATA)), dimension, numSamples,
                                    (vl_int8 const *)mxGetData(IN(LABELS)),
                                    lambda,
                                    biasMultiplier,
                                    startingIteration,
                                    numIterations,
                                    NULL,
                                    permutation,
                                    permutationSize,
                                    (float const*)preconditioner) ;
      break ;
    case VL_TYPE_DOUBLE:
      vl_pegasos_train_binary_svm_d((double *)mxGetData(OUT(MODEL)),
                                    (double const *)mxGetData(IN(DATA)), dimension, numSamples,
                                    (vl_int8 const *)mxGetData(IN(LABELS)),
                                    lambda,
                                    biasMultiplier,
                                    startingIteration,
                                    numIterations,
                                    NULL,
                                    permutation,
                                    permutationSize,
                                    (double const *)preconditioner) ;
      break ;
  }

  if (permutation) vl_free(permutation) ;

}
