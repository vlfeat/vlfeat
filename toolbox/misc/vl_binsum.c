/** @internal
 ** @file     binsum.c
 ** @author   Andrea Vedaldi
 ** @brief    vl_binsum.c - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum { IN_H = 0, IN_X, IN_B, IN_DIM } ;
  enum { OUT_Y = 0 } ;
  vl_size KH, KX, KB ;
  vl_index j ;
  const double *H_pt, *X_pt, *B_pt ;
  const double *B_end ;
  double *R_pt ;

  if (nin < 3) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nin > 4) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if(! vlmxIsPlain (IN(H)) ||
     ! vlmxIsPlain (IN(X)) ||
     ! vlmxIsPlain (IN(B)) ) {
    vlmxError(vlmxErrInvalidArgument,
             "All arguments must be plain arrays.") ;
  }

  KH = mxGetNumberOfElements(IN(H)) ; /* accumulator */
  KX = mxGetNumberOfElements(IN(X)) ; /* values */
  KB = mxGetNumberOfElements(IN(B)) ; /* accumulator indeces */

  H_pt = mxGetPr(IN(H)) ;
  X_pt = mxGetPr(IN(X)) ;
  B_pt = mxGetPr(IN(B)) ;

  B_end = B_pt + KB ;

  out[0] = mxDuplicateArray(IN(H)) ;
  R_pt = mxGetPr(out[0]) ;

  if ((KX != KB) && (KX > 1)) {
    vlmxError(vlmxErrInvalidArgument,
             "X and B must have the same number of elements, or X must be a scalar.") ;
  }

  /* All dimensions mode ------------------------------------------- */
  if (nin == 3) {

    while (B_pt < B_end) {

      /* next bin to accumulate */
      j = (vl_index)(*B_pt++) - 1 ;

      /* bin index out of bounds ?*/
      if(j < -1 || j >= (signed) KH) {
        vlmxError(vlmxErrInconsistentData,
                 "Index B(%"  VL_FMT_INDEX ") = %" VL_FMT_INDEX " out of bound",
                 (vl_index)(B_pt - mxGetPr(IN(B))),
                 j + 1) ;
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
    vl_index k ;
    vl_size d  = (vl_size)*mxGetPr(IN(DIM)) - 1 ;

    mwSize HD = mxGetNumberOfDimensions(IN(H)) ;
    mwSize XD = mxGetNumberOfDimensions(IN(X)) ;
    mwSize BD = mxGetNumberOfDimensions(IN(B)) ;

    mwSize const * Hdims = mxGetDimensions(IN(H)) ;
    mwSize const * Xdims = mxGetDimensions(IN(X)) ;
    mwSize const * Bdims = mxGetDimensions(IN(B)) ;

    double const * B_brk ;
    double const * B_nbrk ;

    vl_size srd ;

    /* We need to check a few more details about the matrices */
    if (d >= HD) {
      vlmxError(vlmxErrInconsistentData,
               "DIM out of bounds.") ;
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
        vlmxError(vlmxErrInconsistentData,
                 "H and B must have the same number of dimensions.") ;
      }
    }

    if ((BD != XD) && (KX > 1)) {
      vlmxError(vlmxErrInconsistentData,
               "X mut have the same number of dimensions of B or be a scalar.") ;
    }

    /* This will contain the stride required to advance to the next
     * element along dimension DIM. This is the product of all
     * dimensions < d. */

    srd = 1 ;

    for(k = 0 ; k < (signed) XD ; ++k) {
      if (KX > 1 &&  Xdims[k] != Bdims[k]) {
        vlmxError(vlmxErrInconsistentData,
                 "X and B have incompatible dimensions.") ;
      }
      if (k != (signed) d && (Bdims[k] != Hdims[k])) {
        vlmxError(vlmxErrInconsistentData,
                 "B and H have incompatible dimensions.") ;
      }
      if (k < (signed) d) {
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
      j = (vl_index)(*B_pt) - 1;

      /* index out of bounds? */
      if(j < -1 || j >= (signed) KH) {
        vlmxError(vlmxErrInconsistentData,
                 "Index B(%"  VL_FMT_INDEX ") = %" VL_FMT_INDEX " out of bounds.",
                 (vl_index)(B_pt - mxGetPr(IN(B))),
                 j + 1) ;
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
