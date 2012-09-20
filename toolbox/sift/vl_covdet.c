/** @internal @file covdet.c
 ** @author Andrea Vedaldi
 ** @brief Scale Invariant Feature Transform (SIFT) - MEX
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/covdet2.h>
#include <vl/mathop.h>
#include <vl/sift.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_method = 0,
  opt_octave_resolution,
  opt_double_image,
  opt_peak_threshold,
  opt_edge_threshold,
  opt_estimate_orientation,
  opt_estimate_affine_shape,
  opt_frames,
  opt_descriptor,
  opt_patch_resolution,
  opt_patch_relative_smoothing,
  opt_patch_relative_extent,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
  {"Method",               1,   opt_method                 },
  {"OctaveResolution",     1,   opt_octave_resolution      },
  {"DoubleImage",          1,   opt_double_image           },
  {"PeakThreshold",        1,   opt_peak_threshold         },
  {"EdgeThreshold",        1,   opt_edge_threshold         },

  {"EstimateOrientation",  1,   opt_estimate_orientation   },
  {"EstimateAffineShape",  1,   opt_estimate_affine_shape  },

  {"Frames",               1,   opt_frames                 },

  {"Descriptor",            1,   opt_descriptor              },
  {"PatchResolution",       1,   opt_patch_resolution        },
  {"PatchRelativeExtent",   1,   opt_patch_relative_extent   },
  {"PatchRelativeSmoothing",1,   opt_patch_relative_smoothing},
  {"Verbose",               0,   opt_verbose                 },
  {0,                       0,   0                           }
} ;


/** @brief Types of feature frames */
typedef enum _VlCovDetDescritporType {
  VL_COVDET_DESC_NONE = 0,
  VL_COVDET_DESC_PATCH,
  VL_COVDET_DESC_SIFT,
  VL_COVDET_DESC_NUM
} VlCovDetDescriptorType ;

const char* vlCovDetDescriptorNames [VL_COVDET_DESC_NUM] =
{
  "None", "Patch", "SIFT"
} ;

VlEnumerator vlCovDetDescriptorTypes [VL_COVDET_DESC_NUM] =
{
  {"None" ,   (vl_index)VL_COVDET_DESC_NONE             },
  {"Patch",   (vl_index)VL_COVDET_DESC_PATCH            },
  {"SIFT",    (vl_index)VL_COVDET_DESC_SIFT             }
} ;

/** ------------------------------------------------------------------
 ** @brief Export scale space into a MATLAB structure
 ** @param ss Pointer to the scale space to be xported
 ** @return Pointer to matlab structure with the scale space
 **/
