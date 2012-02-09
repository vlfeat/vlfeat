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
#include <vl/lbp.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  float * image ;
  vl_size width, height ;
  vl_size cellSize = 16 ;
  enum {IN_I = 0, IN_CELLSIZE} ;
  enum {OUT_FEATURES = 0} ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin > 2) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nin < 2) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (! mxIsNumeric(IN(I)) ||
      ! vlmxIsReal(IN(I)) ||
      ! vlmxIsMatrix(IN(I), -1, -1)) {
    vlmxError(vlmxErrInvalidArgument,
              "I is not a numeric matrix.") ;
  }

  if (mxGetClassID(IN(I)) != mxSINGLE_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
              "I is not of class SINGLE.") ;
  }

  if (! vlmxIsPlainScalar(IN(CELLSIZE))) {
    vlmxError(vlmxErrInvalidArgument,
              "CELLSIZE is not a plain scalar.") ;
  }

  if (mxGetScalar(IN(CELLSIZE)) < 1.0) {
    vlmxError(vlmxErrInvalidArgument,
              "CELLSIZE is less than 1.") ;
  }

  cellSize = (vl_size) mxGetScalar(IN(CELLSIZE)) ;
  image = mxGetData(IN(I)) ;
  width = mxGetN(IN(I)) ;
  height = mxGetM(IN(I)) ;

  /* do job */
  {
    /* recall that MATLAB images are transposed */
    mwSize dimensions [3] ;

    /* get LBP object */
    VlLbp * lbp = vl_lbp_new (VlLbpUniform, VL_TRUE) ;
    if (lbp == NULL) {
      vlmxError(vlmxErrAlloc, NULL) ;
    }

    /* get output buffer */
    dimensions[0] = height / cellSize ;
    dimensions[1] = width / cellSize ;
    dimensions[2] = vl_lbp_get_dimension(lbp) ;

    OUT(FEATURES) = mxCreateNumericArray(3, dimensions, mxSINGLE_CLASS, mxREAL) ;

    vl_lbp_process(lbp, mxGetData(OUT(FEATURES)), image, height, width, cellSize) ;
    vl_lbp_delete(lbp) ;
  }
}
