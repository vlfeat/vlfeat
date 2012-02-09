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
  vl_uint32 numericType ;
  vl_uint32 pos ;
  vl_uint32 length;
} VlBlockHeader ;

/* Sparse Block Struct */
typedef struct _VlSparseBlockHeader {
  VlBlockHeader header ;
  vl_uint32 numberOfElements ; 
} VlSparseBlockHeader ;

/* Block Sparse Array Struct */
typedef struct _VlBlockSparseArrayHeader {
  vl_uint32 transpose ;
  vl_uint32 length ;
  vl_uint32 byteDimension ; 
  vl_uint32 numberOfBlocks ;
} VlBlockSparseArrayHeader ;
  
/* Block Sparse Array Iterator */
typedef struct _VlBlockSparseArrayIterator {
  vl_uindex currentPos ;
  VlBlockSparseArrayHeader* bsarray ;
} VlBlockSparseArrayIterator ;

  
/*  */
VL_EXPORT 
VlBlockSparseArrayHeader* vl_bsarray_new (vl_uint32 transpose, vl_uint32 numberOfBlocks, vl_uint32** data) ; 

/*  */
VL_EXPORT 
void vl_bsarray_delete (VlBlockSparseArrayHeader* bsarray) ; 

/*  */
VL_EXPORT 
VlBlockSparseArrayIterator* vl_bsarray_iterator_new (VlBlockSparseArrayHeader* bsarray) ; 

/*  */
VL_EXPORT 
void vl_bsarray_iterator_delete (VlBlockSparseArrayIterator* iter) ;

/*  */
VL_EXPORT 
VlBlockHeader* vl_bsarray_iterator_next (VlBlockSparseArrayIterator* iter) ; 

/*  */
VL_EXPORT 
double vl_bsarray_mtimes (VlBlockSparseArrayHeader* a, float* b ) ;

/* VL_BSARRAY_H */
#endif
