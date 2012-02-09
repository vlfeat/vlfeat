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
VlBlockSparseArrayHeader* vl_bsarray_new(vl_uint32 transpose, vl_uint32 numberOfBlocks,  vl_uint32** data)
{
  vl_uindex k, i, j, l,  dimension ;
  vl_uint32 type ;
  vl_uint32 length = 0 ; 

  VlBlockHeader *block ; 

  dimension = sizeof(VlBlockSparseArrayHeader) ;

  /* Check dimension of array*/
  for (j = 0,i = 0; j < numberOfBlocks; j++)
    {
      type = data[i][0];
      switch (type)
	{
	case VL_BLOCK_DENSE :
	  dimension += sizeof(VlBlockHeader) + data[i][3]*sizeof(vl_uint32) ;
	  i += 2 ;
	  break ;
	case VL_BLOCK_CONSTANT :
	  dimension += sizeof(VlBlockHeader) + sizeof(vl_uint32) ;
	  i += 2 ;
	  break ;
	case VL_BLOCK_SPARSE :
	  dimension += sizeof(VlSparseBlockHeader) + data[i][4]*2*sizeof(vl_uint32);
	  i += 3 ; 
	  break ;
	}
    }


  /* Allocates memory */
  void *bsarray = (void*) vl_malloc(dimension) ;

  /* Initializes relative pointer internal to the bsarray */
  k = 0 ;

  /* Initializes Header */
  ((VlBlockSparseArrayHeader*)bsarray)->transpose = transpose ;
  ((VlBlockSparseArrayHeader*)bsarray)->length = length ;
  ((VlBlockSparseArrayHeader*)bsarray)->byteDimension = dimension ; 
  ((VlBlockSparseArrayHeader*)bsarray)->numberOfBlocks = numberOfBlocks ; 
  /* update pointer */
  k += sizeof(VlBlockSparseArrayHeader);
  

  /* Read blocks */
  for (j = 0,i = 0; j < numberOfBlocks; j++)
    {

 
      block = (VlBlockHeader*)(bsarray + k) ;
      block->blockType =  data[i][0] ;
      block->numericType = data[i][1] ;
      block->pos = data[i][2] - 1 ;
      block->length = data[i][3] ;
      
      /* Updates array length */
      length += block->length ;

      

      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  k +=  sizeof(VlBlockHeader) ;
	  memcpy(bsarray+k,data[i+1],block->length*sizeof(vl_uint32));
	  k += block->length*sizeof(vl_uint32) ;
	  i += 2 ;
	  break ;
	case VL_BLOCK_CONSTANT :
	  k +=  sizeof(VlBlockHeader) ;
	  memcpy(bsarray+k,data[i+1],sizeof(vl_uint32));
	  k += sizeof(vl_uint32) ;
	  i += 2 ;
	  break ;
	case VL_BLOCK_SPARSE :
	  l = data[i][4] ; 
	  *((vl_uint32*)(bsarray+ k + sizeof(VlBlockHeader))) = l ;
	  k += sizeof(VlSparseBlockHeader)  ; 
 
	  memcpy(bsarray+k,data[i+1],l*sizeof(vl_uint32)) ;
	  k += l*sizeof(vl_uint32) ;

	  memcpy(bsarray+k,data[i+2],l*sizeof(vl_uint32)) ;
	  k += l*sizeof(vl_uint32) ;
	  
	  i += 3 ;
	  
	  break ;
	}

    }
  
  assert(dimension == k);
  ((VlBlockSparseArrayHeader*)bsarray)->length = length ;

  return (VlBlockSparseArrayHeader*)bsarray ; 
}


/*  */
VL_EXPORT 
void vl_bsarray_delete (VlBlockSparseArrayHeader* bsarray)
{
  vl_free((void*)bsarray) ;
}


/*  */
VL_EXPORT 
VlBlockSparseArrayIterator* vl_bsarray_iterator_new (VlBlockSparseArrayHeader* bsarray) 
{
  VlBlockSparseArrayIterator* iter = (VlBlockSparseArrayIterator*) vl_malloc(sizeof(VlBlockSparseArrayIterator)) ;
  
  iter->currentPos = 0 ;
  iter->bsarray = bsarray ;

  return iter ;
}


/*  */
VL_EXPORT 
void vl_bsarray_iterator_delete (VlBlockSparseArrayIterator* iter) 
{
  vl_free(iter) ;
}

/*  */
VL_EXPORT 
VlBlockHeader* vl_bsarray_iterator_next (VlBlockSparseArrayIterator* iter) 
{
  VlSparseBlockHeader* sparseBlock ; 
  if (iter->currentPos + sizeof(VlSparseBlockHeader)>= iter->bsarray->byteDimension)
    {
      return NULL;
    }

  VlBlockHeader* block = (VlBlockHeader*) (((void*)iter->bsarray) + sizeof(VlBlockSparseArrayHeader) + iter->currentPos);

  /* Updates current byte position */
  switch (block->blockType)
    {
    case VL_BLOCK_DENSE :
      iter->currentPos += sizeof(VlBlockHeader) + block->length*sizeof(vl_uint32) ;
      break ;
    case VL_BLOCK_CONSTANT :
      iter->currentPos += sizeof(VlBlockHeader) + sizeof(vl_uint32) ;
      break ;
    case VL_BLOCK_SPARSE :
      sparseBlock = (VlSparseBlockHeader*) block;
      iter->currentPos += sizeof(VlSparseBlockHeader) + 2*sparseBlock->numberOfElements*sizeof(vl_uint32) ;
      break ;
    }

  return block ;
  
}


/*  */
VL_EXPORT 
double vl_bsarray_mtimes (VlBlockSparseArrayHeader* a, float* b ) 
{
  VlBlockSparseArrayIterator *iter ;
  VlBlockHeader *block ;  

  VlSparseBlockHeader* sparseBlock ; 

  double acc = 0 ;

  vl_uint32 i, pos, *index ;

  
  iter = vl_bsarray_iterator_new (a) ;


  while (block = vl_bsarray_iterator_next(iter))
    {
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  for (i = 0 ; i <  block->length; i++)
	    {
	      acc += (double)(*((float*)((vl_uint32*)block) + 4 + i)*b[i + block->pos]) ;
	    }
	  break ;
	case VL_BLOCK_CONSTANT :
	  for (i = 0 ; i <  block->length; i++)
	    {
	      acc += b[i + block->pos] ;
	    }
	  acc *= (double)(*((float*)((vl_uint32*)block) + 4)) ; 
	  break ;
	case VL_BLOCK_SPARSE :
	  sparseBlock = (VlSparseBlockHeader*) block ; 
	  
	  index = (vl_uint32*) block ; 

	  for (i = 0 ; i <  sparseBlock->numberOfElements; i++)
	    {
	      pos = *(index + 5 + i) - 1;
	      acc += (double)(*((float*) (index + 5 + i+sparseBlock->numberOfElements)) * b[block->pos + pos]) ;
	    }

	  break ;
	}
      
    }

  return acc ; 
}
