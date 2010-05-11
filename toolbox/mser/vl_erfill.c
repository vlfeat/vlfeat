/* file:        erfill.mex.c
** description: Extremal Regions filling
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file
 ** @brief Maximally Stable Extremal Regions - MEX implementation
 **/

#include <mexutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))

typedef char unsigned val_t ;
typedef int           idx_t ;
typedef vl_uint64 acc_t ;

/* advance N-dimensional subscript */
void
adv(mwSize const* dims, int ndims, int* subs_pt)
{
  int d = 0 ;
  while(d < ndims) {
    if( ++subs_pt[d]  < (signed) dims[d] ) return ;
    subs_pt[d++] = 0 ;
  }
}

/* driver */
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{

  enum {IN_I=0, IN_ER} ;
  enum {OUT_MEMBERS} ;

  idx_t i ;
  int k, nel, ndims ;
  mwSize const * dims ;
  val_t const * I_pt ;
  int last = 0 ;
  int last_expanded = 0 ;
  val_t value = 0 ;

  double const * er_pt ;

  int*   subs_pt ;       /* N-dimensional subscript                 */
  int*   nsubs_pt ;      /* diff-subscript to point to neigh.       */
  idx_t* strides_pt ;    /* strides to move in image array          */
  val_t* visited_pt ;    /* flag                                    */
  idx_t* members_pt ;    /* region members                          */
  bool invert = VL_FALSE ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  if (nin != 2) {
    mexErrMsgTxt("Two arguments required.") ;
  } else if (nout > 4) {
    mexErrMsgTxt("Too many output arguments.");
  }

  if(mxGetClassID(in[IN_I]) != mxUINT8_CLASS) {
    mexErrMsgTxt("I must be of class UINT8.") ;
  }

  if(!vlmxIsPlainScalar(in[IN_ER])) {
    mexErrMsgTxt("ER must be a DOUBLE scalar.") ;
  }

  /* get dimensions */
  nel   = mxGetNumberOfElements(in[IN_I]) ;
  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims  = mxGetDimensions(in[IN_I]) ;
  I_pt  = mxGetData(in[IN_I]) ;

  /* allocate stuff */
  subs_pt    = mxMalloc( sizeof(int)      * ndims ) ;
  nsubs_pt   = mxMalloc( sizeof(int)      * ndims ) ;
  strides_pt = mxMalloc( sizeof(idx_t)    * ndims ) ;
  visited_pt = mxMalloc( sizeof(val_t)    * nel   ) ;
  members_pt = mxMalloc( sizeof(idx_t)    * nel   ) ;

  er_pt = mxGetPr(in[IN_ER]) ;

  /* compute strides to move into the N-dimensional image array */
  strides_pt [0] = 1 ;
  for(k = 1 ; k < ndims ; ++k) {
    strides_pt [k] = strides_pt [k-1] * dims [k-1] ;
  }

  /* load first pixel */
  memset(visited_pt, 0, sizeof(val_t) * nel) ;
  {
    idx_t idx = (idx_t) *er_pt ;
    if (idx < 0) {
      idx = -idx;
      invert = VL_TRUE ;
    }
    if( idx < 1 || idx > nel ) {
      char buff[80] ;
      snprintf(buff,80,"ER=%d out of range [1,%d]",idx,nel) ;
      mexErrMsgTxt(buff) ;
    }
    members_pt [last++] = idx - 1 ;
  }
  value = I_pt[ members_pt[0] ]  ;

  /* -----------------------------------------------------------------
   *                                                       Fill region
   * -------------------------------------------------------------- */
  while(last_expanded < last) {

    /* pop next node xi */
    idx_t index = members_pt[last_expanded++] ;

    /* convert index into a subscript sub; also initialize nsubs
       to (-1,-1,...,-1) */
    {
      idx_t temp = index ;
      for(k = ndims-1 ; k >=0 ; --k) {
        nsubs_pt [k] = -1 ;
        subs_pt  [k] = temp / strides_pt [k] ;
        temp         = temp % strides_pt [k] ;
      }
    }

    /* process neighbors of xi */
    while(VL_TRUE) {
      int good = VL_TRUE ;
      idx_t nindex = 0 ;

      /* compute NSUBS+SUB, the correspoinding neighbor index NINDEX
         and check that the pixel is within image boundaries. */
      for(k = 0 ; k < ndims && good ; ++k) {
        int temp = nsubs_pt [k] + subs_pt [k] ;
        good &= 0 <= temp && temp < (signed) dims[k] ;
        nindex += temp * strides_pt [k] ;
      }

      /* process neighbor
         1 - the pixel is within image boundaries;
         2 - the pixel is indeed different from the current node
         (this happens when nsub=(0,0,...,0));
         3 - the pixel has value not greather than val
         is a pixel older than xi
         4 - the pixel has not been visited yet
      */
      if(good
         && nindex != index
         && ((!invert && I_pt [nindex] <= value) ||
             ( invert && I_pt [nindex] >= value))
         && ! visited_pt [nindex] ) {

        /* mark as visited */
        visited_pt [nindex] = 1 ;

        /* add to list */
        members_pt [last++] = nindex ;
      }

      /* move to next neighbor */
      k = 0 ;
      while(++ nsubs_pt [k] > 1) {
        nsubs_pt [k++] = -1 ;
        if(k == ndims) goto done_all_neighbors ;
      }
    } /* next neighbor */
  done_all_neighbors : ;
  } /* goto pop next member */

  /*
   * Save results
   */
  {
    mwSize dims[2] ;
    int unsigned * pt ;
    dims[0] = last ;
    out[OUT_MEMBERS] = mxCreateNumericArray(1,dims,mxUINT32_CLASS,mxREAL);
    pt = mxGetData(out[OUT_MEMBERS]) ;
    for (i = 0 ; i < last ; ++i) {
      *pt++ = members_pt[i] + 1 ;
    }
  }

  /* free stuff */
  mxFree( members_pt ) ;
  mxFree( visited_pt ) ;
  mxFree( strides_pt ) ;
  mxFree( nsubs_pt   ) ;
  mxFree( subs_pt    ) ;
}
