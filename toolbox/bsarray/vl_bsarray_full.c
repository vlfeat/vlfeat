/** @internal
 ** @file     vl_bsarray_full.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Array - Returns a full double array - MEX
 **/

/*
Copyright Statement
*/

#include <mexutils.h>

#include <vl/bsarray.h>

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_BSARRAY=0, IN_END} ;
  enum {OUT_FULL_ARRAY=0} ;

  VlBlockSparseArrayHeader* bsarray ;


  double *output ;


  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin > 1)
    {
      mexErrMsgTxt("One argument required.") ;
    }

  if (nout > 1)
    {
      mexErrMsgTxt("One output required.") ;
    }

  bsarray = (VlBlockSparseArrayHeader*) mxGetData(in[IN_BSARRAY]) ;

  /* ...............................................................
   *                                                     Fill output
   * ............................................................ */

  output = vl_bsarray_full(bsarray) ;


  /* ...............................................................
   *                                                   Define output
   * ............................................................ */


  mwSize dims [2] ;

  /* create an empty array */
  dims [0] = 0 ;
  dims [1] = 0 ;
  out[OUT_FULL_ARRAY] = mxCreateNumericArray
    (2, dims, mxDOUBLE_CLASS, mxREAL) ;

  /* create output array */
  dims [0] = vl_bsarray_length(bsarray) ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_FULL_ARRAY], (double*)output) ;
  mxSetDimensions (out[OUT_FULL_ARRAY], dims, 2) ;





}
