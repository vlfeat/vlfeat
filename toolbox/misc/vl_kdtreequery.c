/** @internal
 ** @file     vl_kdtreequery.c
 ** @brief    vl_kdtreequery - MEX implementation
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

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* option codes */
enum {
  opt_verbose, opt_num_neighs, opt_max_num_comparisons, opt_multithreading
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"NumNeighbors",      1,   opt_num_neighs          },
  {"MaxComparisons",    1,   opt_max_num_comparisons },
  {"MaxNumComparisons", 1,   opt_max_num_comparisons },
  {0,                   0,   0                       }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_FOREST = 0, IN_DATA, IN_QUERY, IN_END} ;
  enum {OUT_INDEX = 0, OUT_DISTANCE} ;

  int verbose = 0 ;
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  VlKDForest * forest ;
  mxArray const * forest_array = in[IN_FOREST] ;
  mxArray const * data_array = in[IN_DATA] ;
  mxArray const * query_array = in[IN_QUERY] ;
  void * query ;
  vl_uint32 * index ;
  void * distance ;
  vl_size numNeighbors = 1 ;
  vl_size numQueries ;
  unsigned int numComparisons = 0 ;
  unsigned int maxNumComparisons = 0 ;
  mxClassID dataClass ;
  vl_index i ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nout > 2) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  forest = new_kdforest_from_array (forest_array, data_array) ;

  dataClass = mxGetClassID (data_array) ;
  if (mxGetClassID (query_array) != dataClass) {
    vlmxError(vlmxErrInvalidArgument,
              "QUERY must have the same storage class as DATA.") ;
  }
  if (! vlmxIsReal (query_array)) {
    vlmxError(vlmxErrInvalidArgument,
              "QUERY must be real.") ;
  }
  if (! vlmxIsMatrix (query_array, forest->dimension, -1)) {
    vlmxError(vlmxErrInvalidArgument,
              "QUERY must be a matrix with TREE.NUMDIMENSIONS rows.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_num_neighs :
        if (! vlmxIsScalar(optarg) ||
            (numNeighbors = mxGetScalar(optarg)) < 1) {
          vlmxError(vlmxErrInvalidArgument,
                    "NUMNEIGHBORS must be a scalar not smaller than one.") ;
        }
        break;

      case opt_max_num_comparisons :
        if (! vlmxIsScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument,
                    "MAXNUMCOMPARISONS must be a scalar.") ;
        }
        maxNumComparisons = mxGetScalar(optarg) ;
        break;

      case opt_verbose :
        ++ verbose ;
        break ;
    }
  }

  vl_kdforest_set_max_num_comparisons (forest, maxNumComparisons) ;

  query = mxGetData (query_array) ;
  numQueries = mxGetN (query_array) ;

  out[OUT_INDEX] = mxCreateNumericMatrix (numNeighbors, numQueries, mxUINT32_CLASS, mxREAL) ;
  out[OUT_DISTANCE] = mxCreateNumericMatrix (numNeighbors, numQueries, dataClass, mxREAL) ;

  index = mxGetData (out[OUT_INDEX]) ;
  distance = mxGetData (out[OUT_DISTANCE]) ;

  if (verbose) {
    VL_PRINTF ("vl_kdforestquery: number of queries: %d\n", numQueries) ;
    VL_PRINTF ("vl_kdforestquery: number of neighbors per query: %d\n", numNeighbors) ;
    VL_PRINTF ("vl_kdforestquery: max num of comparisons per query: %d\n",
               vl_kdforest_get_max_num_comparisons (forest)) ;
  }

  numComparisons = vl_kdforest_query_with_array (forest, index, numNeighbors, numQueries, distance, query) ;

  vl_kdforest_delete(forest) ;

  /* adjust for MATLAB indexing */
  for (i = 0 ; i < (signed) (numNeighbors * numQueries) ; ++i) { index[i] ++ ; }

  if (verbose) {
    VL_PRINTF ("vl_kdforestquery: number of comparisons per query: %.3f\n",
               ((double) numComparisons) / numQueries) ;
    VL_PRINTF ("vl_kdforestquery: number of comparisons per neighbor: %.3f\n",
               ((double) numComparisons) / (numQueries * numNeighbors)) ;
  }
}
