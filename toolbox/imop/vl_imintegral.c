/** @internal
 ** @file    imintegral.c
 ** @author  Brian Fulkerson
 ** @brief   Create an integral image - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/generic.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

/** @internal
 ** @brief Compute the integral image J of image I
 ** 
 ** The integral image is defined as:
 ** J(x,y) = \sum_{x} \sum_{y} I(x,y)
 ** J(x,y) = J(x,y-1) + J(x-1,y) + I(x,y) - J(x-1,y-1)
 ** where:
 ** J(0,0) = I(0,0)
 ** J(0,y) = J(0,y-1) + I(0,y)
 ** J(x,0) = J(x-1,0) + I(x,y)
 ** The sum of any window in I starting after x1,y1 ending at x2,y2 =
 ** J(x1,y1) + J(x2,y2) - ( J(x1,y2) + J(x2,y1) )
 **/
void integral(double *I, double *J, int M, int N)
{
    int row,col;

    *J = *I;                    /* J(0,0) = I(0,0) */
    I++; J++;
    for(row=1; row<M; row++) {
        *J = *(J-1) + *I;       /* J(row,0) = J(row-1,0) + I(row,0) */
        I++; J++;
    }

    for(col=1; col<N; col++) {
        *J = *(J-M) + *I;       /* J(0,col) = J(0,col-1) + I(0,col) */
        I++; J++;

        for(row=1; row<M; row++) {
            *J = *(J-M) + *(J-1) - *(J-M-1) + *I;
            I++; J++;
        }
    }

}

void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  int M,N,K,ndims ;
  mwSize const *dims ;
  double* I_pt ;
  double* J_pt ;
  int k;
  enum {I=0} ;
  enum {J=0} ;

  /* ------------------------------------------------------------------
  **                                                Check the arguments
  ** --------------------------------------------------------------- */ 
  if (nin != 1) {
    mexErrMsgTxt("Exactly one input argument is required.");
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if (!mxIsDouble(in[I])) {
    mexErrMsgTxt("All arguments must be real.") ;
  }
  
  if(mxGetNumberOfDimensions(in[I]) > 3)
    mexErrMsgTxt("I must be a two dimensional array.") ;

  ndims = mxGetNumberOfDimensions(in[I]) ;
  dims  = mxGetDimensions(in[I]) ;
  M = dims[0] ;
  N = dims[1] ;
  K = (ndims > 2) ? dims[2] : 1 ;

  out[J] = mxCreateNumericArray(ndims, dims, mxDOUBLE_CLASS, mxREAL) ;
  
  I_pt   = mxGetPr(in[I]) ;
  J_pt   = mxGetPr(out[J]) ;

  /* ------------------------------------------------------------------
  **                                                         Do the job
  ** --------------------------------------------------------------- */ 
  for(k = 0 ; k < K ; ++k) {
      integral(I_pt, J_pt, M, N);
      I_pt += M*N ;
      J_pt += M*N ;
  }
}

