/** @internal
 ** @file     vl_bmatrix_list_add_column.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Matrix List - Add Column - MEX
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
  enum {IN_BSMATRIX_LIST=0, IN_BSARRAY=1, IN_END} ; 
  enum {OUT_DATA=0} ;

  

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin != 2) 
    {
      mexErrMsgTxt("Two argument required.") ;
    }

  if (nout > 0) 
    {
      mexErrMsgTxt("No output required.") ;
    }

  

  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseMatrixListHeader* bsMatrix = (VlBlockSparseMatrixListHeader*) mxGetData(in[IN_BSMATRIX_LIST]) ;

  VlBlockSparseArrayHeader* bsArray = (VlBlockSparseArrayHeader*) mxGetData(in[IN_BSARRAY]) ; 
  
  vl_bsmatrix_list_add_column(bsMatrix,bsArray) ; 

  mexMakeMemoryPersistent(bsMatrix->bsArrays);

}
