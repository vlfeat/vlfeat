/* file:        ikmeans.c
** description: MEX weighted ikmeans function.
** author:      Andrea Vedaldi
**/

#include"mexutils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/ikmeans.h>
#include <vl/generic.h>

enum {
  opt_max_passes,
  opt_verbose
} ;

uMexOption options [] = {
  {"MaxPasses",    1,   opt_max_passes  },
  {"Verbose",      0,   opt_verbose     },
  {0,              0,   0               }
} ;


/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_X = 0, IN_K, IN_END} ;  
  enum {OUT_C = 0, OUT_I} ;
  
  int             verbose = 0 ;
  int             opt ;
  int             next = IN_END ;
  mxArray const  *optarg ;
  
  int M, N, K = 0 ;
  int max_passes = 200 ;
  int err = 0 ;

  vl_uint  *asgn = NULL;
  vl_int32 *centers;
  vl_uint8 *data;
  
  /* ------------------------------------------------------------------
   *                                                Check the arguments
   * --------------------------------------------------------------- */
  if (nin < 2) {
    mexErrMsgTxt ("At least two arguments required.");
  }
  else if (nout > 2) {
    mexErrMsgTxt ("Too many output arguments.");
  }
  if (mxGetClassID (in[IN_X]) != mxUINT8_CLASS) {
    mexErrMsgTxt ("X must be of class uint8");
  }

  M = mxGetM (in[IN_X]);   /* n of components */
  N = mxGetN (in[IN_X]);   /* n of elements */
  
  if (!uIsRealScalar (in[IN_K])          ||
      (K = (int) *mxGetPr(in[IN_K])) < 1 ||
      K > N                               ) { 
    mexErrMsgTxt ("K must be a positive integer smaller than the number of data.");
  }
  
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      
    case opt_verbose :
      ++ verbose ;
      break ;
      
    case opt_max_passes :
      if (!uIsRealScalar(optarg) || (max_passes = (int) *mxGetPr(optarg)) < 1) {
        mexErrMsgTxt("MaxPasses must be not smaller than 1.") ;
      }
      break ;
      
    default :
      assert(0) ;
      break ;
    }
  }

  /* ------------------------------------------------------------------
   *                                                         Do the job
   * --------------------------------------------------------------- */
  
  if (verbose > 1) {
    mexPrintf("ikmeans: MaxPasses = %d\n", max_passes) ;
  }

  out[OUT_C] = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL) ;  
  data       = mxGetData (in[IN_X]);
  centers    = mxGetData (out[OUT_C]) ;
  
  if (nout > 1) {
    out[OUT_I] = mxCreateNumericMatrix (1, N, mxUINT32_CLASS, mxREAL) ;
    asgn       = mxGetData (out[OUT_I]) ;
  }
  
  /* K is the number of clusters */
  /* M is the dimension of each datapoint */
  /* N is the number of data points */
  err = vl_ikmeans (centers, asgn, data, M, N, K, max_passes);

  if (err) mexWarnMsgTxt("ikmeans: possible overflow!") ;
  
  /* adjust indeces base */
  if (asgn) {
    int j ;
    for (j = 0 ; j < N ; ++j)
      ++ asgn [j] ;
  }
  
  if (verbose) {
    mexPrintf("ikmeans: done\n", max_passes) ;
  }
}
