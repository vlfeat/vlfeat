/** @file bsarray.c
 ** @brief Block Sparse Array  - Definition
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/


#include "bsarray.h"
#include <string.h>
#include <assert.h>

/* START UTILITY FUNCTIONS */

/*  */
VL_EXPORT
double innerProductSparseBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 numElements = (block->blockLength - sizeof(vl_uint32)) / (sizeof(vl_uint32)*2) ;

  vl_uint32 * index = (vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader)) ;

  vl_uint32 j, i ;

  double res = 0 ;

  if (mapFunc)
    {
      double temp[order] ;

      for (i = 0 ; i <  numElements*2; i += 2)
	{
	  mapFunc(map,temp,1,(double)(*((float*)(index + i + 1))));

	  for (j = 0; j < order; j++)
	    {
	      res += array[ (position + *(index + i))*order + j]*temp[j] ;
	    }
	}
    }
  else
    {

      for (j = 0 ; j <  numElements*2; j += 2)
	{
	  res += array[ position + *(index + j)]  * (double)(*((float*)(index + j + 1))) ;
	}
    }

  return res ;
}

/*  */
VL_EXPORT
double innerProductConstantBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 numElements = *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader))) ;

  vl_uint32 j, i ;


  double res = 0 ;


  if (mapFunc)
    {
      double temp[order] ;


      mapFunc(map,temp,1,(double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))));

      for (i = 0 ; i <  numElements; i++)
	{

	  for (j = 0; j < order; j++)
	    {
	      res += array[(position + i)*order + j]*temp[j] ;
	    }
	}
    }
  else
    {
      for (j = 0 ; j <  numElements; j++)
	{
	  res += array[position + j]  * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))) ;
	}
    }

  return res ;
}

/*  */
VL_EXPORT
double innerProductDenseBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 j, i ;


  double res = 0 ;

  if (mapFunc)
    {
      double temp[order] ;

      for (i = 0 ; i <  block->blockLength / sizeof(vl_uint32); i++)
	{
	  mapFunc(map,temp,1,(double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + i*sizeof(vl_uint32)))));
	  for (j = 0; j < order; j++)
	    {
	      res += array[(position + i)*order + j]*temp[j] ;
	    }
	}
    }
  else
    {
      for (j = 0 ; j <  block->blockLength / sizeof(vl_uint32); j++)
	{
	  res += array[position + j]  * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + j*sizeof(vl_uint32)))) ;
	}
    }

  return res ;
}

/*  */
VL_EXPORT
void accumulatorSparseBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 numElements = (block->blockLength - sizeof(vl_uint32)) / (sizeof(vl_uint32)*2) ;

  vl_uint32 * index = (vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader)) ;

  vl_uint32 j, i ;

  if (mapFunc)
    {
      double temp[order] ;


      if (preConditioner)
	for (i = 0 ; i <  numElements*2; i += 2)
	  {
	    mapFunc(map,temp,1,(double)(*((float*)(index + i + 1))));
	    for (j = 0; j < order; j++)
	    {
	      array[ (position + *(index + i))*order + j ] += multiplier * preConditioner[ (position + *(index + i))*order + j] * temp[j] ;
	    }
	  }
      else
	for (i = 0 ; i <  numElements*2; i += 2)
	  {
	    mapFunc(map,temp,1,(double)(*((float*)(index + i + 1))));
	    for (j = 0; j < order; j++)
	    {
	      array[ (position + *(index + i))*order + j ] += multiplier * temp[j] ;
	    }
	  }

    }
  else
    {
      if (preConditioner)
	for (j = 0 ; j <  numElements*2; j += 2)
	  {
	    array[ position + *(index + j)] += multiplier * preConditioner[ position + *(index + j)] * (double)(*((float*)(index + j + 1))) ;
	  }
      else
	for (j = 0 ; j <  numElements*2; j += 2)
	  {
	    array[ position + *(index + j)] += multiplier  * (double)(*((float*)(index + j + 1))) ;
	  }
    }
}

/*  */
VL_EXPORT
void accumulatorConstantBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 numElements = *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader))) ;

  vl_uint32 j, i ;
  if (mapFunc)
    {
      double temp[order] ;

      mapFunc(map,temp,1,(double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))));

      if (preConditioner)
	for (i = 0 ; i <  numElements; i++)
	  {
	    for (j = 0; j < order; j++)
	      {
		array[(position + i)*order + j] += multiplier * preConditioner[(position + i)*order + j] * temp[j]  ;
	      }
	  }
      else
	for (i = 0 ; i <  numElements; i++)
	  {
	    for (j = 0; j < order; j++)
	      {
		array[(position + i)*order + j] += multiplier  * temp[j] ;
	      }
	  }
    }
  else
    {
      if (preConditioner)
	for (j = 0 ; j <  numElements; j++)
	  {
	    array[position + j] += multiplier * preConditioner[position + j] * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))) ;
	  }
      else
	for (j = 0 ; j <  numElements; j++)
	  {
	    array[position + j] += multiplier * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + sizeof(vl_uint32)))) ;
	  }
    }
}

