/** @file scalespace.h
 ** @brief Scale Space (@ref scalespace)
 ** @author Andrea Vedaldi
 ** @author Karel Lenc
 ** @author Michal Perdoch
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_SCALESPACE_H
#define VL_SCALESPACE_H

#include "pgm.h"

#include <stdio.h>
#include <math.h>
#include "generic.h"
#include "imopv.h"

/** ------------------------------------------------------------------
 ** @brief Scale space feature frame
 **
 ** This structure represent a feature frame as extracted by the
 ** ::VlScaleSpace object. It represents a point in space and scale,
 ** including both continuous and approimated integer coordinates
 ** pointing to a discrete sample in the scale space structure.
 **/

typedef struct _VlScaleSpaceFrame
{
  vl_index o ;           /**< o coordinate (octave). */
  vl_index ix ;          /**< Integer unnormalized x coordinate. */
  vl_index iy ;          /**< Integer unnormalized y coordinate. */
  vl_index is ;          /**< Integer s coordinate. */
  float x ;              /**< x coordinate. */
  float y ;              /**< y coordinate. */
  float s ;              /**< s coordinate. */
  float sigma ;          /**< scale. */
} VlScaleSpaceFrame ;

/** ------------------------------------------------------------------
 ** @brief Scale space processing callback
 **
 ** Scale space callback appliable to separate planes.
 **/

typedef void (VlScaleSpaceCallback) (float const *srcImage,
                                     vl_size src_width, vl_size src_height,
                                     float *dstImage,
                                     vl_size dst_width, vl_size dst_height,
                                     vl_index octave, vl_index level,
                                     void *params);

/* ---------------------------------------------------------------- */

typedef struct _VlScaleSpaceGeometry
{
  vl_size width ; /**< Image width */
  vl_size height ; /**< Image height */
  vl_index firstOctave ; /**< Index of the fisrt octave */
  vl_index lastOctave ; /**< Index of the last octave */
  vl_size octaveResolution ; /**< Number of octave subdivisions */
  vl_index octaveFirstSubdivision ; /**< Index of the first octave subdivsion */
  vl_index octaveLastSubdivision ; /**< Index of the last octave subdivision */
  double sigma0 ; /**< Base smoothing (smoothing of octave 0, level 0) */
  double sigman ; /**< Bominal image smoothing */
} VlScaleSpaceGeometry ;

typedef struct _VlScaleSpaceOctaveGeometry
{
  vl_size width ; /**< Width (number of pixels) */
  vl_size height ; /**< Height (number of pixels) */
  double step ; /**< Sampling step (size of a pixel) */
} VlScaleSpaceOctaveGeometry ;

/* ---------------------------------------------------------------- */

/** @brief Gaussian scale space */
typedef struct _VlScaleSpace
{
  VlScaleSpaceGeometry geom ; /**< Geometry of the scale space */
  float **octaves ; /**< Data */
} VlScaleSpace ;


/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlScaleSpace *
vl_scalespace_new (vl_size width, vl_size height,
                   vl_index numOctaves, vl_index firstOctave,
                   vl_size numLevels, vl_index firstLevel, vl_index lastLevel) ;

VL_EXPORT VlScaleSpace *
vl_scalespace_new_with_geometry (VlScaleSpaceGeometry geom) ;

VL_EXPORT void  vl_scalespace_delete (VlScaleSpace *self) ;
VL_EXPORT VlScaleSpace *vl_scalespace_clone_structure (VlScaleSpace* src);
VL_EXPORT VlScaleSpace *vl_scalespace_clone (VlScaleSpace* src);
/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT void
vl_scalespace_put_image (VlScaleSpace *self, float const* image);
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_EXPORT VlScaleSpaceGeometry vl_scalespace_get_geometry (VlScaleSpace const * self) ;
VL_EXPORT VlScaleSpaceOctaveGeometry vl_scalespace_get_octave_geometry (VlScaleSpace const * self, vl_index o) ;
VL_EXPORT float *
vl_scalespace_get_level (VlScaleSpace const *self, vl_index o, vl_index s) ;
VL_EXPORT double
vl_scalespace_get_level_sigma (VlScaleSpace const *self, vl_index o, vl_index s) ;
/** @} */

/* VL_SCALESPACE_H */
#endif

