/* file:        tpsumx.c
** author:      Andrea Vedaldi
** description: vl_tpsumx - MEX definition
**/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <stdlib.h>
#include <math.h>

/** Matlab driver.
 **/
#define getM(arg) mxGetM(in[arg])
#define getN(arg) mxGetN(in[arg])
#define getPr(arg) mxGetPr(in[arg])

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum { X=0,Y } ;
  enum { U } ;

  int NP, NCP ;
  int i,j ;
  double *X_pt, *Y_pt, *U_pt ;
#undef small
  const double small = 2.220446049250313e-16 ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  if (nin != 2) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  } else if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if(!vlmxIsMatrix(in[X], 2, -1)) {
    mexErrMsgTxt("X must be a 2xNP real matrix") ;
  }

  if(!vlmxIsMatrix(in[Y], 2, -1)) {
    mexErrMsgTxt("Y must be a 2xNCP real matrix") ;
  }

  NP  = getN(X) ;
  NCP = getN(Y) ;

  X_pt = getPr(X);
  Y_pt = getPr(Y) ;

  /* Allocate the result. */
  out[U] = mxCreateDoubleMatrix(NP, NCP, mxREAL) ;
  U_pt = mxGetPr(out[U]) ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  for(j = 0 ; j < NCP ; ++j) {
    double xcp = *Y_pt++ ;
    double ycp = *Y_pt++ ;
    for(i = 0 ; i < NP ; ++i) {
      double dx = *X_pt++ - xcp ;
      double dy = *X_pt++ - ycp ;
      double r2 = dx*dx + dy*dy ;
      *U_pt++ = (r2 <= small) ? 0 : r2 * log (r2) ;
    }
    X_pt -= 2*NP ;
  }
}
