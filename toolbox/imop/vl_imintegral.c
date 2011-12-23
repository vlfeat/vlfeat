/** @internal
 ** @file    imintegral.c
 ** @author  Brian Fulkerson
 ** @brief   Create an integral image - MEX definition
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/imopv.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  vl_size numDimensions, numChannels ;
  mwSize const *dimensions ;
  mxClassID classId ;
  void * integral ;
  void const * image ;
  vl_uindex k ;
  enum {IN_I=0} ;
  enum {OUT_J=0} ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin > 1) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (! mxIsNumeric(IN(I))) {
    vlmxError(vlmxErrInvalidArgument,
              "I is not numeric.") ;
  }

  dimensions = mxGetDimensions(IN(I)) ;
  numDimensions = mxGetNumberOfDimensions(IN(I)) ;
  if (numDimensions > 3) {
    vlmxError(vlmxErrInvalidArgument,
              "I has more than 3 dimensions (%d).", numDimensions) ;
  }
  if (numDimensions > 2) {
    numChannels = dimensions [2] ;
  } else {
    numChannels = 1 ;
  }

  classId = mxGetClassID(IN(I)) ;
  if (classId != mxSINGLE_CLASS &&
      classId != mxDOUBLE_CLASS &&
      classId != mxUINT32_CLASS &&
      classId != mxINT32_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
              "I is not of a supported storage class.") ;
  }

  OUT(J) = mxCreateNumericArray(numDimensions, dimensions,
                                classId, mxREAL) ;

  image = mxGetData(IN(I)) ;
  integral = mxGetData(OUT(J)) ;

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

#define DO(CLASS, T, SFX) \
case mx ## CLASS ## _CLASS : \
vl_imintegral_ ## SFX (integral, dimensions[0], \
  image, dimensions[0], dimensions[1], dimensions[0]) ; \
  integral = ((T *) integral) + dimensions[0]*dimensions[1] ; \
  image = ((T const *) image) + dimensions[0]*dimensions[1] ; \
break

  for (k = 0 ; k < numChannels ; ++k) {
    switch (classId) {
        DO(SINGLE, float, f) ;
        DO(DOUBLE, double, d) ;
        DO(UINT32, vl_uint32, ui32) ;
        DO(INT32, vl_int32, i32) ;
      default:
        abort() ;
    }
  }
}
