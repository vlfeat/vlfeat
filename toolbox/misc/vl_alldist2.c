/* file:        alldist2.c
** description: All pairwise distances
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/mathop.h>
#include <vl/generic.h>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

enum {
  opt_LINF,
  opt_L2,
  opt_L1,
  opt_L0,
  opt_CHI2,
  opt_HELL,

  opt_KL2,
  opt_KL1,
  opt_KCHI2,
  opt_KHELL,

  opt_MIN
} ;

vlmxOption  options [] = {
  {"linf",         0,   opt_LINF          },
  {"l2",           0,   opt_L2            },
  {"l1",           0,   opt_L1            },
  {"l0",           0,   opt_L0            },
  {"chi2",         0,   opt_CHI2          },
  {"hell",         0,   opt_HELL          },

  {"kl2",          0,   opt_KL2           },
  {"kl1",          0,   opt_KL1           },
  {"kchi2",        0,   opt_KCHI2         },
  {"khell",        0,   opt_KHELL         },

  {"min",          0,   opt_MIN           },
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

#ifndef sqrtf
#define sqrtf(x) ((float)sqrt(x))
#endif

/* for L2 norm */
/*#define CMP(s1,s2) ((double)(s1*s2)) */

/* for L1 norm */
#define CMP(s1,s2) ((double)MIN(s1,s2))

#define UINT8_t  vl_uint8
#define  INT8_t  vl_int8
#define UINT16_t vl_uint16
#define  INT16_t vl_int16
#define UINT32_t vl_uint32
#define  INT32_t vl_int32
#define SINGLE_t float
#define DOUBLE_t double

