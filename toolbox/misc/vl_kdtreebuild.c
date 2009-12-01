/** @internal
 ** @file     vl_kdetreebuild.c
 ** @brief    vl_KDForestbuild MEX implementation
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>
#include <vl/kdtree.h>
#include <vl/stringop.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* option codes */
enum {
  opt_verbose, opt_threshold_method, opt_num_trees
} ;

/* options */
uMexOption options [] = {
{"Verbose",          0,   opt_verbose          },
{"ThresholdMethod",  1,   opt_threshold_method },
{"NumTrees",         1,   opt_num_trees        },
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
  float * data ;
  int numData ;
  int numDimensions ;
  int thresholdingMethod = VL_KDTREE_MEDIAN ;
  int numTrees = 1 ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    mexErrMsgTxt("One argument required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }

  if (! uIsMatrix (in[IN_DATA], -1, -1) ||
      mxGetClassID(in[IN_DATA]) != mxSINGLE_CLASS) {
    mexErrMsgTxt("DATA must be a real matrix of class SINGLE.") ;
  }

  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    char buffer [1024] ;
    switch (opt) {
      case opt_threshold_method :
        mxGetString (optarg, buffer, sizeof(buffer)/sizeof(buffer[0])) ;
        if (! uIsString(optarg, -1)) {
          mexErrMsgTxt("THRESHOLDMETHOD must be a string") ;
        }
        if (vl_string_casei_cmp(buffer, "median") == 0) {
          thresholdingMethod = VL_KDTREE_MEDIAN ;
        } else if (vl_string_casei_cmp(buffer, "mean") == 0) {
          thresholdingMethod = VL_KDTREE_MEAN ;
        } else {
          uErrMsgTxt("Unknown thresholding method %s", buffer) ;
        }
        break ;

      case opt_num_trees :
        if (! uIsScalar(optarg) ||
            (numTrees = mxGetScalar(optarg)) < 1) {
          mexErrMsgTxt("NUMTREES must be not smaller than one") ;
        }
        break ;

      case opt_verbose :
        ++ verbose ;
        break ;
    }
  }

  data = (float*) mxGetData (in[IN_DATA]) ;
  numData = mxGetN (in[IN_DATA]) ;
  numDimensions = mxGetM (in[IN_DATA]) ;

  forest = vl_kdforest_new (numDimensions, numTrees) ;
  vl_kdforest_set_thresholding_method (forest, thresholdingMethod) ;

  if (verbose) {
    char const * str = 0 ;
    mexPrintf("vl_kdforestbuild: data %d x %d\n", numDimensions, numData) ;
    switch (vl_kdforest_get_thresholding_method(forest)) {
      case VL_KDTREE_MEAN : str = "mean" ; break ;
      case VL_KDTREE_MEDIAN : str = "median" ; break ;
      default: assert(0) ;
    }
    mexPrintf("vl_kdforestbuild: threhsold selection method: %s\n", str) ;
    mexPrintf("vl_kdforestbuild: number of trees: %d\n", vl_kdforest_get_num_trees(forest)) ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  vl_kdforest_build (forest, numData, data) ;

  if (verbose) {
    int ti ;
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
