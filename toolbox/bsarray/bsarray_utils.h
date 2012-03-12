/** @file utils.h
 ** @brief Block Sparse Array Useful Functions for mex files
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#ifndef VL_BSARRAY_UTILS_H
#define VL_BSARRAT_UTILS_H

#include <mexutils.h>

#include <vl/bsarray.h>

VL_INLINE
VlBlockSparseArrayHeader * vl_bsarray_automatic_extraction(VlBlockSparseArrayHeader * bsArray,const vl_uint32 * data, vl_uindex M, vl_uint32 type) 
{
  vl_uindex  i ; 
  
  vl_uint32  pos ; 

  vl_uint32 index = 0 ;  
  vl_uint32 length = 0 ; 
  vl_uint32 zeros = 0 ; 

  vl_bool block = 1 ;

  VlBlockHeader *tempBlock ; 

  vl_uint32 threshold = sizeof(VlBlockHeader) / sizeof(vl_uint32) ; 

 



  if (data[0] == 0)
    block = 0;
  

  for (i = 0; i < M; i++,length++)
    {
      if (block)
	{
	  if (data[i] == 0 )
	    zeros++ ;
	  else
	    zeros = 0 ;
	  
	  if (zeros > threshold)
	    {
	      block = 0 ; 
	      
	      length -= threshold ; 

	      if (isSparseBlock(data +index,length))
		{
		  tempBlock = getSparseBlock(data+index,length,index,type) ;
		}
	      else if(isConstantBlock(data +index,length))
		{
		  tempBlock = getConstantBlock(data+index,length,index,type) ;
		}
	      else
		{
		  tempBlock = getDenseBlock(data+index,length,index,type) ;
		}

	      
	    
	      bsArray = vl_bsarray_add_block(bsArray,tempBlock,VL_FALSE) ;
	      vl_free(tempBlock) ;
	      index = i ;
	      length = 0 ; 
	      zeros = 0 ; 
	    }
	  
	  
	}
      else
	{
	  if (data[i] != 0 )
	    {
	      block = 1 ;
	      index = i ; 
	      length = 0 ;
	      zeros = 0 ; 
	    }
	}
    }
  
  if (block)
    { 

      if (isSparseBlock(data +index,length))
	{
	  tempBlock = getSparseBlock(data+index,length,index,type) ;
	}
      else if(isConstantBlock(data +index,length))
	{
	  tempBlock = getConstantBlock(data+index,length,index,type) ;
	}
      else
	{
	  tempBlock = getDenseBlock(data+index,length,index,type) ;
	}

	       
      bsArray = vl_bsarray_add_block(bsArray,tempBlock,VL_FALSE) ;
      vl_free(tempBlock) ;
    }
  else
    {
      pos = bsArray->numberOfBlocks - 1 ; 

      tempBlock = vl_bsarray_get_block(bsArray,pos) ;

      if (tempBlock->blockType == VL_BLOCK_SPARSE)
	{
	  VlSparseBlockHeader* sparseBlock = (VlSparseBlockHeader*) tempBlock ;
	  sparseBlock->length += length ; 
	} 
      else
	{
	  tempBlock = getSparseBlock(data+index,length,index,type) ;
	  bsArray = vl_bsarray_add_block(bsArray,tempBlock,VL_FALSE) ;
	  vl_free(tempBlock) ;
	}
    }

  

  return bsArray ; 
} 


/* VL_BSARRAY_UTILS_H */
#endif
