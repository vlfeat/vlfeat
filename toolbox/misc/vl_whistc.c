/** @internal
 ** @file    vl_whistc.c
 ** @brief   vl_whistc - MEX definition
 ** @author  Andrea Vedaldi
 **/


/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/* 2006-2-17  NaN entries are correctly handled. See m-file for
              further infos.
*/

/** @file
 ** @brief HISTC MEX function implementation.
 **/

#include <mexutils.h>
#include <vl/generic.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

/** WHISTC(X,W,EDGES) 
 **/

typedef struct 
{
  double x ;
  double w ; 
} pairs_t ; 

/** Compares pairs of values. For natural ordering (ascending), it
 ** returns
 **
 ** - < 0  if a < b
 ** - = 0  if a == b
 ** - > 0  if a > b
 **
 ** We move NaNs to the end of the sorted sequence. In this way, they
 ** will be scanned when filling the tail of the histogram and
 ** skipped.
 **/
int 
cmp_pairs(const void* a, const void* b) 
{
  pairs_t* pa = (pairs_t*) a;
  pairs_t* pb = (pairs_t*) b;
  bool nana = isnan(pa->x) ;
  bool nanb = isnan(pb->x) ;
  if(!nana && !nanb) {
    double diff = pa->x - pb->x ;
    if(diff < 0) return -1  ;
    else if(diff > 0) return +1 ;
    else return 0 ;
  } else {
    /* consider nans to be bigger than any */
    if(nana && nanb) return 0 ; /* both NaNs are euqal */
    if(nanb) return -1 ; /* only b is NaN and thus it is bigger */
    return 1 ; /* only a is NaN and this it is bigger */
  }
}

/** @brief MEX driver.
 ** @param nout number of MATLAB output arguments.
 ** @param out MATLAB output arguments.
 ** @param nin number of MATLAB input arguments.
 ** @param in MATLAB input arguments.
 **/  
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  int M, N, NE ;
  double* Xpt ;
  double* Wpt ; 
  double* EDGESpt ;
  double* RESpt ;
  enum {X=0, W, EDGES} ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  if (nin != 3) {
    mexErrMsgTxt("Three arguments required.");
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }

  if (!mxIsDouble(in[X]) || 
      !mxIsDouble(in[W]) ||
      !mxIsDouble(in[W])) {
    mexErrMsgTxt("The arguments must be real matrices.") ;
  }

  M = mxGetM(in[X]) ;
  N = mxGetN(in[X]) ;
  if( M != mxGetM(in[W]) ||
      N != mxGetN(in[W]) ) {
    mexErrMsgTxt("X and W must have the same dimension.") ;
  }

  if(VL_MIN( mxGetM(in[EDGES]), mxGetN(in[EDGES]) ) != 1) {
    mexErrMsgTxt("EDGES must be a vector.") ;
  }
  
  NE = VL_MAX(mxGetM(in[EDGES]), mxGetN(in[EDGES])) ;

  if(NE < 2) {
    mexErrMsgTxt("At least two edges are required.\n") ;
  }
  
  Xpt = mxGetPr(in[X]) ;
  Wpt = mxGetPr(in[W]) ;
  EDGESpt = mxGetPr(in[EDGES]) ;

  {
    double x = EDGESpt[0] ;
    int j ;
    int ok = 1 ; 
    for(j = 1 ; j < NE ; ++j) {
      ok &= (x < EDGESpt[j]) ;
      x = EDGESpt[j] ;
    }
    if(!ok) mexErrMsgTxt("EDGES must be increasing.") ;
  }

  /*if(nout == 0) return ;*/

  /* If the input is a vector, make it a column. */
  if(M == 1) {
    M = N ; 
    N = 1 ;
  }

  /* Alloc the result. */
  out[0] = mxCreateDoubleMatrix(NE, N, mxREAL) ;
  RESpt = mxGetPr(out[0]) ; 

  /** -----------------------------------------------------------------
   **                                                        Do the job
   ** -------------------------------------------------------------- */
  {
    pairs_t* pairs = mxMalloc(sizeof(pairs_t)*M) ;
    int e, i, j ;
    double c = 0 ;
    double* RESiterator = RESpt ;
    double* Xiterator = Xpt ;
    double* Witerator = Wpt ;

    for(j = 0 ; j < N ; ++j) {              
      e = 0;

      for(i = 0 ; i < M ; ++i) {
        pairs[i].x = *Xiterator++ ;
        pairs[i].w = *Witerator++  ;
      }

      qsort(pairs, M, sizeof(pairs_t), cmp_pairs) ;

      /* Head */
      for(i = 0 ; pairs[i].x < EDGESpt[e] ; ++i) ;

      /* Body */
      while(i < M) {
        c = 0 ;
        e += 1 ;
        if(e == NE) break ;

        for( ;  pairs[i].x < EDGESpt[e] && i < M ; ++i)
          c += pairs[i].w ;

        RESiterator[e-1] = c ;
      }

      /* Tail */
      c = 0 ;
      for( ; pairs[i].x == EDGESpt[NE-1] && i < M ; ++i)
        c += pairs[i].w ;
      RESiterator[NE-1] = c ;

      /* Next column */
      RESiterator += NE ;
    }
    
    mxFree(pairs) ;                      
  }
  return ;
}


