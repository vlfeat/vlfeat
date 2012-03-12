/** @file bsmatrix.c
 ** @brief Block Sparse Matrix  - Definition
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#include "bsmatrix.h"
#include <string.h>

/*  */
VL_EXPORT 
VlBlockSparseMatrixListHeader* vl_bsmatrix_list_new (vl_uint32 dataByteDimension ) 
{
  VlBlockSparseMatrixListHeader* bsMatrix = (VlBlockSparseMatrixListHeader*) vl_malloc(sizeof(VlBlockSparseMatrixListHeader) + dataByteDimension) ;

  bsMatrix->header.matrixType = VL_BSMATRIX_LIST ; 

  bsMatrix->header.byteDimension = dataByteDimension ; 
  bsMatrix->header.numColumns = 0 ;
  bsMatrix->header.indexPos = sizeof(VlBlockSparseMatrixListHeader) ;

  return bsMatrix ; 
}


/*  */
VL_EXPORT 
VlBlockSparseMatrixListHeader*  vl_bsmatrix_list_add_column (VlBlockSparseMatrixListHeader *bsMatrix, VlBlockSparseArrayHeader *bsArray, vl_bool copy) 
{
  vl_uint32 allocatedMemory, *indexPos ; 

  allocatedMemory = vl_bsmatrix_allocated_memory((VlBlockSparseMatrixHeader*)bsMatrix);

  if (copy)
    {
      VlBlockSparseMatrixListHeader* temp ; 

      temp = (VlBlockSparseMatrixListHeader*)vl_malloc(sizeof(VlBlockSparseMatrixListHeader) + VL_MAX(allocatedMemory*2,allocatedMemory + bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) + sizeof(vl_uint32) )) ;

      memcpy(temp,bsMatrix,sizeof(VlBlockSparseMatrixHeader) + bsMatrix->header.byteDimension) ; 
      bsMatrix = temp ; 
    }
  else if (bsMatrix->header.byteDimension - allocatedMemory <  bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) + sizeof(vl_uint32) )
    {
      /* Aggressive choice, it reduces number of reallocations of memory */
      
      
      bsMatrix = (VlBlockSparseMatrixListHeader *)vl_realloc((void*)bsMatrix,sizeof(VlBlockSparseMatrixListHeader) + VL_MAX(allocatedMemory*2,allocatedMemory + bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) + sizeof(vl_uint32) ) ) ;

      bsMatrix->header.byteDimension =  VL_MAX(allocatedMemory*2,allocatedMemory + bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) + sizeof(vl_uint32)) ; 
      
    }

  if (bsMatrix->header.numColumns)
    {
      memcpy(((void*)bsMatrix) + bsMatrix->header.indexPos + bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader), ((void*)bsMatrix) + bsMatrix->header.indexPos, sizeof(vl_uint32)*bsMatrix->header.numColumns) ;
    }

  memcpy(((void*)bsMatrix) + bsMatrix->header.indexPos, bsArray, bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) ) ;

  

  indexPos = (vl_uint32*)(((void*)bsMatrix) + bsMatrix->header.indexPos + bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader)) ;
  
  
  indexPos[bsMatrix->header.numColumns] = bsMatrix->header.indexPos ;

  
  bsMatrix->header.numColumns++ ;

  bsMatrix->header.indexPos += bsArray->byteDimension + sizeof(VlBlockSparseArrayHeader) ;

  return bsMatrix ; 
}


VL_EXPORT
VlBlockSparseMatrixHeader*  vl_bsmatrix_finalise (VlBlockSparseMatrixHeader *bsMatrix) 
{
  vl_uint32 allocatedMemory ; 

  allocatedMemory = vl_bsmatrix_allocated_memory((VlBlockSparseMatrixHeader*)bsMatrix) ; 

  if (bsMatrix->byteDimension > allocatedMemory)
    {
      bsMatrix = (VlBlockSparseMatrixHeader *)vl_realloc((void*)bsMatrix,sizeof(VlBlockSparseMatrixHeader) + allocatedMemory) ;
      bsMatrix->byteDimension = allocatedMemory ; 
    }

  return bsMatrix ; 
}


/*  */
VL_EXPORT 
void vl_bsmatrix_delete (VlBlockSparseMatrixHeader* bsMatrix) 
{
  vl_free(bsMatrix) ;
}


