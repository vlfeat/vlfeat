/** @internal
 ** @file     binsum.c
 ** @author   Andrea Vedaldi
 ** @brief    vl_binsum.c - MEX definition
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>

#define REPEAT1_1(m,p) m(1,p)
#define REPEAT1_2(m,p) REPEAT1_1(m,p) m(2,p)
#define REPEAT1_3(m,p) REPEAT1_2(m,p) m(3,p)
#define REPEAT1_4(m,p) REPEAT1_3(m,p) m(4,p)
#define REPEAT1_5(m,p) REPEAT1_4(m,p) m(5,p)
#define REPEAT1_6(m,p) REPEAT1_5(m,p) m(6,p)
#define REPEAT1_7(m,p) REPEAT1_6(m,p) m(7,p)
#define REPEAT1_8(m,p) REPEAT1_7(m,p) m(8,p)
#define REPEAT1_9(m,p) REPEAT1_8(m,p) m(9,p)
#define REPEAT1_10(m,p) REPEAT1_9(m,p) m(10,p)

#define REPEAT2_1(m,p) m(1,p)
#define REPEAT2_2(m,p) REPEAT2_1(m,p) m(2,p)
#define REPEAT2_3(m,p) REPEAT2_2(m,p) m(3,p)
#define REPEAT2_4(m,p) REPEAT2_3(m,p) m(4,p)
#define REPEAT2_5(m,p) REPEAT2_4(m,p) m(5,p)
#define REPEAT2_6(m,p) REPEAT2_5(m,p) m(6,p)
#define REPEAT2_7(m,p) REPEAT2_6(m,p) m(7,p)
#define REPEAT2_8(m,p) REPEAT2_7(m,p) m(8,p)
#define REPEAT2_9(m,p) REPEAT2_8(m,p) m(9,p)
#define REPEAT2_10(m,p) REPEAT2_9(m,p) m(10,p)

#define TYPE_1 double
#define TYPE_2 float
#define TYPE_3 vl_int64
#define TYPE_4 vl_uint64
#define TYPE_5 vl_int32
#define TYPE_6 vl_uint32
#define TYPE_7 vl_int16
#define TYPE_8 vl_uint16
#define TYPE_9 vl_int8
#define TYPE_10 vl_uint8
#define TYPE_AUX(x) TYPE_ ## x
#define TYPE(x) TYPE_AUX(x)

#define CLASS_1  DOUBLE
#define CLASS_2  SINGLE
#define CLASS_3  INT64
#define CLASS_4  UINT64
#define CLASS_5  INT32
#define CLASS_6  UINT32
#define CLASS_7  INT16
#define CLASS_8  UINT16
#define CLASS_9  INT8
#define CLASS_10 UINT8
#define CLASS_AUX(x) CLASS_ ## x
#define CLASS(x) CLASS_AUX(x)

#include "vl_binsum.def"

/* ---------------------------------------------------------------- */
/* */
/* ---------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_ACCUMULATOR = 0, IN_VALUES, IN_INDEXES, IN_DIM} ;
  enum {OUT_ACCUMULATOR = 0} ;
  mxClassID valueClass ;
  mxClassID indexClass ;
  vl_index dim = 0 ;

  if (nin < 3) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nin > 4) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (!vlmxIsReal(IN(ACCUMULATOR))) {
    vlmxError(vlmxErrInvalidArgument, "ACCUMULATOR is not a numeric real array.") ;
  }
  if (!vlmxIsReal(IN(VALUES))) {
    vlmxError(vlmxErrInvalidArgument, "VALUES is not a numeric real array.") ;
  }
  if (!vlmxIsReal(IN(INDEXES))) {
    vlmxError(vlmxErrInvalidArgument, "INDEXES is not a numeric real array.") ;
  }

  indexClass = mxGetClassID(IN(INDEXES)) ;
  valueClass = mxGetClassID(IN(ACCUMULATOR)) ;

  if (valueClass != mxGetClassID(IN(VALUES))) {
    vlmxError(vlmxErrInvalidArgument, "ACCUMULATOR and VALUES do not have the same storage class.") ;
  }

  if (nin > 3) {
    if (!vlmxIsPlainScalar(IN(DIM))) {
      vlmxError(vlmxErrInvalidArgument, "DIM is not a plain scalar.") ;
    }
    dim = (vl_index)mxGetScalar(IN(DIM)) ;
  }

  out[0] = mxDuplicateArray(IN(ACCUMULATOR)) ;

#define DISPATCH(I,V) \
case VL_XCAT3(mx,CLASS(I),_CLASS): \
VL_XCAT4(_vl_binsum_,TYPE(V),_,TYPE(I))(out[0], IN(VALUES), IN(INDEXES), dim) ; break ;

#define DISPATCH_VALUE(V,_) \
  case VL_XCAT3(mx,CLASS(V),_CLASS) : \
    switch (indexClass) { \
      REPEAT2_10(DISPATCH,V) ; \
    default: \
      vlmxError(vlmxErrInvalidArgument, "INDEXES is not of one of the supported storage classes.") ; \
  } \
  break ;

  switch (valueClass) {
      REPEAT1_10(DISPATCH_VALUE,_) ;
    default:
      vlmxError(vlmxErrInvalidArgument, "ACCUMULATOR is not of one of the supported storage classes.") ;
  }
}
