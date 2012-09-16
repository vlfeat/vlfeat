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

typedef struct _VlScaleSpaceGeometry
{
  vl_size width ;
  vl_size height ;
  vl_index firstOctave ;
  vl_index lastOctave ;
  vl_size octaveResolution ;
  vl_index octaveFirstSubdivision ;
  vl_index octaveLastSubdivision ;
  double sigma0 ;
} VlScaleSpaceGeometry ;

/** ------------------------------------------------------------------
 ** @brief Scale space
 ** An object to compute an image Gaussian scale space.
 **/

typedef struct _VlScaleSpace
{
  double sigman ;             /**< nominal image smoothing. */
  double sigma0 ;             /**< smoothing of pyramid base. */
  double sigmak ;             /**< k-smoothing. */
  double dsigma0 ;            /**< delta-smoothing. */

  VlScaleSpaceGeometry geom ;

  vl_size width ;             /**< image width. */
  vl_size height ;            /**< image height. */
  vl_size numOctaves ;        /**< number of octaves. */
  vl_size numLevels ;         /**< number of levels per octave. */
  vl_index firstOctave ;      /**< minimum octave index. */
  vl_index lastOctave ;       /**< maximum octave index. */
  vl_index firstLevel ;       /**< minimum level index. */
  vl_index lastLevel ;        /**< maximum level index. */

  float **octaves ;           /**< GSS data. */

  float *patch ;              /** buffer for operations on patches. */
  vl_size patchSize ;         /** size of the @c patch buffer. */

  VlScaleSpaceFrame *frames ; /**< detected feature frames. */
  vl_size framesSize ;        /**< size of the VlScaleSpace::frames buffer. */
  vl_size numFrames ;         /**< number of stored feature frames. */
} VlScaleSpace ;


/** @name Create and destroy
 ** @{
 **/
VL_EXPORT VlScaleSpace
*vl_scalespace_new (vl_size width, vl_size height,
                    vl_index numOctaves, vl_index firstOctave,
                    vl_size numLevels, vl_index firstLevel, vl_index lastLevel) ;
VL_EXPORT void  vl_scalespace_delete (VlScaleSpace *self) ;
VL_EXPORT VlScaleSpace *vl_scalespace_clone_structure (VlScaleSpace* src);
VL_EXPORT VlScaleSpace *vl_scalespace_clone (VlScaleSpace* src);
VL_EXPORT void vl_scalespace_init(VlScaleSpace *self, float const* image);
/** @} */

/** @name Process data
 ** @{
 **/
VL_EXPORT void
vl_scalespace_apply (VlScaleSpace const *self, VlScaleSpace* dst,
                     VlScaleSpaceCallback* callback, void* params);

VL_EXPORT void
vl_scalespace_diff (VlScaleSpace const* self, VlScaleSpace* dst);

VL_EXPORT void
vl_scalespace_find_local_extrema (VlScaleSpace* self,
                                  double peakThreshold,
                                  vl_size borderSize) ;

VL_EXPORT void
vl_scalespace_refine_local_extrema (VlScaleSpace * self,
                                    double peakThrehsold,
                                    double edgeThreshold,
                                    vl_size borderSize) ;


VL_EXPORT int
vl_scalespace_extract_affine_patch (VlScaleSpace *self,
                                    float *patch,
                                    vl_size patchWidth,
                                    vl_size patchHeight,
                                    double patchSigma,
                                    double t1, double t2,
                                    double a11, double a21,
                                    double a12, double a22) ;

VL_EXPORT void
vl_scalespace_frame_init (VlScaleSpace const * self,
                          VlScaleSpaceFrame * frame,
                          double x, double y, double sigma) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
VL_EXPORT VlScaleSpaceGeometry vl_scalespace_get_geometry (VlScaleSpace const * self) ;

VL_INLINE vl_size vl_scalespace_get_octaves_num (VlScaleSpace const *self) ;
VL_INLINE vl_index vl_scalespace_get_octave_min (VlScaleSpace const *self) ;
VL_INLINE vl_index vl_scalespace_get_octave_max (VlScaleSpace const *self) ;
VL_INLINE vl_size vl_scalespace_get_octave_width (VlScaleSpace const *self, vl_index o) ;
VL_INLINE vl_size vl_scalespace_get_octave_height (VlScaleSpace const *self, vl_index o) ;
VL_INLINE vl_size vl_scalespace_get_levels_num (VlScaleSpace const *self) ;
VL_INLINE vl_index vl_scalespace_get_level_min (VlScaleSpace const *self) ;
VL_INLINE vl_index vl_scalespace_get_level_max (VlScaleSpace const *self) ;
VL_INLINE vl_size vl_scalespace_get_frames_num (VlScaleSpace const *self) ;
VL_INLINE double vl_scalespace_get_sigma0 (VlScaleSpace const *self) ;
VL_INLINE double vl_scalespace_get_sigmak (VlScaleSpace const *self) ;
VL_INLINE float *vl_scalespace_get_octave  (VlScaleSpace const *self, vl_index o, vl_index s) ;
VL_INLINE double vl_scalespace_get_sigma_for_scale (VlScaleSpace const *self, vl_index o, vl_index s) ;
VL_INLINE double vl_scalespace_get_octave_sampling_step (VlScaleSpace const *self, vl_index o) ;
VL_INLINE VlScaleSpaceFrame const *vl_scalespace_get_frames (VlScaleSpace const *self) ;
/** @} */

/* -------------------------------------------------------------------
 *                         ScaleSpace Inline functions implementation
 * ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/** @brief Get number of octaves.
 ** @param self ::VlScaleSpace object instance..
 ** @return number of octaves.
 **/

