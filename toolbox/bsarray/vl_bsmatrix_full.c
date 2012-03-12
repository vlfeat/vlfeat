/** @internal
 ** @file     vl_bsmatrix_full.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse matrix - Returns a full double array - MEX
 **/

/*
Copyright Statement
*/

#include <mexutils.h>

#include <vl/bsmatrix.h>


/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_BSARRAY=0, IN_END} ;
  enum {OUT_FULL_ARRAY=0} ;

  VlBlockSparseMatrixHeader* bsMatrix;

  vl_uint32 i, N ;
  vl_uint32 M = 0 ; 

  double *output ; 
  
  VlBlockSparseArrayHeader* column ; 

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

  bsMatrix = (VlBlockSparseMatrixHeader*) mxGetData(in[IN_BSARRAY]) ;

  N = bsMatrix->numColumns ; 

  if (N != 0)
    {
      column = vl_bsmatrix_get_column (bsMatrix, 0) ; 

      M = vl_bsarray_length(column) ;
    }

  /* ...............................................................
   *                                                     Fill output
   * ............................................................ */
  
  output = vl_bsmatrix_full(bsMatrix) ; 

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
  dims [0] = M ;
  dims [1] = N ;
  mxSetPr         (out[OUT_FULL_ARRAY], (double*)output) ;
  mxSetDimensions (out[OUT_FULL_ARRAY], dims, 2) ;
  


}
