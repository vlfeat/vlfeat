/** @internal
 ** @file     ihashfind.c
 ** @author   Andrea Vedaldi
 ** @brief    BINSUM - MEX
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/generic.h>

#include <string.h>

/* hash function */
unsigned int fnv_hash (void const *key, int len)
{
  unsigned char const *p = key;
  unsigned int h = 2166136261U ;
  int i;

  for ( i = 0; i < len; i++ )
    h = ( h * 16777619 ) ^ p[i];

  return h;
}

int
is_null (vl_uint8 const * x, int n)
{
  int i ;
  for (i = 0 ; i < n ; ++i) {
    if (x[i]) return 0 ;
  }
  return 1 ;
}

int
is_equal (vl_uint8 const * x, vl_uint8 const * y, int n)
{
  int i ;
  for (i = 0 ; i < n ; ++i) {
    if (x[i] != y[i]) return 0 ;
  }
  return 1 ;
}

void
cpy (vl_uint8 * x, vl_uint8 const * y, int n)
{
  int i ;
  for (i = 0 ; i < n ; ++i){
    /*    mexPrintf("cpy:%d %d\n",x[i],y[i]);*/
    x[i] = y[i] ;
  }
}

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
  enum { IN_ID, IN_NEXT, IN_K, IN_X } ;
  enum { OUT_SEL } ;

  vl_uint32 const * next ;
  vl_uint32 * sel ;
  vl_uint8 const  * id ;
  vl_uint8 const  * x ;

  unsigned int K, i, N, res, last, ndims ;

  /* -----------------------------------------------------------------
   *                                                   Check arguments
   * -------------------------------------------------------------- */

  if( nin != 4 ) {
    mexErrMsgTxt("Four arguments required") ;
  } else if (nout > 1) {
    mexErrMsgTxt("At most one output argument.") ;
  }

  if(! mxIsNumeric(in[IN_NEXT])|| mxGetClassID(in[IN_NEXT])!= mxUINT32_CLASS) {
    mexErrMsgTxt("NEXT must be UINT32.") ;
  }

  if(! mxIsNumeric(in[IN_X])   || mxGetClassID(in[IN_X])!= mxUINT8_CLASS) {
    mexErrMsgTxt("X must be UINT8") ;
  }

  if (mxGetM(in[IN_NEXT]) != 1) {
    mexErrMsgTxt("NEXT must be a row vector") ;
  }

  if(! mxIsNumeric(in[IN_ID])  || mxGetClassID(in[IN_ID])!= mxUINT8_CLASS) {
    mexErrMsgTxt("ID must be UINT8.") ;
  }

  ndims = mxGetM(in[IN_ID]) ;
  res   = mxGetN(in[IN_ID]) ;

  if(res != mxGetN(in[IN_NEXT])) {
    mexErrMsgTxt("ID, NEXT must have the same number of columns") ;
  }

  if(ndims != mxGetM(in[IN_X])) {
    mexErrMsgTxt("ID and X must havethe same number of rows") ;
  }

  if(! vlmxIsPlainScalar(in[IN_K])) {
    mexErrMsgTxt("K must be a scalar") ;
  }
  K     = (unsigned int) *mxGetPr(in[IN_K]) ;

  N    = mxGetN(in[IN_X]) ;
  id   = mxGetData(in[IN_ID]) ;
  next = mxGetData(in[IN_NEXT]) ;
  x    = mxGetData(in[IN_X]) ;

  out[OUT_SEL] = mxCreateNumericMatrix
    (1, N, mxUINT32_CLASS, mxREAL) ;

  sel = mxGetData (out[OUT_SEL]) ;
  /* search for last occupied slot */
  last = res ;
  for (i = 0 ; i < res ; ++i) last = VL_MAX(last, next [i]) ;

  /* REMARK: last and next are 1 based */

  if (K > res) {
    mexErrMsgTxt("K cannot be larger then the size of H") ;
  }
  if (last > res) {
    mexErrMsgTxt("An element of NEXT is greater than the size of the table") ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  for (i = 0 ; i < N ; ++i) {
    /* hash */
    unsigned int h1, h2 ;
    unsigned int j, p = 0 ;

    if (is_null (x + i * ndims, ndims)) {
      *sel++ = 0 ;
      continue ;
    }

    h1 = fnv_hash(x + i * ndims, ndims) % K ;
    h2 = h1 | 0x1 ; /* this needs to be odd */

    /* search first free or matching position */
    p = h1 % K ;
    for (j = 0 ; j < K ; ++j) {
      if (is_null (id + p * ndims,                ndims) ||
          is_equal(id + p * ndims, x + i * ndims, ndims)) break ;
      h1 += h2 ;
      p = h1 % K ;
    }

    /* handle extended table */
    while (! is_null (id + p * ndims,                ndims) &&
           ! is_equal(id + p * ndims, x + i * ndims, ndims)) {
      if (next[p] == 0) break ;
      p = next [p] - 1 ;
    }

    /* found or not ? */
    if (is_equal(id + p * ndims, x + i * ndims, ndims)) {
      /* found */
      *sel++ = p + 1 ;
    } else {
      /* not found */
      *sel++ = 0 ;
    }
  } /* next guy to search for */
}
