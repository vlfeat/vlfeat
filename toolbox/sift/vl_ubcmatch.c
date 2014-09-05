/** @internal
 ** @file    vl_siftmatch.c
 ** @brief   vl_sitfmatch - MEX definition
 ** @author  Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>

#include <vl/generic.h>

#include<stdlib.h>
#include<string.h>
#include<math.h>

#define TYPEOF_mxDOUBLE_CLASS double
#define TYPEOF_mxSINGLE_CLASS float
#define TYPEOF_mxINT8_CLASS   char
#define TYPEOF_mxUINT8_CLASS  unsigned char

#define PROMOTE_mxDOUBLE_CLASS double
#define PROMOTE_mxSINGLE_CLASS float
#define PROMOTE_mxINT8_CLASS   int
#define PROMOTE_mxUINT8_CLASS  int

#define MAXVAL_mxDOUBLE_CLASS mxGetInf()
#define MAXVAL_mxSINGLE_CLASS ((float)mxGetInf())
#define MAXVAL_mxINT8_CLASS   0x7fffffff
#define MAXVAL_mxUINT8_CLASS  0x7fffffff

typedef struct
{
  int k1 ;
  int k2 ;
  double score ;
} Pair ;

/*
 * This macro defines the matching function for abstract type; that
 * is, it is a sort of C++ template.  This is also a good illustration
 * of why C++ is preferable for templates :-)
 */
