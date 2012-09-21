/** @file covdet.h
 ** @brief Covariant feature detectors (@ref covdet)
 ** @author Karel Lenc
 ** @author Andrea Vedaldi
 ** @author Michal Perdoch
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#ifndef VL_COVDET_H
#define VL_COVDET_H

#include "generic.h"
#include "stringop.h"
#include "scalespace.h"

#include <stdio.h>

/* ---------------------------------------------------------------- */
/*                                                   Feature Frames */
/* ---------------------------------------------------------------- */

/** @name Feature frames
 ** @{ */

/** @brief Types of feature frames */
typedef enum _VlFrameType {
  VL_FRAMETYPE_DISC = 1,         /**< A disc. */
  VL_FRAMETYPE_ORIENTED_DISC,    /**< An oriented disc. */
  VL_FRAMETYPE_ELLIPSE,          /**< An ellipse. */
  VL_FRAMETYPE_ORIENTED_ELLIPSE, /**< An oriented ellipse. */
  VL_FRAMETYPE_NUM
} VlFrameType ;

/** @brief Names of the frame types */
VL_EXPORT const char* vlFrameNames [VL_FRAMETYPE_NUM] ;

/** @brief Mapping between string values and VlFrameType values */
VL_EXPORT VlEnumerator vlFrameTypes [VL_FRAMETYPE_NUM] ;

/** @brief Disc feature frame */
typedef struct _VlFrameDisc
{
  float x ;     /**< center x-coordinate */
  float y ;     /**< center y-coordinate */
  float sigma ; /**< radius or scale */
} VlFrameDisc ;

/** @brief Oriented disc feature frame
 ** An upright frame has @c angle equal to zero.
 **/
typedef struct _VlFrameOrientedDisc {
  float x ;     /**< center x-coordinate */
  float y ;     /**< center y-coordinate */
  float sigma ; /**< radius or scale */
  float angle ; /**< rotation angle (rad) */
} VlFrameOrientedDisc ;

/** @brief Ellipse feature frame */
typedef struct _VlFrameEllipse {
  float x ;     /**< center x-coordinate */
  float y ;     /**< center y-coordinate */
  float e11 ;   /**< */
  float e12 ;
  float e22 ;
} VlFrameEllipse ;

/** @brief Oriented ellipse feature frame
 ** The affine transformation transforms the ellipse shape into
 ** a circular region. */
typedef struct _VlFrameOrientedEllipse {
  float x ;     /**< center x-coordinate */
  float y ;     /**< center y-coordinate */
  float a11 ;   /**< */
  float a12 ;
  float a21 ;
  float a22 ;
} VlFrameOrientedEllipse;

/** @brief Get the size of a frame structure
 ** @param frameType identifier of the type of frame.
 ** @return size of the corresponding frame structure in bytes.
 **/
VL_INLINE vl_size
vl_get_frame_size (VlFrameType frameType) {
  switch (frameType) {
    case VL_FRAMETYPE_DISC: return sizeof(VlFrameDisc);
    case VL_FRAMETYPE_ORIENTED_DISC: return sizeof(VlFrameOrientedDisc);
    case VL_FRAMETYPE_ELLIPSE: return sizeof(VlFrameEllipse);
    case VL_FRAMETYPE_ORIENTED_ELLIPSE: return sizeof(VlFrameOrientedEllipse);
    default:
      assert(0);
      break;
  }
  return 0;
}

/** @brief Get the size of a frame structure
 ** @param frameType identifier of the type of frame.
 ** @return size of the corresponding frame structure in bytes.
 **/
VL_INLINE VlFrameType
vl_get_frame_type (vl_bool affineAdaptation, vl_bool orientation)
{
  if (affineAdaptation) {
    if (orientation) {
      return VL_FRAMETYPE_ORIENTED_ELLIPSE;
    } else {
      return VL_FRAMETYPE_ELLIPSE;
    }
  } else {
    if (orientation) {
      return VL_FRAMETYPE_ORIENTED_DISC;
    } else {
      return VL_FRAMETYPE_DISC;
    }
  }
}

/* ---------------------------------------------------------------- */
/*                                            Local extrema finding */
/* ---------------------------------------------------------------- */

typedef struct _VlCovDetExtremum2
{
  vl_index xi ;
  vl_index yi ;
  float x ;
  float y ;
  float peakScore ;
  float edgeScore ;
} VlCovDetExtremum2 ;

