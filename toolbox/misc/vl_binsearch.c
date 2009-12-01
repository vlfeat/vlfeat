/** @internal
 ** @file     binsearch.c
 ** @brief    vl_binsearch - MEX definition
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

/** @brief Driver.
 **
 ** @param nount number of output arguments.
 ** @param out output arguments.
 ** @param nin number of input arguments.
 ** @param in input arguments.
 **/
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum { IN_B=0, IN_X, IN_END } ;
  enum { OUT_IDX=0 } ;
  int NX, NB ;
  const double *X, *B ;
  double *IDX ;

  if( nin != 2 ) {
    mexErrMsgTxt("Exactly two arguments are required.") ;
  }

  if(! uIsPlainArray(in[IN_B]) ||
     ! uIsPlainArray(in[IN_X])) {
    mexErrMsgTxt("All arguments must be plain arrays.") ;
  }

  NX = mxGetNumberOfElements(in[IN_X]) ;
  NB = mxGetNumberOfElements(in[IN_B]) ;

  {
    mwSize const * dims = mxGetDimensions(in[IN_X]) ;
    int unsigned ndims = mxGetNumberOfDimensions(in[IN_X]) ;
    out[OUT_IDX] = mxCreateNumericArray (ndims, dims, mxDOUBLE_CLASS, mxREAL) ;
  }

  /* if B is empty it defines only [-inf, +inf) */
  if (NB == 0) return ;

  IDX = mxGetPr(out[OUT_IDX]) ;
  X = mxGetPr(in[IN_X]) ;
  B = mxGetPr(in[IN_B]) ;

  {
    int i ;
    for (i = 0 ; i < NX ; ++i) {
      double x = X[i] ;
      int blower = 0 ;
      int bupper = NB - 1 ;
      int bsplit ;

      if (x < B[0]) {
        IDX [i] = 0 ;
        continue ;
      }

      if (x >= B[NB-1]) {
        IDX [i] = NB ;
        continue ;
      }

      while (blower + 1 < bupper) {
        bsplit = (bupper + blower) / 2 ;
        if (x < B[bsplit]) bupper = bsplit ;
        else blower = bsplit ;
      }
      IDX [i] = blower + 1 ;
    }
  }
}
