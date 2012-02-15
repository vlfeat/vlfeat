/** @internal
 ** @file     vl_bmatrix_list_new.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Matrix List - Constructor - MEX
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
  enum {OUT_DATA=0} ;

  

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin > 0) 
    {
      mexErrMsgTxt("No argument required.") ;
    }

  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  

  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseMatrixListHeader* bsMatrix = vl_bsmatrix_list_new(0);
  
  
  mexMakeMemoryPersistent(bsMatrix->bsArrays);

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
  dims [0] =  sizeof(VlBlockSparseMatrixListHeader)/sizeof(vl_uint32) ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_DATA], (double*)bsMatrix) ;
  mxSetDimensions (out[OUT_DATA], dims, 2) ;

}
