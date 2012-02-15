/** @internal
 ** @file     vl_bmatrix_list_delete.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Matrix List - Delete - MEX
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
  enum {IN_BSMATRIX_LIST=0, IN_END} ; 
 

  

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin != 1) 
    {
      mexErrMsgTxt("One argument required.") ;
    }

  if (nout > 0) 
    {
      mexErrMsgTxt("No output required.") ;
    }

  

  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseMatrixListHeader* bsMatrix = (VlBlockSparseMatrixListHeader*) mxGetData(in[IN_BSMATRIX_LIST]) ;

  vl_bsmatrix_list_delete(bsMatrix) ; 

 

}
