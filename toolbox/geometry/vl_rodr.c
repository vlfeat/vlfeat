/* file:        rodr.mex.c
** author:      Andrea Vedaldi
** description: Rodrigues formula
**/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/rodrigues.h>

enum {
  IN_OM = 0
} ;

enum {
  OUT_R=0,
  OUT_DR
} ;

/* -------------------------------------------------------------------
**                                                              Driver
** ---------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  int k,K ;
  double const * om_pt ;
  double* R_pt ;
  double* dR_pt ;

  if(nin != 1) {
    mexErrMsgTxt("Exactly one argument required.") ;
  }

  if(!vlmxIsMatrix(in[IN_OM],-1,-1)) {
    mexErrMsgTxt("OM must be a DOUBLE array") ;
  }

  K = mxGetNumberOfElements(in[IN_OM]) ;
  if(K % 3 || K < 3) {
    mexErrMsgTxt("The number of elements of OM must be a multiple of 3") ;
  }
  K /= 3 ;
  om_pt = mxGetPr(in[IN_OM]) ;

  /* space for output (R) */
  if( K == 1 ) {
    out[OUT_R] = mxCreateDoubleMatrix(3,3,mxREAL) ;
  } else {
    mwSize dims [3] ;
    dims[0] = 3 ; dims[1] = 3 ; dims[2] = K ;
    out[OUT_R] = mxCreateNumericArray(3,dims,mxDOUBLE_CLASS,mxREAL) ;
  }
  R_pt = mxGetPr(out[OUT_R]) ;

  /* space for optional output (dR) */
  dR_pt = NULL ;
  if( nout > 1 ) {
    if( K == 1 ) {
      out[OUT_DR] = mxCreateDoubleMatrix(9,3,mxREAL) ;
    } else {
      mwSize dims [3] ;
      dims[0] = 9 ; dims[1] = 3 ; dims[2] = K ;
      out[OUT_DR] = mxCreateNumericArray(3,dims,mxDOUBLE_CLASS,mxREAL) ;
    }
    dR_pt = mxGetPr(out[OUT_DR]) ;
  }

  /* -----------------------------------------------------------------
  **                                                           Process
  ** -------------------------------------------------------------- */
  for(k = 0 ; k < K ; ++k) {
    vl_rodrigues(R_pt, dR_pt, om_pt) ;
    om_pt += 3 ;
    R_pt  += 3*3 ;
    dR_pt += 9*3 ;
  }
}
