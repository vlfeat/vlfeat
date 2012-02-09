/** @internal
 ** @file     vl_bsarray_new.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Array - Constructor - MEX
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
  enum {IN_TRANSPOSE=0, IN_NUMBER_OF_BLOCKS=1, IN_DATA=2, IN_END} ;
  enum {OUT_DATA=0} ;


  vl_uint32 transpose ;
  vl_uint32 numberOfBlocks ;
  vl_uint32** data;

  vl_uindex i,n;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) 
    {
      mexErrMsgTxt("Seven argument required.") ;
    }
 
  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  if (!mxIsCell(in[IN_DATA]))
    {
      mexErrMsgTxt("Third Input expected to be a cell array.") ;
    }

  transpose = (vl_uint32) mxGetScalar(in[IN_TRANSPOSE]) ;
  numberOfBlocks = (vl_uint32) mxGetScalar(in[IN_NUMBER_OF_BLOCKS]) ;

  /* Reads cell array */
  n = mxGetNumberOfElements (in[IN_DATA]);
  
  data = (vl_uint32**) vl_malloc(sizeof(vl_uint32*)*n);
  
  for ( i = 0; i < n; i++)
    {
      data[i] = (vl_uint32*) mxGetData ( mxGetCell (in[IN_DATA], i)) ; 
    }
  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseArrayHeader* bsarray = vl_bsarray_new(transpose, numberOfBlocks,data); 
  

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
  dims [0] =  bsarray->byteDimension ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_DATA], (double*)bsarray) ;
  mxSetDimensions (out[OUT_DATA], dims, 2) ;

  /* cleanup */
  vl_free(data) ;
}