VL_INLINE vl_size
vl_scalespace_get_octaves_num (VlScaleSpace const *self)
{
  assert(self) ;
  return self->numOctaves ;
}

/** @brief Get index of first octave.
 ** @param self ::VlScaleSpace object instance.
 ** @return index of the first octave.
 **/

VL_INLINE vl_index
vl_scalespace_get_octave_min (VlScaleSpace const *self)
{
  assert(self) ;
  return self->firstOctave ;
}

/** @brief Get index of last octave.
 ** @param self ::VlScaleSpace object instance.
 ** @return index of the last octave.
 **/

VL_INLINE vl_index
vl_scalespace_get_octave_max (VlScaleSpace const *self)
{
  assert(self) ;
  return self->lastOctave ;
}

/** @brief Get octave width
 ** @param self ::VlScaleSpace object instance.
 ** @param o octave index.
 ** @return current octave width.
 **/

VL_INLINE vl_size
vl_scalespace_get_octave_width (VlScaleSpace const *self, vl_index o)
{
  assert(self) ;
  return VL_SHIFT_LEFT(self->width, -o) ;
}

/** @brief Get octave height
 ** @param self ::VlScaleSpace object instance.
 ** @param o octave index.
 ** @return current octave height.
 **/

VL_INLINE vl_size
vl_scalespace_get_octave_height (VlScaleSpace const *self, vl_index o)
{
  assert(self) ;
  return VL_SHIFT_LEFT(self->height, -o) ;
}

/** ------------------------------------------------------------------
 ** @brief Get the data of a scale level
 ** @param self ::VlScaleSpace object instance.
 ** @param o octave index.
 ** @param s level index.
 ** @return pointer to the data for octave @a o, level @a s.
 **
 ** The octave index @a o must be in the range @c firstOctave
 ** to @c lastOctave and the scale index @a s must be in the
 ** range @c firstLevel to @c lastLevel.
 **/

VL_INLINE float *
vl_scalespace_get_octave (VlScaleSpace const *self, vl_index o, vl_index s)
{
  vl_size width = vl_scalespace_get_octave_width(self, o) ;
  vl_size height = vl_scalespace_get_octave_height(self, o) ;
  float *octave = self->octaves[o - self->firstOctave] ;
  assert(self) ;
  assert(o >= self->firstOctave) ;
  assert(o <= self->lastOctave) ;
  assert(s >= self->firstLevel) ;
  assert(s <= self->lastLevel) ;
  return octave + width * height * (s - self->firstLevel) ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of levels per octave
 ** @param self ::VlScaleSpace object instance.
 ** @return number of leves per octave.
 **/

VL_INLINE vl_size
vl_scalespace_get_levels_num (VlScaleSpace const *self)
{
  assert(self) ;
  return self->numLevels ;
}

/** ------------------------------------------------------------------
 ** @brief Get the index of the lowest level
 ** @param self ::VlScaleSpace object instance.
 ** @return index of the lowest level.
 **/

VL_INLINE vl_index
vl_scalespace_get_level_min (VlScaleSpace const *self)
{
  assert(self) ;
  return self->firstLevel ;
}

/** ------------------------------------------------------------------
 ** @brief Get the index of the top level
 ** @param self ::VlScaleSpace object instance..
 ** @return index of the top level.
 **/

VL_INLINE vl_index
vl_scalespace_get_level_max (VlScaleSpace const *self)
{
  assert(self) ;
  return self->lastLevel ;
}

/** ------------------------------------------------------------------
 ** @brief Get the number of stored feature frames.
 ** @param self ::VlScaleSpace object instance.
 ** @return number of frames.
 **/

VL_INLINE vl_size
vl_scalespace_get_frames_num (VlScaleSpace const *self)
{
  assert(self) ;
  return self->numFrames ;
}

/** ------------------------------------------------------------------
 ** @brief Get the stored feature frames.
 ** @param self ::VlScaleSpace object instance.
 ** @return pointer to the frames list.
 **/

VL_INLINE VlScaleSpaceFrame const *
vl_scalespace_get_frames (VlScaleSpace const *self)
{
  assert(self) ;
  return self->frames ;
}

/** ------------------------------------------------------------------
 ** @brief Get stddev of smoothing of pyramid base.
 ** @param self ::VlScaleSpace object instance.
 ** @return Standard deviation of smoothing of pyramid base.
 **/

VL_INLINE double
vl_scalespace_get_sigma0 (VlScaleSpace const *self)
{
  assert(self) ;
  return self->sigma0 ;
}

/** ------------------------------------------------------------------
 ** @brief Get stddev of smoothing of pyramid base.
 ** @param self ::VlScaleSpace object instance.
 ** @return Standard deviation of smoothing of pyramid base.
 **/

VL_INLINE double
vl_scalespace_get_sigmak (VlScaleSpace const *self)
{
  assert(self) ;
  return self->sigmak ;
}


/** ------------------------------------------------------------------
 ** @brief Get continous scale coordinate from scale index
 ** @param self ::VlScaleSpace object instance.
 ** @param o octave.
 ** @param s octave sub-level.
 ** @return contiunous scale coordinate.
 **/

VL_INLINE double
vl_scalespace_get_sigma_for_scale (VlScaleSpace const *self,
                                   vl_index o,
                                   vl_index s)
{
  assert(self) ;
  return self->sigma0 * pow(2.0, (double)s / self->numLevels + o) ;
}


VL_INLINE double vl_scalespace_get_octave_sampling_step (VlScaleSpace const *self, vl_index o)
{
  assert(self) ;
  return pow(2.0, o) ;
}

/* VL_SCALESPACE_H */
#endif

