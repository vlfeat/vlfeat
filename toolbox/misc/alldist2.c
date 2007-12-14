/* file:        alldist2.c
** description: All pairwise distances
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include <mexutils.h>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

enum {
  opt_L0,
  opt_L1,
  opt_L2,
  opt_LINF,
  opt_MIN,
  opt_CHI2,
  opt_KCHI2,
  opt_KL1
} ;

uMexOption options [] = {
  {"l2",           0,   opt_L2            },
  {"l1",           0,   opt_L1            },
  {"linf",         0,   opt_LINF          },
  {"l0",           0,   opt_L0            },  
  {"min",          0,   opt_MIN           },  
  {"chi2",         0,   opt_CHI2          },
  {"kchi2",        0,   opt_KCHI2         },
  {"kl1",          0,   opt_KL1           },
  {0,              0,   0                 }
} ;


#undef MIN
#undef MAX
#undef ABS
#undef ABS_DIFF
#undef CORE

#define MIN(x,y)      ((x) <= (y) ? (x) :  (y))
#define MAX(x,y)      ((x) >= (y) ? (x) :  (y))
#define ABS(x)        ((x) >= 0   ? (x) : -(x))
#define ABS_DIFF(x,y) ((x) >= (y) ? ((x) - (y)) : ((y) - (x)))

/* for L2 norm */
/*#define CMP(s1,s2) ((double)(s1*s2)) */

/* for L1 norm */
#define CMP(s1,s2) ((double)MIN(s1,s2)) 

#define UINT8_t  unsigned char
#define  INT8_t  char
#define UINT16_t unsigned short
#define  INT16_t short
#define UINT32_t unsigned int
#define  INT32_t int
#define SINGLE_t float
#define DOUBLE_t double

#define CORE(NORM,F,DC,AC)                                              \
  void                                                                  \
  dist ## NORM ## _ ## DC ## _ ## AC                                    \
  (                                                                     \
   AC ## _t * pt, DC ## _t * s1_pt,                                     \
   DC ## _t * s2_pt,                                                    \
   int L, int N1, int N2,                                               \
   bool self)                                                           \
  {                                                                     \
    int j1,j2,l ;                                                       \
    for(j2 = 0 ; j2 < N2 ; ++j2)  {                                     \
      for(j1 = 0 ; j1 < N1 ; ++j1) {                                    \
        if(! self || j1>=j2) {                                          \
          AC ## _t acc = 0 ;                                            \
          DC ## _t * s1_it = s1_pt + L*j1 ;                             \
          DC ## _t * s2_it = s2_pt + L*j2 ;                             \
          for(l = 0 ; l < L ; ++l) {                                    \
            AC ## _t s1 = *s1_it++ ;                                    \
            AC ## _t s2 = *s2_it++ ;                                    \
            F(AC, s1, s2)                                               \
          }                                                             \
          *pt = acc;                                                    \
        } else {                                                        \
          *pt = *(pt + (j1 - j2) * (N1 - 1))  ;                         \
        }                                                               \
        pt++ ;                                                          \
      }                                                                 \
    }                                                                   \
  }                                                                     \
  
#define DEF_CLASS(NORM,F)                          \
  CORE (NORM,  F,  INT8,     INT32)                \
  CORE (NORM,  F,  UINT8,   UINT32)                \
  CORE (NORM,  F,  INT16,    INT32)                \
  CORE (NORM,  F,  UINT16,  UINT32)                \
  CORE (NORM,  F,  INT32,    INT32)                \
  CORE (NORM,  F,  UINT32,  UINT32)                \
  CORE (NORM,  F,  SINGLE,  SINGLE)                \
  CORE (NORM,  F,  DOUBLE,  DOUBLE)                \

#define  F_L0(AC,x,y)   { acc += (x) != (y) ; }
#define  F_L1(AC,x,y)   { acc += ABS_DIFF(x,y) ; }
#define  F_L2(AC,x,y)   { AC ## _t tmp = ABS_DIFF(x,y) ; acc += tmp * tmp ; }
#define  F_LINF(AC,x,y) { acc = MAX(acc, ABS_DIFF(x,y)) ; }
#define  F_MIN(AC,x,y)  { acc += MIN(x,y) ; }
#define  F_CHI2(AC,x,y)                                  \
  {                                                      \
    AC ## _t  meant2 = ((x) + (y))  ;                    \
    if (meant2 != 0) {                                   \
      AC ## _t tmp  = ABS_DIFF(x,y) ;                    \
      acc += tmp * tmp / meant2 ;                        \
    }                                                    \
  }
#define  F_KL1(AC,x,y)  { acc += MIN(x,y) ; }
#define  F_KCHI2(AC,x,y)                                 \
  {                                                      \
    AC ## _t  mean = ((x) + (y)) / 2  ;                  \
    if (mean != 0) {                                     \
      AC ## _t tmp  = (x) * (y) ;                        \
      acc += tmp / mean ;                                \
    }                                                    \
  }

