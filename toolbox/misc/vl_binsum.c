/** @internal
 ** @file     binsum.c
 ** @author   Andrea Vedaldi
 ** @brief    BINSUM - MEX
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include <mexutils.h>

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
  const double *B_end ;
  double *R_pt ;
  
  if( nin < 3 ) {
    mexErrMsgTxt("At least three arguments required.") ;
  } else if( nin > 4 ) {
    mexErrMsgTxt("At most four arguments.") ;
  } else if (nout > 1) {
    mexErrMsgTxt("At most one output argument.") ;
  }
  
  if(! uIsReal(in[IN_H]) ||
     ! uIsReal(in[IN_X]) ||
     ! uIsReal(in[IN_B]) )
    mexErrMsgTxt("All arguments must be real.") ;
  
  KH = mxGetNumberOfElements(in[IN_H]) ; /* accumulator */
  KX = mxGetNumberOfElements(in[IN_X]) ; /* values */
  KB = mxGetNumberOfElements(in[IN_B]) ; /* accumulator indeces */

  H_pt = mxGetPr(in[IN_H]) ;
  X_pt = mxGetPr(in[IN_X]) ;
  B_pt = mxGetPr(in[IN_B]) ;
  
  B_end = B_pt + KB ;
  
  out[0] = mxDuplicateArray(in[IN_H]) ;
  R_pt   = mxGetPr(out[0]) ;
  
  if ((KX != KB) && (KX > 1)) {
    mexErrMsgTxt("X and B must have the same number of elements, or X must be a scalar.") ;
  }
  
  /* All dimensions mode ------------------------------------------- */
  if (nin == 3) {
    
    while (B_pt < B_end) {
      
      /* next bin to accumulate */
      j = (int)(*B_pt++) - 1;
      
      /* bin index out of bounds ?*/
      if(j < -1 || j >= KH) {
        char str [256] ;
        snprintf(str, 256, 
                 "Index out of bounds "
                 "(B(%ld)=%d).",
                 B_pt - mxGetPr(in[IN_B]), j + 1) ;
        mexErrMsgTxt(str) ;
      }

      /* accumulate (but skip null indeces) */
      if (j >= 0) {
        R_pt[j] += *X_pt ;
      }
      
      /* except for the scalar X case, keep X and B synchronized */
      if (KX > 1) ++ X_pt ;
    }
  }
  
  /* One dimension mode -------------------------------------------- */
  else { 
    int k ;    
    unsigned int d  = (unsigned int)*mxGetPr(in[IN_DIM]) - 1 ;
    
    unsigned int HD = mxGetNumberOfDimensions(in[IN_H]) ;
    unsigned int XD = mxGetNumberOfDimensions(in[IN_X]) ;
    unsigned int BD = mxGetNumberOfDimensions(in[IN_B]) ;
    
    mwSize const* Hdims = mxGetDimensions(in[IN_H]) ;
    mwSize const* Xdims = mxGetDimensions(in[IN_X]) ;
    mwSize const* Bdims = mxGetDimensions(in[IN_B]) ;
    
    const double* B_brk ;
    const double* B_nbrk ;
    
    unsigned int srd ;
    
    /* We need to check a few more details about the matrices */
    if( d >= HD ) {
      mexErrMsgTxt("DIM out of bound.") ;
    }
    
    /* 
       Here either B has the same number of dimensions of H, or B has
       exactly one dimension less and DIM=end.  The latter is a
       special case due to the fact that MATLAB deletes singleton
       dimensions at the ends of arrays, so it would be impossible to
       operate with DIM=end and size(B,end)=1, which is a logically
       acceptable case. 
    */
    
    if (HD != BD) {
      if (! d == HD - 1 && BD == HD-1) { 
        mexErrMsgTxt("H and B must have the same number of dimensions.") ;
      }
    }
    
    if ((BD != XD) && (KX > 1)) {
      mexErrMsgTxt("X mut have the same number of dimensions of B or be a scalar.") ;    
    }
        
    /* This will contain the stride required to advance to the next
     * element along dimension DIM. This is the product of all
     * dimensions < d. */
    
    srd = 1 ;
    
    for(k = 0 ; k < XD ; ++k) {
      if (KX > 1 &&  Xdims[k] != Bdims[k]) {
        mexErrMsgTxt("X and B have incompatible dimensions.") ;
      }
      if (k != d && (Bdims[k] != Hdims[k])) {
        mexErrMsgTxt("B and H have incompatible dimensions.") ;
      }
      if (k < d) {
        srd = srd * Bdims[k] ;
      }
    }
    
    /* We sum efficiently by a single pass of B_pt in memory order.
     * This however makes the algorithm a bit more involved. How the
     * sum is performed is easeir understood by an example. Consider
     * the case d = 3 and BD = 5. So elements in B are indexed as
     *
     *  B[i0, i1, id, i4, i5]  (note that id=i3)
     *
     * where the indexes (i0,i1,id,i4,i5) are scanned in column-major
     * orer. For each of such elements, we acces the element
     *
     *  R[i0, i1, B[i0,i1,id,i4,i5], i4, i5]
     *
     * For greater efficiency, we do not compute the multi-indexes
     * explicity; instead we advance B_pt at each iteration and we
     * keep R_pt properly synchronized.
     *
     * In particular, at each iteration we want R_pt to point to
     *
     *  R[i0, i1, 0, i4, i5]
     *
     * Therefore, whenever advancing B_pt correspnds to advancing
     *
     * a - i0, i1 : we advance R_pt
     * b - id     : we advance R_pt and then we subtract srd to move id back
     *              one position;
     * c - i4,i5  : we do the same as before, but then we need to add 
     *              srd * Hdims[d] *  elements to move R_pt up one place
     *              after dimension d.
     *
     * We can easily keep track of the three cases without computing
     * explicitly the indexs. In fact, case (a) occurs srd-1
     * consecutive steps and case (b) at the srd step. Similarly, case
     * (c) accurs at step srd * Bdims[d] steps. The pattern repeats
     * regularly.
     */

    KH     = Hdims[d] ;
    B_brk  = B_pt + srd ;              /* next time case (b) */
    B_nbrk = B_pt + srd * Bdims [d] ;  /* next time case (c) */

    while (B_pt < B_end) {
      
      /* next bin to accumulate */
      j = (int)(*B_pt) - 1;

      /* index out of bounds? */
      if(j < -1 || j >= KH) {
        char str [256] ;
        snprintf(str, 256,
                 "Index out of bounds "
                 "(B(%ld)=%d).",
                 B_pt-mxGetPr(in[IN_B]),j + 1) ;
        mexErrMsgTxt (str) ;
      }
      
      /* accumulate (but skip null indeces) */
      if (j >= 0) {
        R_pt [j * srd] += *X_pt ;
      }

      /* next element */
      if (KX > 1) X_pt++ ;
      B_pt ++ ;
      R_pt ++ ;
      
      if (B_pt == B_brk) {                 /* case (b) occurs */
        B_brk += srd ;                     /* next time case (b) */
        R_pt  -= srd ;
        if (B_pt == B_nbrk) {              /* case (c) occurs */
          B_nbrk += srd * Bdims[d] ;       /* next time case (c) */
          R_pt   += srd * Hdims[d] ;
        }
      }
    }
  }
}
