/** @file   vl_fisher.c
 ** @brief  vl_fisher MEX definition.
 ** @author Andrea Vedaldi
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/fisher.h>
#include <mexutils.h>
#include <string.h>
#include <stdio.h>

enum {
  opt_verbose,
  opt_normalized,
  opt_square_root,
  opt_improved,
  opt_fast
} ;

vlmxOption  options [] = {
  {"Verbose",             0,   opt_verbose                  },
  {"Normalized",          0,   opt_normalized               },
  {"SquareRoot",          0,   opt_square_root              },
  {"Improved",            0,   opt_improved                 },
  {"Fast",                0,   opt_fast                     }
} ;

/* driver */
void
mexFunction (int nout VL_UNUSED, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_MEANS, IN_COVARIANCES, IN_PRIORS, IN_END} ;
  enum {OUT_ENC} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  vl_size numClusters = 10;
  vl_size dimension ;
  vl_size numData ;
  int flags = 0 ;

  void * covariances = NULL;
  void * means = NULL;
  void * priors = NULL;
  void * data = NULL ;
  vl_size numTerms ;

  int verbosity = 0 ;

  vl_type dataType ;
  mxClassID classID ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 4) {
    vlmxError (vlmxErrInvalidArgument,
               "At least four arguments required.");
  }
  if (nout > 1) {
    vlmxError (vlmxErrInvalidArgument,
               "At most one output argument.");
  }

  classID = mxGetClassID (IN(DATA)) ;
  switch (classID) {
    case mxSINGLE_CLASS: dataType = VL_TYPE_FLOAT ; break ;
    case mxDOUBLE_CLASS: dataType = VL_TYPE_DOUBLE ; break ;
    default:
      vlmxError (vlmxErrInvalidArgument,
                 "DATA is neither of class SINGLE or DOUBLE.") ;
  }

  if (mxGetClassID (IN(MEANS)) != classID) {
    vlmxError(vlmxErrInvalidArgument, "MEANS is not of the same class as DATA.") ;
  }
  if (mxGetClassID (IN(COVARIANCES)) != classID) {
    vlmxError(vlmxErrInvalidArgument, "COVARIANCES is not of the same class as DATA.") ;
  }
  if (mxGetClassID (IN(PRIORS)) != classID) {
    vlmxError(vlmxErrInvalidArgument, "PRIORS is not of the same class as DATA.") ;
  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;
  numClusters = mxGetN (IN(MEANS)) ;

  if (dimension == 0) {
    vlmxError (vlmxErrInvalidArgument, "SIZE(DATA,1) is zero.") ;
  }
  if (!vlmxIsMatrix(IN(MEANS), dimension, numClusters)) {
    vlmxError (vlmxErrInvalidArgument, "MEANS is not a matrix or does not have the correct size.") ;
  }
  if (!vlmxIsMatrix(IN(COVARIANCES), dimension, numClusters)) {
    vlmxError (vlmxErrInvalidArgument, "COVARIANCES is not a matrix or does not have the correct size.") ;
  }
  if (!vlmxIsVector(IN(PRIORS), numClusters)) {
    vlmxError (vlmxErrInvalidArgument, "PRIORS is not a vector or does not have the correct size.") ;
  }
  if (!vlmxIsMatrix(IN(DATA), dimension, numData)) {
    vlmxError (vlmxErrInvalidArgument, "DATA is not a matrix or does not have the correct size.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_verbose : ++ verbosity ; break ;
      case opt_normalized: flags |= VL_FISHER_FLAG_NORMALIZED ; break ;
      case opt_square_root: flags |= VL_FISHER_FLAG_SQUARE_ROOT ; break ;
      case opt_improved: flags |= VL_FISHER_FLAG_IMPROVED ; break ;
      case opt_fast: flags |= VL_FISHER_FLAG_FAST ; break ;
      default : abort() ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  data = mxGetPr(IN(DATA)) ;
  means = mxGetPr(IN(MEANS)) ;
  covariances = mxGetPr(IN(COVARIANCES)) ;
  priors = mxGetPr(IN(PRIORS)) ;

  if (verbosity) {
    mexPrintf("vl_fisher: num data: %d\n", numData) ;
    mexPrintf("vl_fisher: num clusters: %d\n", numClusters) ;
    mexPrintf("vl_fisher: data dimension: %d\n", dimension) ;
    mexPrintf("vl_fisher: code dimension: %d\n", numClusters * dimension) ;
    mexPrintf("vl_fisher: square root: %s\n", VL_YESNO(flags & VL_FISHER_FLAG_SQUARE_ROOT)) ;
    mexPrintf("vl_fisher: normalized: %s\n", VL_YESNO(flags & VL_FISHER_FLAG_NORMALIZED)) ;
    mexPrintf("vl_fisher: fast: %s\n", VL_YESNO(flags & VL_FISHER_FLAG_FAST)) ;
  }

  /* -------------------------------------------------------------- */
  /*                                                       Encoding */
  /* -------------------------------------------------------------- */

  OUT(ENC) = mxCreateNumericMatrix (dimension * numClusters * 2, 1, classID, mxREAL) ;

  numTerms = vl_fisher_encode (mxGetData(OUT(ENC)), dataType,
                               means, dimension, numClusters,
                               covariances,
                               priors,
                               data, numData,
                               flags) ;

  if (verbosity) {
    mexPrintf("vl_fisher: sparsity of assignments: %.2f%% (%d non-negligible assignments)\n",
              100.0 * (1.0 - (double)numTerms/((double)numData*(double)numClusters+1e-12)),
              numTerms) ;
  }
}
