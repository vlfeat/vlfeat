/** @internal
 ** @file     vl_kdtreequery.c
 ** @brief    vl_kdtreequery - MEX implementation
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>
#include <vl/kdtree.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* option codes */
enum {
  opt_verbose, opt_num_neighs, opt_max_comparisons
} ;

/* options */
uMexOption options [] = {
{"Verbose",        0,   opt_verbose         },
{"NumNeighbors",   1,   opt_num_neighs      },
{"MaxComparisons", 1,   opt_max_comparisons },
{0,                0,   0                   }
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

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  VlKDForest * forest ;
  mxArray const * forest_array = in[IN_FOREST] ;
  mxArray const * data_array = in[IN_DATA] ;
  mxArray const * query_array = in[IN_QUERY] ;
  mxArray * index_array ;
  mxArray * distance_array ;
  float * query ;
  vl_uint32 * index ;
  float * distance ;
  int numNeighbors = 1 ;
  int numQueries ;
  int qi, ni;
  unsigned int numComparisons = 0 ;
  unsigned int maxNumComparisons = 0 ;
  VlKDForestNeighbor * neighbors ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    mexErrMsgTxt("Three argument required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }

  forest = new_kdforest_from_array (forest_array, data_array) ;
  if (!uIsMatrix (query_array, forest->numDimensions, -1) ||
      mxGetClassID (query_array) != mxSINGLE_CLASS) {
    mexErrMsgTxt ("QUERY must be a SINGLE matrix with TREE.NUMDIMENSIONS rows") ;
  }

  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_num_neighs :
        if (! uIsScalar(optarg) ||
            (numNeighbors = mxGetScalar(optarg)) < 1) {
          mexErrMsgTxt("NUMNEIGHBORS must be a scalar not smaller than one") ;
        }
        break;

      case opt_max_comparisons :
        if (! uIsScalar(optarg)) {
          mexErrMsgTxt("MAXCOMPARISONS must be a scalar") ;
        }
        maxNumComparisons = mxGetScalar(optarg) ;
        break;

      case opt_verbose :
        ++ verbose ;
        break ;
    }
  }

  vl_kdforest_set_max_num_comparisons (forest, maxNumComparisons) ;

  neighbors = vl_malloc (sizeof(VlKDForestNeighbor) * numNeighbors) ;

  query = mxGetData (query_array) ;
  numQueries = mxGetN (query_array) ;

  out[OUT_INDEX] = index_array = mxCreateNumericMatrix
    (numNeighbors, numQueries, mxUINT32_CLASS, mxREAL) ;

  out[OUT_DISTANCE] = distance_array = mxCreateNumericMatrix
    (numNeighbors, numQueries, mxSINGLE_CLASS, mxREAL) ;

  index = mxGetData (index_array) ;
  distance = mxGetData (distance_array) ;

  if (verbose) {
    VL_PRINTF ("vl_kdforestquery: number of queries: %d\n", numQueries) ;
    VL_PRINTF ("vl_kdforestquery: number of neighbors per query: %d\n", numNeighbors) ;
    VL_PRINTF ("vl_kdforestquery: max num of comparisons per query: %d\n",
               vl_kdforest_get_max_num_comparisons (forest)) ;
  }

  for (qi = 0 ; qi < numQueries ; ++ qi) {
    numComparisons += vl_kdforest_query (forest, neighbors, numNeighbors,
                                    query + forest->numDimensions * qi) ;
    for (ni = 0 ; ni < numNeighbors ; ++ni) {
      *index++    = neighbors[ni].index + 1 ;
      *distance++ = neighbors[ni].distance ;
    }
  }

  if (verbose) {
    VL_PRINTF ("vl_kdforestquery: number of comparisons per query: %.3f\n",
               ((double) numComparisons) / numQueries) ;
    VL_PRINTF ("vl_kdforestquery: number of comparisons per neighbor: %.3f\n",
               ((double) numComparisons) / (numQueries * numNeighbors)) ;
  }

  vl_kdforest_delete (forest) ;
  vl_free (neighbors) ;
}