/*  */
VL_EXPORT
void accumulatorDenseBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_uint32 j, i ;

  if (mapFunc)
    {
      double temp[order] ;
      if (preConditioner)
	for (i = 0 ; i <  block->blockLength / sizeof(vl_uint32); i++)
	  {
	    mapFunc(map,temp,1,(double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + i*sizeof(vl_uint32))))) ;
	    for (j = 0; j < order; j++)
	      {
		array[(position + i)*order + j] += multiplier * preConditioner[(position + i)*order + j] *temp[j] ;
	      }

	  }
      else
	for (i = 0 ; i <  block->blockLength / sizeof(vl_uint32); i++)
	  {
	    mapFunc(map,temp,1,(double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + i*sizeof(vl_uint32))))) ;
	    for (j = 0; j < order; j++)
	      {
		array[(position + i)*order + j] += multiplier  *temp[j] ;
	      }

	  }
    }
  else
    {
      if (preConditioner)
	for (j = 0 ; j <  block->blockLength / sizeof(vl_uint32); j++)
	  {
	    array[position + j] += multiplier * preConditioner[position + j] * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + j*sizeof(vl_uint32)))) ;
	  }
      else
	for (j = 0 ; j <  block->blockLength / sizeof(vl_uint32); j++)
	  {
	    array[position + j] += multiplier  * (double)(*((float*)(((void*)block) + sizeof(VlBlockHeader) + j*sizeof(vl_uint32)))) ;
	  }
    }
}

/*  */
VL_EXPORT
vl_uint32 vl_bsarray_length(VlBlockSparseArrayHeader *bsArray)
{
  vl_uint32 i, pos,length ;

  VlBlockHeader *block ;


  length = 0 ;

  pos = sizeof(VlBlockSparseArrayHeader) ;
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      block = (VlBlockHeader*) (((void*)bsArray) + pos) ;

      switch(block->blockType)
	{
	case VL_BLOCK_SPARSE:
	case VL_BLOCK_CONSTANT:
	  length = block->position + *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader))) ;
	  break ;
	case VL_BLOCK_DENSE:
	  length = block->position + block->blockLength / sizeof(vl_uint32) ;
	}

      pos += block->blockLength + sizeof(VlBlockHeader) ;
    }

  return length ;
}

/*  */
VL_INLINE
vl_uint32 vl_bsarray_allocated_memory(VlBlockSparseArrayHeader *bsArray)
{
  vl_uint32 i, pos ;

  VlBlockHeader *tempBlock ;


  vl_uint32 allocatedMemory  = 0 ;

  pos = sizeof(VlBlockSparseArrayHeader) ;
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      tempBlock = (VlBlockHeader*) (((void*)bsArray) + pos) ;

      allocatedMemory += tempBlock->blockLength + sizeof(VlBlockHeader) ;

      pos += tempBlock->blockLength + sizeof(VlBlockHeader) ;
    }

  return allocatedMemory ;
}

/*  */
vl_uint32 vl_block_dense_get(VlBlockHeader* block, vl_uint32 pos)
{
  if (pos >= block->blockLength / sizeof(vl_uint32))
    {
      return 0 ;
    }

  return *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader) + pos*sizeof(vl_uint32))) ;
}

/*  */
vl_uint32 vl_block_constant_get(VlBlockHeader* block, vl_uint32 pos)
{
  if (pos >= *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader))))
    {
      return 0 ;
    }

  return *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader) + sizeof(vl_uint32))) ;
}

/*  */
vl_uint32 vl_block_sparse_get(VlBlockHeader* block, vl_uint32 pos)
{
  vl_uint32 numElements, i, *index ;

  if (pos >= *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader))))
    {
      return 0 ;
    }

  numElements = (block->blockLength - sizeof(vl_uint32)) / (sizeof(vl_uint32)*2) ;

  index = (vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader)) ;

  for (i = 0; i < numElements; i += 2)
    {
      if (pos == *(index + i))
	{
	  return *(index + i + 1) ;
	}
      if (pos < *(index + i))
	{
	  return 0 ;
	}
    }

  return 0 ;
}

/*  */
VL_EXPORT
VlBlockHeader* vl_bsarray_get_block (VlBlockSparseArrayHeader* bsArray, vl_uint32 pos)
{
  vl_uint32 i, bytePos ;
  VlBlockHeader *tempBlock;

  if (pos >= bsArray->numberOfBlocks)
    return NULL ;

  bytePos = sizeof(VlBlockSparseArrayHeader) ;

  for ( i = 0; i <= pos; i++)
    {
       tempBlock = (VlBlockHeader*) (((void*)bsArray) + bytePos) ;
       bytePos += tempBlock->blockLength + sizeof(VlBlockHeader) ;
    }

  return tempBlock ;
}

