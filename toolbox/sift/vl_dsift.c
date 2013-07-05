/** @internal
 ** @file     dsift.c
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (SIFT) - MEX
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/mathop.h>
#include <vl/dsift.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_step = 0,
  opt_bounds,
  opt_size,
  opt_fast,
  opt_norm,
  opt_window_size,
  opt_float_descriptors,
  opt_geometry,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
{"Bounds",           1,   opt_bounds           },
{"Step",             1,   opt_step             },
{"Size",             1,   opt_size             },
{"Fast",             0,   opt_fast             },
{"Norm",             0,   opt_norm             },
{"WindowSize",       1,   opt_window_size      },
{"FloatDescriptors", 0,   opt_float_descriptors},
{"Geometry",         1,   opt_geometry         },
{"Verbose",          0,   opt_verbose          },
{0,                  0,   0                    }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_END} ;
  enum {OUT_FRAMES=0, OUT_DESCRIPTORS} ;

  int verbose = 0 ;
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  float const *data ;
  int M, N ;

  int step [2] = {1,1} ;
  vl_bool norm = 0 ;

  vl_bool floatDescriptors = VL_FALSE ;
  vl_bool useFlatWindow = VL_FALSE ;
  double windowSize = -1.0 ;

  double *bounds = NULL ;
  double boundBuffer [4] ;
  VlDsiftDescriptorGeometry geom ;

  VL_USE_MATLAB_ENV ;

  geom.numBinX = 4 ;
  geom.numBinY = 4 ;
  geom.numBinT = 8 ;
  geom.binSizeX = 3 ;
  geom.binSizeY = 3 ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  } else if (nout > 2) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (mxGetNumberOfDimensions (in[IN_I]) != 2              ||
      mxGetClassID            (in[IN_I]) != mxSINGLE_CLASS ) {
    vlmxError(vlmxErrInvalidArgument,
              "I must be a matrix of class SINGLE.") ;
  }

  data = (float*) mxGetData (in[IN_I]) ;
  M    = mxGetM (in[IN_I]) ;
  N    = mxGetN (in[IN_I]) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

      case opt_verbose :
        ++ verbose ;
        break ;

      case opt_fast :
        useFlatWindow = 1 ;
        break ;

      case opt_norm :
        norm = 1 ;
        break ;

      case opt_bounds :
        if (!vlmxIsPlainVector(optarg, 4)) {
          mexErrMsgTxt("BOUNDS must be a 4-dimensional vector.") ;
        }
        bounds = boundBuffer ;
        bounds [0] = mxGetPr(optarg)[0] - 1 ;
        bounds [1] = mxGetPr(optarg)[1] - 1 ;
        bounds [2] = mxGetPr(optarg)[2] - 1 ;
        bounds [3] = mxGetPr(optarg)[3] - 1 ;
        break ;

      case opt_size :
        if (!vlmxIsPlainVector(optarg,-1)) {
          vlmxError(vlmxErrInvalidArgument,"SIZE is not a plain vector.") ;
        }
        if (mxGetNumberOfElements(optarg) == 1) {
          geom.binSizeX = (int) mxGetPr(optarg)[0] ;
          geom.binSizeY = (int) mxGetPr(optarg)[0] ;
        } else if (mxGetNumberOfElements(optarg) == 2) {
          geom.binSizeX = (int) mxGetPr(optarg)[1] ;
          geom.binSizeY = (int) mxGetPr(optarg)[0] ;
        } else {
          vlmxError(vlmxErrInvalidArgument,"SIZE is neither a scalar or a 2D vector.") ;
        }
        if (geom.binSizeX < 1 || geom.binSizeY < 1) {
          vlmxError(vlmxErrInvalidArgument,"SIZE value is invalid.") ;
        }
        break ;

      case opt_step :
        if (!vlmxIsPlainVector(optarg,-1)) {
          vlmxError(vlmxErrInvalidArgument,"STEP is not a plain vector.") ;
        }
        if (mxGetNumberOfElements(optarg) == 1) {
          step[0] = (int) mxGetPr(optarg)[0] ;
          step[1] = (int) mxGetPr(optarg)[0] ;
        } else if (mxGetNumberOfElements(optarg) == 2) {
          step[0] = (int) mxGetPr(optarg)[1] ;
          step[1] = (int) mxGetPr(optarg)[0] ;
        } else {
          vlmxError(vlmxErrInvalidArgument,"STEP is neither a scalar or a 2D vector.") ;
        }
        if (step[0] < 1 || step[1] < 1) {
          vlmxError(vlmxErrInvalidArgument,"STEP value is invalid.") ;
        }
        break ;

      case opt_window_size :
        if (!vlmxIsPlainScalar(optarg) || (windowSize = *mxGetPr(optarg)) < 0) {
          vlmxError(vlmxErrInvalidArgument,"WINDOWSIZE is not a scalar or it is negative.") ;
        }
        break ;

      case opt_float_descriptors :
        floatDescriptors = VL_TRUE ;
        break ;

      case opt_geometry :
        if (!vlmxIsPlainVector(optarg,3)) {
          vlmxError(vlmxErrInvalidArgument, "GEOMETRY is not a 3D vector.") ;
        }
        geom.numBinY = (int)mxGetPr(optarg)[0] ;
        geom.numBinX = (int)mxGetPr(optarg)[1] ;
        geom.numBinT = (int)mxGetPr(optarg)[2] ;
        if (geom.numBinX < 1 ||
            geom.numBinY < 1 ||
            geom.numBinT < 1) {
          vlmxError(vlmxErrInvalidArgument, "GEOMETRY value is invalid.") ;
        }
        break ;

      default :
        abort() ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  {
    int numFrames ;
    int descrSize ;
    VlDsiftKeypoint const *frames ;
    float const *descrs ;
    int k, i ;

    VlDsiftFilter *dsift ;

    /* note that the image received from MATLAB is transposed */
    dsift = vl_dsift_new (M, N) ;
    vl_dsift_set_geometry(dsift, &geom) ;
    vl_dsift_set_steps(dsift, step[0], step[1]) ;

    if (bounds) {
      vl_dsift_set_bounds(dsift,
                          VL_MAX(bounds[1], 0),
                          VL_MAX(bounds[0], 0),
                          VL_MIN(bounds[3], M - 1),
                          VL_MIN(bounds[2], N - 1));
    }
    vl_dsift_set_flat_window(dsift, useFlatWindow) ;

    if (windowSize >= 0) {
      vl_dsift_set_window_size(dsift, windowSize) ;
    }

    numFrames = vl_dsift_get_keypoint_num (dsift) ;
    descrSize = vl_dsift_get_descriptor_size (dsift) ;
    geom = *vl_dsift_get_geometry (dsift) ;

    if (verbose) {
      int stepX ;
      int stepY ;
      int minX ;
      int minY ;
      int maxX ;
      int maxY ;
      vl_bool useFlatWindow ;

      vl_dsift_get_steps (dsift, &stepY, &stepX) ;
      vl_dsift_get_bounds (dsift, &minY, &minX, &maxY, &maxX) ;
      useFlatWindow = vl_dsift_get_flat_window(dsift) ;

      mexPrintf("vl_dsift: image size         [W, H] = [%d, %d]\n", N, M) ;
      mexPrintf("vl_dsift: bounds:            [minX,minY,maxX,maxY] = [%d, %d, %d, %d]\n",
                minX+1, minY+1, maxX+1, maxY+1) ;
      mexPrintf("vl_dsift: subsampling steps: stepX=%d, stepY=%d\n", stepX, stepY) ;
      mexPrintf("vl_dsift: num bins:          [numBinT, numBinX, numBinY] = [%d, %d, %d]\n",
                geom.numBinT,
                geom.numBinX,
                geom.numBinY) ;
      mexPrintf("vl_dsift: descriptor size:   %d\n", descrSize) ;
      mexPrintf("vl_dsift: bin sizes:         [binSizeX, binSizeY] = [%d, %d]\n",
                geom.binSizeX,
                geom.binSizeY) ;
      mexPrintf("vl_dsift: flat window:       %s\n", VL_YESNO(useFlatWindow)) ;
      mexPrintf("vl_dsift: window size:       %g\n", vl_dsift_get_window_size(dsift)) ;
      mexPrintf("vl_dsift: num of features:   %d\n", numFrames) ;
    }

    vl_dsift_process (dsift, data) ;

    frames = vl_dsift_get_keypoints (dsift) ;
    descrs = vl_dsift_get_descriptors (dsift) ;

    /* ---------------------------------------------------------------
     *                                            Create output arrays
     * ------------------------------------------------------------ */
    {
      mwSize dims [2] ;

      dims [0] = descrSize ;
      dims [1] = numFrames ;

      if (floatDescriptors) {
        out[OUT_DESCRIPTORS] = mxCreateNumericArray
        (2, dims, mxSINGLE_CLASS, mxREAL) ;
      } else {
        out[OUT_DESCRIPTORS] = mxCreateNumericArray
        (2, dims, mxUINT8_CLASS, mxREAL) ;
      }

      dims [0] = norm ? 3 : 2 ;

      out[OUT_FRAMES] = mxCreateNumericArray
      (2, dims, mxDOUBLE_CLASS, mxREAL) ;
    }

    /* ---------------------------------------------------------------
     *                                                       Copy back
     * ------------------------------------------------------------ */
    {
      float *tmpDescr = mxMalloc(sizeof(float) * descrSize) ;
      double *outFrameIter = mxGetPr(out[OUT_FRAMES]) ;
      void *outDescrIter = mxGetData(out[OUT_DESCRIPTORS]) ;
      for (k = 0 ; k < numFrames ; ++k) {
        *outFrameIter++ = frames[k].y + 1 ;
        *outFrameIter++ = frames[k].x + 1 ;

        /* We have an implied / 2 in the norm, because of the clipping
           below */
        if (norm)
          *outFrameIter++ = frames [k].norm ;

        vl_dsift_transpose_descriptor (tmpDescr,
                                       descrs + descrSize * k,
                                       geom.numBinT,
                                       geom.numBinX,
                                       geom.numBinY) ;

        if (floatDescriptors) {
          for (i = 0 ; i < descrSize ; ++i) {
            float * pt = (float*) outDescrIter ;
            *pt++ = VL_MIN(512.0F * tmpDescr[i], 255.0F) ;
            outDescrIter = pt ;
          }
        } else {
          for (i = 0 ; i < descrSize ; ++i) {
            vl_uint8 * pt = (vl_uint8*) outDescrIter ;
            *pt++ = (vl_uint8) (VL_MIN(512.0F * tmpDescr[i], 255.0F)) ;
            outDescrIter = pt ;

          }
        }
      }
      mxFree(tmpDescr) ;
    }
    vl_dsift_delete (dsift) ;
  }
}
