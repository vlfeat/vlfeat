/** @internal @file vl_liop.c
 ** @brief Local Intensity Order Pattern (LIOP) descriptor - Definition
 ** @author Hana Sarbortova
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Hana Sarbortova and Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/liop.h>
#include <vl/mathop.h>

#include <assert.h>

/* option codes */
enum {
  opt_num_neighbours,
  opt_num_spatial_bins,
  opt_radius,
  opt_intensity_threshold,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
{"NumNeighbours",      1,   opt_num_neighbours       },
{"NumSpatialBins",     1,   opt_num_spatial_bins     },
{"Radius",             1,   opt_radius               },
{"IntensityThreshold", 1,   opt_intensity_threshold  },
{"Verbose",            0,   opt_verbose              },
{0,                    0,   0                        }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_END} ;
  enum {OUT_DESCRIPTOR = 0} ;

  int verbose = 0 ;
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  float *data ;
  vl_size M, N ;

  int numNeighbours = 4 ;
  int numSpatialBins = 6 ;
  float radius = 6.0 ;
  float intensityThreshold = VL_NAN_F ;
  vl_size numPatches = 1 ;
  mwSize const * dimensions ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  } else if (nout > 2) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (mxGetClassID(in[IN_I]) != mxSINGLE_CLASS ) {
    vlmxError(vlmxErrInvalidArgument,
              "I must be a matrix of class SINGLE.") ;
  }

  dimensions = mxGetDimensions(IN(I)) ;
  if(mxGetNumberOfDimensions(IN(I)) == 2){
    numPatches = 1 ;
  } else if (mxGetNumberOfDimensions(IN(I)) == 3) {
    numPatches = (vl_size) dimensions [2] ;
  } else {
    vlmxError(vlmxErrInvalidArgument,
              "I must be a matrix with 2 or 3 dimensions.") ;
  }

  data = (float*) mxGetData (in[IN_I]) ;
  M = (vl_size) dimensions[0] ;
  N = (vl_size) dimensions[1] ;

  if((N != M) || (N % 2 == 0)){
      vlmxError(vlmxErrInvalidArgument,
                "I is not square or does not have an odd side length.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

      case opt_verbose :
        ++ verbose ;
        break ;

      case opt_num_neighbours :
        if (!vlmxIsPlainScalar(optarg) || (numNeighbours = (int) *mxGetPr(optarg)) < 2) {
            vlmxError(vlmxErrInvalidArgument,"NUMNEIGHBOURS is not a scalar or it is less than two.") ;
        }
        break ;

      case opt_num_spatial_bins :
        if (!vlmxIsPlainScalar(optarg) || (numSpatialBins = (int) *mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument,"NUMSPATIALBINS is not a positive integer.") ;
        }
        break ;

      case opt_radius :
        if (!vlmxIsPlainScalar(optarg) || (radius = (int) *mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument,"RADIUS is not a positive scalar.") ;
        }
        break ;

      case opt_intensity_threshold :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument,"INTENSITYTHRESHOLD is not a scalar.") ;
        }
        intensityThreshold =  *mxGetPr(optarg) ;
        break ;

      default :
        abort() ;
    }
  }

  if (radius >= M / 2) {
    vlmxError(vlmxErrInconsistentData, "RADIUS is larger than half the width of I.") ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  {

    VlLiopDesc *liop ;
    float * desc ;
    vl_size dimension ;
    vl_index i ;

    liop = vl_liopdesc_new (numNeighbours, numSpatialBins, radius, M) ;
    if (!vl_is_nan_f(intensityThreshold)) {
     vl_liopdesc_set_intensity_threshold(liop, intensityThreshold) ;
    }
    dimension = vl_liopdesc_get_dimension(liop) ;

    out[OUT_DESCRIPTOR] = mxCreateNumericMatrix(dimension, numPatches, mxSINGLE_CLASS, mxREAL);
    desc = mxGetData(out[OUT_DESCRIPTOR]) ;

    for (i = 0 ; i < (signed)numPatches ; ++i) {
      vl_liopdesc_process(liop, desc + dimension * i, data + (M*M) * i) ;
    }

    if (verbose) {
      mexPrintf("vl_liop: image size: [W, H] = [%d, %d]\n", N, M) ;
      mexPrintf("vl_liop: num images: %d\n", numPatches) ;
      mexPrintf("vl_liop: num neighbours: %d\n", vl_liopdesc_get_num_neighbours(liop)) ;
      mexPrintf("vl_liop: num spatial bins: %d\n", vl_liopdesc_get_num_spatial_bins(liop)) ;
      mexPrintf("vl_liop: neighbourhood radius: %f\n", vl_liopdesc_get_neighbourhood_radius(liop)) ;
      mexPrintf("vl_liop: intensity threshold %f\n", vl_liopdesc_get_intensity_threshold(liop)) ;
      mexPrintf("vl_liop: descriptor dimension: %d\n", vl_liopdesc_get_dimension(liop)) ;
    }
  }
}
