/* file:        alldist2.c
** description: All pairwise distances
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS */

#include "mexutils.h"

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#undef MIN
#undef MAX
#define MIN(x,y) ( (x) <= (y) ? (x) : (y) )
#define MAX(x,y) ( (x) >= (y) ? (x) : (y) )

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

#define DEF_CLASS(DC,AC)                                                \
  void                                                                  \
  dist2_ ## DC ## _ ## AC                                               \
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
            AC ## _t  d  = s2 - s1 ;                                    \
            acc += d * d ;                                              \
          }                                                             \
          *pt = acc;                                                    \
        } else {                                                        \
          *pt = *( pt + (j1-j2)*(N1-1) )  ;                             \
        }                                                               \
        pt++ ;                                                          \
      }                                                                 \
    }                                                                   \
  }                                                                     \
  
DEF_CLASS( INT8 ,UINT32) 
DEF_CLASS(UINT8 ,UINT32) 
DEF_CLASS( INT16,UINT32) 
DEF_CLASS(UINT16,UINT32) 
DEF_CLASS( INT32,UINT32) 
DEF_CLASS(UINT32,UINT32)
DEF_CLASS(SINGLE,SINGLE)
DEF_CLASS(DOUBLE,DOUBLE) 

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
  bool self ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  self = (nin == 1) ;

  if (nin != 2 && nin != 1) {
    mexErrMsgTxt("One or two arguments required.") ;
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if( ! mxIsNumeric(in[IN_S1]) ) {
    mexErrMsgTxt("S1 must be numeric") ;
  }
  
  if( (!self) && ! mxIsNumeric(in[IN_S2]) ) {
    mexErrMsgTxt("S2 must be numeric") ;
  }
  
  data_class = mxGetClassID(in[IN_S1]) ;
  if( (!self) && data_class != mxGetClassID(in[IN_S2]) ) {
    mexErrMsgTxt("S1 and S2 must have the same numeric class") ;
  }
  
  L = mxGetM(in[IN_S1]) ; 
  N1 = mxGetN(in[IN_S1]) ;
  N2 =  self ?  N1 : mxGetN(in[IN_S2]) ;
  
  dims[0] = N1 ; 
  dims[1] = N2 ;
  
  if( (!self) && L != mxGetM(in[IN_S2]) ) {
    mexErrMsgTxt("S1 and S2 must have the same number of rows") ;
  }  
  
  s1_pt = mxGetData(in[IN_S1]) ; 
  s2_pt = self ? s1_pt : mxGetData(in[IN_S2]) ;
  
#define DO_CLASS(DC,AC)                                                 \
  case mx ## DC ## _CLASS :                                             \
    acc_class = mx ## AC ## _CLASS ;                                    \
  out[OUT_D] = mxCreateNumericArray(2,dims,acc_class,mxREAL) ;          \
  dist2_ ## DC ## _ ## AC                                               \
    ( (AC ## _t *)mxGetData(out[OUT_D]),                                \
      (DC ## _t *)s1_pt,                                                \
      (DC ## _t *)s2_pt,                                                \
      L, N1, N2,                                                        \
      self ) ;                                                          \
  break ;

  switch( data_class ) {
    DO_CLASS( UINT8 ,UINT32)
    DO_CLASS(  INT8 ,UINT32)
    DO_CLASS( UINT16,UINT32)
    DO_CLASS(  INT16,UINT32)
    DO_CLASS( UINT32,UINT32)
    DO_CLASS(  INT32,UINT32)
    DO_CLASS( SINGLE,SINGLE)
    DO_CLASS( DOUBLE,DOUBLE)
  default:
    mexErrMsgTxt("Data class not supported!") ;
  }
  
}
