/** @internal
 ** @file     vl_bsmatrix_new.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Matrix - Constructor - MEX
 **/

/*
Copyright Statement
*/


#include "bsarray_utils.h"
#include <vl/bsmatrix.h>

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATA, IN_END} ;
  enum {OUT_DATA=0} ;

  VlBlockSparseMatrixListHeader *bsMatrix ; 

  VlBlockSparseArrayHeader *bsArray ; 

  mwSize  M, N,i ; 

  vl_uint32* data,type ; 
 
  if (nin > 1) 
    {
      mexErrMsgTxt("Only one argument required.") ;
    }

  if (nout > 1) 
    {
      mexErrMsgTxt("Only One output provided.") ;
    }

  bsMatrix = vl_bsmatrix_list_new(0) ; 

  if (nin == 1)
    {
      M = mxGetM (in[IN_DATA]) ;
      N = mxGetN (in[IN_DATA]) ;

      if (mxIsSingle(in[IN_DATA])) 
	type = VL_TYPE_FLOAT ;
      else if (mxIsUint32(in[IN_DATA]))
	type = VL_TYPE_UINT32 ;
      else if (mxIsInt32(in[IN_DATA]))
	type = VL_TYPE_INT32 ;
      else
	mexErrMsgTxt("Input type not supported.") ;

      data = (vl_uint32*) mxGetData(in[IN_DATA]) ; 

      for (i = 0; i < M*N; i += M)
	{
	  bsArray = vl_bsarray_new(0) ;

	  VlBlockHeader* tBlock ;

	  /* tBlock = getDenseBlock(data + i, M,0,type) ; */

	  bsArray = vl_bsarray_automatic_extraction(bsArray,data + i, M, type) ;

	  /* bsArray = vl_bsarray_add_block(bsArray,tBlock,VL_FALSE); */

	  bsMatrix = vl_bsmatrix_list_add_column(bsMatrix,bsArray,VL_FALSE) ; 

	  vl_bsarray_delete(bsArray) ; 
	}
    }

  bsMatrix = (VlBlockSparseMatrixListHeader*)vl_bsmatrix_finalise((VlBlockSparseMatrixHeader*)bsMatrix) ; 

  /* ...............................................................
   *                                                       Save back
   * ............................................................ */
  mwSize dims [2] ;

  /* create an empty array */
  dims [0] = 0 ;
  dims [1] = 0 ;
  out[OUT_DATA] = mxCreateNumericArray
    (2, dims, mxUINT32_CLASS, mxREAL) ;


  //set array content to be the bsarray
  dims [0] =  sizeof(VlBlockSparseMatrixListHeader)/sizeof(vl_uint32) + bsMatrix->header.byteDimension / sizeof(vl_uint32) ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_DATA], (double*)bsMatrix) ;
  mxSetDimensions (out[OUT_DATA], dims, 2) ;

}
