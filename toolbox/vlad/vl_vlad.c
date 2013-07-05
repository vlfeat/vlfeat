/** @file   vl_vlad.c
 ** @brief  vl_vlad MEX definition.
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/vlad.h>
#include <mexutils.h>
#include <string.h>
#include <stdio.h>

enum {
  opt_verbose,
  opt_normalize_components,
  opt_unnormalized,
  opt_square_root,
  opt_normalize_mass
} ;


vlmxOption  options [] = {
  {"Verbose",             0,   opt_verbose                  },
  {"Unnormalized",        0,   opt_unnormalized             },
  {"NormalizeComponents", 0,   opt_normalize_components     },
  {"NormalizeMass",       0,   opt_normalize_mass           },
  {"SquareRoot",          0,   opt_square_root              }
} ;

/* driver */
void
mexFunction (int nout VL_UNUSED, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_MEANS, IN_ASSIGNMENTS, IN_END} ;
  enum {OUT_ENC} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  mxArray const * means_array = in[IN_MEANS] ;
  mxArray const * data_array = in[IN_DATA] ;
  mxArray const * assign_array = in[IN_ASSIGNMENTS] ;

  vl_size numClusters ;
  vl_size dimension ;
  vl_size numData ;
  int flags = 0 ;

  void const * means = NULL;
  void const * assignments = NULL;
  void const * data = NULL ;
  int verbosity = 0 ;

  vl_type dataType ;
  mxClassID classID ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError (vlmxErrInvalidArgument,
               "At least three arguments required.");
  }
  if (nout > 1) {
    vlmxError (vlmxErrInvalidArgument,
               "At most one output argument.");
  }

  if (!vlmxIsMatrix(IN(DATA),-1,-1)) {
    vlmxError (vlmxErrInvalidArgument,
               "DATA is not a dense matrix.") ;
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
  if (mxGetClassID (IN(ASSIGNMENTS)) != classID) {
    vlmxError(vlmxErrInvalidArgument, "ASSIGNMENTS is not of the same class as DATA.") ;

  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;
  numClusters = mxGetN (IN(MEANS)) ;

  if (dimension == 0) {
    vlmxError (vlmxErrInvalidArgument, "SIZE(DATA,1) is zero.") ;
  }

  if (!vlmxIsMatrix(IN(MEANS), dimension, -1)) {
    vlmxError (vlmxErrInvalidArgument, "MEANS is not a matrix or does not have the right size.") ;
  }

  if (!vlmxIsMatrix(IN(ASSIGNMENTS), numClusters, -1)) {
    vlmxError (vlmxErrInvalidArgument, "ASSIGNMENTS is not a matrix or does not have the right size.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_verbose : ++ verbosity ; break ;
      case opt_unnormalized: flags |= VL_VLAD_FLAG_UNNORMALIZED ; break ;
      case opt_normalize_components: flags |= VL_VLAD_FLAG_NORMALIZE_COMPONENTS ; break ;
      case opt_normalize_mass: flags |= VL_VLAD_FLAG_NORMALIZE_MASS ; break ;
      case opt_square_root: flags |= VL_VLAD_FLAG_SQUARE_ROOT ; break ;
      default :
        abort() ;
      break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  data = mxGetPr(data_array);
  means = mxGetPr(means_array);
  assignments = mxGetData(assign_array);

  if (verbosity) {
    mexPrintf("vl_vlad: num data: %d\n", numData) ;
    mexPrintf("vl_vlad: num clusters: %d\n", numClusters) ;
    mexPrintf("vl_vlad: data dimension: %d\n", dimension) ;
    mexPrintf("vl_vlad: code dimension: %d\n", numClusters * dimension) ;
    mexPrintf("vl_vlad: unnormalized: %s\n", VL_YESNO(flags & VL_VLAD_FLAG_UNNORMALIZED)) ;
    mexPrintf("vl_vlad: normalize mass: %s\n", VL_YESNO(flags & VL_VLAD_FLAG_NORMALIZE_MASS)) ;
    mexPrintf("vl_vlad: normalize components: %s\n", VL_YESNO(flags & VL_VLAD_FLAG_NORMALIZE_COMPONENTS)) ;
    mexPrintf("vl_vlad: square root: %s\n", VL_YESNO(flags & VL_VLAD_FLAG_SQUARE_ROOT)) ;
  }

  OUT(ENC) = mxCreateNumericMatrix (dimension * numClusters, 1, classID, mxREAL) ;

  vl_vlad_encode (mxGetPr(OUT(ENC)), dataType,
                  means, dimension, numClusters,
                  data, numData,
                  assignments,
                  flags) ;
}
