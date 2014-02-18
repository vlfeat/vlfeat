/** @internal @file covdet.c
 ** @brief Covariant Detector - MEX
 ** @author Karel Lenc
 ** @author Andrea Vedaldi
 ** @author Michal Perdoch
 **/

/*
Copyright (C) 2007-12 Karel Lencl, Andrea Vedaldi and Michal Perdoch.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/covdet.h>
#include <vl/mathop.h>
#include <vl/sift.h>
#include <vl/liop.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_method = 0,
  opt_octave_resolution,
  opt_double_image,
  opt_peak_threshold,
  opt_edge_threshold,
  opt_laplacian_peak_threshold,
  opt_estimate_orientation,
  opt_estimate_affine_shape,
  opt_frames,
  opt_descriptor,
  opt_liop_bins,
  opt_liop_neighbours,
  opt_liop_threshold,
  opt_liop_radius,
  opt_patch_resolution,
  opt_patch_relative_smoothing,
  opt_patch_relative_extent,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
  {"Method",                1,   opt_method                  },
  {"OctaveResolution",      1,   opt_octave_resolution       },
  {"DoubleImage",           1,   opt_double_image            },
  {"PeakThreshold",         1,   opt_peak_threshold          },
  {"EdgeThreshold",         1,   opt_edge_threshold          },
  {"LaplacianPeakThreshold",1,   opt_laplacian_peak_threshold},

  {"EstimateOrientation",   1,   opt_estimate_orientation    },
  {"EstimateAffineShape",   1,   opt_estimate_affine_shape   },

  {"Frames",                1,   opt_frames                  },

  {"Descriptor",            1,   opt_descriptor              },
  {"LiopNumSpatialBins",    1,   opt_liop_bins               },
  {"LiopNumNeighbours",     1,   opt_liop_neighbours         },
  {"LiopIntensityThreshold",1,   opt_liop_threshold          },
  {"LiopRadius",            1,   opt_liop_radius             },
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
  VL_COVDET_DESC_LIOP,
  VL_COVDET_DESC_NUM
} VlCovDetDescriptorType ;

const char* vlCovDetDescriptorNames [VL_COVDET_DESC_NUM] =
{
    "None", "Patch", "SIFT","LIOP"
} ;

VlEnumerator vlCovDetDescriptorTypes [VL_COVDET_DESC_NUM] =
{
  {"None" ,   (vl_index)VL_COVDET_DESC_NONE             },
  {"Patch",   (vl_index)VL_COVDET_DESC_PATCH            },
  {"SIFT",    (vl_index)VL_COVDET_DESC_SIFT             },
  {"LIOP",    (vl_index)VL_COVDET_DESC_LIOP             }
} ;

/** ------------------------------------------------------------------
 ** @brief Export scale space into a MATLAB structure
 ** @param ss Pointer to the scale space to be exported
 ** @return Pointer to matlab structure with the scale space
 **/