#define CORE(NORM,F,DC,AC)                                              \
  void                                                                  \
  dist ## NORM ## _ ## DC ## _ ## AC                                    \
  (                                                                     \
   AC ## _t * pt, DC ## _t * s1_pt,                                     \
   DC ## _t * s2_pt,                                                    \
   vl_size L, vl_size N1, vl_size N2,                                   \
   bool self)                                                           \
  {                                                                     \
    vl_uindex j1,j2,l ;                                                 \
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

#define CORE_SPARSE(NORM, F)                                            \
  {                                                                     \
    double const * s1_pt = mxGetPr(in[IN_S1]) ;                         \
    mwIndex const * s1_ir  = mxGetIr(in[IN_S1]) ;                       \
    mwIndex const * s1_jc  = mxGetJc(in[IN_S1]) ;                       \
    double const * s2_pt = 0 ;                                          \
    mwIndex  const * s2_ir  = 0 ;                                       \
    mwIndex const * s2_jc  = 0 ;                                        \
    double * pt = mxGetPr(out[OUT_D]) ;                                 \
    vl_uindex j1, j2 ;                                                  \
                                                                        \
    if (self) {                                                         \
      s2_pt = s1_pt ;                                                   \
      s2_ir = s1_ir ;                                                   \
      s2_jc = s1_jc ;                                                   \
    } else {                                                            \
      s2_pt = mxGetPr(in[IN_S2]) ;                                      \
      s2_ir = mxGetIr(in[IN_S2]) ;                                      \
      s2_jc = mxGetJc(in[IN_S2]) ;                                      \
    }                                                                   \
                                                                        \
    for (j2 = 0 ; j2 < N2 ; ++j2)  {                                    \
      for (j1 = 0 ; j1 < N1 ; ++j1) {                                   \
        int nz1 = s1_jc [j1+1] - s1_jc [j1] ;                           \
        int nz2 = s2_jc [j2+1] - s2_jc [j2] ;                           \
        if(! self || j1 >= j2) {                                        \
          double acc = 0 ;                                              \
          double const * s1_it = s1_pt + s1_jc [j1] ;                   \
          double const * s2_it = s2_pt + s2_jc [j2] ;                   \
          mwIndex const * s1_ir_it = s1_ir + s1_jc [j1] ;               \
          mwIndex const * s2_ir_it = s2_ir + s2_jc [j2] ;               \
          mwIndex ir1 ; \
          mwIndex ir2 ; \
          while (nz1 || nz2) {                                          \
            if (nz2 == 0) {                                             \
              double a = *s1_it++ ;                                     \
              F(DOUBLE, a, 0) ;                                         \
              s1_ir_it ++ ;                                             \
              nz1 -- ;                                                  \
              continue ;                                                \
            }                                                           \
            if (nz1 == 0) {                                             \
              double b = *s2_it++ ;                                     \
              F(DOUBLE, 0, b) ;                                         \
              s2_ir_it ++ ;                                             \
              nz2 -- ;                                                  \
              continue ;                                                \
            }                                                           \
            ir1 = *s1_ir_it ;                                           \
            ir2 = *s2_ir_it ;                                           \
            if (ir1 < ir2) {                                            \
              double a = *s1_it++ ;                                     \
              F(DOUBLE, a, 0) ;                                         \
              s1_ir_it ++ ;                                             \
              nz1 -- ;                                                  \
              continue ;                                                \
            }                                                           \
            if (ir1 > ir2) {                                            \
              double b = *s2_it++ ;                                     \
              F(DOUBLE, 0, b) ;                                         \
              s2_ir_it ++ ;                                             \
              nz2 -- ;                                                  \
              continue ;                                                \
            }                                                           \
            {                                                           \
              double a = *s1_it++ ;                                     \
              double b = *s2_it++ ;                                     \
              F(DOUBLE, a, b) ;                                         \
              s1_ir_it ++ ;                                             \
              s2_ir_it ++ ;                                             \
              nz1 -- ;                                                  \
              nz2 -- ;                                                  \
            }                                                           \
          }                                                             \
          *pt = acc;                                                    \
        } else {                                                        \
          *pt = *(pt + (j1 - j2) * (N1 - 1))  ;                         \
        }                                                               \
        pt++ ;                                                          \
      }                                                                 \
    }                                                                   \
  }


#define DEF_CLASS(NORM,F)                          \
  CORE (NORM,  F,  INT8,     INT32)                \
  CORE (NORM,  F,  UINT8,   UINT32)                \
  CORE (NORM,  F,  INT16,    INT32)                \
  CORE (NORM,  F,  UINT16,  UINT32)                \
  CORE (NORM,  F,  INT32,    INT32)                \
  CORE (NORM,  F,  UINT32,  UINT32)                \
  CORE (NORM,  F,  SINGLE,  SINGLE)                \
  CORE (NORM,  F,  DOUBLE,  DOUBLE)

#define  F_L0(AC,x,y)   { acc += (x) != (y) ; }
#define  F_L1(AC,x,y)   { acc += ABS_DIFF(x,y) ; }
#define  F_L2(AC,x,y)   { AC ## _t tmp = ABS_DIFF(x,y) ; acc += tmp * tmp ; }
#define  F_LINF(AC,x,y) { acc = MAX(acc, ABS_DIFF(x,y)) ; }
#define  F_CHI2(AC,x,y)                                  \
  {                                                      \
    AC ## _t  meant2 = ((x) + (y))  ;                    \
    if (meant2 != 0) {                                   \
      AC ## _t tmp  = ABS_DIFF(x,y) ;                    \
      acc += tmp * tmp / meant2 ;                        \
    }                                                    \
  }
#define F_HELL_SINGLE(x,y) { acc += x + y - 2 * sqrtf (x * y) ; }
#define F_HELL_DOUBLE(x,y) { acc += x + y - 2 * sqrt  (x * y) ; }
#define F_HELL_UINT32(x,y) { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL_INT32(x,y)  { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL_UINT16(x,y) { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL_INT16(x,y)  { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL_UINT8(x,y)  { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL_INT8(x,y)   { acc += x + y - 2 * vl_fast_sqrt_ui32 (x * y) ; }
#define F_HELL(AC,x,y) F_HELL_ ## AC (x,y)

#define  F_KL2(AC,x,y)  { acc += (x*y) ; }
#define  F_KL1(AC,x,y)  { acc += MIN(x,y) ; }
#define  F_MIN(AC,x,y)  { acc += MIN(x,y) ; }
#define  F_KCHI2(AC,x,y)                                 \
  {                                                      \
    AC ## _t  mean = ((x) + (y)) / 2  ;                  \
    if (mean != 0) {                                     \
      AC ## _t tmp  = (x) * (y) ;                        \
      acc += tmp / mean ;                                \
    }                                                    \
  }
#define F_KHELL_SINGLE(x,y) { acc += sqrtf (x * y) ; }
#define F_KHELL_DOUBLE(x,y) { acc += sqrt  (x * y) ; }
#define F_KHELL_UINT32(x,y) { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL_INT32(x,y)  { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL_UINT16(x,y) { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL_INT16(x,y)  { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL_UINT8(x,y)  { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL_INT8(x,y)   { acc += vl_fast_sqrt_ui32 (x * y) ; }
#define F_KHELL(AC,x,y) F_KHELL_ ## AC (x,y)

DEF_CLASS (LINF,  F_LINF )
DEF_CLASS (L2,    F_L2   )
DEF_CLASS (L1,    F_L1   )
DEF_CLASS (L0,    F_L0   )
DEF_CLASS (CHI2,  F_CHI2 )
DEF_CLASS (HELL,  F_HELL )

DEF_CLASS (KL2,   F_KL2  )
DEF_CLASS (KL1,   F_KL1  )
DEF_CLASS (KCHI2, F_KCHI2)
DEF_CLASS (KHELL, F_KHELL)

DEF_CLASS (MIN,   F_MIN  )

/* driver */
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{

  typedef int  unsigned data_t ;

  /*  mxClassID data_class = mxINT8_CLASS ;*/
  enum {IN_S1,IN_S2} ;
  enum {OUT_D=0} ;
  vl_size L,N1,N2 ;
  vl_bool sparse = 0 ;
  void const * s1_pt ;
  void const * s2_pt ;
  mxClassID data_class ;
  mxClassID acc_class ;
  mwSize dims [2] ;

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

  sparse = mxIsSparse(in[IN_S1]) ;

  if (sparse && nin >=2 && mxIsNumeric(in[IN_S2])) {
    if (! mxIsSparse(in[IN_S2])) {
      mexErrMsgTxt ("X and Y must be either both full or sparse.") ;
    }
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_LINF :
    case opt_L2 :
    case opt_L1 :
    case opt_L0 :
    case opt_CHI2 :
    case opt_HELL :

    case opt_KL2 :
    case opt_KL1 :
    case opt_KCHI2 :
    case opt_KHELL :

    case opt_MIN :
      norm = opt ;
      break ;

    default:
      abort() ;
    }
  }

  data_class = mxGetClassID(in[IN_S1]) ;
  if ((!self) && data_class != mxGetClassID(in[IN_S2])) {
    mexErrMsgTxt("X and Y must have the same numeric class") ;
  }

  assert ((! sparse) || (data_class == mxDOUBLE_CLASS)) ;

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

#define DISPATCH_NORM(NORM)                                             \
  case opt_ ## NORM :                                                   \
    if (sparse) {                                                       \
      out[OUT_D] = mxCreateNumericArray(2,dims,mxDOUBLE_CLASS,mxREAL) ; \
      CORE_SPARSE(NORM, VL_XCAT(F_, NORM))                              \
    } else {                                                            \
      switch (data_class) {                                             \
        DISPATCH_CLASS(NORM,  UINT8 , UINT32)                           \
          DISPATCH_CLASS(NORM,  INT8 ,  INT32)                          \
          DISPATCH_CLASS(NORM, UINT16, UINT32)                          \
          DISPATCH_CLASS(NORM,  INT16,  INT32)                          \
          DISPATCH_CLASS(NORM, UINT32, UINT32)                          \
          DISPATCH_CLASS(NORM,  INT32,  INT32)                          \
          DISPATCH_CLASS(NORM, SINGLE, SINGLE)                          \
          DISPATCH_CLASS(NORM, DOUBLE,DOUBLE)                           \
      default:                                                          \
        mexErrMsgTxt("Data class not supported!") ;                     \
      }                                                                 \
    }                                                                   \
  break ;

  switch (norm) {
    DISPATCH_NORM(LINF ) ;
    DISPATCH_NORM(L2   ) ;
    DISPATCH_NORM(L1   ) ;
    DISPATCH_NORM(L0   ) ;
    DISPATCH_NORM(CHI2 ) ;
    DISPATCH_NORM(HELL ) ;

    DISPATCH_NORM(KL2  ) ;
    DISPATCH_NORM(KL1  ) ;
    DISPATCH_NORM(KCHI2) ;
    DISPATCH_NORM(KHELL) ;

    DISPATCH_NORM(MIN  ) ;
  default:
    abort() ;
  }
}
