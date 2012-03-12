/** @file bsmatrix.h
 ** @brief Block Sparse matrix 
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/

#ifndef VL_BSMATRIX_H
#define VL_BSMATRIX_H

#include "generic.h"
#include "bsarray.h"
#include "svm_solver.h"

#define VL_BSMATRIX_LIST 1
#define VL_BSMATRIX_COMPACT 2


/* Block Sparse Matrix Struct */
typedef struct _VlBlockSparseMatrixHeader {
  vl_uint32 matrixType ;
  vl_uint32 byteDimension ; 
  vl_uint32 numColumns ;
  vl_uint32 indexPos ; 
} VlBlockSparseMatrixHeader ;

/* Block Sparse Matrix List Struct */
typedef struct _VlBlockSparseMatrixListHeader {
  VlBlockSparseMatrixHeader header ;
} VlBlockSparseMatrixListHeader ;

/* Block Sparse Matrix Compact Struct */
typedef struct _VlBlockSparseMatrixCompactHeader {
  VlBlockSparseMatrixHeader header ;
  VlBlockSparseArrayHeader columnHeader ; 
} VlBlockSparseMatrixCompactHeader ;


/* VlBlockSparseMatrix Public Functions */
/*  */
VL_EXPORT 
VlBlockSparseMatrixListHeader* vl_bsmatrix_list_new (vl_uint32 dataByteDimension ) ; 

/*  */
VL_EXPORT 
VlBlockSparseMatrixListHeader*  vl_bsmatrix_list_add_column (VlBlockSparseMatrixListHeader *bsMatrix, VlBlockSparseArrayHeader *bsArray,vl_bool copy) ; 

/* frees not used memory */
VL_EXPORT
VlBlockSparseMatrixHeader*  vl_bsmatrix_finalise (VlBlockSparseMatrixHeader *bsMatrix) ;

/*  */
VL_EXPORT 
void vl_bsmatrix_delete (VlBlockSparseMatrixHeader* bsMatrix) ; 

/*  */
VL_EXPORT 
double* vl_bsmatrix_full (VlBlockSparseMatrixHeader* bsMatrix) ; 

/*  */
VL_EXPORT 
VlBlockSparseArrayHeader * vl_bsmatrix_get_column (const VlBlockSparseMatrixHeader* bsMatrix, vl_uint32 column) ;


/* SVM */

VL_EXPORT
double vlSvmInnerProductFunctionBlockSparseMatrixList(const double* model, vl_size dimension,  const void* data, vl_size dataDimension, vl_uindex element, vlSvmFeatureMap mapFunc, const void * map) ; 

VL_EXPORT
void vlSvmAccumulatorFunctionBlockSparseMatrixList(VlSvm* svm,  vl_size dimension, const void* data, vl_size dataDimension, vl_uindex element, double multiplier, vlSvmFeatureMap mapFunc, const void * map) ;


/* Utility Functions */

VL_INLINE
vl_uint32 vl_bsmatrix_allocated_memory(VlBlockSparseMatrixHeader *bsMatrix) 
{
  vl_uint32 i, pos ;

  VlBlockSparseArrayHeader *tempArray ;

  
  vl_uint32 allocatedMemory  = bsMatrix->numColumns*sizeof(vl_uint32) ;

  
  
  if (bsMatrix->matrixType == VL_BSMATRIX_LIST)
    {
      pos = sizeof(VlBlockSparseMatrixListHeader) ; 

      for ( i = 0; i < bsMatrix->numColumns ; i++ )
	{
	  tempArray = (VlBlockSparseArrayHeader*) (((void*)bsMatrix) + pos) ;
	  allocatedMemory += tempArray->byteDimension + sizeof(VlBlockSparseArrayHeader) ;
	  
	  pos += tempArray->byteDimension + sizeof(VlBlockSparseArrayHeader) ;
	}
    }
  return allocatedMemory ; 
}

/* VL_BSMATRIX_H */
#endif
