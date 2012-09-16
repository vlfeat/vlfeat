/** @file scalespace.c
 ** @brief Scale Space - Definition
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

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page scalespace Gaussian Scale Space
@author Karel Lenc
@author Andrea Vedaldi
@author Michal Perdoch
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref scalespace.h implements a scale space, a data structure
fundamental in the computation of covariant features such as SIFT,
Hessian-Affine, Harris-Affine, Harris-Laplace, etc.

- @ref scalespace-overview
- @ref scalespace-usage
- @ref scalespace-tech

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section scalespace-overview Overview
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

A scale space is a data structure representing an image at multiple
resolution levels. Mathematically, it is defined as a three-dimensional
function of two spatial coordinates (usually
denoted as @$f x @f$ and @f$ y @f$) and a scale coordiante (@f$ \sigma
@f$). It is usually stored in a pyramid, with the coarse scales
begin represented with a lower resolution, in order to reduce
redundancy (as low-pass images can be represented accurately with
a coarser sampling rate).

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section scalespace-usage Usage
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

A scale space is represented by an instance of the ::VlScaleSpace
object.

@code
 VlScaleSpace ss = vl_scalespace_new(width, height,
                                     numOctave, firstOctave,
                                     numLevel, firstLevel, lastLevel) ;
@endcode

The scale space objec class has a number of functionalities meant to
help developing feature detectors:

- Local maxima/minima in space and scale can be detected with
 ::vl_scalespace_find_local_extrema() and refined to sub-pixel accuracy
 with ::vl_scalespace_refine_local_extrema(). Local extremas
 are filtered based on the <em>peak threshold</em> and the
 <em>edge threshold</em>.

- An affinely-warped image patch can be extracted from the scale
  space by using ::vl_scalespace_affinely_normalize_patch().

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection scalespace-tech Scale space
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

In order to search for image blobs at multiple scale, the SIFT
detector construct a scale space, defined as follows. Let
@f$I_0(\mathbf{x})@f$ denote an idealized <em>infinite resolution</em>
image. Consider the  <em>Gaussian kernel</em>

@f[
 g_{\sigma}(\mathbf{x})
 =
 \frac{1}{2\pi\sigma^2}
 \exp
 \left(
 -\frac{1}{2}
 \frac{\mathbf{x}^\top\mathbf{x}}{\sigma^2}
 \right)
@f]

The <b>Gaussian scale space</b> is the collection of smoothed images

@f[
 I_\sigma = g_\sigma * I,  \quad \sigma \geq 0.
@f]

The image at infinite resolution @f$ I_0 @f$ is useful conceptually,
but is not available to us; instead, the input image @f$ I_{\sigma_n}
@f$ is assumed to be pre-smoothed at a nominal level @f$ \sigma_n =
0.5 @f$ to account for the finite resolution of the pixels. Thus in
practice the scale space is computed by

@f[
I_\sigma = g_{\sqrt{\sigma^2 - \sigma_n^2}} * I_{\sigma_n},
\quad \sigma \geq \sigma_n.
@f]

Scales are sampled at logarithmic steps given by

@f[
\sigma = \sigma_0 2^{o+s/S},
\quad s = 0,\dots,S-1,
\quad o = o_{\min}, \dots, o_{\min}+O-1,
@f]

where @f$ \sigma_0 = 1.6 @f$ is the <em>base scale</em>, @f$ o_{\min}
@f$ is the <em>first octave index</em>, @em O the <em>number of
octaves</em> and @em S the <em>number of scales per octave</em>.

Blobs are detected as local extrema of the <b>Difference of
Gaussians</b> (DoG) scale space, obtained by subtracting successive
scales of the Gaussian scale space:

@f[
\mathrm{DoG}_{\sigma(o,s)} = I_{\sigma(o,s+1)} - I_{\sigma(o,s)}
@f]

At each next octave, the resolution of the images is halved to save
computations. The images composing the Gaussian and DoG scale space
can then be arranged as in the following figure:

@image html sift-ss.png  "GSS and DoG scale space  structures."

The black vertical segments represent images of the Gaussian Scale
Space (GSS), arranged by increasing scale @f$\sigma@f$.  Notice that
the scale level index @e s varies in a slightly redundant set

@f[
s = -1, \dots, S+2
@f]

This simplifies glueing together different octaves and extracting DoG
maxima (required by the SIFT detector).

*/

#include "scalespace.h"
#include "mathop.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


VlScaleSpaceGeometry
vl_scalespace_get_geometry (VlScaleSpace const * self)
{
  return self->geom ;
}

/** ------------------------------------------------------------------
 ** @internal @brief Upsample the rows and take the transpose
 ** @param destination output image.
 ** @param source input image.
 ** @param width input image width.
 ** @param height input image height.
 **
 ** The output image has dimensions @a height by 2 @a width (so the
 ** destination buffer must be at least as big as two times the
 ** input buffer).
 **
 ** Upsampling is performed by linear interpolation.
 **/

