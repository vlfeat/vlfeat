/** @file vl_slic.c
 ** @brief vl_slic
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/slic.h>

#include <assert.h>
#include <string.h>

/* option codes */
enum {
  opt_verbose,
  opt_min_segment_size
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"MinRegionSize",     1,   opt_min_segment_size    },
  {0,                   0,   0                       }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_IMAGE, IN_REGIONSIZE, IN_REGULARIZER, IN_END} ;
  enum {OUT_SEGMENTATION = 0} ;

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  float const * image ;
  vl_size width ;
  vl_size height ;
  vl_size numChannels ;
  vl_size regionSize ;
  double regularizer ;
  vl_uint32 * segmentation ;
  int minRegionSize = -1 ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError(vlmxErrInvalidArgument,
              "At least three arguments are required.") ;
  } else if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  image = mxGetData(IN(IMAGE)) ;
  if (!mxIsNumeric(IN(IMAGE)) || mxIsComplex(IN(IMAGE))) {
    vlmxError(vlmxErrInvalidArgument, "IMAGE is not a real matrix.") ;
  }
  if (mxGetClassID(IN(IMAGE)) != mxSINGLE_CLASS) {
    vlmxError(vlmxErrInvalidArgument, "IMAGE is not of class SINGLE.") ;
  }
  if (mxGetNumberOfDimensions(IN(IMAGE)) > 3) {
    vlmxError(vlmxErrInvalidArgument, "IMAGE has more than three dimensions.") ;
  }

  width = mxGetDimensions(IN(IMAGE))[1] ;
  height = mxGetDimensions(IN(IMAGE))[0] ;
  if (mxGetNumberOfDimensions(IN(IMAGE)) == 2) {
    numChannels = 1 ;
  } else {
    numChannels = mxGetDimensions(IN(IMAGE))[2] ;
  }

  if (!vlmxIsPlainScalar(IN(REGIONSIZE))) {
    vlmxError(vlmxErrInvalidArgument, "REGIONSIZE is not a plain scalar.") ;
  }
  regionSize = mxGetScalar(IN(REGIONSIZE)) ;
  if (regionSize < 1) {
    vlmxError(vlmxErrInvalidArgument, "REGIONSIZE=%d is smaller than one.", regionSize) ;
  }

  if (!vlmxIsPlainScalar(IN(REGULARIZER))) {
    vlmxError(vlmxErrInvalidArgument, "REGULARIZER is not a plain scalar.") ;
  }
  regularizer = mxGetScalar(IN(REGULARIZER)) ;
  if (regularizer < 0) {
    vlmxError(vlmxErrInvalidArgument, "REGULARIZER=%g is smaller than one.", regularizer) ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_verbose :
        ++ verbose ;
        break ;
      case opt_min_segment_size :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "MINREGIONSIZE is not a plain scalar.") ;
        }
        minRegionSize = mxGetScalar(optarg) ;
        if (minRegionSize < 0) {
          vlmxError(vlmxErrInvalidArgument, "MINREGIONSIZE=%d is smaller than zero.", minRegionSize) ;
        }
        break ;
    }
  }

  if (minRegionSize < 0) {
    minRegionSize = (regionSize * regionSize) / (6*6) ;
  }

  if (verbose) {
    mexPrintf("vl_slic: image = [%d x %d x %d]\n",
              width, height, numChannels) ;
    mexPrintf("vl_slic: regionSize = %d\n", regionSize) ;
    mexPrintf("vl_slic: regularizer = %g\n", regularizer) ;
    mexPrintf("vl_slic: minRegionSize = %d\n", minRegionSize) ;
  }

  /* -----------------------------------------------------------------
   *                                                           Do work
   * -------------------------------------------------------------- */

  OUT(SEGMENTATION) = mxCreateNumericMatrix((mwSize)height, (mwSize)width, mxUINT32_CLASS, mxREAL) ;
  segmentation = mxGetData(OUT(SEGMENTATION)) ;

  vl_slic_segment(segmentation,
                  image, height, width, numChannels, /* the image is transposed */
                  regionSize, regularizer, minRegionSize) ;
}
