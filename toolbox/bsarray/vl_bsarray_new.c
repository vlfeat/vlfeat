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


VlBlockHeader* getSparseBlock(const mxArray* array, vl_uint32 position)
{
  mwSize nz, i, k ; 
  mwIndex *rows;
  double *values ;
  VlSparseBlockHeader* block ;

  mwIndex *Jc;

  float temp ; 

  Jc = mxGetJc(array) ; 

  nz = Jc[1];

  rows =  mxGetIr(array) ; 

  values = (double*) mxGetData(array) ; 

  
  block = (VlSparseBlockHeader*) vl_malloc(sizeof(VlSparseBlockHeader) + nz*2*sizeof(vl_uint32)) ;

  block->header.blockType = VL_BLOCK_SPARSE ; 
  block->header.blockLength = (nz*2 + 1)*sizeof(vl_uint32) ; 
  
  block->header.numericType = VL_TYPE_FLOAT ;

  /* if (mxIsSingle(array)) */
  /*   block->header.numericType = VL_TYPE_FLOAT ;  */
  /* else if (mxIsUint32(array)) */
  /*   block->header.numericType = VL_TYPE_UINT32 ;  */
  /* else if (mxIsInt32(array)) */
  /*   block->header.numericType = VL_TYPE_INT32 ;  */
  /* else */
  /*   mexErrMsgTxt("Input type not supported.") ;  */

  block->header.position = position ; 

  block->length = (vl_uint32) mxGetM(array) ; 

  for (i = 0,k = 0; i < nz; i++,k += 2)
    {
      *((vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader) + k*sizeof(vl_uint32))) = (vl_uint32)rows[i] ; 
   
      temp = (float) values[i] ; 

      *((vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader) + (k+1)*sizeof(vl_uint32))) = *((vl_uint32*)(&temp)) ; 
	
      
    }


  return (VlBlockHeader*)block ; 
}


VlBlockHeader* getConstantBlock(const mxArray* array, vl_uint32 position)
{
  VlConstantBlockHeader* block ;

  block = (VlConstantBlockHeader*) vl_malloc(sizeof(VlConstantBlockHeader)) ;
  
  block->header.blockType = VL_BLOCK_CONSTANT ; 
  block->header.blockLength = 2*sizeof(vl_uint32) ; 
  
  if (mxIsSingle(array))
      block->header.numericType = VL_TYPE_FLOAT ; 
  else if (mxIsUint32(array))
      block->header.numericType = VL_TYPE_UINT32 ; 
  else if (mxIsInt32(array))
      block->header.numericType = VL_TYPE_INT32 ; 
  else
      mexErrMsgTxt("Input type not supported.") ; 

  block->header.position = position ; 

  block->length = (vl_uint32) mxGetM(array) ; 

  block->value = *((vl_uint32*)mxGetData(array)) ; 

  return (VlBlockHeader*) block ; 
}

VlBlockHeader* getDenseBlock(const mxArray* array, vl_uint32 position)
{
  mwSize M, i  ; 

  VlBlockHeader* block ;
  
  vl_uint32* data;
  
  M = mxGetM(array) ;

  data = (vl_uint32*) mxGetData(array) ; 

  block = (VlBlockHeader*) vl_malloc(sizeof(VlBlockHeader) + M*sizeof(vl_uint32)) ;

  block->blockType = VL_BLOCK_DENSE ; 
  block->blockLength = M*sizeof(vl_uint32) ; 
  
  if (mxIsSingle(array))
      block->numericType = VL_TYPE_FLOAT ; 
  else if (mxIsUint32(array))
      block->numericType = VL_TYPE_UINT32 ; 
  else if (mxIsInt32(array))
      block->numericType = VL_TYPE_INT32 ; 
  else
      mexErrMsgTxt("Input type not supported.") ; 

  block->position = position ; 

  for (i = 0; i < M; i++)
    {
      *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader) + i*sizeof(vl_uint32))) = data[i] ; 
    }

  return block ; 
}

bool isConstantBlock (const mxArray* array) 
{
  mwSize M, i ; 
  vl_uint32 *values ;

  values = (vl_uint32*) mxGetData(array) ;

  M = mxGetM(array) ; 
  
  for (i = 1; i < M; i++) 
    {
      if (values[0] != values[i])
	return false ; 
    }

  return true ; 
}

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {OUT_DATA=0} ;

  int i ;
  const mxArray* temp ;
  vl_uint32 position = 0 ;
  mwSize  M, N ; 

  VlBlockHeader *block ; 

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) 
    {
      mexErrMsgTxt("One argument required.") ;
    }

  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  

  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  VlBlockSparseArrayHeader* bsArray = vl_bsarray_new(0);

  for (i = 0; i < nin;i++)
    {
      temp = in[i] ; 

      M = mxGetM (temp) ;
      N = mxGetN (temp) ;

      
      if (!mxIsSparse(temp) && M == 1 && N == 1)
	{
	  if (mxGetScalar(temp) < position) 
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
      

      if (mxIsSparse(temp))
	{
	  block = getSparseBlock(temp,position) ;
	  bsArray = vl_bsarray_add_block(bsArray,block) ;
	  vl_free(block) ; 
	}
      else if (isConstantBlock(temp))
	{
	  block = getConstantBlock(temp,position) ; 
	  bsArray = vl_bsarray_add_block(bsArray,block) ;
	  vl_free(block) ; 
	}
      else
	{
	  block = getDenseBlock(temp,position) ; 
	  bsArray = vl_bsarray_add_block(bsArray,block) ;
	  vl_free(block) ; 
	}
      

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
