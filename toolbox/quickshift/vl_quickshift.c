/** @internal
 ** @file:       quickshift.c
 ** @author:     Andrea Vedaldi
 ** @author:     Brian Fulkerson
 ** @brief:      Quickshift MEX driver
 **/

#include <mexutils.h>

#include <vl/quickshift.h>
#include <string.h>

enum {
  opt_medoid,
  opt_verbose
} ;

vlmxOption options [] = {
  {"Medoid",              0,   opt_medoid         },
  {"Verbose",             0,   opt_verbose        },
  {0,                     0,   0                  }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {
    IN_I=0,     /* Input image */
    IN_KERNEL_SIZE,  /* The bandwidth parameter for density estimation */
    IN_MAX_DIST,     /* The maximum distance to a neighbor which increases
                   the density */
    IN_END
  } ;
  enum {
    OUT_PARENTS=0, /* parents (same size as I) */
    OUT_DISTS,     /* dists (same size as I) */
    OUT_DENSITY    /* density (same size as I) */
  } ;

  int             verb = 0 ;
  int             opt ;
  int             next = IN_END ;
  mxArray const  *optarg ;

  double const *I ;
  double *parents, *dists, *density ;
  int *parentsi;
  double sigma ;
  double tau ;

  int K,N1,N2;

  int medoid = 0 ;

  mwSize const *dims ;
  int ndims ;

  int i;

  VlQS * q;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                                   Check arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    mexErrMsgTxt("At least two arguments.") ;
  }

  if (nout > 3) {
    mexErrMsgTxt("At most three output arguments.") ;
  }

  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims  = mxGetDimensions(in[IN_I]) ;

  if (ndims > 3) {
    mexErrMsgTxt("I must have at most 3 dimensions.") ;
  }

  if (mxGetClassID(in[IN_I]) != mxDOUBLE_CLASS) {
    mexErrMsgTxt("I must be DOUBLE.")  ;
  }

  N1 = dims [0] ;
  N2 = dims [1] ;
  K = (ndims == 3) ? dims [2] : 1 ;

  I     =  mxGetPr (in[IN_I]) ;
  sigma = *mxGetPr (in[IN_KERNEL_SIZE]) ;
  tau   = 3*sigma;
  if (nin > 2)
    tau = *mxGetPr (in[IN_MAX_DIST]) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_medoid: /* Do medoid shift instead of mean shift */
      medoid = 1 ;
      break ;
    case opt_verbose :
      ++ verb ;
      break ;
    }
  } /* while opts */

  /* Create outputs */
  out[OUT_PARENTS] = mxCreateDoubleMatrix(N1, N2, mxREAL) ;
  parents          = mxGetPr (out[OUT_PARENTS]) ;

  out[OUT_DISTS]   = mxCreateDoubleMatrix(N1, N2, mxREAL) ;
  dists            = mxGetPr (out[OUT_DISTS]) ;

  out[OUT_DENSITY] = mxCreateDoubleMatrix(N1, N2, mxREAL) ;
  density          = mxGetPr (out[OUT_DENSITY]) ;

  if (verb) {
    mexPrintf("quickshift: [N1,N2,K]: [%d,%d,%d]\n", N1,N2,K) ;
    mexPrintf("quickshift: type: %s\n", medoid ? "medoid" : "quick");
    mexPrintf("quickshift: kernel size:  %g\n", sigma) ;
    mexPrintf("quickshift: maximum gap:  %g\n", tau) ;
  }

  /* Do job */
  q = vl_quickshift_new(I, N1, N2, K);

  vl_quickshift_set_kernel_size (q, sigma) ;
  vl_quickshift_set_max_dist     (q, tau) ;
  vl_quickshift_set_medoid      (q, medoid) ;

  vl_quickshift_process(q);

  parentsi = vl_quickshift_get_parents(q);
  /* Copy results */
  for(i = 0; i < N1*N2; i++) parents[i] = parentsi[i] + 1;
  memcpy(dists, vl_quickshift_get_dists(q), sizeof(double)*N1*N2);
  memcpy(density, vl_quickshift_get_density(q), sizeof(double)*N1*N2);

  /* Delete quick shift object */
  vl_quickshift_delete(q);
}