/*  */
VL_EXPORT 
VlBlockSparseArrayHeader * vl_bsmatrix_get_column (const VlBlockSparseMatrixHeader* bsMatrix, vl_uint32 column) 
{
  vl_uint32 *indexPos ;

  if (column >= bsMatrix->numColumns)
    return NULL ;

  if (bsMatrix->matrixType == VL_BSMATRIX_LIST)
    {
      indexPos = (vl_uint32*)(((void*)bsMatrix) + bsMatrix->indexPos) ;

      return (VlBlockSparseArrayHeader*)(((void*)bsMatrix) + indexPos[column]) ; 
    }

  return NULL ; 
}


/*  */
VL_EXPORT 
double* vl_bsmatrix_full (VlBlockSparseMatrixHeader* bsMatrix) 
{

  double* output, *fullColumn ; 
  
  VlBlockSparseArrayHeader* column ; 

  vl_uint32 N, M, i ; 

  N = bsMatrix->numColumns ; 

  if ( N == 0)
    {
      output = (double*) vl_malloc(sizeof(double)) ; 

      *output = 0 ;

      return output ; 
    }
  
  column = vl_bsmatrix_get_column (bsMatrix, 0) ; 

  M = vl_bsarray_length(column) ;

  output = (double*) vl_calloc(M*N,sizeof(double)) ;

  if (bsMatrix->matrixType == VL_BSMATRIX_LIST)
    {
      for(i = 0; i < N; i++)
	{
	  column = vl_bsmatrix_get_column (bsMatrix, i) ;
	  fullColumn = vl_bsarray_full(column) ; 
	  memcpy(output,fullColumn,M*sizeof(double)) ;
	  vl_free(fullColumn) ; 
	}
    }
  
  return output ; 
}


VL_EXPORT
double vlSvmInnerProductFunctionBlockSparseMatrixList(const double* model, vl_size dimension,  const void* data, vl_size dataDimension, vl_uindex element, vlSvmFeatureMap mapFunc, const void * map)
{
  const VlBlockSparseMatrixHeader* bsMatrix = (const VlBlockSparseMatrixHeader*) data ; 
  
  VlBlockSparseArrayHeader * bsArray = vl_bsmatrix_get_column(bsMatrix,element) ;

  VlBlockHeader *block ; 

  double ret = 0 ; 
  
  vl_uint32 i,pos ;
 
  vl_size order = dimension / dataDimension ; 

  pos = sizeof(VlBlockSparseArrayHeader) ; 
 
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      block = (VlBlockHeader*) (((void*)bsArray) + pos) ; 
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  ret += innerProductDenseBlock(block, model,  block->position,order,mapFunc,map) ;
	  break ;
	case VL_BLOCK_CONSTANT :
	  ret += innerProductConstantBlock(block, model, block->position,order,mapFunc,map) ;
	  break ;
	case VL_BLOCK_SPARSE :
	  ret += innerProductSparseBlock(block, model, block->position, order,mapFunc,map) ;
	  break ;
	}
      pos += block->blockLength + sizeof(VlBlockHeader) ;
    }

  return ret ; 
} 

VL_EXPORT
void vlSvmAccumulatorFunctionBlockSparseMatrixList(VlSvm* svm,  vl_size dimension, const void* data, vl_size dataDimension, vl_uindex element, double multiplier, vlSvmFeatureMap mapFunc, const void * map)
{
  const VlBlockSparseMatrixHeader* bsMatrix = (const VlBlockSparseMatrixHeader*) data ; 
  
  VlBlockSparseArrayHeader * bsArray = vl_bsmatrix_get_column(bsMatrix,element) ;

  VlBlockHeader *block ; 

  
  vl_uint32 i,pos ;
 
  vl_size order = dimension / dataDimension ; 

  pos = sizeof(VlBlockSparseArrayHeader) ; 
 
  for ( i = 0; i < bsArray->numberOfBlocks ; i++ )
    {
      block = (VlBlockHeader*) (((void*)bsArray) + pos) ; 
      switch (block->blockType)
	{
	case VL_BLOCK_DENSE :
	  accumulatorDenseBlock(block, svm->model, block->position, svm->preConditioner, multiplier,order,mapFunc,map) ;
	  break ;
	case VL_BLOCK_CONSTANT :
	  accumulatorConstantBlock(block, svm->model, block->position, svm->preConditioner, multiplier,order,mapFunc,map) ;
	  break ;
	case VL_BLOCK_SPARSE :
	  accumulatorSparseBlock(block, svm->model, block->position, svm->preConditioner, multiplier,order,mapFunc,map) ;
	  break ;
	}
      pos += block->blockLength + sizeof(VlBlockHeader) ;
    }
    
  
}
    
  