DEF_CLASS (L0,    F_L0   )
DEF_CLASS (L1,    F_L1   )
DEF_CLASS (L2,    F_L2   )
DEF_CLASS (LINF,  F_LINF )
DEF_CLASS (MIN,   F_MIN  )
DEF_CLASS (CHI2,  F_CHI2 )
DEF_CLASS (KL1,   F_KL1  )
DEF_CLASS (KCHI2, F_KCHI2)

/* driver */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{

  typedef int  unsigned data_t ; 

  /*  mxClassID data_class = mxINT8_CLASS ;*/
  enum {IN_S1,IN_S2} ;
  enum {OUT_D=0} ;
  int L,N1,N2 ;
  void const * s1_pt ;
  void const * s2_pt ;
  mxClassID data_class ;
  mxClassID acc_class ;
  int dims [2] ; 
  
  /* for option parsing */
  bool           self = 1 ;      /* called with one numeric argument? */
  int            norm = opt_L2 ; /* type of norm to be computed       */
  int            opt ;
  int            next = 1 ;
  mxArray const *optarg ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  
  if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }

  if (nin < 1) {
    mexErrMsgTxt("At leat one argument required.") ;    
  }
  
  if(! mxIsNumeric(in[IN_S1])) {
    mexErrMsgTxt ("X must be numeric") ;
  }
  
  if (nin >= 2 && mxIsNumeric(in[IN_S2])) {
    self = 0 ;
    next = 2 ;
  }

  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_L0   : case opt_L1   :
    case opt_L2   : case opt_LINF :
    case opt_MIN  : case opt_CHI2 :
    case opt_KCHI2: case opt_KL1  :
      norm = opt ;
      break ;
    default: 
      assert(0) ;
    }
  }
    
  data_class = mxGetClassID(in[IN_S1]) ;
  if ((!self) && data_class != mxGetClassID(in[IN_S2])) {
    mexErrMsgTxt("X and Y must have the same numeric class") ;
  }
  
  L  = mxGetM(in[IN_S1]) ; 
  N1 = mxGetN(in[IN_S1]) ;
  N2 = self ?  N1 : mxGetN(in[IN_S2]) ;
  
  dims[0] = N1 ; 
  dims[1] = N2 ;
  
  if ((!self) && L != mxGetM(in[IN_S2])) {
    mexErrMsgTxt("X and Y must have the same number of rows") ;
  }
  
  s1_pt = mxGetData(in[IN_S1]) ; 
  s2_pt = self ? s1_pt : mxGetData(in[IN_S2]) ;
  
#define DISPATCH_CLASS(NORM, DC,AC)                                     \
  case mx ## DC ## _CLASS :                                             \
    acc_class = mx ## AC ## _CLASS ;                                    \
  out[OUT_D] = mxCreateNumericArray(2,dims,acc_class,mxREAL) ;          \
  dist ## NORM ## _ ## DC ## _ ## AC                                    \
    ( (AC ## _t *)mxGetData(out[OUT_D]),                                \
      (DC ## _t *)s1_pt,                                                \
      (DC ## _t *)s2_pt,                                                \
      L, N1, N2,                                                        \
      self ) ;                                                          \
  break ;
  
#define DISPATCH_NORM(NORM)                                    \
  case opt_ ## NORM :                                          \
    switch (data_class) {                                      \
      DISPATCH_CLASS(NORM,  UINT8 , UINT32)                    \
        DISPATCH_CLASS(NORM,  INT8 ,  INT32)                   \
        DISPATCH_CLASS(NORM, UINT16, UINT32)                   \
        DISPATCH_CLASS(NORM,  INT16,  INT32)                   \
        DISPATCH_CLASS(NORM, UINT32, UINT32)                   \
        DISPATCH_CLASS(NORM,  INT32,  INT32)                   \
        DISPATCH_CLASS(NORM, SINGLE, SINGLE)                   \
        DISPATCH_CLASS(NORM, DOUBLE,DOUBLE)                    \
    default:                                                   \
      mexErrMsgTxt("Data class not supported!") ;              \
    }                                                          \
  break ;
  
  switch (norm) {
    DISPATCH_NORM(L0   ) ;
    DISPATCH_NORM(L1   ) ;
    DISPATCH_NORM(L2   ) ;
    DISPATCH_NORM(LINF ) ;
    DISPATCH_NORM(MIN  ) ;
    DISPATCH_NORM(CHI2 ) ;
    DISPATCH_NORM(KL1  ) ;
    DISPATCH_NORM(KCHI2) ;
  default:
    assert(0) ;
  }
}
