/** @file bsarray.c
 ** @brief Block Sparse Array  - Definition
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/


#include "bsarray.h"
#include <stdarg.h>
#include <string.h>
#include <assert.h>



/*  */
VL_EXPORT 
VlBlockSparseArrayHeader* vl_bsarray_new (vl_uint32 dataByteDimension )
{
  VlBlockSparseArrayHeader* bsArray = (VlBlockSparseArrayHeader*) vl_malloc(sizeof(VlBlockSparseArrayHeader) + dataByteDimension) ; 

  bsArray->numberOfBlocks = 0 ; 
  bsArray->byteDimension = dataByteDimension ; 
  
  return bsArray ;
}

/*  */
VL_EXPORT 
VlBlockSparseArrayHeader*  vl_bsarray_add_block (VlBlockSparseArrayHeader *bsArray,VlBlockHeader *block ) 
{
  vl_uint32 allocatedMemory, blockLength ; 
  VlBlockHeader *newBlock ; 

  /* need to allocate more memory? */
  allocatedMemory = vl_bsarray_allocated_memory(bsArray) ; 

  if (bsArray->byteDimension - allocatedMemory <  block->blockLength + sizeof(VlBlockHeader) )
    {
      /* Aggressive choice, reduces number of reallocations of memory */
      
      
      bsArray = (VlBlockSparseArrayHeader *)vl_realloc((void*)bsArray,sizeof(VlBlockSparseArrayHeader) + VL_MAX(allocatedMemory*2,block->blockLength + sizeof(VlBlockHeader)) ) ;

      bsArray->byteDimension =  VL_MAX(allocatedMemory*2,block->blockLength + sizeof(VlBlockHeader)) ; 
      
    }

  blockLength = block->blockLength ; 

  newBlock = (VlBlockHeader*) (((void*)bsArray) + sizeof(VlBlockSparseArrayHeader)+ allocatedMemory) ;

  memcpy(newBlock,block,sizeof(VlBlockHeader) + blockLength ) ;

  bsArray->numberOfBlocks++ ; 

  return bsArray ; 
}

/* free not used memory */
VL_EXPORT
VlBlockSparseArrayHeader *  vl_bsarray_finalise (VlBlockSparseArrayHeader *bsArray) 
{
  vl_uint32 allocatedMemory ; 

  allocatedMemory = vl_bsarray_allocated_memory(bsArray) ; 

  if (bsArray->byteDimension > allocatedMemory)
    {
      bsArray = (VlBlockSparseArrayHeader *)vl_realloc((void*)bsArray,sizeof(VlBlockSparseArrayHeader) + allocatedMemory) ;
      bsArray->byteDimension = allocatedMemory ; 
    }

  return bsArray ; 
}


/*  */
VL_EXPORT 
void vl_bsarray_delete (VlBlockSparseArrayHeader* bsarray)
{
  vl_free(bsarray) ;
}

/*  */
VL_EXPORT 
double*  vl_bsarray_full (VlBlockSparseArrayHeader* bsArray) 
{
  double* output ; 

  vl_uint32 M ; 
  
  VlBlockHeader *block ;  

  vl_uint32 i, j,pos, *index, numElements ;

  M = vl_bsarray_length(bsArray) ;

  output = (double*) vl_calloc(M,sizeof(double)) ;

    
  pos = sizeof(VlBlockSparseArrayHeader) ; 
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      block = (VlBlockHeader*) (((void*)bsArray) + pos) ; 
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  for (j = 0 ; j <  block->blockLength / sizeof(vl_uint32); j++)
	    {
	      output[j + block->position] = (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + j*sizeof(vl_uint32)))) ;
	    }
	  break ;
	case VL_BLOCK_CONSTANT :
	  numElements = *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader))) ; 
       
	  for (j = 0 ; j <  numElements; j++)
	    {
	      output[j + block->position] = (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) +sizeof(vl_uint32)))) ; 
	    }
	  break ;
	case VL_BLOCK_SPARSE :
	  
	  numElements = (block->blockLength - sizeof(vl_uint32))  / (sizeof(vl_uint32)*2) ; 

	  index = (vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader)) ;

	  for (j = 0 ; j <  numElements*2; j += 2)
	    {
	      output[block->position + *(index + j)] = (double)(*((float*)(index + j + 1))) ;
	    }

	  break ;
	}
      pos += block->blockLength + sizeof(VlBlockHeader) ;
    }

  return output ; 
}


/*  */
VL_EXPORT 
double vl_bsarray_get (VlBlockSparseArrayHeader* bsArray, vl_uint32 pos) 
{
  vl_uint32 i, bytePos ; 
  VlBlockHeader *tempBlock, *prevBlock ;

  if (bsArray->numberOfBlocks == 0)
    {
      return 0 ;
    }


  bytePos = sizeof(VlBlockSparseArrayHeader) ; 

  tempBlock = NULL ; 

  for ( i = 0; i < bsArray->numberOfBlocks; i++)
    {
      prevBlock = tempBlock ; 
      tempBlock = (VlBlockHeader*) (((void*)bsArray) + pos) ; 

      if (pos < tempBlock->position)
	{
	  if (prevBlock == NULL)
	    {
	      return 0 ;
	    }

	  if (pos > prevBlock->position)
	    {
	      switch (prevBlock->blockType)
		{
		case VL_BLOCK_DENSE:
		  return vl_block_dense_get(prevBlock,pos - prevBlock->position) ;
		case VL_BLOCK_CONSTANT:
		  return vl_block_constant_get(prevBlock,pos - prevBlock->position) ;
		case VL_BLOCK_SPARSE:
		  return vl_block_sparse_get(prevBlock,pos - prevBlock->position) ;
		}
	    }
	  else
	    {
	      return 0 ;
	    }
	}

      pos += tempBlock->blockLength + sizeof(VlBlockHeader) ;
    }

  return 0 ;

}


/*  */
VL_EXPORT 
double vl_bsarray_mtimes (VlBlockSparseArrayHeader* bsArray, double* b ) 
{
  VlBlockHeader *block ;  

  double acc = 0 ;

  vl_uint32 i, j,pos, *index, numElements ;

  
  pos = sizeof(VlBlockSparseArrayHeader) ; 
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      block = (VlBlockHeader*) (((void*)bsArray) + pos) ; 
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  for (j = 0 ; j <  block->blockLength / sizeof(vl_uint32); j++)
	    {
	      acc += (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + j*sizeof(vl_uint32)))*b[j + block->position]) ;
	    }
	  break ;
	case VL_BLOCK_CONSTANT :
	  numElements = *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader))) ; 
	  for (j = 0 ; j <  numElements; j++)
	    {
	      acc += b[j + block->position] ;
	    }
	  acc *= (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))) ; 
	  break ;
	case VL_BLOCK_SPARSE :
	  
	  numElements = (block->blockLength - sizeof(vl_uint32)) / (sizeof(vl_uint32)*2) ; 

	  index = (vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader)) ;

	  for (j = 0 ; j <  numElements*2; j += 2)
	    {
	      acc += (double)(*((float*)(index + j + 1)) * b[block->position + *(index + j)]) ;
	    }

	  break ;
	}
      pos += block->blockLength + sizeof(VlBlockHeader) ;
    }

  return acc ; 
}

