/** file:        binsum.c
 ** author:      Andrea Vedaldi
 ** description: MEX implementation of binsum.m
 **/

/* AUTORIGHTS */

#include "mexutils.h"

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
  enum { IN_H, IN_X, IN_B, IN_DIM } ;
  int KH, KX, KB, j ;
  const double *H_pt, *X_pt, *B_pt ;
  const double *X_end ;
  double *R_pt ;
  
  if( nin < 3 ) {
    mexErrMsgTxt("At least three arguments required") ;
  } else if( nin > 4 ) {
    mexErrMsgTxt("At most four arguments") ;
  } else if (nout > 1) {
    mexErrMsgTxt("At most one output argument") ;
  }
  
  if(! uIsReal(in[IN_H]) ||
     ! uIsReal(in[IN_X]) ||
     ! uIsReal(in[IN_B]) )
    mexErrMsgTxt("Illegal arguments") ;
  
  KH = mxGetNumberOfElements(in[IN_H]) ;
  KX = mxGetNumberOfElements(in[IN_X]) ;
  KB = mxGetNumberOfElements(in[IN_B]) ;

  H_pt = mxGetPr(in[IN_H]) ;
  X_pt = mxGetPr(in[IN_X]) ;
  B_pt = mxGetPr(in[IN_B]) ;

  X_end = X_pt + KX ;
  
  out[0] = mxDuplicateArray(in[IN_H]) ;
  R_pt   = mxGetPr(out[0]) ;
  
  if( KX != KB ) {
    mexErrMsgTxt("X and B must have the same number of elements") ;
  }
  
  /* All dimensions mode ------------------------------------------- */
  if( nin == 3 ) {

    while( X_pt < X_end ) { 
      j = (int)(*B_pt++) - 1;
      if(j < 0 || j >= KH) {
        char str [256] ;
        snprintf(str, 256, 
                 "Index out of bounds "
                 "(B(%d)=%d)",
                 B_pt-mxGetPr(in[IN_B]),j) ;
        mexErrMsgTxt(str) ;
      }
      R_pt[j] += *X_pt++ ;
    }
  }

  /* One dimension mode -------------------------------------------- */
  else { 
    int k ;    
    unsigned int d  = (unsigned int)*mxGetPr(in[IN_DIM]) - 1 ;
    
    unsigned int HD = mxGetNumberOfDimensions(in[IN_H]) ;
    unsigned int XD = mxGetNumberOfDimensions(in[IN_X]) ;
    unsigned int BD = mxGetNumberOfDimensions(in[IN_B]) ;

    int const* Hdims = mxGetDimensions(in[IN_H]) ;
    int const* Xdims = mxGetDimensions(in[IN_X]) ;
    int const* Bdims = mxGetDimensions(in[IN_B]) ;

    const double* X_brk ;
    const double* X_nbrk ;

    unsigned int srd ;

    /* We need to check a few more details about the matrices */
    if( d >= HD ) {
      mexErrMsgTxt("DIM out of bound") ;
    }

    /* Here either B,X have the same number of dimensions of H, or B,X
       have exactly one dimension less and DIM=end.  The latter is a
       special case due to the fact that MATLAB deletes singleton
       dimensions at the ends of array, so it would be impossible to
       operate with DIM=end and size(B,end)=1, which is a logically
       acceptable case. */
       
    if( HD != XD || HD != BD ) {
      if( !( d == HD-1 && XD == BD && XD == HD-1) ) { 
        mexErrMsgTxt("H, X and B must have the same number of dimensions") ;
      }
    }
   
    /* This will contain the stride required to go from one element of
     * the histogram to the next. This crossess all dimensions < d. */

    srd = 1 ;

    for(k = 0 ; k < XD ; ++k) {
      if( Xdims[k] != Bdims[k] ) {
        mexErrMsgTxt("X and B have incompatible dimensions") ;
      }
      if( k != d && (Xdims[k] != Hdims[k]) ) {
        mexErrMsgTxt("H, X and B have incompatible dimensions") ;
      }
      if( k < d ) {
        srd = srd * Xdims[k] ;
      }
    }

    /* We scan all data points in X. We partition the dimensions in
     * (a) the ones < d and (b) the ones > d. We detect the times in
     * which X_pt crossess (a) or (b) by the dynamic bounds X_brk and
     * X_nbrk respectively.
     *
     * For case (a) we need to re-position R_pt back to the
     * beginning. For case (b) we need also to move R_pt to the next
     * slice. */

    KH     = Hdims[d] ;
    X_brk  = X_pt + srd ;
    X_nbrk = X_pt + srd * Xdims[d] ; 

    while( X_pt < X_end ) {
      
      j = (int)(*B_pt) - 1;
      if(j < 0 || j >= KH) {
        char str [256] ;
        snprintf(str, 256, 
                 "Index out of bounds "
                 "(B(%d)=%d)",
                 B_pt-mxGetPr(in[IN_B]),j) ;
        mexErrMsgTxt(str) ;
      }
      R_pt[j * srd] += *X_pt ;

      /* next element */
      X_pt++ ;
      B_pt++ ;
      R_pt++ ;
      
      if( X_pt == X_brk ) {
        X_brk += srd ;
        R_pt  -= srd ;
        if( X_pt == X_nbrk ) {
          X_nbrk += srd * Xdims[d] ;
          R_pt   += srd * Hdims[d] ;
        }
      }
    }
  }
}
