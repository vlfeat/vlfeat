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
  VlBlockSparseArrayIterator* iter ; 

  VlBlockHeader* block ;
  VlSparseBlockHeader* sparseBlock ; 

  float *output ; 

  vl_uint32 i ; 
  vl_uint32* index ;

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
   *                                                   Define output
   * ............................................................ */

  output = (float*) vl_malloc(sizeof(float)*bsarray->length) ; 

  mwSize dims [2] ;

  /* create an empty array */
  dims [0] = 0 ;
  dims [1] = 0 ;
  out[OUT_FULL_ARRAY] = mxCreateNumericArray
    (2, dims, mxSINGLE_CLASS, mxREAL) ;

  /* create output array */
  dims [0] = bsarray->length ;
  dims [1] = 1 ;
  mxSetPr         (out[OUT_FULL_ARRAY], (double*)output) ;
  mxSetDimensions (out[OUT_FULL_ARRAY], dims, 2) ;
  

  for ( i = 0 ; i < bsarray->length ; i++)
    {
      output[i] = 0; 
    }
  
  /* ...............................................................
   *                                                     Fill output
   * ............................................................ */
  
  

  iter = vl_bsarray_iterator_new(bsarray) ;

  while (block = vl_bsarray_iterator_next(iter))
    {
      mexPrintf("%d - %d\n",block->blockType,iter->currentPos);
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  
	  for ( i = 0; i < block->length; i++)
	    {
	      index = (vl_uint32*) block ; 
	      output[block->pos + i] = *((float*) (index + 4 + i)) ;
	    }

	  break ;
	case VL_BLOCK_CONSTANT :
	  
	  for ( i = 0; i < block->length; i++)
	    {
	      index = (vl_uint32*) block ; 
	      output[block->pos + i] = *((float*) (index + 4)) ;
	    }

	  break ;
	case VL_BLOCK_SPARSE :
	  sparseBlock = (VlSparseBlockHeader*) block;
	  
	  index = (vl_uint32*) block ; 

	  for ( i = 0; i < sparseBlock->numberOfElements; i++)
	    {
	      

	      output[block->pos + *(index + 5 + i) - 1] = *((float*) (index + 5 + i+sparseBlock->numberOfElements)) ;
	    }

	  break ;
	}
    }

}
