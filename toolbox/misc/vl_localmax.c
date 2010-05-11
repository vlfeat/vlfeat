/** file:        localmax.c
 ** author:      Andrea Vedaldi
 ** description: Find local maximizer of multi-dimensional array.
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <stdlib.h>

/** Matlab driver.
 **/
#define greater(a,b) ((a) > (b)+threshold)

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  int M, N ;
  const double* F_pt ;
  int ndims ;
  int pdims = -1 ;
  int* offsets ;
  int* midx ;
  int* neighbors ;
  int nneighbors ;
  int* dims ;
  enum {F=0,THRESHOLD,P} ;
  enum {MAXIMA=0} ;
  double threshold = - mxGetInf() ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    mexErrMsgTxt("At least one input argument is required.");
  } else if (nin > 3) {
    mexErrMsgTxt("At most three arguments are allowed.") ;
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments");
  }

  /* The input must be a real matrix. */
  if (!mxIsDouble(in[F]) || mxIsComplex(in[F])) {
    mexErrMsgTxt("Input must be real matrix.");
  }

  if(nin > 1) {
    if(!vlmxIsPlainScalar(in[THRESHOLD])) {
      mexErrMsgTxt("THRESHOLD must be a real scalar.") ;
    }
    threshold = *mxGetPr(in[THRESHOLD]) ;
  }

  if(nin > 2) {
    if(!vlmxIsPlainScalar(in[P]))
      mexErrMsgTxt("P must be a non-negative integer") ;
    pdims = (int) *mxGetPr(in[P])  ;
    if(pdims < 0)
      mexErrMsgTxt("P must be a non-negative integer") ;
  }

  ndims = mxGetNumberOfDimensions(in[F]) ;
  {
    /* We need to make a copy because in one special case (see below)
       we need to adjust dims[].
    */
    int d ;
    mwSize const * const_dims = mxGetDimensions(in[F]) ;
    dims = mxMalloc(sizeof(int)*ndims) ;
    for(d=0 ; d < ndims ; ++d) dims[d] = const_dims[d] ;
  }
  M = dims[0] ;
  N = dims[1] ;
  F_pt = mxGetPr(in[F]) ;

  /*
     If there are only two dimensions and if one is singleton, then
     assume that a vector has been provided as input (and treat this
     as a COLUMN matrix with p=1). We do this because Matlab does not
     distinguish between vectors and 1xN or Mx1 matrices and because
     the cases 1xN and Mx1 are trivial (the result is alway empty).
   */
  if((ndims == 2) && (pdims < 0) && (M == 1 || N == 1)) {
    pdims = 1 ;
    M = (M>N)?M:N ;
    N = 1 ;
    dims[0]=M ;
    dims[1]=N ;
  }

  /* search the local maxima along the first p dimensions only */
  if(pdims < 0)
    pdims = ndims ;

  if(pdims > ndims) {
    mxFree(dims) ;
    mexErrMsgTxt("P must not be greater than the number of dimensions") ;
  }

  /* ------------------------------------------------------------------
   *                                                         Do the job
   * --------------------------------------------------------------- */
  {
    int maxima_size = M*N ;
    int* maxima_start = mxMalloc(sizeof(int) * maxima_size) ;
    int* maxima_iterator = maxima_start ;
    int* maxima_end = maxima_start + maxima_size ;
    int i,h,o ;
    const double* pt = F_pt ;

    /* Compute the offsets between dimensions. */
    offsets = mxMalloc(sizeof(int) * ndims) ;
    offsets[0] = 1 ;
    for(h = 1 ; h < ndims ; ++h)
      offsets[h] = offsets[h-1]*dims[h-1] ;

    /* Multi-index. */
    midx = mxMalloc(sizeof(int) * ndims) ;
    for(h = 0 ; h < ndims ; ++h)
      midx[h] = 1 ;

    /* Neighbors. */
    nneighbors = 1 ;
    o=0 ;
    for(h = 0 ; h < pdims ; ++h) {
      nneighbors *= 3 ;
      midx[h] = -1 ;
      o -= offsets[h] ;
    }
    nneighbors -= 1 ;
    neighbors = mxMalloc(sizeof(int) * nneighbors) ;
    i = 0 ;

    while(VL_TRUE) {
      if(o != 0 )
        neighbors[i++] = o ;
      h = 0 ;
      while( o += offsets[h], (++midx[h]) > 1 ) {
        o -= 3*offsets[h] ;
        midx[h] = -1 ;
        if(++h >= pdims)
          goto stop ;
      }
    }
  stop: ;

    /* Starts at the corner (1,1,...,1,0,0,...0) */
    for(h = 0 ; h < pdims ; ++h) {
      midx[h] = 1 ;
      pt += offsets[h] ;
    }
    for(h = pdims ; h < ndims ; ++h) {
      midx[h] = 0 ;
    }

    /* ---------------------------------------------------------------
     *                                                            Loop
     * ------------------------------------------------------------ */

    /*
      If any dimension in the first P is less than 3 elements wide
      then just return the empty matrix (if we proceed without doing
      anything we break the carry reporting algorithm below).
    */
    for(h=0 ; h < pdims ; ++h)
      if(dims[h] < 3) goto end ;

    while(VL_TRUE) {
      double v ;
      bool is_greater;

      /* Propagate carry along multi index midx */
      h = 0 ;
      while((midx[h]) >= dims[h] - 1) {
        pt += 2*offsets[h] ; /* skip first and last el. */
        midx[h] = 1 ;
        if(++h >= pdims)
          goto next_layer ;
        ++midx[h] ;
      }

      /*
        for(h = 0 ; h < ndims ; ++h )
          mexPrintf("%d  ", midx[h]) ;
        mexPrintf(" -- %d -- pdims %d \n", pt - F_pt,pdims) ;
      */

      /*  Scan neighbors */
      v = *pt ;
      is_greater = (v >= threshold) ;
      i = 0  ;
      while(is_greater && i < nneighbors)
        is_greater &= v > *(pt + neighbors[i++]) ;

        /* Add the local maximum */
      if(is_greater) {
        /* Need more space? */
        if(maxima_iterator == maxima_end) {
          maxima_size += M*N ;
          maxima_start = mxRealloc(maxima_start,
                                   maxima_size*sizeof(int)) ;
          maxima_end = maxima_start + maxima_size ;
          maxima_iterator = maxima_end - M*N ;
        }

        *maxima_iterator++ = pt - F_pt + 1 ;
      }

      /* Go to next element */
      pt += 1 ;
      ++midx[0] ;
      continue ;

    next_layer: ;
      if( h >= ndims )
        goto end ;

      while((++midx[h]) >= dims[h]) {
        midx[h] = 0 ;
        if(++h >= ndims)
          goto end ;
      }
    }
  end:;
    /* Return. */
    {
      double* M_pt ;
      out[MAXIMA] = mxCreateDoubleMatrix
        (1, maxima_iterator-maxima_start, mxREAL) ;
      maxima_end = maxima_iterator ;
      maxima_iterator = maxima_start ;
      M_pt = mxGetPr(out[MAXIMA]) ;
      while(maxima_iterator != maxima_end) {
        *M_pt++ = *maxima_iterator++ ;
      }
    }

    /* Release space. */
    mxFree(offsets) ;
    mxFree(neighbors) ;
    mxFree(midx) ;
    mxFree(maxima_start) ;
  }
  mxFree(dims) ;
}
