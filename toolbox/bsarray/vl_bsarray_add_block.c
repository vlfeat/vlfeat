/** @internal
 ** @file     vl_bsarray_add_block.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Array - Add Block - MEX
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
  enum {IN_BSARRAY, IN_BLOCK, IN_END} ;
  enum {OUT_DATA=0} ;

  mwSize  M, N ; 

  int i ;

  const mxArray* temp ;

  VlBlockSparseArrayHeader* bsArray ;
  VlBlockHeader *block ;
  
  vl_uint32 type, position = 0 ;

  vl_bool copy = VL_TRUE ; 
  
  if (nin < 2) 
    {
      mexErrMsgTxt("One argument required.") ;
    }

  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  bsArray = (VlBlockSparseArrayHeader*) mxGetData(in[IN_BSARRAY]) ;

  position = vl_bsarray_length(bsArray) ; 

  
  for (i = 1; i < nin;i++)
       {
	 temp = in[i] ; 

	 M = mxGetM (temp) ;
	 N = mxGetN (temp) ;

      
	 if (!mxIsSparse(temp) && M == 1 && N == 1)
	   {
	     if (mxGetScalar(temp) - 1 < position) 
	       mexErrMsgTxt("Blocks can't overlap.") ; 

	     position = mxGetScalar(temp) - 1 ; 

	     continue ;
	   }

	 if (mxIsDouble(temp) && !mxIsSparse(temp))
	   {
	     mexErrMsgTxt("Input must be a single precision array.") ;
	   }

	 if (N > 1)
	   {
	     mexErrMsgTxt("Input must be a one dimensional array.") ;
	   }
      
	 if (mxIsSingle(temp)) 
	   type = VL_TYPE_FLOAT ;
	 else if (mxIsUint32(temp))
	   type = VL_TYPE_UINT32 ;
	 else if (mxIsInt32(temp))
	   type = VL_TYPE_INT32 ;
	 else
	   mexErrMsgTxt("Input type not supported.") ;
	 

	 if (isSparseBlock((vl_uint32*)mxGetData(temp),mxGetM(temp)))
	   {
	     block = getSparseBlock((vl_uint32*)mxGetData(temp),mxGetM(temp),position,type) ; 
	     
	   }
	 else if (isConstantBlock((vl_uint32*)mxGetData(temp),mxGetM(temp)))
	   {
	     block = getConstantBlock((vl_uint32*)mxGetData(temp),mxGetM(temp),position,type) ;
	     
	   }
	 else
	   {
	     block = getDenseBlock((vl_uint32*)mxGetData(temp),mxGetM(temp),position,type) ;
	     
	   }
	 
	 bsArray = vl_bsarray_add_block(bsArray,block,copy) ;
	 vl_free(block) ;

	 copy = VL_FALSE ; 

	 position += M ; 
       }

  bsArray = vl_bsarray_finalise(bsArray) ; 

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
