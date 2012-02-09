/** @internal
 ** @file    vl_imdisttf.c
 ** @brief   vl_imdisttf - MEX definition
 ** @author  Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/mathop.h>
#include <vl/imopv.h>

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  vl_size M, N ;
  enum {IN_I = 0, IN_PARAM, IN_END} ;
  enum {OUT_DT = 0, OUT_INDEXES} ;
  vl_uindex * indexes = NULL ;
  mxClassID classId ;
  double const defaultParam [] = {1.0, 0.0, 1.0, 0.0} ;
  double const * param = defaultParam ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nin > 2) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }
  if (nout > 2) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }
  classId = mxGetClassID(IN(I)) ;
  if (! vlmxIsMatrix(IN(I), -1, -1) ||
      (classId != mxSINGLE_CLASS && classId != mxDOUBLE_CLASS)) {
    vlmxError(vlmxErrInvalidArgument,
             "I is not a SINGLE or DOUBLE matrix.") ;
  }
  if (nin == 2) {
    if (! vlmxIsPlainVector(IN(PARAM), 4)) {
      vlmxError(vlmxErrInvalidArgument,
               "PARAM is not a 4-dimensional vector.") ;
    }
    param = mxGetPr (IN(PARAM)) ;
    if (param[0] < 0.0 ||
        param[2] < 0.0) {
      vlmxError(vlmxErrInvalidArgument,
                "Either PARAM[0] or PARAM[2] is negative.") ;
    }
  }

  M = mxGetM (IN(I)) ;
  N = mxGetN (IN(I)) ;

  OUT(DT) = mxCreateNumericMatrix (M, N, classId, mxREAL) ;
  if (nout > 1) {
    vl_uindex i ;
    OUT(INDEXES) = mxCreateDoubleMatrix (M, N, mxREAL) ;
    indexes = mxMalloc(sizeof(vl_uindex) * M * N) ;
    for (i = 0 ; i < M * N ; ++i) indexes[i] = i + 1 ;
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  switch (classId) {
    case mxSINGLE_CLASS:
      vl_image_distance_transform_f((float const*)mxGetData(IN(I)),
                                    M, N,
                                    1, M,
                                    (float*)mxGetPr(OUT(DT)),
                                    indexes,
                                    param[2],
                                    param[3]) ;

      vl_image_distance_transform_f((float*)mxGetPr(OUT(DT)),
                                    N, M,
                                    M, 1,
                                    (float*)mxGetPr(OUT(DT)),
                                    indexes,
                                    param[0],
                                    param[1]) ;
      break ;

    case mxDOUBLE_CLASS:
      vl_image_distance_transform_d((double const*)mxGetData(IN(I)),
                                    M, N,
                                    1, M,
                                    (double*)mxGetPr(OUT(DT)),
                                    indexes,
                                    param[2],
                                    param[3]) ;

      vl_image_distance_transform_d((double*)mxGetPr(OUT(DT)),
                                    N, M,
                                    M, 1,
                                    (double*)mxGetPr(OUT(DT)),
                                    indexes,
                                    param[0],
                                    param[1]) ;
      break;

    default:
      abort() ;
  }

  if (indexes) {
    vl_uindex i ;
    double * pt = mxGetPr(OUT(INDEXES)) ;
    for (i = 0 ; i < M * N ; ++i) pt[i] = indexes[i] ;
    mxFree(indexes) ;
  }
}
