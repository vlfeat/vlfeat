/* file:        irodr.mex.c
** author:      Andrea Vedaldi
** description: Inverse rodrigues formula
**/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/rodrigues.h>

enum {
  IN_R = 0
} ;

enum {
  OUT_OM=0,
  OUT_DOM
} ;

/* -------------------------------------------------------------------
**                                                              Driver
** ----------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  int k,K ;
  double* om_pt ;
  double* dom_pt ;
  double const * R_pt ;

  /* -----------------------------------------------------------------
  **                                               Check the arguments
  ** -------------------------------------------------------------- */
  if(nin != 1) {
    mexErrMsgTxt("Exactly one argument required.") ;
  }

  if(!vlmxIsMatrix(in[IN_R],-1,-1)) {
    mexErrMsgTxt("R must be a DOUBLE array") ;
  }

  K = mxGetNumberOfElements(in[IN_R]) ;
  if(K % 9 || K < 9) {
    mexErrMsgTxt("The elements of R must be a multiple of 9.") ;
  }
  K /= 9 ;
  R_pt = mxGetPr(in[IN_R]) ;

  /* space for output (OM) */
  out[OUT_OM] = mxCreateDoubleMatrix(3,1,mxREAL) ;
  om_pt = mxGetPr(out[OUT_OM]) ;

  /* space for optional output (dR) */
  dom_pt = NULL ;
  if( nout > 1 ) {
    if( K == 1 ) {
      out[OUT_DOM] = mxCreateDoubleMatrix(3,9,mxREAL) ;
    } else {
      mwSize dims [3] ;
      dims[0] = 3 ; dims[1] = 9 ; dims[2] = K ;
      out[OUT_DOM] = mxCreateNumericArray(3,dims,mxDOUBLE_CLASS,mxREAL) ;
    }
    dom_pt = mxGetPr(out[OUT_DOM]) ;
  }

  /* -----------------------------------------------------------------
  **                                                           Process
  ** -------------------------------------------------------------- */
  for(k = 0 ; k < K ; ++k) {
    vl_irodrigues(om_pt, dom_pt, R_pt) ;
    om_pt  += 3 ;
    dom_pt += 3*9 ;
    R_pt   += 3*3 ;
  }

}
