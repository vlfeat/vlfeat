/** @file vl_cummax.c
 ** @brief vl_cummax MEX defition
 ** @author Andrea Vedaldi
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

#include "vl_cummax.def"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_X, IN_DIM, IN_END} ;
  enum {OUT_Y = 0} ;
  vl_index whichDimension = 0 ;
  vl_size dimension ;
  vl_size numDimensions ;
  mwSize const * dimensions ;
  mxClassID classId ;
  vl_size stride = 1 ;
  vl_index k ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  } else if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  } else if (nin > 2) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }

  /* get information on the data */
  if (!mxIsNumeric(IN(X))) {
    vlmxError(vlmxErrInvalidArgument, "X is not a numeric array.") ;
  }
  classId = mxGetClassID(IN(X)) ;
  numDimensions = mxGetNumberOfDimensions(IN(X)) ;
  dimensions = mxGetDimensions(IN(X)) ;

  /* get the operating dimension */
  if (nin == 2) {
    if (!vlmxIsPlainScalar(IN(DIM))) {
      vlmxError(vlmxErrInvalidArgument, "DIM is not a plain scalar.") ;
    }
    whichDimension = mxGetScalar(IN(DIM)) ;
    if (whichDimension < 1 || whichDimension > (signed)numDimensions) {
      vlmxError(vlmxErrInvalidArgument,
                "DIM=%d is not a valid dimension of X.", whichDimension) ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  OUT(Y) = mxCreateNumericArray(numDimensions, dimensions, classId, mxREAL) ;

  if (whichDimension >= 1) {
    /*
      Compute the stride to whichDimension.
    */
    for (k = 0 ; k < whichDimension - 1 ; ++k) {
      stride *= dimensions[k] ;
    }
  } else {
    /*
      Pick the first non-singleton dimension, if any. Note that in
      this case the stride is equal to 1.
    */
    for (k = 0 ; k < (signed)numDimensions-1 && dimensions[k] == 1 ; ++k) ;
    whichDimension = k + 1 ;
  }
  dimension = dimensions[whichDimension - 1] ;

#define DISPATCH(I,V) \
case VL_XCAT3(mx,CLASS(I),_CLASS): \
  VL_XCAT(_vl_cummax_,TYPE(I))(OUT(Y), IN(X), stride, dimension) ; break ;

  switch (classId) {
    REPEAT1_10(DISPATCH,_) ;
  default :
      vlmxError(vlmxErrInvalidArgument, "X is not of one of the supported storage classes.") ;
  }
}