typedef struct _VlCovDetExtremum3
{
  vl_index xi ;
  vl_index yi ;
  vl_index zi ;
  float x ;
  float y ;
  float z ;
  float peakScore ;
  float edgeScore ;
} VlCovDetExtremum3 ;

VL_EXPORT vl_size
vl_find_local_extrema_3 (vl_index ** extrema, vl_size * bufferSize,
                         float const * map,
                         vl_size width, vl_size height, vl_size depth,
                         double threshold) ;

VL_EXPORT vl_size
vl_find_local_extrema_2 (vl_index ** extrema, vl_size * bufferSize,
                         float const * map,
                         vl_size width, vl_size height,
                         double threshold) ;

VL_EXPORT vl_bool
vl_refine_local_extreum_3 (VlCovDetExtremum3 * refined,
                           float const * map,
                           vl_size width, vl_size height, vl_size depth,
                           vl_index x, vl_index y, vl_index z) ;

VL_EXPORT vl_bool
vl_refine_local_extreum_2 (VlCovDetExtremum2 * refined,
                           float const * map,
                           vl_size width, vl_size height,
                           vl_index x, vl_index y) ;

/* ---------------------------------------------------------------- */
/*                                       Covariant Feature Detector */
/* ---------------------------------------------------------------- */

typedef struct _VlCovDetFeature
{
  VlFrameOrientedEllipse frame ;
  float peakScore ;
  float edgeScore ;
} VlCovDetFeature ;

/**
 ** @brief Image response functions
 **
 ** Response functions appliable to the input image used for frames
 ** detection.
 **/

typedef enum _VlCovDetMethod
{
  VL_COVDET_METHOD_DOG = 1,
  VL_COVDET_METHOD_HESSIAN,
  VL_COVDET_METHOD_HESSIAN_LAPLACE,
  VL_COVDET_METHOD_HARRIS_LAPLACE,
  VL_COVDET_METHOD_MULTISCALE_HESSIAN,
  VL_COVDET_METHOD_MULTISCALE_HARRIS,
  VL_COVDET_METHOD_NUM
} VlCovDetMethod;


/** @brief Mapping between strings and ::VlCovDetMethod values
 **/

VL_EXPORT VlEnumerator vlCovdetMethods [VL_COVDET_METHOD_NUM] ;

/** @brief Covariant feature detector
 **/

enum {
  VL_COVDET_MAX_NUM_ORIENTATIONS = 4,
  VL_COVDET_MAX_NUM_LAPLACIAN_SCALES = 4,
  VL_COVDET_AA_PATCH_RESOLUTION = 20,
  VL_COVDET_AA_MAX_NUM_ITERATIONS = 15,
  VL_COVDET_OR_NUM_ORIENTATION_HISTOGAM_BINS = 36
} ;

#define VL_COVDET_AA_RELATIVE_INTEGRATION_SIGMA 1.3
#define VL_COVDET_AA_MAX_ANISOTROPY 2.5
#define VL_COVDET_AA_CONVERGENCE_THRESHOLD 1.001
#define VL_COVDET_AA_PATCH_EXTENT (3*VL_COVDET_AA_RELATIVE_INTEGRATION_SIGMA)
#define VL_COVDET_OR_ADDITIONAL_PEAKS_RELATIVE_SIZE 0.8
#define VL_COVDET_LAP_NUM_LEVELS 10
#define VL_COVDET_LAP_PATCH_RESOLUTION 12
#define VL_COVDET_DEF_REFERENCE_ANGLE (VL_PI/2)
#define VL_COVDET_DEF_PEAK_THRESHOLD 0.001
#define VL_COVDET_DEF_EDGE_THRESHOLD 10.0

