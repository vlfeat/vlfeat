/** @internal
 ** @file     vl_binsearch.c
 ** @brief    vl_binsearch - MEX definition
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum { IN_B=0, IN_X, IN_END } ;
  enum { OUT_IDX=0 } ;
  vl_size numElementsX, numElementsB ;
  const double *X, *B ;
  double *IDX ;

  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }
  if (nin != 2) {
    vlmxError(vlmxErrInvalidArgument,
              "Incorrect number of arguments.") ;
  }
  if (! vlmxIsPlain (IN(B)) ||
      ! vlmxIsPlain (IN(X)) ) {
    vlmxError(vlmxErrInvalidArgument,
              "All arguments must be plain arrays.") ;
  }

  numElementsX = mxGetNumberOfElements(IN(X)) ;
  numElementsB = mxGetNumberOfElements(IN(B)) ;

  {
    mwSize const * dimensions = mxGetDimensions(IN(X)) ;
    vl_size numDimensions = mxGetNumberOfDimensions(IN(X)) ;
    OUT(IDX) = mxCreateNumericArray (numDimensions, dimensions, mxDOUBLE_CLASS, mxREAL) ;
  }

  /* if B is empty it defines only [-inf, +inf) */
  if (numElementsB == 0) return ;

  IDX = mxGetPr(OUT(IDX)) ;
  X = mxGetPr(IN(X)) ;
  B = mxGetPr(IN(B)) ;

  {
    vl_uindex i ;
    for (i = 0 ; i < numElementsX ; ++i) {
      double x = X[i] ;
      vl_uindex blower = 0 ;
      vl_uindex bupper = numElementsB - 1 ;
      vl_uindex bsplit ;

      if (x < B[0]) {
        IDX [i] = 0 ;
        continue ;
      }

      if (x >= B[numElementsB - 1]) {
        IDX [i] = numElementsB ;
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
