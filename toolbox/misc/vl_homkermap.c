/** @file   vl_homkmap.c
 ** @brief  vl_homkmap - MEX implementation
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS */

#include "mexutils.h"
#include <vl/homkermap.h>
#include <vl/mathop.h>

enum {
  opt_KCHI2,
  opt_KL1
} ;

vlmxOption options [] = {
{"kl1",          0,   opt_KL1           },
{"kchi2",        0,   opt_KCHI2         },
{0,              0,   0                 }
} ;

/* -------------------------------------------------------------------
 *                                                              Driver
 * ---------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{

  typedef int unsigned data_t ;

  enum {IN_X = 0, IN_N, IN_L, IN_END} ;
  enum {OUT_V = 0} ;
  VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
  mwSize numDimensions ;
  mwSize const * dimensions ;
  int n ;
  double L ;

  /* for option parsing */
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }
  if (nin < 3) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (! vlmxIsPlain(IN(X))){
    vlmxError(vlmxErrInvalidArgument, "X must be a plain array.") ;
  }

  numDimensions = mxGetNumberOfDimensions(IN(X)) ;
  dimensions = mxGetDimensions(IN(X)) ;

  if (! vlmxIsScalar(IN(N))) {
    vlmxError(vlmxErrInvalidArgument, "N must be a scalar.") ;
  }
  n = *mxGetPr(IN(N)) ;
  if (n < 0) {
    vlmxError(vlmxErrInvalidArgument, "N must be non-negative.") ;
  }

  if (! vlmxIsScalar(IN(L))){
    vlmxError(vlmxErrInvalidArgument, "L must be a scalar.") ;
  }
  L = *mxGetPr(IN(L)) ;
  if (L < 0) {
    vlmxError(vlmxErrInvalidArgument, "N must be non-negative.") ;
  }

  while ((opt = vlmxNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_KL1   :
        kernelType = VlHomogeneousKernelIntersection ;
        break ;
      case opt_KCHI2 :
        kernelType = VlHomogeneousKernelChi2 ;
        break ;
      default:
        assert(0) ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                Pre-compute tables
   * -------------------------------------------------------------- */
/*
   The feature map representation of a real number @$f x @$f is given by
   @f[


   @f]

*/
  {
    VlHomogeneousKernelMap * map = vl_homogeneouskernelmap_new (kernelType, n, L) ;
    mwSize extDimensions [20] ;
    double * V ;
    double const * X = mxGetPr(IN(X)) ;
    int numElements = mxGetNumberOfElements(IN(X)) ;
    int j ;
    for (j = 0 ; (unsigned)j < numDimensions ; ++j) extDimensions[j] = dimensions[j] ;
    extDimensions[0] *= 2*n+1 ;
    out[OUT_V] = mxCreateNumericArray(numDimensions, extDimensions, mxDOUBLE_CLASS, mxREAL) ;
    V = mxGetPr(out[OUT_V]) ;
    for (j = 0 ; j < numElements ; ++j) {
      vl_homogeneouskernelmap_evaluate_d(map, V, 1, *X++) ;
      V += 2*n+1 ;
    }
    vl_homogeneouskernelmap_delete (map) ;
  }
}