/* END UTILITY FUNCTIONS */

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
VlBlockSparseArrayHeader*  vl_bsarray_add_block (VlBlockSparseArrayHeader *bsArray,VlBlockHeader *block, vl_bool copy )
{
  vl_uint32 allocatedMemory, blockLength ;
  VlBlockHeader *newBlock ;

  /* need to allocate more memory? */
  allocatedMemory = vl_bsarray_allocated_memory(bsArray) ;

  if (bsArray->byteDimension - allocatedMemory <  block->blockLength + sizeof(VlBlockHeader) )
    {
      /* Aggressive choice, it reduces number of reallocations of memory */

      if (copy)
	{
	  VlBlockSparseArrayHeader* temp ;
	  temp = (VlBlockSparseArrayHeader *)vl_malloc(sizeof(VlBlockSparseArrayHeader) + VL_MAX(allocatedMemory*2,allocatedMemory + block->blockLength + sizeof(VlBlockHeader)) ) ;
	  memcpy(temp,bsArray,sizeof(VlBlockSparseArrayHeader) + bsArray->byteDimension) ;
	  bsArray = temp ;
	}
      else
	{

	  bsArray = (VlBlockSparseArrayHeader *)vl_realloc((void*)bsArray,sizeof(VlBlockSparseArrayHeader) + VL_MAX(allocatedMemory*2,allocatedMemory + block->blockLength + sizeof(VlBlockHeader)) ) ;
	}

      bsArray->byteDimension =  VL_MAX(allocatedMemory*2,allocatedMemory + block->blockLength + sizeof(VlBlockHeader)) ;

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
      tempBlock = (VlBlockHeader*) (((void*)bsArray) + bytePos) ;

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

      bytePos += tempBlock->blockLength + sizeof(VlBlockHeader) ;
    }

  return 0 ;

}


/*  */
VL_EXPORT
double vl_bsarray_mtimes (const VlBlockSparseArrayHeader* bsArray, const double* b )
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



/*  */
VL_EXPORT
VlBlockHeader* getSparseBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type)
{
  VlSparseBlockHeader* block ;
  vl_uindex  i,k ;
  vl_size counter = 0 ;


  for (i = 0; i < M; i++)
    {
      counter += (array[i] != 0) ;
    }

  block = (VlSparseBlockHeader*) vl_malloc(sizeof(VlSparseBlockHeader) + counter*2*sizeof(vl_uint32)) ;

  block->header.blockType = VL_BLOCK_SPARSE ;
  block->header.blockLength = (counter*2 + 1)*sizeof(vl_uint32) ;

  block->header.numericType = type ;

  block->header.position = position ;

  block->length = M ;

  for (i = 0,k = 0; i < M; i++)
    {
      if (array[i] != 0)
	{
	  *((vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader) + k*sizeof(vl_uint32))) = i ;

	  *((vl_uint32*)(((void*)block) + sizeof(VlSparseBlockHeader) + (k+1)*sizeof(vl_uint32))) = array[i] ;

	  k += 2 ;
	}

    }


  return (VlBlockHeader*)block ;

}


/*  */
VL_EXPORT
VlBlockHeader* getConstantBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type)
{
  VlConstantBlockHeader* block ;

  block = (VlConstantBlockHeader*) vl_malloc(sizeof(VlConstantBlockHeader)) ;

  block->header.blockType = VL_BLOCK_CONSTANT ;
  block->header.blockLength = 2*sizeof(vl_uint32) ;

  block->header.numericType = type ;

  block->header.position = position ;

  block->length = M ;

  block->value = array[0] ;

  return (VlBlockHeader*) block ;
}

/*  */
VL_EXPORT
VlBlockHeader* getDenseBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type)
{
  vl_uindex i  ;

  VlBlockHeader* block ;




  block = (VlBlockHeader*) vl_malloc(sizeof(VlBlockHeader) + M*sizeof(vl_uint32)) ;

  block->blockType = VL_BLOCK_DENSE ;
  block->blockLength = M*sizeof(vl_uint32) ;

  block->numericType = type ;

  block->position = position ;

  for (i = 0; i < M; i++)
    {
      *((vl_uint32*)(((void*)block) + sizeof(VlBlockHeader) + i*sizeof(vl_uint32))) = array[i] ;
    }

  return block ;
}

/*  */
VL_EXPORT
vl_bool isSparseBlock (const vl_uint32* array, vl_uindex M)
{
  vl_uindex  i ;
  vl_size counter = 0 ;


  for (i = 0; i < M; i++)
    {
      counter += (array[i] == 0) ;
    }

  return (counter) && (M / counter) < 2  ;
}


/*  */
VL_EXPORT
vl_bool isConstantBlock (const vl_uint32* array, vl_uindex M)
{
  vl_uindex i ;


  for (i = 1; i < M; i++)
    {
      if (array[0] != array[i])
	return 0 ;
    }

  return 1 ;
}