static mxArray *
_createArrayFromScaleSpace(VlScaleSpace const *ss)
{
  mxArray *data_array = NULL;
  vl_size numOctaves, numSubdivisions ;
  vl_index o ;

  if (ss == NULL) {
    return mxCreateDoubleMatrix(0,0,mxREAL);
  }

  VlScaleSpaceGeometry geom = vl_scalespace_get_geometry(ss) ;
  numOctaves = geom.lastOctave - geom.firstOctave + 1 ;
  numSubdivisions = geom.octaveLastSubdivision - geom.octaveFirstSubdivision + 1 ;

  data_array = mxCreateCellMatrix(1, numOctaves);
  for (o = geom.firstOctave ; o <= geom.lastOctave ; ++o) {
    vl_size w = vl_scalespace_get_octave_width(ss, o) ;
    vl_size h = vl_scalespace_get_octave_height(ss, o) ;
    float const * octave = vl_scalespace_get_octave(ss, o, geom.octaveFirstSubdivision) ;
    mwSize dims [3] = {w, h, numSubdivisions} ;
    mxArray * octave_array = mxCreateNumericArray(3, dims, mxSINGLE_CLASS, mxREAL) ;
    memcpy(mxGetData(octave_array), octave, w*h*numSubdivisions*sizeof(float)) ;
    mxSetCell(data_array, o - geom.firstOctave, octave_array) ;
  }

  {
    const char* names[] = {
      "firstOctave",
      "lastOctave",
      "octaveResolution",
      "octaveFirstSubdivision",
      "octaveLastSubdivision",
      "sigma0",
      "data" };
    mxArray * array = mxCreateStructMatrix(1, 1, 7, names) ;
    mxSetFieldByNumber(array, 0, 0, vlmxCreatePlainScalar(geom.firstOctave)) ;
    mxSetFieldByNumber(array, 0, 1, vlmxCreatePlainScalar(geom.lastOctave)) ;
    mxSetFieldByNumber(array, 0, 2, vlmxCreatePlainScalar(geom.octaveResolution)) ;
    mxSetFieldByNumber(array, 0, 3, vlmxCreatePlainScalar(geom.octaveFirstSubdivision)) ;
    mxSetFieldByNumber(array, 0, 4, vlmxCreatePlainScalar(geom.octaveLastSubdivision)) ;
    mxSetFieldByNumber(array, 0, 5, vlmxCreatePlainScalar(geom.sigma0)) ;
    mxSetFieldByNumber(array, 0, 6, data_array);
    return array ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Transpose desriptor
 ** @param dst destination buffer.
 ** @param src source buffer.
 **
 ** The function writes to @a dst the transpose of the SIFT descriptor
 ** @a src. The tranpsose is defined as the descriptor that one
 ** obtains from computing the normal descriptor on the transposed
 ** image.
 **/

static void
transpose_descriptor (float *dst, float const *src)
{
  int const BO = 8 ;  /* number of orientation bins */
  int const BP = 4 ;  /* number of spatial bins     */
  int i, j, t ;

  for (j = 0 ; j < BP ; ++j) {
    int jp = BP - 1 - j ;
    for (i = 0 ; i < BP ; ++i) {
      int o  = BO * i + BP*BO * j  ;
      int op = BO * i + BP*BO * jp ;
      dst [op] = src[o] ;
      for (t = 1 ; t < BO ; ++t)
        dst [BO - t + op] = src [t + o] ;
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_I = 0, IN_END} ;
  enum {OUT_FRAMES=0, OUT_DESCRIPTORS, OUT_INFO, OUT_END} ;

  int verbose = 0 ;
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;
  VlEnumerator *pair ;

  float const *image ;
  vl_size numCols, numRows ;

  VlCovDetMethod method = VL_COVDET_METHOD_DOG;
  vl_bool estimateAffineShape = VL_FALSE ;
  vl_bool estimateOrientation = VL_FALSE ;

  vl_bool doubleImage = VL_TRUE ;
  vl_size OctaveResolution = 3 ;
  double edgeThreshold = -1 ;
  double peakThreshold = -1 ;

  int descriptorType = -1 ;
  vl_index patchResolution = -1 ;
  double patchRelativeExtent = -1 ;
  double patchRelativeSmoothing = -1 ;
  float *patch = NULL ;
  float *patchXY = NULL ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < IN_END) {
    vlmxError(vlmxErrNotEnoughInputArguments, 0) ;
  } else if (nout > OUT_END) {
    vlmxError(vlmxErrTooManyOutputArguments, 0) ;
  }

  if (mxGetNumberOfDimensions(IN(I)) != 2 ||
      mxGetClassID(IN(I)) != mxSINGLE_CLASS){
    vlmxError(vlmxErrInvalidArgument, "I must be a matrix of class SINGLE.") ;
  }

  image = (float*) mxGetData(IN(I)) ;
  numRows = mxGetM(IN(I)) ;
  numCols = mxGetN(IN(I)) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

    case opt_verbose :
      ++ verbose ;
      break ;

    case opt_method:
      pair = vlmxDecodeEnumeration(optarg, vlCovdetMethods, VL_TRUE) ;
      if (pair == NULL) {
        vlmxError(vlmxErrInvalidArgument, "METHOD is not a supported detection method.") ;
      }
      method = (VlCovDetMethod)pair->value ;
      break;

      case opt_descriptor:
        pair = vlmxDecodeEnumeration(optarg, vlCovDetDescriptorTypes, VL_TRUE) ;
        if (pair == NULL) {
          vlmxError(vlmxErrInvalidArgument, "DESCRIPTOR is not a supported descriptor.") ;
        }
        descriptorType = (VlCovDetDescriptorType)pair->value ;
        break;

    case opt_estimate_affine_shape:
      if (!mxIsLogicalScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "ESTIMATEAFFINESHAPE must be a logical scalar value.") ;
      } else {
        estimateAffineShape = *mxGetLogicals(optarg) ;
      }
      break ;

    case opt_estimate_orientation:
      if (!mxIsLogicalScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "ESTIMATEORIENTATION must be a logical scalar value.") ;
      } else {
        estimateOrientation = *mxGetLogicals(optarg);
      }
      break ;

    case opt_edge_threshold :
      if (!vlmxIsPlainScalar(optarg) || (edgeThreshold = *mxGetPr(optarg)) < 1) {
        vlmxError(vlmxErrInvalidArgument, "EDGETHRESHOLD must be a real not smaller than 1.") ;
      }
      break ;

    case opt_peak_threshold :
      if (!vlmxIsPlainScalar(optarg) || (peakThreshold = *mxGetPr(optarg)) < 0) {
        vlmxError(vlmxErrInvalidArgument, "PEAKTHRESHOLD must be a non-negative real.") ;
      }
      break ;

    case opt_patch_relative_smoothing :
      if (!vlmxIsPlainScalar(optarg) || (patchRelativeSmoothing = *mxGetPr(optarg)) < 0) {
        vlmxError(vlmxErrInvalidArgument, "PATCHRELATIVESMOOTHING must be a non-negative real.") ;
      }
      break ;

    case opt_patch_relative_extent :
        if (!vlmxIsPlainScalar(optarg) || (patchRelativeExtent = *mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument, "PATCHRELATIVEEXTENT must be a posiive real.") ;
        }
        break ;

      case opt_patch_resolution :
        if (!vlmxIsPlainScalar(optarg) || (patchResolution = (vl_index)*mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument, "PATCHRESOLUTION must be a positive integer.") ;
        }
        break ;

    default :
      abort() ;
    }
  }

  if (descriptorType < 0) descriptorType = VL_COVDET_DESC_SIFT ;

  switch (descriptorType) {
    case VL_COVDET_DESC_NONE :
      break ;

    case VL_COVDET_DESC_PATCH :
      if (patchResolution < 0)  patchResolution = 20 ;
      if (patchRelativeExtent < 0) patchRelativeExtent = 3 ;
      if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 1 ;
      break ;

    case VL_COVDET_DESC_SIFT :
      if (patchResolution < 0)  patchResolution = 20 ;
      if (patchRelativeExtent < 0) patchRelativeExtent = 3 ;
      if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 1 ;
      break ;
  }

  if (patchResolution > 0) {
    vl_size w = 2*patchResolution + 1 ;
    patch = mxMalloc(sizeof(float) * w * w);
    patchXY = mxMalloc(2 * sizeof(float) * w * w);
  }

  /* -----------------------------------------------------------------
   *                                                          Detector
   * -------------------------------------------------------------- */
  {
    VlCovDet * covdet = vl_covdet_new(method) ;

    /* set covdet parameters */
    vl_covdet_set_transposed(covdet, VL_TRUE) ;
    if (peakThreshold >= 0) vl_covdet_set_peak_threshold(covdet, peakThreshold) ;
    if (edgeThreshold >= 0) vl_covdet_set_edge_threshold(covdet, edgeThreshold) ;

    /* process the image */
    vl_covdet_put_image(covdet, image, numRows, numCols) ;

    /* fill with frames: eitehr run the detector of poure them in */
    vl_covdet_detect(covdet) ;

    if (verbose) {
      vl_size numFrames = vl_covdet_get_num_features(covdet) ;
      mexPrintf("vl_covdet: detected %d features\n", numFrames) ;
    }

    /* affine adaptation if needed */
    if (estimateAffineShape) {
      if (verbose) {
        vl_size numFeaturesBefore = vl_covdet_get_num_features(covdet) ;
        mexPrintf("vl_covdet: estimating affine shape for %d features\n", numFeaturesBefore) ;
      }

      vl_covdet_extract_affine_shape(covdet) ;

      if (verbose) {
        vl_size numFeaturesAfter = vl_covdet_get_num_features(covdet) ;
        mexPrintf("vl_covdet: %d features passed affine adaptation\n", numFeaturesAfter) ;
      }
    }

    /* orientation estimation if needed */
    if (estimateOrientation) {
      if (verbose) {
        vl_size numFeaturesBefore = vl_covdet_get_num_features(covdet) ;
        mexPrintf("vl_covdet: estimating orientations for %d features\n", numFeaturesBefore) ;
      }

      vl_covdet_extract_orientations(covdet) ;

      if (verbose) {
        vl_size numFeaturesAfter = vl_covdet_get_num_features(covdet) ;
        mexPrintf("vl_covdet: %d features after estimating orientations\n", numFeaturesAfter) ;
      }

    }

    /* store results back */
    {
      vl_index i  ;
      vl_size numFrames = vl_covdet_get_num_features(covdet) ;
      VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
      double * pt ;

      OUT(FRAMES) = mxCreateDoubleMatrix (6, numFrames, mxREAL) ;
      pt = mxGetPr(OUT(FRAMES)) ;

      for (i = 0 ; i < (signed)numFrames ; ++i) {
        /* save the transposed frame, adjust origin to MATLAB's*/
        *pt++ = feature[i].frame.y + 1 ;
        *pt++ = feature[i].frame.x + 1 ;
        *pt++ = feature[i].frame.a22 ;
        *pt++ = feature[i].frame.a12 ;
        *pt++ = feature[i].frame.a21 ;
        *pt++ = feature[i].frame.a11 ;
      }
    }

    if (nout >= 2) {
      //      descriptorType = DESC_NONE;
      switch (descriptorType) {
        case VL_COVDET_DESC_NONE:
          OUT(DESCRIPTORS) = mxCreateDoubleMatrix(0,0,mxREAL);
          break ;

        case VL_COVDET_DESC_PATCH:
        {
          if (verbose) {
            mexPrintf("vl_covdet: descriptors: type=patch, "
                      "resolution=%d, extent=%g, smoothing=%g\n",
                      patchResolution, patchRelativeExtent,
                      patchRelativeSmoothing);
          }
          vl_size numFrames = vl_covdet_get_num_features(covdet) ;
          VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
          vl_index i ;
          vl_size w = 2*patchResolution + 1 ;
          float * desc ;
          OUT(DESCRIPTORS) = mxCreateNumericMatrix(w*w, numFrames, mxSINGLE_CLASS, mxREAL) ;
          desc = mxGetData(OUT(DESCRIPTORS)) ;
          for (i = 0 ; i < (signed)numFrames ; ++i) {
            vl_covdet_extract_patch_for_frame(covdet,
                                    desc,
                                    patchResolution,
                                    patchRelativeExtent,
                                    patchRelativeSmoothing,
                                    feature[i].frame) ;
            desc += w*w ;
          }
        }
        case VL_COVDET_DESC_SIFT:
        {
          if (verbose) {
            mexPrintf("vl_covdet: descriptors: type=sift, "
                      "resolution=%d, extent=%g, smoothing=%g\n",
                      patchResolution, patchRelativeExtent,
                      patchRelativeSmoothing);
          }
          vl_size numFrames = vl_covdet_get_num_features(covdet) ;
          VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
          VlSiftFilt * sift = vl_sift_new(16, 16, 1, 3, 0) ;
          vl_index i ;
          vl_size dimension = 128 ;
          vl_size patchSide = 2 * patchResolution + 1 ;
          float * desc ;
          OUT(DESCRIPTORS) = mxCreateNumericMatrix(dimension, numFrames, mxSINGLE_CLASS, mxREAL) ;
          desc = mxGetData(OUT(DESCRIPTORS)) ;
          for (i = 0 ; i < (signed)numFrames ; ++i) {
            vl_covdet_extract_patch_for_frame(covdet,
                                              patch,
                                              patchResolution,
                                              patchRelativeExtent,
                                              patchRelativeSmoothing,
                                              feature[i].frame) ;

            vl_imgradient_polar_f (patchXY, patchXY +1,
                                   2, 2 * patchSide,
                                   patch, patchSide, patchSide, patchSide) ;


            vl_sift_calc_raw_descriptor (sift,
                                         patchXY,
                                         desc,
                                         (int)patchSide, (int)patchSide,
                                         (double)patchSide / 2, (double)patchSide / 2,
                                         (double)patchSide / (3.0 * (4 + 1)),
                                         0) ;
            desc += dimension ;
          }
          vl_sift_delete(sift) ;
        }
      }
    }

    if (nout >= 3) {
      vl_index i ;
      vl_size numFrames = vl_covdet_get_num_features(covdet) ;
      VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
      const char* names[] = {
        "gss",
        "css",
        "peakScores",
        "edgeScores"
      };
      mxArray * gss_array = _createArrayFromScaleSpace(covdet->gss) ;
      mxArray * css_array = _createArrayFromScaleSpace(covdet->css) ;
      mxArray * peak_array = mxCreateNumericMatrix(1,numFrames,mxSINGLE_CLASS,mxREAL) ;
      mxArray * edge_array = mxCreateNumericMatrix(1,numFrames,mxSINGLE_CLASS,mxREAL) ;
      float * peak = mxGetData(peak_array) ;
      float * edge = mxGetData(edge_array) ;
      for (i = 0 ; i < (signed)numFrames ; ++i) {
        peak[i] = feature[i].peakScore ;
        edge[i] = feature[i].edgeScore ;
      }

      OUT(INFO) = mxCreateStructMatrix(1, 1, 4, names) ;
      mxSetFieldByNumber(OUT(INFO), 0, 0, gss_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 1, css_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 2, peak_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 3, edge_array) ;
    }
    /* cleanup */
    vl_covdet_delete (covdet) ;
  }

    if (patchXY) mxFree(patchXY) ;
  if (patch) mxFree(patch) ;
}
