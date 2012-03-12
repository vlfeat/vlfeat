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
#include "svm_solver.h"


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
VlBlockSparseArrayHeader*  vl_bsarray_add_block (VlBlockSparseArrayHeader *bsArray,VlBlockHeader *block, vl_bool copy ) ; 

/* frees not used memory */
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
double vl_bsarray_mtimes (const VlBlockSparseArrayHeader* bsArray, const double* b ) ;



/* TODO */
//VL_EXPORT 
//double vl_bsarray_accumulate (VlBlockSparseArrayHeader* a, double b ) ;







/* Utility Functions */

/*  */
VL_EXPORT 
VlBlockHeader* getSparseBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type) ;

/*  */
VL_EXPORT
VlBlockHeader* getConstantBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type) ;

/*  */
VL_EXPORT
VlBlockHeader* getDenseBlock(const vl_uint32* array, vl_uindex M, vl_uindex position, vl_uint32 type) ;

/*  */
VL_EXPORT
vl_bool isSparseBlock (const vl_uint32* array, vl_uindex M) ;

/*  */
VL_EXPORT
vl_bool isConstantBlock (const vl_uint32* array, vl_uindex M) ;


/*  */
VL_EXPORT
vl_uint32 vl_bsarray_length(VlBlockSparseArrayHeader *bsArray) ;

/*  */
VL_EXPORT
VlBlockHeader* vl_bsarray_get_block (VlBlockSparseArrayHeader* bsArray, vl_uint32 pos) ; 

/*  */
VL_EXPORT
double innerProductSparseBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map)  ; 

/*  */
VL_EXPORT
double innerProductConstantBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map)  ; 

/*  */
VL_EXPORT
double innerProductDenseBlock(const VlBlockHeader* block, const double * array, vl_uint32 position, vl_size order, vlSvmFeatureMap mapFunc, const void * map) ;

/*  */
VL_EXPORT
void accumulatorSparseBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map) ;

/*  */
VL_EXPORT
void accumulatorConstantBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map)  ;

/*  */
VL_EXPORT
void accumulatorDenseBlock(const VlBlockHeader* block, double * array, vl_uint32 position, double * preConditioner, double multiplier, vl_size order, vlSvmFeatureMap mapFunc, const void * map) ;
/* VL_BSARRAY_H */
#endif
