/** @internal
 ** @file     ihashsum.c
 ** @author   Andrea Vedaldi
 ** @brief    BINSUM - MEX
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
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
  enum { IN_H, IN_ID, IN_NEXT, IN_K, IN_X } ;
  enum { OUT_H, OUT_ID, OUT_NEXT} ;

  mxArray *h_,  *id_, *next_ ;

  vl_uint32 * h ;
  vl_uint32 * next ;

  vl_uint8       * id ;
  vl_uint8 const * x ;

  unsigned int K, i, N, res, last, ndims ;

  /* -----------------------------------------------------------------
   *                                                   Check arguments
   * -------------------------------------------------------------- */

  if( nin != 5 ) {
    mexErrMsgTxt("Five arguments required") ;
  } else if (nout > 3) {
    mexErrMsgTxt("At most three output argument.") ;
  }

  if(! mxIsNumeric(in[IN_H])   || mxGetClassID(in[IN_H]   )!= mxUINT32_CLASS ||
     ! mxIsNumeric(in[IN_NEXT])|| mxGetClassID(in[IN_NEXT])!= mxUINT32_CLASS) {
    mexErrMsgTxt("H, NEXT must be UINT32.") ;
  }

  if(! mxIsNumeric(in[IN_X])   || mxGetClassID(in[IN_X])   != mxUINT8_CLASS) {
    mexErrMsgTxt("X must be UINT8") ;
  }

  if (mxGetM(in[IN_H])    != 1 ||
      mxGetM(in[IN_NEXT]) != 1) {
    mexErrMsgTxt("H, NEXT must be row vectors") ;
  }

  if(! mxIsNumeric(in[IN_ID])  || mxGetClassID(in[IN_ID])!= mxUINT8_CLASS) {
    mexErrMsgTxt("ID must be UINT8.") ;
  }

  ndims = mxGetM(in[IN_ID]) ;
  res   = mxGetN(in[IN_H]) ;

  if(res != mxGetN(in[IN_ID]) ||
     res != mxGetN(in[IN_NEXT])) {
    mexErrMsgTxt("H, ID, NEXT must have the same number of columns") ;
  }

  if(ndims != mxGetM(in[IN_X])) {
    mexErrMsgTxt("ID and X must havethe same number of rows") ;
  }

  if(! vlmxIsPlainScalar(in[IN_K])) {
    mexErrMsgTxt("K must be a scalar") ;
  }
  K     = (unsigned int) *mxGetPr(in[IN_K]) ;

  h_    = mxDuplicateArray(in[IN_H]) ;
  id_   = mxDuplicateArray(in[IN_ID]) ;
  next_ = mxDuplicateArray(in[IN_NEXT]) ;

  N    = mxGetN(in[IN_X]) ;

  h    = mxGetData(h_   ) ;
  id   = mxGetData(id_  ) ;
  next = mxGetData(next_) ;
  x    = mxGetData(in[IN_X]) ;

  /*
     Temporary remove mxArray pointers to these buffer as we will
     mxRealloc them and if the user presses Ctrl-C matlab will attempt
     to free unvalid memory
  */

  mxSetData(h_,    0) ;
  mxSetData(id_,   0) ;
  mxSetData(next_, 0) ;

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

  /*  mexPrintf("last:%d\n",last) ;*/

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  for (i = 0 ; i < N ; ++i) {
    /* hash */
    unsigned int h1, h2 ;
    unsigned int j, p = 0 ;

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

    /* search or make a free slot in the bucket */
    while (! is_null (id + p * ndims,                ndims) &&
           ! is_equal(id + p * ndims, x + i * ndims, ndims)) {
      if (next [p] > res) {
        mexErrMsgTxt("An element of NEXT is greater than the size of the table") ;
      }
      /* append */
      if (next [p] == 0) {
        if (last >= res) {
          size_t res_ = res + VL_MAX(res / 2, 2) ;
          h    = mxRealloc(h,    res_ * sizeof(vl_uint32)       ) ;
          next = mxRealloc(next, res_ * sizeof(vl_uint32)       ) ;
          id   = mxRealloc(id,   res_ * sizeof(vl_uint8) * ndims) ;
          memset (h    + res,         0, (res_ - res) * sizeof(vl_uint32)       ) ;
          memset (next + res,         0, (res_ - res) * sizeof(vl_uint32)       ) ;
          memset (id   + res * ndims, 0, (res_ - res) * sizeof(vl_uint8) * ndims) ;
          res = res_ ;
        }
        next [p] = ++ last ;
      }
      p = next [p] - 1 ;
    }

    /* accumulate */
    h  [p] += 1 ;
    /*    mexPrintf("p %d dims %d i %d N %d\n ", p, ndims, i, N) ;*/
    cpy(id + p * ndims, x + i * ndims, ndims) ;
  }

  mxSetData(h_,    mxRealloc(h,    last * sizeof(vl_uint32)        )) ;
  mxSetData(next_, mxRealloc(next, last * sizeof(vl_uint32)        )) ;
  mxSetData(id_,   mxRealloc(id,   last * sizeof(vl_uint8 ) * ndims)) ;

  mxSetN(h_,    last) ;
  mxSetN(id_,   last) ;
  mxSetN(next_, last) ;

  mxSetM(h_,    1) ;
  mxSetM(next_, 1) ;
  mxSetM(id_,   ndims) ;

  out[OUT_H]    = h_ ;
  out[OUT_ID]   = id_ ;
  out[OUT_NEXT] = next_ ;
}
