/** @internal
 ** @file     vl_kdetreebuild.c
 ** @brief    vl_KDForestbuild MEX implementation
 ** @author   Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/kdtree.h>
#include <vl/stringop.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* option codes */
enum {
  opt_verbose, opt_threshold_method, opt_num_trees, opt_distance
} ;

/* options */
vlmxOption  options [] = {
{"Verbose",          0,   opt_verbose          },
{"ThresholdMethod",  1,   opt_threshold_method },
{"NumTrees",         1,   opt_num_trees        },
{"Distance",         1,   opt_distance         },
{0,                  0,   0                    }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATA = 0, IN_END} ;
  enum {OUT_TREE = 0} ;

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  VlKDForest * forest ;
  void * data ;
  vl_size numData ;
  vl_size dimension ;
  mxClassID dataClass ;
  vl_type dataType ;
  int thresholdingMethod = VL_KDTREE_MEDIAN ;
  VlVectorComparisonType distance = VlDistanceL2;
  vl_size numTrees = 1 ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrInvalidArgument,
             "At least one argument required") ;
  } else if (nout > 2) {
    vlmxError(vlmxErrInvalidArgument,
             "Too many output arguments");
  }

  dataClass = mxGetClassID(IN(DATA)) ;

  if (! vlmxIsMatrix (IN(DATA), -1, -1) ||
      ! vlmxIsReal (IN(DATA))) {
    vlmxError(vlmxErrInvalidArgument,
             "DATA must be a real matrix ") ;
  }

  switch (dataClass) {
    case mxSINGLE_CLASS : dataType = VL_TYPE_FLOAT ; break ;
    case mxDOUBLE_CLASS : dataType = VL_TYPE_DOUBLE ; break ;
    default:
      vlmxError(vlmxErrInvalidArgument,
               "DATA must be either SINGLE or DOUBLE") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buffer [1024] ;
    switch (opt) {
      case opt_threshold_method :
        mxGetString (optarg, buffer, sizeof(buffer)/sizeof(buffer[0])) ;
        if (! vlmxIsString(optarg, -1)) {
          vlmxError(vlmxErrInvalidOption,
                   "THRESHOLDMETHOD must be a string") ;
        }
        if (vl_string_casei_cmp(buffer, "median") == 0) {
          thresholdingMethod = VL_KDTREE_MEDIAN ;
        } else if (vl_string_casei_cmp(buffer, "mean") == 0) {
          thresholdingMethod = VL_KDTREE_MEAN ;
        } else {
          vlmxError(vlmxErrInvalidOption,
                   "Unknown thresholding method %s", buffer) ;
        }
        break ;

      case opt_num_trees :
        if (! vlmxIsScalar(optarg) ||
            (numTrees = mxGetScalar(optarg)) < 1) {
          vlmxError(vlmxErrInvalidOption,
                   "NUMTREES must be not smaller than one") ;
        }
        break ;

      case opt_verbose :
        ++ verbose ;
        break ;

      case opt_distance :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                    "DISTANCE must be a string.") ;
        }
        if (mxGetString (optarg, buffer, sizeof(buffer))) {
          vlmxError (vlmxErrInvalidArgument,
                    "DISTANCE argument too long.") ;
        }
        if (vlmxCompareStringsI("l2", buffer) == 0) {
          distance = VlDistanceL2 ;
        } else if (vlmxCompareStringsI("l1", buffer) == 0) {
          distance = VlDistanceL1 ;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                    "Invalid value %s for DISTANCE", buffer) ;
        }
        break ;

    }
  }

  data = mxGetData (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;
  dimension = mxGetM (IN(DATA)) ;

  if (dimension < 1) {
    vlmxError (vlmxErrInconsistentData,
               "DATA must have at least one row.") ;
  }

  if (numData < 1) {
    vlmxError (vlmxErrInconsistentData,
               "DATA must have at least one column.") ;
  }


  forest = vl_kdforest_new (dataType, dimension, numTrees, distance) ;
  vl_kdforest_set_thresholding_method (forest, thresholdingMethod) ;

  if (verbose) {
    char const * str = 0 ;
    mexPrintf("vl_kdforestbuild: data %s [%d x %d]\n",
              vl_get_type_name (dataType), dimension, numData) ;
    switch (vl_kdforest_get_thresholding_method(forest)) {
      case VL_KDTREE_MEAN : str = "mean" ; break ;
      case VL_KDTREE_MEDIAN : str = "median" ; break ;
      default: abort() ;
    }
    mexPrintf("vl_kdforestbuild: threshold selection method: %s\n", str) ;
    mexPrintf("vl_kdforestbuild: number of trees: %d\n",
              vl_kdforest_get_num_trees(forest)) ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  vl_kdforest_build (forest, numData, data) ;

  if (verbose) {
    vl_uindex ti ;
    for (ti = 0 ; ti < vl_kdforest_get_num_trees(forest) ; ++ ti) {
      mexPrintf("vl_kdforestbuild: tree %d: depth %d, num nodes %d\n",
                ti,
                vl_kdforest_get_depth_of_tree(forest, ti),
                vl_kdforest_get_num_nodes_of_tree(forest, ti)) ;
    }
  }

  out[OUT_TREE] = new_array_from_kdforest (forest) ;
  vl_kdforest_delete (forest) ;
}
