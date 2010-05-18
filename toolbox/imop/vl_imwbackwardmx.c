/* file:        imwbackward.c
** author:      Andrea Vedaldi
** description: Backward projection of an image.
**/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/* TODO.
 * - Make a faster version for the uniform grid case.
 * - Rename variables.
 */

#include <mexutils.h>

#include <vl/generic.h>

#include <math.h>
#include <stdlib.h>

/** Matlab driver.
 **/
#define greater(a,b) (a) > (b)
#define getM(arg) mxGetM(in[arg])
#define getN(arg) mxGetN(in[arg])
#define getPr(arg) mxGetPr(in[arg])

VL_INLINE int
findNeighbor(double x, const double* X, int K) {
  int i = 0 ;
  int j = K - 1 ;
  int pivot = 0 ;
  double y = 0 ;
  if(x <  X[i]) return i-1 ;
  if(x >= X[j]) return j ;

  while(i < j - 1) {
    pivot = (i+j) >> 1 ;
    y = X[pivot] ;
    /*mexPrintf("%d %d %d %f %f\n",i,j,pivot,x,y) ;*/
    if(x < y) {
      j = pivot ;
    } else {
      i = pivot ;
    }
  }
  return i ;
}

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum { X=0,Y,I,iwXp,iwYp } ;
  enum { wI=0, wIx, wIy } ;

  int M, N, Mp, Np, ip, jp ;
  double
    *X_pt,
    *Y_pt,
    *I_pt,
    *iwXp_pt,
    *iwYp_pt,
    *wI_pt,
    *wIx_pt   = 0,
    *wIy_pt   = 0 ;

  double Xmin, Xmax, Ymin, Ymax ;
  const double NaN = mxGetNaN() ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  if (nin < 5) {
    vlmxError (vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nin > 5) {
    vlmxError (vlmxErrTooManyOutputArguments, NULL) ;
  }
  if (nout > 3) {
    vlmxError (vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (! vlmxIsPlainMatrix(in[I], -1, -1)) {
    vlmxError (vlmxErrInvalidArgument, "I is not a plain matrix.") ;
  }

  if (! vlmxIsPlainMatrix(in[iwXp], -1, -1)) {
    vlmxError(vlmxErrInvalidArgument, "iwXp is not a plain matrix.") ;
  }

  M = getM(I) ;
  N = getN(I) ;
  Mp = getM(iwXp) ;
  Np = getN(iwXp) ;

  if(!vlmxIsPlainMatrix(in[iwYp], Mp, Np)) {
    vlmxError(vlmxErrInvalidArgument,
              "iwXp is not a plain matrix of the same idmension of iwYp.") ;
  }

  if(!vlmxIsPlainVector(in[X],N) || !vlmxIsPlainVector(in[Y],M)) {
    vlmxError(vlmxErrInvalidArgument,
              "X and Y are not plain vectors with a length equal to the"
	      " number of columns and rows of I.") ;
  }

  X_pt = getPr(X);
  Y_pt = getPr(Y) ;
  I_pt = getPr(I) ;
  iwXp_pt = getPr(iwXp) ;
  iwYp_pt = getPr(iwYp) ;

  /* Allocate the result. */
  out[wI] = mxCreateDoubleMatrix(Mp, Np, mxREAL) ;
  wI_pt = mxGetPr(out[wI]) ;

  if (nout > 1) {
    out[wIx] = mxCreateDoubleMatrix(Mp, Np, mxREAL) ;
    out[wIy] = mxCreateDoubleMatrix(Mp, Np, mxREAL) ;
    wIx_pt = mxGetPr (out[wIx]) ;
    wIy_pt = mxGetPr (out[wIy]) ;
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */
  Xmin = X_pt [0] ;
  Xmax = X_pt [N - 1] ;
  Ymin = Y_pt [0] ;
  Ymax = Y_pt [M - 1] ;

  if (nout <= 1) {

    /* optimized for only image output */
    for(jp = 0 ; jp < Np ; ++jp) {
      for(ip = 0 ; ip < Mp ; ++ip) {
	/* Search for the four neighbors of the backprojected point. */
	double x = *iwXp_pt++ ;
	double y = *iwYp_pt++ ;
	double z = NaN ;

	/* This messy code allows the identity transformation
	 * to be processed as expected. */
	if(x >= Xmin && x <= Xmax &&
	   y >= Ymin && y <= Ymax) {
	  int j = findNeighbor(x, X_pt, N) ;
	  int i = findNeighbor(y, Y_pt, M) ;
	  double* pt  = I_pt + j*M + i ;

	  /* Weights. */
	  double x0 = X_pt[j] ;
	  double x1 = (j < N-1) ? X_pt[j+1] : x0 + 1;
	  double y0 = Y_pt[i] ;
	  double y1 = (i < M-1) ? Y_pt[i+1] : y0 + 1;
	  double wx = (x-x0)/(x1-x0) ;
	  double wy = (y-y0)/(y1-y0) ;

	  /* Load all possible neighbors. */
	  double z00 = 0.0 ;
	  double z10 = 0.0 ;
	  double z01 = 0.0 ;
	  double z11 = 0.0 ;

	  if(j > -1) {
	    if(i > -1 ) z00 = *pt ;
	    pt++ ;
	    if(i < M-1) z10 = *pt ;
	  } else {
	    pt++ ;
	  }

	  pt += M - 1;

	  if(j < N-1) {
	    if(i > -1 ) z01 = *pt ;
	    pt++ ;
	    if(i < M-1) z11 = *pt ;
	  }

	  /* Bilinear interpolation. */
	  z =
	    (1 - wy) * ((1-wx) * z00 + wx * z01) +
	    (    wy) * ((1-wx) * z10 + wx * z11) ;
	}

	*(wI_pt + jp*Mp + ip) = z ;
      }
    }
  }

  /* do also the derivatives */
  else {

    /* optimized for only image output */
    for(jp = 0 ; jp < Np ; ++jp) {
      for(ip = 0 ; ip < Mp ; ++ip) {
	/* Search for the four neighbors of the backprojected point. */
	double x = *iwXp_pt++ ;
	double y = *iwYp_pt++ ;
	double z = NaN, zx = NaN, zy = NaN ;

	/* This messy code allows the identity transformation
	 * to be processed as expected. */
	if(x >= Xmin && x <= Xmax &&
	   y >= Ymin && y <= Ymax) {
	  int j = findNeighbor(x, X_pt, N) ;
	  int i = findNeighbor(y, Y_pt, M) ;
	  double* pt  = I_pt + j*M + i ;

	  /* Weights. */
	  double x0 = X_pt[j] ;
	  double x1 = X_pt[j+1] ;
	  double y0 = Y_pt[i] ;
	  double y1 = Y_pt[i+1] ;
	  double wx = (x-x0)/(x1-x0) ;
	  double wy = (y-y0)/(y1-y0) ;

	  /* Load all possible neighbors. */
	  double z00 = 0.0 ;
	  double z10 = 0.0 ;
	  double z01 = 0.0 ;
	  double z11 = 0.0 ;

	  if(j > -1) {
	    if(i > -1 ) z00 = *pt ;
	    pt++ ;
	    if(i < M-1) z10 = *pt ;
	  } else {
	    pt++ ;
	  }

	  pt += M - 1;

	  if(j < N-1) {
	    if(i > -1 ) z01 = *pt ;
	    pt++ ;
	    if(i < M-1) z11 = *pt ;
	  }

	  /* Bilinear interpolation. */
	  z =
	    (1-wy)*( (1-wx) * z00 + wx * z01) +
	    wy*( (1-wx) * z10 + wx * z11) ;

	  zx =
	    (1-wy) * (z01 - z00) +
	       wy  * (z11 - z10) ;

	  zy =
	    (1-wx) * (z10 - z00) +
	       wx  * (z11 - z01) ;
	}

	*(wI_pt  + jp*Mp + ip) = z ;
	*(wIx_pt + jp*Mp + ip) = zx ;
	*(wIy_pt + jp*Mp + ip) = zy ;
      }
    }
  }
}