#define _COMPARE_TEMPLATE(MXC)                                          \
  Pair* compare_##MXC (Pair* pairs_iterator,                            \
                       const TYPEOF_##MXC * L1_pt,                      \
                       const TYPEOF_##MXC * L2_pt,                      \
                       int K1, int K2, int ND, float thresh)            \
  {                                                                     \
    int k1, k2 ;                                                        \
    const PROMOTE_##MXC maxval = MAXVAL_##MXC ;                         \
    for(k1 = 0 ; k1 < K1 ; ++k1, L1_pt += ND ) {                        \
                                                                        \
      PROMOTE_##MXC best = maxval ;                                     \
      PROMOTE_##MXC second_best = maxval ;                              \
      int bestk = -1 ;                                                  \
                                                                        \
      /* For each point P2[k2] in the second image... */                \
      for(k2 =  0 ; k2 < K2 ; ++k2, L2_pt += ND) {                      \
                                                                        \
        int bin ;                                                       \
        PROMOTE_##MXC acc = 0 ;                                         \
        for(bin = 0 ; bin < ND ; ++bin) {                               \
          PROMOTE_##MXC delta =                                         \
            ((PROMOTE_##MXC) L1_pt[bin]) -                              \
            ((PROMOTE_##MXC) L2_pt[bin]) ;                              \
          acc += delta*delta ;                                          \
          if (acc >= second_best) {                                     \
            break ;                                                     \
          }                                                             \
        }                                                               \
                                                                        \
        /* Filter the best and second best matching point. */           \
        if(acc < best) {                                                \
          second_best = best ;                                          \
          best = acc ;                                                  \
          bestk = k2 ;                                                  \
        } else if(acc < second_best) {                                  \
          second_best = acc ;                                           \
        }                                                               \
      }                                                                 \
                                                                        \
      L2_pt -= ND*K2 ;                                                  \
                                                                        \
      /* Lowe's method: accept the match only if unique. */             \
      if(thresh * (float) best < (float) second_best &&                 \
         bestk != -1) {                                                 \
        pairs_iterator->k1 = k1 ;                                       \
        pairs_iterator->k2 = bestk ;                                    \
        pairs_iterator->score = best ;                                  \
        pairs_iterator++ ;                                              \
      }                                                                 \
    }                                                                   \
                                                                        \
    return pairs_iterator ;                                             \
  }                                                                     \

_COMPARE_TEMPLATE( mxDOUBLE_CLASS )
_COMPARE_TEMPLATE( mxSINGLE_CLASS )
_COMPARE_TEMPLATE( mxINT8_CLASS   )
_COMPARE_TEMPLATE( mxUINT8_CLASS  )

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  int unsigned K1, K2, ND ;
  void* L1_pt ;
  void* L2_pt ;
  double thresh = 1.5 ;
  mxClassID data_class ;
  enum {L1=0,L2,THRESH} ;
  enum {MATCHES=0,D} ;

  /* -----------------------------------------------------------------
  **                                               Check the arguments
  ** -------------------------------------------------------------- */
  if (nin < 2) {
    mexErrMsgTxt("At least two input arguments required");
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments");
  }

  if(!mxIsNumeric(in[L1]) ||
     !mxIsNumeric(in[L2]) ||
     mxGetNumberOfDimensions(in[L1]) > 2 ||
     mxGetNumberOfDimensions(in[L2]) > 2) {
    mexErrMsgTxt("L1 and L2 must be two dimensional numeric arrays") ;
  }

  K1 = mxGetN(in[L1]) ;
  K2 = mxGetN(in[L2]) ;
  ND = mxGetM(in[L1]) ;

  if(mxGetM(in[L2]) != ND) {
    mexErrMsgTxt("L1 and L2 must have the same number of rows") ;
  }

  data_class = mxGetClassID(in[L1]) ;
  if(mxGetClassID(in[L2]) != data_class) {
    mexErrMsgTxt("L1 and L2 must be of the same class") ;
  }

  L1_pt = mxGetData(in[L1]) ;
  L2_pt = mxGetData(in[L2]) ;

  if(nin == 3) {
    if(!vlmxIsPlainScalar(in[THRESH])) {
      mexErrMsgTxt("THRESH should be a real scalar") ;
    }
    thresh = *mxGetPr(in[THRESH]) ;
  } else if(nin > 3) {
    mexErrMsgTxt("At most three arguments are allowed") ;
  }

  /* -----------------------------------------------------------------
  **                                                        Do the job
  ** -------------------------------------------------------------- */
  {
    Pair* pairs_begin = (Pair*) mxMalloc(sizeof(Pair) * (K1+K2)) ;
    Pair* pairs_iterator = pairs_begin ;


#define _DISPATCH_COMPARE( MXC )                                        \
    case MXC :                                                          \
      pairs_iterator = compare_##MXC(pairs_iterator,                    \
                                     (const TYPEOF_##MXC*) L1_pt,       \
                                     (const TYPEOF_##MXC*) L2_pt,       \
                                     K1,K2,ND,thresh) ;                 \
    break ;                                                             \

    switch (data_class) {
    _DISPATCH_COMPARE( mxDOUBLE_CLASS ) ;
    _DISPATCH_COMPARE( mxSINGLE_CLASS ) ;
    _DISPATCH_COMPARE( mxINT8_CLASS   ) ;
    _DISPATCH_COMPARE( mxUINT8_CLASS  ) ;
    default :
      mexErrMsgTxt("Unsupported numeric class") ;
      break ;
    }

    /* ---------------------------------------------------------------
     *                                                        Finalize
     * ------------------------------------------------------------ */
    {
      Pair* pairs_end = pairs_iterator ;
      double* M_pt ;
      double* D_pt = NULL ;

      out[MATCHES] = mxCreateDoubleMatrix
        (2, pairs_end-pairs_begin, mxREAL) ;

      M_pt = mxGetPr(out[MATCHES]) ;

      if(nout > 1) {
        out[D] = mxCreateDoubleMatrix(1,
                                      pairs_end-pairs_begin,
                                      mxREAL) ;
        D_pt = mxGetPr(out[D]) ;
      }

      for(pairs_iterator = pairs_begin ;
          pairs_iterator < pairs_end  ;
          ++pairs_iterator) {
        *M_pt++ = pairs_iterator->k1 + 1 ;
        *M_pt++ = pairs_iterator->k2 + 1 ;
        if(nout > 1) {
          *D_pt++ = pairs_iterator->score ;
        }
      }
    }
    mxFree(pairs_begin) ;
  }
}
