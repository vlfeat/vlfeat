/** @file bsarray.h
 ** @brief Block Sparse Array 
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#ifndef VL_BSARRAY_H
#define VL_BSARRAY_H

#include "generic.h"


/* Block Types */
#define  VL_BLOCK_DENSE 1
#define  VL_BLOCK_CONSTANT 2
#define  VL_BLOCK_SPARSE 3


/* Block Struct */
typedef struct _VlBlockHeader {
  vl_uint32 blockType ;
  /* block length is the size in bytes of the block  without considering the header*/
  vl_uint32 blockLength ;
  vl_uint32 numericType ;
  vl_uint32 position ;
} VlBlockHeader ;

/* Sparse Block Struct */
typedef struct _VlSparseBlockHeader {
  VlBlockHeader header ;
  vl_uint32 length ;
} VlSparseBlockHeader ;

/* Constant Block Struct */
typedef struct _VlConstantBlockHeader {
  VlBlockHeader header ;
  vl_uint32 length ;
  vl_uint32 value ; 
} VlConstantBlockHeader ;

/* Block Sparse Array Struct */
typedef struct _VlBlockSparseArrayHeader {
  vl_uint32 byteDimension ; 
  vl_uint32 numberOfBlocks ;
} VlBlockSparseArrayHeader ;
  




/* VlBlockSparseArray Public Functions */
/*  */
VL_EXPORT 
VlBlockSparseArrayHeader* vl_bsarray_new (vl_uint32 dataByteDimension ) ; 

/*  */
VL_EXPORT 
VlBlockSparseArrayHeader*  vl_bsarray_add_block (VlBlockSparseArrayHeader *bsArray,VlBlockHeader *block ) ; 

/* free not used memory */
VL_EXPORT
VlBlockSparseArrayHeader *  vl_bsarray_finalise (VlBlockSparseArrayHeader *bsArray) ;

/*  */
VL_EXPORT 
void vl_bsarray_delete (VlBlockSparseArrayHeader* bsArray) ; 

/*  */
VL_EXPORT 
double* vl_bsarray_full (VlBlockSparseArrayHeader* bsArray) ; 

/*  */
VL_EXPORT 
double vl_bsarray_get (VlBlockSparseArrayHeader* bsArray, vl_uint32 pos) ; 

/*  */
VL_EXPORT 
double vl_bsarray_mtimes (VlBlockSparseArrayHeader* bsArray, double* b ) ;



/* TODO */
//VL_EXPORT 
//double vl_bsarray_accumulate (VlBlockSparseArrayHeader* a, double b ) ;







/* Utility Functions */

VL_INLINE
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
VL_INLINE
vl_uint32 vl_block_dense_get(VlBlockHeader* block, vl_uint32 pos)
{
  if (pos >= block->blockLength / sizeof(vl_uint32))
    {
      return 0 ;
    }

  return *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader) + pos*sizeof(vl_uint32))) ; 
}

/*  */
VL_INLINE
vl_uint32 vl_block_constant_get(VlBlockHeader* block, vl_uint32 pos)
{
  if (pos >= *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader))))
    {
      return 0 ;
    }

  return *((vl_uint32*)(((void*) block) + sizeof(VlBlockHeader) + sizeof(vl_uint32))) ; 
}

/*  */
VL_INLINE
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

/* VL_BSARRAY_H */
#endif