static void
copy_and_upsample
(float *destination,
 float const *source, vl_size width, vl_size height)
{
  vl_index x, y, ox, oy ;
  float v00, v10, v01, v11 ;

  assert(destination) ;
  assert(source) ;

  for(y = 0 ; y < (signed)height ; ++y) {
    oy = (y < ((signed)height - 1)) * width ;
    v10 = source[0] ;
    v11 = source[oy] ;
    for(x = 0 ; x < (signed)width ; ++x) {
      ox = x < ((signed)width - 1) ;
      v00 = v10 ;
      v01 = v11 ;
      v10 = source[ox] ;
      v11 = source[ox + oy] ;
      destination[0] = v00 ;
      destination[1] = 0.5f * (v00 + v10) ;
      destination[2*width] = 0.5f * (v00 + v01) ;
      destination[2*width+1] = 0.25f * (v00 + v01 + v10 + v11) ;
      destination += 2 ;
      source ++;
    }
    destination += 2*width ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Downsample
 ** @param destination output imgae buffer.
 ** @param source input image buffer.
 ** @param width input image width.
 ** @param height input image height.
 ** @param numOctaves octaves (non negative).
 **
 ** The function downsamples the image @a d times, reducing it to @c
 ** 1/2^d of its original size. The parameters @a width and @a height
 ** are the size of the input image. The destination image @a dst is
 ** assumed to be <code>floor(width/2^d)</code> pixels wide and
 ** <code>floor(height/2^d)</code> pixels high.
 **/

static void
copy_and_downsample
(float *destination,
 float const *source,
 vl_size width, vl_size height, vl_size numOctaves)
{
  vl_index x, y ;
  vl_size step = 1 << numOctaves ; /* step = 2^numOctaves */

  assert(destination) ;
  assert(source) ;

  if (numOctaves == 0) {
    memcpy(destination, source, sizeof(float) * width * height) ;
  } else {
    for(y = 0 ; y < (signed)height ; y += step) {
      float const *p = source + y * width ;
      for(x = 0 ; x < (signed)width - ((signed)step - 1) ; x += step) {
        *destination++ = *p ;
        p += step ;
      }
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief Creates a new ::VlScaleSpace object
 ** @param width image width.
 ** @param height image height.
 ** @param numOctaves number of octaves.
 ** @param firstOctave index of the first octave.
 ** @param numLevels numeber of levels per octave.
 ** @param firstLevel index of the first level.
 ** @param lastLevel index of the last level.
 ** @return the new scale space.
 **
 ** The function allocates and returns a new ::VlScaleSpace object
 ** of the specified geometry.
 **
 ** If @a numOctaves is a negative number, the number of octaves is
 ** selected to the maximum possible given the size of the image.
 **
 ** Parameters @a firstLevel and @a lastLevel allow to define additional
 ** levels on top or bottom of the scale-space although the scale-space
 ** would be calculated with parameters based on numLevels. This is for
 ** example used when we need to compute additional levels for local
 ** extrema localisation when e.g.:
 **
 ** @code
 ** numLevels = 3, firstLevel = -1, lastLevel = 3
 ** @endcode
 **
 ** would create scale space with 5 levels indexed as:
 **
 ** @code
 ** -1  0  1  2  3
 ** @endcode
 **
 ** @sa ::vl_scalespace_delete().
 **/

VlScaleSpace *
vl_scalespace_new (vl_size width, vl_size height,
                   vl_index numOctaves, vl_index firstOctave,
                   vl_size numLevels, vl_index firstLevel, vl_index lastLevel)
{
  vl_size baseWidth, baseHeight, numElements ;
  vl_size totalNumLevels = lastLevel - firstLevel + 1 ;
  vl_index o ;
  VlScaleSpace *self = vl_calloc(1, sizeof(VlScaleSpace)) ;
  if (self == NULL) goto err_alloc_self ;

  assert(self) ;
  assert(lastLevel >= firstLevel) ;
  assert(firstLevel <= 0) ;
  assert(lastLevel >= (signed)numLevels - 1) ;

  baseWidth = VL_SHIFT_LEFT(width, -firstOctave) ;
  baseHeight = VL_SHIFT_LEFT(height, -firstOctave) ;
  numElements = baseWidth * baseHeight ;
  if (numOctaves < 0) {
    numOctaves = VL_MAX(floor(vl_log2_d(VL_MIN(width, height))) - firstOctave - 3, 1) ;
  }

  self->width = width ;
  self->height = height ;
  self->numOctaves = numOctaves ;
  self->numLevels = numLevels ;
  self->firstOctave = firstOctave ;
  self->lastOctave = numOctaves + firstOctave - 1 ;
  self->firstLevel = firstLevel ;
  self->lastLevel = lastLevel ;

  self->sigman = 0.5 ;
  self->sigmak = pow(2.0, 1.0 / numLevels) ;
  self->sigma0 = 1.6 * self->sigmak ;
  self->dsigma0 = self->sigma0 * sqrt(1.0 - 1.0 / (self->sigmak*self->sigmak)) ;

  self->geom.width = width ;
  self->geom.height = height ;
  self->geom.firstOctave = firstOctave ;
  self->geom.lastOctave = numOctaves + firstOctave - 1 ;
  self->geom.octaveResolution= numLevels ;
  self->geom.octaveFirstSubdivision = firstLevel ;
  self->geom.octaveLastSubdivision = lastLevel ;
  self->geom.sigma0 = self->sigma0 ;

  self->octaves = vl_calloc(numOctaves, sizeof(float*)) ;
  if (self->octaves == NULL) goto err_alloc_octaves ;
  for (o = self->firstOctave ; o <= self->lastOctave ; ++o) {
    vl_size octaveSize =
    vl_scalespace_get_octave_width(self,o) *
    vl_scalespace_get_octave_height(self,o) *
    totalNumLevels ;
    /*VL_PRINTF("alloc: %d %d \n", o, octaveSize) ;*/
    self->octaves[o - self->firstOctave] = vl_malloc(octaveSize * sizeof(float)) ;
    if (self->octaves[o - self->firstOctave] == NULL) goto err_alloc_octaves_o ;
  }


  return self ;

err_alloc_smoother: ;
err_alloc_octaves_o:
  for (o = self->firstOctave ; o <= self->lastOctave ; ++o) {
    if (self->octaves[o - self->firstOctave]) {
      vl_free(self->octaves[o - self->firstOctave]) ;
    }
  }
err_alloc_octaves:
  vl_free(self) ;
err_alloc_self:
  /* todo: flag error */
  return NULL ;
}

/** -------------------------------------------------------------------
 ** @brief Delete a ::VlScaleSpace object
 ** @param self object to delete.
 ** @sa ::vl_scalespace_new
 **/

void
vl_scalespace_delete (VlScaleSpace *self)
{
  if (self) {
    if (self->patch) vl_free(self->patch) ;
    if (self->frames) vl_free(self->frames) ;
    if (self->octaves) {
      vl_index o ;
      for (o = self->firstOctave ; o <= self->lastOctave ; ++o) {
        if (self->octaves[o - self->firstOctave]) {
          vl_free(self->octaves[o - self->firstOctave]) ;
        }
      }
      vl_free(self->octaves) ;
    }
    vl_free(self) ;
  }
}


/** -------------------------------------------------------------------
 ** @brief Clone the scale space with all its data
 **
 ** Performs deep copy of the scale space.
 **
 ** @param self Scale space which should be cloned.
 **/

VlScaleSpace *
vl_scalespace_clone (VlScaleSpace* self)
{
  vl_index o  ;
  VlScaleSpace *copy = vl_scalespace_clone_structure(self) ;
  if (copy == NULL) goto err_alloc_copy ;
  for (o = self->firstOctave ; o <= self->lastOctave ; ++o) {
    vl_size octaveSize =
    vl_scalespace_get_octave_width(self,o) *
    vl_scalespace_get_octave_height(self,o) *
    (self->lastLevel - self->firstLevel + 1) ;
    memcpy(copy->octaves[o - self->firstOctave],
           self->octaves[o - self->firstOctave],
           octaveSize) ;
  }
  return copy ;

err_alloc_copy:
  /* todo: flag error */
  return NULL ;
}

/** -------------------------------------------------------------------
 ** @brief Clone the object but do not copy the data
 ** @param self ::VlScaleSpace object instance.
 ** @return the object copy, or @c NULL.
 **
 ** The function can fail if the memory is insufficient to hold the
 ** object copy. In this case, the function returns @c NULL.
 **/

VlScaleSpace
*vl_scalespace_clone_structure (VlScaleSpace* self)
{
  VlScaleSpace *copy = vl_scalespace_new(self->width,
                                         self->height,
                                         self->numOctaves,
                                         self->firstOctave,
                                         self->numLevels,
                                         self->firstLevel,
                                         self->lastLevel) ;
  if (copy == NULL) goto err_alloc_copy ;
  copy->sigman = self->sigman ;
  copy->sigma0 = self->sigma0 ;
  copy->sigmak = self->sigmak ;
  copy->dsigma0 = self->dsigma0 ;

  if (self->numFrames) {
    copy->frames = vl_malloc(self->framesSize * sizeof(VlScaleSpaceFrame)) ;
    if (copy->frames == NULL) goto err_alloc_frames ;
    memcpy(copy->frames, self->frames, self->framesSize * sizeof(VlScaleSpaceFrame)) ;
  }

  return copy ;

err_alloc_frames:
  vl_scalespace_delete(self) ;
err_alloc_copy:
  /* todo: flag error */
  return NULL ;
}

/** ------------------------------------------------------------------
 ** @internal @brief Fill octave startinf from first level
 ** @param self ::VlScaleSpace object instance.
 ** @param o octave to process.
 **
 ** The function takes the first level of octave @a o and iteratively
 ** smoothes it to obtain the other octave levels.
 **/

void
_vl_scalespace_fill_octave (VlScaleSpace *self, vl_index o)
{
  vl_index s ;
  for(s = self->firstLevel + 1 ; s <= self->lastLevel ; ++s) {
    double sd = self->dsigma0 * pow(self->sigmak, s) ;
    vl_size width = vl_scalespace_get_octave_width(self, o) ;
    vl_size height = vl_scalespace_get_octave_height(self, o) ;
    float* level = vl_scalespace_get_octave (self, o, s) ;
    float* previous = vl_scalespace_get_octave (self, o, s-1) ;
    vl_imsmooth_f (level, width,
                   previous, width, height, width,
                   sd, sd) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Initialize the first level of an octave from an image
 ** @param self ::VlScaleSpace object instance.
 ** @param image image data.
 ** @param o octave to start.
 **
 ** The function initializes the first level of octave @a o from
 ** image @a image. The dimensions of the image are the ones set
 ** during the creation of the ::VlScaleSpace object instance.
 **/

static void
_vl_scalespace_start_octave_from_image (VlScaleSpace *self,
                                        float const *image,
                                        vl_index o)
{
  float *level ;
  double sa, sb ;
  vl_index op ;

  assert(self) ;
  assert(image) ;
  assert(o >= self->firstOctave) ;
  assert(o <= self->lastOctave) ;

  /*
   * Copy the image to self->firstLevel of octave o, upscaling or
   * downscaling as needed.
   */

  level = vl_scalespace_get_octave(self, VL_MAX(0, o), self->firstLevel) ;
  copy_and_downsample(level, image, self->width, self->height, VL_MAX(0, o)) ;

  for (op = -1 ; op >= o ; --op) {
    float *succLevel = vl_scalespace_get_octave(self, op + 1, self->firstLevel) ;
    level = vl_scalespace_get_octave(self, op, self->firstLevel) ;
    copy_and_upsample(level, succLevel,
                      vl_scalespace_get_octave_width(self, op + 1),
                      vl_scalespace_get_octave_height(self, op + 1)) ;
  }

  /*
   * Adjust the smoothing of the first level just initialised, accounting
   * for the fact that the input image is assumed to be a nominal scale
   * level self->sigman.
   */

  sa = self->sigma0 * pow(self->sigmak, self->firstLevel) ;
  sb = self->sigman * pow(2.0, - o) ;

  if (sa > sb) {
    double sd = sqrt (sa*sa - sb*sb) ;
    level = vl_scalespace_get_octave(self, o, self->firstLevel) ;
    vl_size width = vl_scalespace_get_octave_width(self, o) ;
    vl_size height = vl_scalespace_get_octave_height(self, o) ;
    vl_imsmooth_f (level, width,
                   level, width, height, width,
                   sd, sd) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Initialize the first level of an octave from the previous octave
 ** @param ::VlScaleSpace objet instance.
 ** @param o octave to initialize.
 **
 ** The function initializes the first level of octave @a o from the
 ** content of octave <code>o - 1</code>.
 **/

static void
_vl_scalespace_start_octave_from_previous_octave (VlScaleSpace *self, vl_index o)
{
  double sa, sb ;
  float *level, *prevLevel ;
  vl_index prevLevelIndex ;

  assert(self) ;
  assert(o > self->firstOctave) ; /* must not be the first octave */
  assert(o <= self->lastOctave) ;

  /*
   * From the previous octave pick the level which is closer to
   * self->firstLevel in this octave.
   * The is self->firstLevel + self->numLevels since there are
   * self->numLevels levels in an octave, provided that
   * this value does not exceed self->lastLevel.
   */

  prevLevelIndex = VL_MIN(self->firstLevel + (signed)self->numLevels, self->lastLevel) ;
  prevLevel = vl_scalespace_get_octave (self, o - 1, prevLevelIndex) ;
  level = vl_scalespace_get_octave (self, o, self->firstLevel) ;

  copy_and_downsample (level, prevLevel,
                       vl_scalespace_get_octave_width(self, o - 1),
                       vl_scalespace_get_octave_height(self, o - 1),
                       1) ;

  /*
   * Add remaining smoothing, if any.
   */

  sa = self->sigma0 * powf(self->sigmak, self->firstLevel) ;
  sb = self->sigma0 * powf(self->sigmak, prevLevelIndex - (signed)self->numLevels) ;

  if (sa > sb) {
    double sd = sqrt(sa*sa - sb*sb) ;
    level = vl_scalespace_get_octave(self, o, self->firstLevel) ;
    vl_size width = vl_scalespace_get_octave_width(self, o) ;
    vl_size height = vl_scalespace_get_octave_height(self, o) ;
    vl_imsmooth_f (level, width,
                   level, width, height, width,
                   sd, sd) ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Initialise Scale space with new image
 ** @param self ::VlScaleSpace object instance.
 ** @param image image to process.
 **
 ** Compute the data of all the defined octaves and scales of the scale
 ** space @a self.
 **/

void
vl_scalespace_init (VlScaleSpace *self, float const *image)
{
  vl_index o ;
  _vl_scalespace_start_octave_from_image(self, image, self->firstOctave) ;
  _vl_scalespace_fill_octave(self, self->firstOctave) ;
  for (o = self->firstOctave + 1 ; o <= self->lastOctave ; ++o) {
    _vl_scalespace_start_octave_from_previous_octave(self, o) ;
    _vl_scalespace_fill_octave(self, o) ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Apply a function to all levels of the scale space
 ** @param self ::VlScaleSpace object instance.
 ** @param dst Destination
 ** @param callback Callback applied to all planes
 ** @param params Pointer to auxiliar callback params
 **
 ** This function apply a callback to all the layers of scale space
 ** @a self and stores the result to the scale space @a res.
 ** @a self and @a res must have the same number of octaves and
 ** number of levels in each octave. The size of particular planes
 ** can differ however must be handled by the callback.
 **/

VL_EXPORT void
vl_scalespace_apply (VlScaleSpace const *self, VlScaleSpace *dst,
                     VlScaleSpaceCallback *callback, void *params)
{
  vl_index o ;

  for (o = dst->firstOctave ; o <= dst->lastOctave ; ++o){
    vl_index src_w = vl_scalespace_get_octave_width (self, o) ;
    vl_index src_h = vl_scalespace_get_octave_height(self, o) ;
    vl_index dst_w = vl_scalespace_get_octave_width (dst, o) ;
    vl_index dst_h = vl_scalespace_get_octave_height(dst, o) ;
    vl_index s ;
    for(s = dst->firstLevel ; s <= dst->lastLevel ; ++s) {
      float *src_pt, *dst_pt;
      src_pt = vl_scalespace_get_octave (self, o, s) ;
      dst_pt = vl_scalespace_get_octave (dst, o, s) ;
      callback(src_pt, src_w, src_h, dst_pt, dst_w, dst_h, o, s, params);
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief Compute the scale derivative
 ** @param self ::VlScaleSpace object instance to differentiate.
 ** @param dst output ::VlScaleSpace object instance.
 **
 ** This function computes the differences of the scale level of
 ** the scale space and stores the result to a second scale sapce @a dst.
 ** If the scale space is a Gaussian scale space, then @a dst results
 ** in the so called Difference of Gaussian (DoG).
 **
 ** The geometry of @a dst must be compatible with the geometry of
 ** the scale space @a self. Specifically, the size of @a dst must be identical to the
 ** size of the scale space expect for the fact that there is one scale level
 ** less per octave:
 **
 ** @code
 **  vl_scalespace_get_level_min(self) = vl_scalespace_get_level_min(dst) + 1
 ** @endcode
 **/

void
vl_scalespace_diff (VlScaleSpace const* self, VlScaleSpace *dst)
{
  vl_index o, s ;
  assert(self) ;
  assert(dst) ;
  assert(self->firstOctave == dst->firstOctave) ;
  assert(self->lastOctave == dst->lastOctave) ;
  assert(self->firstLevel == dst->firstLevel) ;
  assert(self->lastLevel == dst->lastLevel + 1) ;

  for (o = self->firstOctave ; o <= self->lastOctave ; ++o) {
    vl_size width = vl_scalespace_get_octave_width(self, o) ;
    vl_size height = vl_scalespace_get_octave_height(self, o) ;
    for (s = self->firstLevel ; s <= self->lastLevel - 1 ; ++s) {
      float const *src_a = vl_scalespace_get_octave (self, o, s) ;
      float const *src_b = vl_scalespace_get_octave (self, o, s + 1) ;
      float const *end_a = src_a + width * height ;
      float *ptr = vl_scalespace_get_octave (dst,  o, s) ;
      while (src_a != end_a) {
        *ptr++ = *src_b++ - *src_a++ ;
      }
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief Detect local extrema
 ** @param self ::VlScaleSpace object.
 ** @param peakThreshold
 ** @param borderSize
 **
 ** The function detects the local extrema in the scale space and stores
 ** the result in the internal feature frame buffer. The feature frames
 ** can be retrieved by ::vl_scalespace_get_frames.
 **
 ** @sa ::vl_scalespace_find_local_extrema()
 **/

void
vl_scalespace_find_local_extrema (VlScaleSpace * self,
                                  double peakThreshold,
                                  vl_size borderSize)
{
  vl_index octave ;
  assert(self) ;

  for (octave = self->firstOctave ; octave <= self->lastOctave ; ++ octave) {
    vl_size width = vl_scalespace_get_octave_width(self, octave) ;
    vl_size height = vl_scalespace_get_octave_height(self, octave) ;

    /* strides in scale and space */
    vl_size const xo = 1 ;
    vl_size const yo = width ;
    vl_size const so = width * height ;

    vl_index x, y, s ;
    float *pt ;
    VlScaleSpaceFrame *frame ;

    /* ---------------------------------------------------------------
     *                                        Find local maxima of DoG
     * ------------------------------------------------------------ */

#define CHECK_NEIGHBORS(v,CMP,SGN) (      \
v CMP ## = SGN 0.8 * peakThreshold &&     \
v CMP *(pt + xo) &&                       \
v CMP *(pt - xo) &&                       \
v CMP *(pt + so) &&                       \
v CMP *(pt - so) &&                       \
v CMP *(pt + yo) &&                       \
v CMP *(pt - yo) &&                       \
\
v CMP *(pt + yo + xo) &&                  \
v CMP *(pt + yo - xo) &&                  \
v CMP *(pt - yo + xo) &&                  \
v CMP *(pt - yo - xo) &&                  \
\
v CMP *(pt + xo      + so) &&             \
v CMP *(pt - xo      + so) &&             \
v CMP *(pt + yo      + so) &&             \
v CMP *(pt - yo      + so) &&             \
v CMP *(pt + yo + xo + so) &&             \
v CMP *(pt + yo - xo + so) &&             \
v CMP *(pt - yo + xo + so) &&             \
v CMP *(pt - yo - xo + so) &&             \
\
v CMP *(pt + xo      - so) &&             \
v CMP *(pt - xo      - so) &&             \
v CMP *(pt + yo      - so) &&             \
v CMP *(pt - yo      - so) &&             \
v CMP *(pt + yo + xo - so) &&             \
v CMP *(pt + yo - xo - so) &&             \
v CMP *(pt - yo + xo - so) &&             \
v CMP *(pt - yo - xo - so) )

    pt = vl_scalespace_get_octave (self, octave, self->firstLevel) ;
    /* start from [1,1,s_min+1] */
    pt += xo * (1 + borderSize) + yo * (1 + borderSize) + so ;

    /* Go over all levels without the top and bottom one */
    for(s = self->firstLevel + 1 ; s <= self->lastLevel - 1 ; ++s) {
      for(y = 1 + borderSize ; y < (signed)height - 1 - (signed)borderSize; ++y) {
        for(x = 1 + borderSize ; x < (signed)width - 1 - (signed)borderSize; ++x) {
          float value = *pt ;
          if (CHECK_NEIGHBORS(value,>,+) || CHECK_NEIGHBORS(value,<,-)) {
            /* make room for more frames */
            if (self->numFrames >= self->framesSize) {
              self->framesSize += 500 ;
              if (self->frames) {
                self->frames = vl_realloc (self->frames,
                                           self->framesSize *
                                           sizeof(VlScaleSpaceFrame)) ;
              } else {
                self->frames = vl_malloc (self->framesSize *
                                          sizeof(VlScaleSpaceFrame)) ;
              }
            }

            frame = self->frames + (self->numFrames ++) ;
            frame->o  = octave;
            frame->ix = x ;
            frame->iy = y ;
            frame->is = s ;
          }
          pt += 1 ;
        }
        pt += 2 * (1 + borderSize) ;
      }
      pt += 2 * (1 + borderSize) * yo ;
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief
 ** @param self ::VlScaleSpace object instance.
 ** @param peakThreshold peak threshold.
 ** @param edgeThreshold edge threshold.
 ** @param borderSize  boderd size.
 ** @return status.
 **
 ** This function filters and adjust with sub-pixel accuracy
 ** the local extrema found by ::vl_scalespace_find_local_extrema().
 **
 ** @f[
 **   [-\frac{\mathtt{patchWidth}}{2}, +\frac{\mathtt{patchWidth}}{2}]
 ** @f]
 **
 ** The function returns ::VL_ERR_ALLOC if there is not enough
 ** free memory to complete the operation.
 **/

int
vl_scalespace_extract_affine_patch (VlScaleSpace *self,
                                    float *patch,
                                    vl_size patchWidth,
                                    vl_size patchHeight,
                                    double patchSigma,
                                    double t1, double t2,
                                    double a11, double a21,
                                    double a12, double a22)
{
  vl_index o, s ;
  float const *image ;
  float *iter ;
  vl_size width ;
  vl_size height ;
  double sigma ;
  double logScale ;
  double factor ;
  double xhat, yhat ;
  double x0hat = - (double)(patchWidth - 1) / 2.0 ;
  double x1hat = + (double)(patchWidth - 1) / 2.0 ;
  double y0hat = - (double)(patchHeight - 1) / 2.0 ;
  double y1hat = + (double)(patchHeight - 1) / 2.0 ;
  double absDetA = fabs(a11*a22 - a12*a21) ;

  assert(self) ;
  assert(patch) ;
  assert(patchSigma > 0) ;
  assert(patchWidth > 0) ;
  assert(patchHeight > 0) ;
  assert(absDetA > 0) ;

  /*
   The isotropic patchSigma smoothing in the patch domain becomes
   an anisotropic smoothing in the image domain of about
   patchSigma * sqrt(det(A)) (this is exact if A is an isotropic transformation).

   Find the octave that matches best this smoothing and prepare
   to operate there.
   */
  sigma = patchSigma * sqrt(absDetA) ;
  logScale = vl_log2_d(sigma / self->sigma0) ;
  o = floor(logScale) ;
  o = VL_MIN(o, self->lastOctave) ;
  o = VL_MAX(o, self->firstOctave) ;
  s = vl_round_d(self->numLevels * (logScale - o)) ;
  s = VL_MIN(s, self->lastLevel) ;
  s = VL_MAX(s, self->firstLevel) ;
  factor = pow(2.0, - o) ;
  a11 *= factor ;
  a21 *= factor ;
  a12 *= factor ;
  a22 *= factor ;
  t1 *= factor ;
  t2 *= factor ;

  image = vl_scalespace_get_octave(self, o, s) ;
  width = vl_scalespace_get_octave_width(self, o) ;
  height = vl_scalespace_get_octave_height(self, o) ;

  /*
   If out-of-bounds, copy and extend the source patch.
   */
  {
    /*
     Warp the patch domain [x0hat,y0hat,x1hat,y1hat] to the image domain/
     Obtain a box [x0,y0,x1,y1] enclosing that wrapped box, and then
     an integer vertexes version [x0i, y0i, x1i, y1i], making room
     for one pixel at the boundaty to simplify bilinear interpolation
     later on.
     */
    vl_index x0i, y0i, x1i, y1i ;
    double x0 = +VL_INFINITY_D ;
    double x1 = -VL_INFINITY_D ;
    double y0 = +VL_INFINITY_D ;
    double y1 = -VL_INFINITY_D ;
    double boxx [4] = {x1hat, x1hat, x0hat, x0hat} ;
    double boxy [4] = {y0hat, y1hat, y1hat, y0hat} ;
    int i ;
    for (i = 0 ; i < 4 ; ++i) {
      double x = a11 * boxx[i] + a12 * boxy[i] + t1 ;
      double y = a21 * boxx[i] + a22 * boxy[i] + t2 ;
      x0 = VL_MIN(x0, x) ;
      x1 = VL_MAX(x1, x) ;
      y0 = VL_MIN(y0, y) ;
      y1 = VL_MAX(y1, y) ;
    }
    x0i = floor(x0) - 1 ;
    y0i = floor(y0) - 1 ;
    x1i = ceil(x1) + 1 ;
    y1i = ceil(y1) + 1 ;

    /*
     If the box [x0i,y0i,x1i,y1i] is not fully contained in the
     image domain, then create a copy of this region by padding
     the image. The image is extended by continuity.
     */

    if (x0i < 0 || x1i > (signed)width-1 ||
        y0i < 0 || y1i > (signed)height-1) {
      return VL_ERR_BAD_ARG;
      int xi, yi ;
      int padx0 = VL_MAX(0, - x0i) ;
      int pady0 = VL_MAX(0, - y0i) ;
      int padx1 = VL_MAX(0, x1i - ((signed)width - 1)) ;
      int pady1 = VL_MAX(0, y1i - ((signed)height - 1)) ;
      int selfPatchWidth = x1i - x0i + 1 ;
      int selfPatchHeight = y1i - y0i + 1 ;
      vl_size selfPatchSize = selfPatchWidth * selfPatchHeight ;
      if (selfPatchSize > self->patchSize) {
        float *newPatch = vl_realloc(self->patch, selfPatchSize * sizeof(float)) ;
        if (newPatch == NULL) return vl_set_last_error(VL_ERR_ALLOC, NULL) ;
        self->patch = newPatch ;
        self->patchSize = selfPatchSize ;
      }
      for (yi = pady0 ; yi < selfPatchHeight - pady1 ; ++ yi) {
        float *dst = self->patch + yi * selfPatchWidth ;
        float const *src = image + (yi - pady0) * width ;
        for (xi = 0 ; xi < padx0 ; ++xi) *dst++ = *src ;
        for ( ; xi < selfPatchWidth - padx1 - 1 ; ++xi) *dst++ = *src++ ;
        for ( ; xi < selfPatchWidth ; ++xi) *dst++ = *src ;
      }
      for (yi = 0 ; yi < pady0 ; ++yi) {
        memcpy(self->patch + yi * selfPatchWidth,
               self->patch + pady0 * selfPatchWidth,
               selfPatchWidth * sizeof(float)) ;
      }
      for (yi = selfPatchHeight - pady1 ; yi < selfPatchHeight ; ++yi) {
        memcpy(self->patch + yi * selfPatchWidth,
               self->patch + (selfPatchHeight - pady1 - 1) * selfPatchWidth,
               selfPatchWidth * sizeof(float)) ;
      }
#if 0
      {
        char name [200] ;
        snprintf(name, 200, "/Users/vedaldi/Desktop/bla/%20.0f-ext.pgm", 1e10*vl_get_cpu_time()) ;
        vl_pgm_write_f(name, self->patch, selfPatchWidth, selfPatchHeight) ;
      }
#endif

      image = self->patch ;
      width = selfPatchWidth ;
      height = selfPatchHeight ;
      t1 -= x0i ;
      t2 -= y0i ;
    }
  }

  /*
   Warp the patch.
   */
  iter = patch ;
  for (yhat = y0hat ; yhat <= y1hat ; yhat += 1.0) {
    double rx = a12 * yhat + t1 ;
    double ry = a22 * yhat + t2 ;
    for (xhat = x0hat ; xhat <= x1hat ; xhat += 1.0) {
      double x = a11 * xhat + rx ;
      double y = a21 * xhat + ry ;
      vl_index xi = vl_floor_d(x) ;
      vl_index yi = vl_floor_d(y) ;
      double i00 = image[yi * width + xi] ;
      double i10 = image[yi * width + xi + 1] ;
      double i01 = image[(yi + 1) * width + xi] ;
      double i11 = image[(yi + 1) * width + xi + 1] ;
      double wx = x - xi ;
      double wy = y - yi ;

      assert(xi >= 0 && xi <= (signed)width - 1) ;
      assert(yi >= 0 && yi <= (signed)height - 1) ;

      *iter++ =
      (1.0 - wy) * ((1.0 - wx) * i00 + wx * i10) + wy * ((1.0 - wx) * i01 + wx * i11) ;
    }
  }

#if 0
  {
    char name [200] ;
    snprintf(name, 200, "/Users/vedaldi/Desktop/bla/%20.0f.pgm", 1e10*vl_get_cpu_time()) ;
    vl_pgm_write_f(name, patch, patchWidth, patchHeight) ;
  }
#endif

  /*
   Do additional smoothing if needed.
   */

  /* TO BE DONE */
  return VL_ERR_OK ;
}

/** ------------------------------------------------------------------
 ** @brief Refine local extrema
 ** @param self ::VlScaleSpace object instance.
 ** @param peakThreshold peak threshold.
 ** @param edgeThreshold edge threshold.
 ** @param borderSize  boderd size.
 **
 ** This function filters and adjust with sub-pixel accuracy
 ** the local extrema found by ::vl_scalespace_find_local_extrema().
 **
 ** @sa ::vl_scalespace_find_local_extrema()
 **/

void
vl_scalespace_refine_local_extrema (VlScaleSpace *self,
                                    double peakThreshold,
                                    double edgeThreshold,
                                    vl_size borderSize)
{
  vl_index const xo = 1 ;
  vl_index ii, jj ;
  vl_uindex ki ;
  float *pt ;
  VlScaleSpaceFrame *frame = self->frames ;

  for (ki = 0 ; ki < self->numFrames ; ++ki) {
    vl_index x = self->frames[ki].ix ;
    vl_index y = self->frames[ki].iy ;
    vl_index s = self->frames[ki].is ;
    vl_index o = self->frames[ki].o ;
    vl_index width = vl_scalespace_get_octave_width(self, o) ;
    vl_index height = vl_scalespace_get_octave_height(self, o) ;
    vl_index const yo = width ;
    vl_index const so = width * height ;
    double xper = pow (2.0, o) ; /* real distance between pixels */

    double Dx=0,Dy=0,Ds=0,Dxx=0,Dyy=0,Dss=0,Dxy=0,Dxs=0,Dys=0 ;
    double A [3*3], b [3] ;

    vl_index dx = 0 ;
    vl_index dy = 0 ;
    vl_index iter, i, j ;

    for (iter = 0 ; iter < 5 ; ++iter) {
      x += dx ;
      y += dy ;
      pt = vl_scalespace_get_octave(self, o, s) + xo * x + yo * y ;

      /** @brief Index GSS @internal */
#define at(dx,dy,ds) (*( pt + (dx)*xo + (dy)*yo + (ds)*so))

      /** @brief Index matrix A @internal */
#define Aat(i,j)     (A[(i)+(j)*3])

      /* compute the gradient */
      Dx = 0.5 * (at(+1,0,0) - at(-1,0,0)) ;
      Dy = 0.5 * (at(0,+1,0) - at(0,-1,0));
      Ds = 0.5 * (at(0,0,+1) - at(0,0,-1)) ;

      /* compute the Hessian */
      Dxx = (at(+1,0,0) + at(-1,0,0) - 2.0 * at(0,0,0)) ;
      Dyy = (at(0,+1,0) + at(0,-1,0) - 2.0 * at(0,0,0)) ;
      Dss = (at(0,0,+1) + at(0,0,-1) - 2.0 * at(0,0,0)) ;

      Dxy = 0.25 * (at(+1,+1,0) + at(-1,-1,0) - at(-1,+1,0) - at(+1,-1,0)) ;
      Dxs = 0.25 * (at(+1,0,+1) + at(-1,0,-1) - at(-1,0,+1) - at(+1,0,-1)) ;
      Dys = 0.25 * (at(0,+1,+1) + at(0,-1,-1) - at(0,-1,+1) - at(0,+1,-1)) ;


      /* solve linear system ....................................... */
      /* TODO: switch to use the direct calculation below?
       | a11 a12 a13 |-1             |   a33a22-a32a23  -(a33a12-a32a13)   a23a12-a22a13  |
       | a21 a22 a23 |    =  1/DET * | -(a33a21-a31a23)   a33a11-a31a13  -(a23a11-a21a13) |
       | a31 a32 a33 |               |   a32a21-a31a22  -(a32a11-a31a12)   a22a11-a21a12  |
       with DET  =  a11(a33a22-a32a23)-a21(a33a12-a32a13)+a31(a23a12-a22a13)
       */
      Aat(0,0) = Dxx ;
      Aat(1,1) = Dyy ;
      Aat(2,2) = Dss ;
      Aat(0,1) = Aat(1,0) = Dxy ;
      Aat(0,2) = Aat(2,0) = Dxs ;
      Aat(1,2) = Aat(2,1) = Dys ;

      b[0] = - Dx ;
      b[1] = - Dy ;
      b[2] = - Ds ;

      /* Gauss elimination */
      for(j = 0 ; j < 3 ; ++j) {
        double maxa = 0 ;
        double maxabsa = 0 ;
        vl_index maxi = -1 ;
        double tmp ;

        /* look for the maximally stable pivot */
        for (i = j ; i < 3 ; ++i) {
          double a    = Aat (i,j) ;
          double absa = vl_abs_d (a) ;
          if (absa > maxabsa) {
            maxa    = a ;
            maxabsa = absa ;
            maxi = i ;
          }
        }

        /* if singular give up */
        if (maxabsa < 1e-10f) {
          b[0] = 0 ;
          b[1] = 0 ;
          b[2] = 0 ;
          break ;
        }

        i = maxi ;

        /* swap j-th row with i-th row and normalize j-th row */
        for(jj = j ; jj < 3 ; ++jj) {
          tmp = Aat(i,jj) ; Aat(i,jj) = Aat(j,jj) ; Aat(j,jj) = tmp ;
          Aat(j,jj) /= maxa ;
        }
        tmp = b[j] ; b[j] = b[i] ; b[i] = tmp ;
        b[j] /= maxa ;

        /* elimination */
        for (ii = j+1 ; ii < 3 ; ++ii) {
          double x = Aat(ii,j) ;
          for (jj = j ; jj < 3 ; ++jj) {
            Aat(ii,jj) -= x * Aat(j,jj) ;
          }
          b[ii] -= x * b[j] ;
        }
      }

      /* backward substitution */
      for (i = 2 ; i > 0 ; --i) {
        double x = b[i] ;
        for (ii = i-1 ; ii >= 0 ; --ii) {
          b[ii] -= x * Aat(ii,i) ;
        }
      }

      /* .......................................................... */
      /* If the translation of the frame is big, move the frame
       * and re-iterate the computation. Otherwise we are all set.
       */

      dx= ((b[0] >  0.6 && x < (signed)width - 2 - (signed)borderSize) ?  1 : 0)
        + ((b[0] < -0.6 && x > 1 + (signed)borderSize) ? -1 : 0) ;

      dy= ((b[1] >  0.6 && y < (signed)height - 2 - (signed)borderSize) ?  1 : 0)
        + ((b[1] < -0.6 && y > 1 + (signed)borderSize    ) ? -1 : 0) ;

      if (dx == 0 && dy == 0) break ;
    }

    /* check threshold and other conditions */
    {
      double val = at(0,0,0)
        + 0.5 * (Dx * b[0] + Dy * b[1] + Ds * b[2]) ;
      double score = (Dxx+Dyy)*(Dxx+Dyy) / (Dxx*Dyy - Dxy*Dxy) ;
      double xn = x + b[0] ;
      double yn = y + b[1] ;
      double sn = s + b[2] ;

      vl_bool good =
        vl_abs_d(val) > peakThreshold &&
        0 <= score && score < (edgeThreshold+1)*(edgeThreshold+1)/edgeThreshold &&
        vl_abs_d(b[0]) < 1.5 &&
        vl_abs_d(b[1]) < 1.5 &&
        vl_abs_d(b[2]) < 1.5 &&
        0 <= xn  && xn <= (signed)width - 1 &&
        0 <= yn  && yn <= (signed)height - 1 &&
        self->firstLevel <= sn && sn <= self->lastLevel ;

      if (good) {
        frame->o = o ;
        frame->ix = x ;
        frame->iy = y ;
        frame->is = s ;
        frame->s = sn ;
        frame->x = xn * xper ;
        frame->y = yn * xper ;
        frame->sigma = self->sigma0 * pow (2.0, sn/self->numLevels) * xper ;
        ++ frame ;
      }
    } /* done checking */
  } /* next frame to refine */

  /* update frame count */
  self-> numFrames = frame - self->frames ;
}

/** ------------------------------------------------------------------
 ** @brief Initialize a frame from its position and scale
 ** @param f     Scale space object.
 ** @param k     Scale space frame (output).
 ** @param x     x coordinate of the frame center.
 ** @param y     y coordinate of the frame center.
 ** @param sigma frame scale.
 **
 ** The function initializes a frame structure @a k from
 ** the location @a x
 ** and @a y and the scale @a sigma of the frame. The frame structure
 ** maps the frame to an octave and scale level of the discretized
 ** Gaussian scale space, which is required for instance to compute the
 ** frame SIFT descriptor.
 **
 ** @par Algorithm
 **
 ** The formula linking the frame scale sigma to the octave and
 ** scale indexes is
 **
 ** @f[ \sigma(o,s) = \sigma_0 2^{o+s/S} @f]
 **
 ** In addition to the scale index @e s (which can be fractional due
 ** to scale interpolation) a frame has an integer scale index @e
 ** is too (which is the index of the scale level where it was
 ** detected in the DoG scale space). We have the constraints (@ref
 ** sift-tech-detector see also the "SIFT detector"):
 **
 ** - @e o is integer in the range @f$ [o_\mathrm{min},
 **   o_{\mathrm{min}}+O-1] @f$.
 ** - @e is is integer in the range @f$ [s_\mathrm{min}+1,
 **   s_\mathrm{max}-2] @f$.  This depends on how the scale is
 **   determined during detection, and must be so here because
 **   gradients are computed only for this range of scale levels
 **   and are required for the calculation of the SIFT descriptor.
 ** - @f$ |s - is| < 0.5 @f$ for detected frames in most cases due
 **   to the interpolation technique used during detection. However
 **   this is not necessary.
 **
 ** Thus octave o represents scales @f$ \{ \sigma(o, s) : s \in
 ** [s_\mathrm{min}+1-.5, s_\mathrm{max}-2+.5] \} @f$. Note that some
 ** scales may be represented more than once. For each scale, we
 ** select the largest possible octave that contains it, i.e.
 **
 ** @f[
 **  o(\sigma)
 **  = \max \{ o \in \mathbb{Z} :
 **    \sigma_0 2^{\frac{s_\mathrm{min}+1-.5}{S}} \leq \sigma \}
 **  = \mathrm{floor}\,\left[
 **    \log_2(\sigma / \sigma_0) - \frac{s_\mathrm{min}+1-.5}{S}\right]
 ** @f]
 **
 ** and then
 **
 ** @f[
 ** s(\sigma) = S  \left[\log_2(\sigma / \sigma_0) - o(\sigma)\right],
 ** \quad
 ** is(\sigma) = \mathrm{round}\,(s(\sigma))
 ** @f]
 **
 ** In practice, both @f$ o(\sigma) @f$ and @f$ is(\sigma) @f$ are
 ** clamped to their feasible range as determined by the SIFT filter
 ** parameters.
 **/

VL_EXPORT void
vl_scalespace_frame_init (VlScaleSpace const *self,
                          VlScaleSpaceFrame *frame,
                          double x,
                          double y,
                          double sigma)
{
  vl_index o, ix, iy, is ;
  double s, phi, xper ;

  phi = vl_log2_d ((sigma + VL_EPSILON_D) / self->sigma0) ;
  o = vl_floor_d (phi -  ((double) self->firstLevel + 0.5) / self->numLevels) ;
  o = VL_MIN (o, self->firstOctave + (signed)self->numOctaves - 1) ;
  o = VL_MAX (o, self->firstOctave) ;
  s = self->numLevels * (phi - o) ;

  is = (int)(s + 0.5) ;
  is = VL_MIN(is, self->lastLevel - 1) ;
  is = VL_MAX(is, self->firstLevel + 1) ;

  xper = pow (2.0, o) ;
  ix = (int)(x / xper + 0.5) ;
  iy = (int)(y / xper + 0.5) ;

  frame->o  = o ;
  frame->ix = ix ;
  frame->iy = iy ;
  frame->is = is ;
  frame->x = x ;
  frame->y = y ;
  frame->s = s ;
  frame->sigma = sigma ;
}