static mxArray *
_createArrayFromScaleSpace(VlScaleSpace const *ss)
{
  mxArray *data_array = NULL;
  vl_size numOctaves, numSubdivisions ;
  vl_index o ;
  VlScaleSpaceGeometry geom ;

  if (ss == NULL) {
    return mxCreateDoubleMatrix(0,0,mxREAL);
  }

  geom = vl_scalespace_get_geometry(ss) ;

  numOctaves = geom.lastOctave - geom.firstOctave + 1 ;
  numSubdivisions = geom.octaveLastSubdivision - geom.octaveFirstSubdivision + 1 ;

  data_array = mxCreateCellMatrix(1, numOctaves);
  for (o = geom.firstOctave ; o <= geom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry oct = vl_scalespace_get_octave_geometry(ss, o) ;
    float const * octave = vl_scalespace_get_level_const(ss, o, geom.octaveFirstSubdivision) ;
    mwSize dims [3] = {oct.width, oct.height, numSubdivisions} ;
    mxArray * octave_array = mxCreateNumericArray(3, dims, mxSINGLE_CLASS, mxREAL) ;
    memcpy(mxGetData(octave_array),
           octave, oct.width * oct.height * numSubdivisions * sizeof(float)) ;
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
    mxSetFieldByNumber(array, 0, 0, vlmxCreatePlainScalar((double)geom.firstOctave)) ;
    mxSetFieldByNumber(array, 0, 1, vlmxCreatePlainScalar((double)geom.lastOctave)) ;
    mxSetFieldByNumber(array, 0, 2, vlmxCreatePlainScalar((double)geom.octaveResolution)) ;
    mxSetFieldByNumber(array, 0, 3, vlmxCreatePlainScalar((double)geom.octaveFirstSubdivision)) ;
    mxSetFieldByNumber(array, 0, 4, vlmxCreatePlainScalar((double)geom.octaveLastSubdivision)) ;
    mxSetFieldByNumber(array, 0, 5, vlmxCreatePlainScalar(geom.baseScale)) ;
    mxSetFieldByNumber(array, 0, 6, data_array);
    return array ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Transpose descriptor
 ** @param dst destination buffer.
 ** @param src source buffer.
 **
 ** The function writes to @a dst the transpose of the SIFT descriptor
 ** @a src. The transpose is defined as the descriptor that one
 ** obtains from computing the normal descriptor on the transposed
 ** image.
 **/

static void
flip_descriptor (float *dst, float const *src)
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
  vl_index octaveResolution = -1 ;
  double edgeThreshold = -1 ;
  double peakThreshold = -1 ;
  double lapPeakThreshold = -1 ;

  int descriptorType = -1 ;
  vl_index patchResolution = -1 ;
  double patchRelativeExtent = -1 ;
  double patchRelativeSmoothing = -1 ;
  float *patch = NULL ;
  float *patchXY = NULL ;

  vl_int liopNumSpatialBins = 6;
  vl_int liopNumNeighbours = 4;
  float liopRadius = 6.0;
  float liopIntensityThreshold = VL_NAN_F ;

  double boundaryMargin = 2.0 ;

  double * userFrames = NULL ;
  vl_size userFrameDimension = 0 ;
  vl_size numUserFrames = 0 ;

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

    case opt_double_image:
      if (!mxIsLogicalScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "DOUBLEIMAGE must be a logical scalar value.") ;
      } else {
        doubleImage = *mxGetLogicals(optarg);
      }
      break ;

    case opt_octave_resolution :
      if (!vlmxIsPlainScalar(optarg) || (octaveResolution = (vl_index)*mxGetPr(optarg)) < 1) {
        vlmxError(vlmxErrInvalidArgument, "OCTAVERESOLUTION must be an integer not smaller than 1.") ;
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
        
    case opt_laplacian_peak_threshold :
      if (!vlmxIsPlainScalar(optarg) || (lapPeakThreshold = *mxGetPr(optarg)) < 0) {
        vlmxError(vlmxErrInvalidArgument, "LAPLACIANPEAKTHRESHOLD must be a non-negative real.") ;
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

    case opt_liop_bins :
      if (!vlmxIsPlainScalar(optarg) || (liopNumSpatialBins = (vl_int)*mxGetPr(optarg)) <= 0) {
        vlmxError(vlmxErrInvalidArgument, "number of LIOPNUMSPATIALBINS is not a positive scalar.") ;
      }
      break ;

    case opt_liop_neighbours :
      if (!vlmxIsPlainScalar(optarg) || (liopNumNeighbours = (vl_int)*mxGetPr(optarg)) <= 0) {
        vlmxError(vlmxErrInvalidArgument, "number of LIOPNUMNEIGHBOURS is not a positive scalar.") ;
      }
      break ;

    case opt_liop_radius :
      if (!vlmxIsPlainScalar(optarg) || (liopRadius = (float)*mxGetPr(optarg)) <= 0) {
        vlmxError(vlmxErrInvalidArgument, "LIOPRADIUS must is not a positive scalar.") ;
      }
      break ;

    case opt_liop_threshold :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "LIOPINTENSITYTHRESHOLD is not a scalar.") ;
      }
      liopIntensityThreshold = *mxGetPr(optarg) ;
      break ;

    case opt_frames:
      if (!vlmxIsPlainMatrix(optarg,-1,-1)) {
        vlmxError(vlmxErrInvalidArgument, "FRAMES must be a palin matrix.") ;
      }
      numUserFrames = mxGetN (optarg) ;
      userFrameDimension = mxGetM (optarg) ;
      userFrames = mxGetPr (optarg) ;
      switch (userFrameDimension) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            /* ok */
          break ;
        default:
          vlmxError(vlmxErrInvalidArgument,
                    "FRAMES of dimensions %d are not recognised",
                    userFrameDimension); ;
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
      if (patchRelativeExtent < 0) patchRelativeExtent = 6 ;
      if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 1 ;
      break ;

    case VL_COVDET_DESC_SIFT :
      /* the patch parameters are selected to match the SIFT descriptor geometry */
      if (patchResolution < 0)  patchResolution = 15 ;
      if (patchRelativeExtent < 0) patchRelativeExtent = 7.5 ;
      if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 1 ;
      break ;

    case VL_COVDET_DESC_LIOP :
      if (patchResolution < 0)  patchResolution = 20 ;
      if (patchRelativeExtent < 0) patchRelativeExtent = 4 ;
      if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 0.5 ;
      break ;
  }

  if (patchResolution > 0) {
    vl_size w = 2*patchResolution + 1 ;
    patch = mxMalloc(sizeof(float) * w * w);
    patchXY = mxMalloc(2 * sizeof(float) * w * w);
  }

  if (descriptorType == VL_COVDET_DESC_LIOP && liopRadius > patchResolution) {
    vlmxError(vlmxErrInconsistentData, "LIOPRADIUS is larger than PATCHRESOLUTION.") ;
  }

  /* -----------------------------------------------------------------
   *                                                          Detector
   * -------------------------------------------------------------- */
  {
    VlCovDet * covdet = vl_covdet_new(method) ;

    /* set covdet parameters */
    vl_covdet_set_transposed(covdet, VL_TRUE) ;
    vl_covdet_set_first_octave(covdet, doubleImage ? -1 : 0) ;
    if (octaveResolution >= 0) vl_covdet_set_octave_resolution(covdet, octaveResolution) ;
    if (peakThreshold >= 0) vl_covdet_set_peak_threshold(covdet, peakThreshold) ;
    if (edgeThreshold >= 0) vl_covdet_set_edge_threshold(covdet, edgeThreshold) ;
    if (lapPeakThreshold >= 0) vl_covdet_set_laplacian_peak_threshold(covdet, lapPeakThreshold) ;
    
    if (verbose) {
      VL_PRINTF("vl_covdet: doubling image: %s\n",
                VL_YESNO(vl_covdet_get_first_octave(covdet) < 0)) ;
    }

    /* process the image */
    vl_covdet_put_image(covdet, image, numRows, numCols) ;

    /* fill with frames: either run the detector of poure them in */
    if (numUserFrames > 0) {
      vl_index k ;

      if (verbose) {
        mexPrintf("vl_covdet: sourcing %d frames\n", numUserFrames) ;
      }

      for (k = 0 ; k < (signed)numUserFrames ; ++k) {
        double const * uframe = userFrames + userFrameDimension * k ;
        double a11, a21, a12, a22 ;
        VlCovDetFeature feature ;
        feature.peakScore = VL_INFINITY_F ;
        feature.edgeScore = 1.0 ;
        feature.frame.x = (float)uframe[1] - 1 ;
        feature.frame.y = (float)uframe[0] - 1 ;

        switch (userFrameDimension) {
          case 2:
            a11 = 1.0 ;
            a21 = 0.0 ;
            a12 = 0.0 ;
            a22 = 1.0 ;
            break ;
          case 3:
            a11 = uframe[2] ;
            a21 = 0.0 ;
            a12 = 0.0 ;
            a22 = uframe[2] ;
            break ;
          case 4:
          {
            double sigma = uframe[2] ;
            double c = cos(uframe[3]) ;
            double s = sin(uframe[3]) ;
            a11 = sigma * c ;
            a21 = sigma * s ;
            a12 = sigma * (-s) ;
            a22 = sigma * c ;
            break ;
          }
          case 5:
          {
            double d2 ;
            if (uframe[2] < 0) {
              vlmxError(vlmxErrInvalidArgument, "FRAMES(:,%d) does not have a PSD covariance.", k+1) ;
            }
            a11 = sqrt(uframe[2]) ;
            a21 = uframe[3] / VL_MAX(a11, 1e-10) ;
            a12 = 0.0 ;
            d2 = uframe[4] - a21*a21 ;
            if (d2 < 0) {
              vlmxError(vlmxErrInvalidArgument, "FRAMES(:,%d) does not have a PSD covariance.", k+1) ;
            }
            a22 = sqrt(d2) ;
            break ;
          }
          case 6:
          {
            a11 = uframe[2] ;
            a21 = uframe[3] ;
            a12 = uframe[4] ;
            a22 = uframe[5] ;
            break ;
          }
          default:
            a11 = 0 ;
            a21 = 0 ;
            a12 = 0 ;
            a22 = 0 ;
            assert(0) ;
        }
        feature.frame.a11 = (float)a22 ;
        feature.frame.a21 = (float)a12 ;
        feature.frame.a12 = (float)a21 ;
        feature.frame.a22 = (float)a11 ;
        vl_covdet_append_feature(covdet, &feature) ;
      }
    } else {
      if (verbose) {
        mexPrintf("vl_covdet: detector: %s\n",
                  vl_enumeration_get_by_value(vlCovdetMethods, method)->name) ;
        mexPrintf("vl_covdet: peak threshold: %g, edge threshold: %g\n",
                  vl_covdet_get_peak_threshold(covdet),
                  vl_covdet_get_edge_threshold(covdet)) ;
      }

      vl_covdet_detect(covdet) ;

      if (verbose) {
        vl_index i ;
        vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
        mexPrintf("vl_covdet: %d features suppressed as duplicate (threshold: %g)\n",
                  vl_covdet_get_num_non_extrema_suppressed(covdet),
                  vl_covdet_get_non_extrema_suppression_threshold(covdet)) ;
        switch (method) {
        case VL_COVDET_METHOD_HARRIS_LAPLACE:
        case VL_COVDET_METHOD_HESSIAN_LAPLACE:
          {
            vl_size numScales ;
            vl_size const * numFeaturesPerScale ;
            numFeaturesPerScale = vl_covdet_get_laplacian_scales_statistics
              (covdet, &numScales) ;
            mexPrintf("vl_covdet: Laplacian scales:") ;
            for (i = 0 ; i <= (signed)numScales ; ++i) {
              mexPrintf("%d with %d scales;", numFeaturesPerScale[i], i) ;
            }
            mexPrintf("\n") ;
          }
          break ;
        default:
          break ;
        }
        mexPrintf("vl_covdet: detected %d features\n", numFeatures) ;
      }

      if (boundaryMargin > 0) {
        vl_covdet_drop_features_outside (covdet, boundaryMargin) ;
        if (verbose) {
          vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
          mexPrintf("vl_covdet: kept %d inside the boundary margin (%g)\n",
                    numFeatures, boundaryMargin) ;
        }
      }
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
      vl_size numFeaturesBefore = vl_covdet_get_num_features(covdet) ;
      vl_size numFeaturesAfter ;

      vl_covdet_extract_orientations(covdet) ;

      numFeaturesAfter = vl_covdet_get_num_features(covdet) ;
      if (verbose && numFeaturesAfter > numFeaturesBefore) {
        mexPrintf("vl_covdet: %d duplicate features were created due to ambiguous "
                  "orientation detection (%d total)\n",
                  numFeaturesAfter - numFeaturesBefore, numFeaturesAfter) ;
      }
    }

    /* store results back */
    {
      vl_index i  ;
      vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
      VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
      double * pt ;

      OUT(FRAMES) = mxCreateDoubleMatrix (6, numFeatures, mxREAL) ;
      pt = mxGetPr(OUT(FRAMES)) ;

      for (i = 0 ; i < (signed)numFeatures ; ++i) {
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
      switch (descriptorType) {
        case VL_COVDET_DESC_NONE:
          OUT(DESCRIPTORS) = mxCreateDoubleMatrix(0,0,mxREAL);
          break ;

        case VL_COVDET_DESC_PATCH:
        {
          vl_size numFeatures ;
          VlCovDetFeature const * feature ;
          vl_index i ;
          vl_size w = 2*patchResolution + 1 ;
          float * desc ;

          if (verbose) {
            mexPrintf("vl_covdet: descriptors: type=patch, "
                      "resolution=%d, extent=%g, smoothing=%g\n",
                      patchResolution, patchRelativeExtent,
                      patchRelativeSmoothing);
          }
          numFeatures = vl_covdet_get_num_features(covdet) ;
          feature = vl_covdet_get_features(covdet);
          OUT(DESCRIPTORS) = mxCreateNumericMatrix(w*w, numFeatures, mxSINGLE_CLASS, mxREAL) ;
          desc = mxGetData(OUT(DESCRIPTORS)) ;
          for (i = 0 ; i < (signed)numFeatures ; ++i) {
            vl_covdet_extract_patch_for_frame(covdet,
                                    desc,
                                    patchResolution,
                                    patchRelativeExtent,
                                    patchRelativeSmoothing,
                                    feature[i].frame) ;
            desc += w*w ;
          }
          break ;
        }
        case VL_COVDET_DESC_SIFT:
        {
          vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
          VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
          VlSiftFilt * sift = vl_sift_new(16, 16, 1, 3, 0) ;
          vl_index i ;
          vl_size dimension = 128 ;
          vl_size patchSide = 2 * patchResolution + 1 ;
          double patchStep = (double)patchRelativeExtent / patchResolution ;
          float tempDesc [128] ;
          float * desc ;
          if (verbose) {
            mexPrintf("vl_covdet: descriptors: type=sift, "
                      "resolution=%d, extent=%g, smoothing=%g\n",
                      patchResolution, patchRelativeExtent,
                      patchRelativeSmoothing);
          }
          OUT(DESCRIPTORS) = mxCreateNumericMatrix(dimension, numFeatures, mxSINGLE_CLASS, mxREAL) ;
          desc = mxGetData(OUT(DESCRIPTORS)) ;
          vl_sift_set_magnif(sift, 3.0) ;
          for (i = 0 ; i < (signed)numFeatures ; ++i) {
            vl_covdet_extract_patch_for_frame(covdet,
                                              patch,
                                              patchResolution,
                                              patchRelativeExtent,
                                              patchRelativeSmoothing,
                                              feature[i].frame) ;

            vl_imgradient_polar_f (patchXY, patchXY +1,
                                   2, 2 * patchSide,
                                   patch, patchSide, patchSide, patchSide) ;


            /*
             Note: the patch is transposed, so that x and y are swapped.
             However, if NBO is not divisible by 4, then the configuration
             of the SIFT orientations is not symmetric by rotations of pi/2.
             Hence the only option is to rotate the descriptor further by
             an angle we need to compute the descriptor rotated by an additional pi/2
             angle. In this manner, x coincides and y is flipped.
             */
            vl_sift_calc_raw_descriptor (sift,
                                         patchXY,
                                         tempDesc,
                                         (int)patchSide, (int)patchSide,
                                         (double)(patchSide-1) / 2, (double)(patchSide-1) / 2,
                                         (double)patchRelativeExtent / (3.0 * (4 + 1) / 2) /
                                         patchStep,
                                         VL_PI / 2) ;

            flip_descriptor (desc, tempDesc) ;
            desc += dimension ;
          }
          vl_sift_delete(sift) ;
          break ;
        }
        case VL_COVDET_DESC_LIOP :
        {          /* TODO: get parameters form input */
          vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
          vl_size dimension ;
          VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
          vl_index i ;

          vl_size patchSide = 2 * patchResolution + 1 ;
          float * desc ;

          VlLiopDesc * liop = vl_liopdesc_new(liopNumNeighbours, liopNumSpatialBins, liopRadius, (vl_size)patchSide) ;
          if (!vl_is_nan_f(liopIntensityThreshold)) {
            vl_liopdesc_set_intensity_threshold(liop, liopIntensityThreshold) ;
          }
          dimension = vl_liopdesc_get_dimension(liop) ;
          if (verbose) {
            mexPrintf("vl_covdet: descriptors: type=liop, "
                      "resolution=%d, extent=%g, smoothing=%g\n",
                      patchResolution, patchRelativeExtent,
                      patchRelativeSmoothing);
          }
          OUT(DESCRIPTORS) = mxCreateNumericMatrix(dimension, numFeatures, mxSINGLE_CLASS, mxREAL);
          desc = mxGetData(OUT(DESCRIPTORS)) ;
          for(i = 0; i < (signed)numFeatures; i++){
              vl_covdet_extract_patch_for_frame(covdet,
                                                patch,
                                                patchResolution,
                                                patchRelativeExtent,
                                                patchRelativeSmoothing,
                                                feature[i].frame);

              vl_liopdesc_process(liop, desc, patch);

              desc += dimension;

          }
          vl_liopdesc_delete(liop);
          break;
        }

        default:
          assert(0) ; /* descriptor type */
      }
    }

    if (nout >= 3) {
      vl_index i ;
      vl_size numFeatures = vl_covdet_get_num_features(covdet) ;
      VlCovDetFeature const * feature = vl_covdet_get_features(covdet);
      const char* names[] = {
        "gss",
        "css",
        "peakScores",
        "edgeScores",
        "orientationScore",
        "laplacianScaleScore"
      };
      mxArray * gss_array = _createArrayFromScaleSpace(vl_covdet_get_gss(covdet)) ;
      mxArray * css_array = _createArrayFromScaleSpace(vl_covdet_get_css(covdet)) ;
      mxArray * peak_array = mxCreateNumericMatrix(1,numFeatures,mxSINGLE_CLASS,mxREAL) ;
      mxArray * edge_array = mxCreateNumericMatrix(1,numFeatures,mxSINGLE_CLASS,mxREAL) ;
      mxArray * orientation_array = mxCreateNumericMatrix(1,numFeatures,mxSINGLE_CLASS,mxREAL) ;
      mxArray * laplacian_array = mxCreateNumericMatrix(1,numFeatures,mxSINGLE_CLASS,mxREAL) ;

      float * peak = mxGetData(peak_array) ;
      float * edge = mxGetData(edge_array) ;
      float * orientation = mxGetData(orientation_array) ;
      float * laplacian = mxGetData(laplacian_array) ;
      for (i = 0 ; i < (signed)numFeatures ; ++i) {
        peak[i] = feature[i].peakScore ;
        edge[i] = feature[i].edgeScore ;
        orientation[i] = feature[i].orientationScore ;
        laplacian[i] = feature[i].laplacianScaleScore ;
      }

      OUT(INFO) = mxCreateStructMatrix(1, 1, 6, names) ;
      mxSetFieldByNumber(OUT(INFO), 0, 0, gss_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 1, css_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 2, peak_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 3, edge_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 4, orientation_array) ;
      mxSetFieldByNumber(OUT(INFO), 0, 5, laplacian_array) ;
    }
    /* cleanup */
    vl_covdet_delete (covdet) ;
  }

  if (patchXY) mxFree(patchXY) ;
  if (patch) mxFree(patch) ;
}