typedef struct _VlCovDet
{
  VlScaleSpace *gss ;       /**< Gaussian scale space. */
  VlScaleSpace *css ;       /**< Cornerness scale space. */
  VlCovDetMethod method ;   /**< feature extraction method. */
  double peakThreshold ;    /**< peak threshold. */
  double edgeThreshold ;    /**< edge threshold. */
  vl_size octaveResolution ;
  vl_index firstOctave ;

  VlCovDetFeature *frames ;
  vl_size numFrames ;
  vl_size numFrameBufferSize ;

  float * patch ;
  vl_size patchBufferSize ;

  vl_bool transposed ;
  double referenceAngle ;
  double orientations [VL_COVDET_MAX_NUM_ORIENTATIONS] ;
  double scales [VL_COVDET_MAX_NUM_LAPLACIAN_SCALES] ;

  float aaPatch [(2*VL_COVDET_AA_PATCH_RESOLUTION+1)*(2*VL_COVDET_AA_PATCH_RESOLUTION+1)] ;
  float aaPatchX [(2*VL_COVDET_AA_PATCH_RESOLUTION+1)*(2*VL_COVDET_AA_PATCH_RESOLUTION+1)] ;
  float aaPatchY [(2*VL_COVDET_AA_PATCH_RESOLUTION+1)*(2*VL_COVDET_AA_PATCH_RESOLUTION+1)] ;
  float aaMask [(2*VL_COVDET_AA_PATCH_RESOLUTION+1)*(2*VL_COVDET_AA_PATCH_RESOLUTION+1)] ;

  float lapPatch [(2*VL_COVDET_LAP_PATCH_RESOLUTION+1)*(2*VL_COVDET_LAP_PATCH_RESOLUTION+1)] ;
  float laplacians [(2*VL_COVDET_LAP_PATCH_RESOLUTION+1)*(2*VL_COVDET_LAP_PATCH_RESOLUTION+1)*VL_COVDET_LAP_NUM_LEVELS] ;

} VlCovDet ;


/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlCovDet * vl_covdet_new (VlCovDetMethod method) ;
VL_EXPORT void vl_covdet_delete (VlCovDet * self) ;
VL_EXPORT void vl_covdet_reset (VlCovDet * self) ;
/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT void vl_covdet_put_image (VlCovDet * self,
                                    float const * image,
                                    vl_size width, vl_size height) ;

VL_EXPORT void vl_covdet_detect (VlCovDet * self) ;
VL_EXPORT int vl_covdet_append_feature (VlCovDet * self, VlCovDetFeature const * feature) ;
VL_EXPORT void vl_covdet_extract_orientations (VlCovDet * self) ;
VL_EXPORT void vl_covdet_extract_laplacian_scales (VlCovDet * self) ;
VL_EXPORT void vl_covdet_extract_affine_shape (VlCovDet * self) ;

VL_EXPORT double *
vl_covdet_extract_orientations_for_frame (VlCovDet * self,
                                          vl_size *numOrientations,
                                          VlFrameOrientedEllipse frame) ;
VL_EXPORT double *
vl_covdet_extract_laplacian_scales_for_frame (VlCovDet * self,
                                              vl_size * numScales,
                                              VlFrameOrientedEllipse frame) ;
VL_EXPORT int
vl_covdet_extract_affine_shape_for_frame (VlCovDet * self,
                                          VlFrameOrientedEllipse * adapted,
                                          VlFrameOrientedEllipse frame) ;

VL_EXPORT vl_bool
vl_covdet_extract_patch_for_frame (VlCovDet * self, float * patch,
                                   vl_size resolution,
                                   double extent,
                                   double sigma,
                                   VlFrameOrientedEllipse frame) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_EXPORT vl_size vl_covdet_get_num_features (VlCovDet const * self) ;
VL_EXPORT void * vl_covdet_get_features (VlCovDet * self) ;
VL_EXPORT vl_index vl_covdet_get_first_octave (VlCovDet const * self) ;
VL_EXPORT vl_size vl_covdet_get_octave_resolution (VlCovDet const * self) ;
VL_EXPORT double vl_covdet_get_peak_threshold (VlCovDet const * self) ;
VL_EXPORT double vl_covdet_get_edge_threshold (VlCovDet const * self) ;
VL_EXPORT vl_bool vl_covdet_get_transposed (VlCovDet const * self) ;

/** @} */

/** @name Set parameters
 ** @{
 **/
VL_EXPORT void vl_covdet_set_first_octave (VlCovDet * self, vl_index o) ;
VL_EXPORT void vl_covdet_set_octave_resolution (VlCovDet * self, vl_size r) ;
VL_EXPORT void vl_covdet_set_peak_threshold (VlCovDet * self, double peakThreshold) ;
VL_EXPORT void vl_covdet_set_edge_threshold (VlCovDet * self, double edgeThreshold) ;
VL_EXPORT void vl_covdet_set_transposed (VlCovDet * self, vl_bool t) ;
/** @} */

/* VL_COVDET_H */
#endif
