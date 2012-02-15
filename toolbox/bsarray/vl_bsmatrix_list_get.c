/** @internal
 ** @file     vl_bmatrix_list_get.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Matrix List - Get Column - MEX
 **/

/*
Copyright Statement
*/

#include <mexutils.h>
#include <assert.h>

#include <vl/bsarray.h>



/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_BSMATRIX_LIST=0, IN_POS=1, IN_END} ; 
  enum {OUT_DATA=0} ;

  vl_uint32 pos,M,N ; 

  

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin != 2) 
    {
      mexErrMsgTxt("Two argument required.") ;
    }

  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  M = mxGetM(in[IN_POS]) ;
  N = mxGetM(in[IN_POS]) ;

  if(!mxIsUint32(in[IN_POS]) || M != 1 || N != 1)
    {
      mexErrMsgTxt("Index must be a UINT32 scalar.") ;
    }

  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseMatrixListHeader* bsMatrix = (VlBlockSparseMatrixListHeader*) mxGetData(in[IN_BSMATRIX_LIST]) ;

  pos = *((vl_uint32*) mxGetData(in[IN_POS])) ; 
  
  VlBlockSparseArrayHeader* bsArray = vl_bsmatrix_list_get(bsMatrix,pos) ; 

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
  dims [0] =  sizeof(VlBlockSparseArrayHeader)/sizeof(vl_uint32) + bsArray->byteDimension / sizeof(vl_uint32) ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_DATA], (double*)bsArray) ;
  mxSetDimensions (out[OUT_DATA], dims, 2) ;

}
