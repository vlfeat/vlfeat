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
 
 
**/

/* todo: complete
 
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
object. This can be created as follows:

@code
VlScaleSpace ss = vl_scalespace_new(width, height,
                                    numOctaves, firstOctave,
                                    octaveResolution,
                                    octaveFirstSubdivision,
                                    octaveLastSubdivision) ;
@endcode
 
Here @c width and @c height are the image dimension, @c numOctave
is the total number of spanned octaves, and @c firstOctave
is the index of the first octave, @c octaveResolution is the number
of subdivisions per octave, and @c octaveFirstSubdivision
and @c octaveLastSubdivision define the range of the octave.



<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection scalespace-tech Scale space
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The Gaussian scale space of an image @f$ \ell(x,y) @f$ is defined as
the three-dimesnional function
 
@f[
\ell(x,y,\sigma) = (g_\sigma * \ell)(x,y),  \quad \sigma \geq 0.
@f]
 
where
 
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
 

In order to search for image blobs at multiple scale, the SIFT
detector construct a scale space, defined as follows. Let
@f$I_0(\mathbf{x})@f$ denote an idealized <em>infinite resolution</em>
image. Consider the  <em>Gaussian kernel</em>



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


/* ---------------------------------------------------------------- */

/** @brief Get the geometry of the scale space.
 ** @param object.
 ** @return the scale space geometry.
 **/

VlScaleSpaceGeometry
vl_scalespace_get_geometry (VlScaleSpace const * self)
{
  return self->geom ;
}

/** @brief Get the geometry of an octave of the scalespace.
 ** @param object.
 ** @param o octave index.
 ** @return the geometry of the specified octave.
 **/

VlScaleSpaceOctaveGeometry
vl_scalespace_get_octave_geometry (VlScaleSpace const * self, vl_index o)
{
  VlScaleSpaceOctaveGeometry ogeom ;
  ogeom.width = VL_SHIFT_LEFT(self->geom.width, -o) ;
  ogeom.height = VL_SHIFT_LEFT(self->geom.height, -o) ;
  ogeom.step = pow(2.0, o) ;
  return ogeom ;
}


/** @brief Get the const data of a scale space level
 ** @param self object.
 ** @param o octave index.
 ** @param s level index.
 ** @return pointer to the data for octave @a o, level @a s.
 **
 ** The octave index @a o must be in the range @c firstOctave
 ** to @c lastOctave and the scale index @a s must be in the
 ** range @c octaveFirstSubdivision to @c octaveLastSubdivision.
 **/

float *
vl_scalespace_get_level (VlScaleSpace const *self, vl_index o, vl_index s)
{
  VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self,o) ;
  float * octave ;
  assert(self) ;
  assert(o >= self->geom.firstOctave) ;
  assert(o <= self->geom.lastOctave) ;
  assert(s >= self->geom.octaveFirstSubdivision) ;
  assert(s <= self->geom.octaveLastSubdivision) ;

  octave = self->octaves[o - self->geom.firstOctave] ;
  return octave + ogeom.width * ogeom.height * (s - self->geom.octaveFirstSubdivision) ;
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


VlScaleSpace *
vl_scalespace_new_with_geometry (VlScaleSpaceGeometry geom)
{
  vl_index o ;
  vl_size totalNumLevels = geom.octaveLastSubdivision - geom.octaveFirstSubdivision + 1 ;
  VlScaleSpace *self = vl_calloc(1, sizeof(VlScaleSpace)) ;
  if (self == NULL) goto err_alloc_self ;

  self->geom = geom ;
  self->octaves = vl_calloc(geom.lastOctave - geom.firstOctave + 1, sizeof(float*)) ;
  if (self->octaves == NULL) goto err_alloc_octaves ;
  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self,o) ;
    vl_size octaveSize = ogeom.width * ogeom.height * totalNumLevels ;
    self->octaves[o - self->geom.firstOctave] = vl_malloc(octaveSize * sizeof(float)) ;
    if (self->octaves[o - self->geom.firstOctave] == NULL) goto err_alloc_octaves_o ;
  }
  return self ;

err_alloc_smoother: ;
err_alloc_octaves_o:
  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    if (self->octaves[o - self->geom.firstOctave]) {
      vl_free(self->octaves[o - self->geom.firstOctave]) ;
    }
  }
err_alloc_octaves:
  vl_free(self) ;
err_alloc_self:
  /* todo: flag error */
  return NULL ;

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
                   vl_size octaveResolution,
                   vl_index octaveFirstSubdivision,
                   vl_index octaveLastSubdivision)
{
  VlScaleSpaceGeometry geom ;
  vl_size baseWidth, baseHeight, numElements ;

  assert(octaveLastSubdivision >= octaveFirstSubdivision) ;
  assert(octaveFirstSubdivision <= 0) ;
  assert(octaveLastSubdivision >= (signed)octaveResolution - 1) ;

  /* automatically figure out the number of octaves if needed */
  baseWidth = VL_SHIFT_LEFT(width, -firstOctave) ;
  baseHeight = VL_SHIFT_LEFT(height, -firstOctave) ;
  numElements = baseWidth * baseHeight ;
  if (numOctaves < 0) {
    numOctaves = VL_MAX(floor(vl_log2_d(VL_MIN(width, height))) - firstOctave - 3, 1) ;
  }

  geom.width = width ;
  geom.height = height ;
  geom.firstOctave = firstOctave ;
  geom.lastOctave = numOctaves + firstOctave - 1 ;
  geom.octaveResolution= octaveResolution ;
  geom.octaveFirstSubdivision = octaveFirstSubdivision ;
  geom.octaveLastSubdivision = octaveLastSubdivision ;
  geom.sigma0 = 1.6 * pow(2.0, 1.0 / octaveResolution) ;
  geom.sigman = 0.5 ;

  return vl_scalespace_new_with_geometry (geom) ;
}

/** @brief Delete a ::VlScaleSpace object
 ** @param self object to delete.
 ** @sa ::vl_scalespace_new
 **/

void
vl_scalespace_delete (VlScaleSpace *self)
{
  if (self) {
    if (self->octaves) {
      vl_index o ;
      for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
        if (self->octaves[o - self->geom.firstOctave]) {
          vl_free(self->octaves[o - self->geom.firstOctave]) ;
        }
      }
      vl_free(self->octaves) ;
    }
    vl_free(self) ;
  }
}

/** @brief Clone the scale space with all its data
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
  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self,o) ;
    vl_size totalNumLevels = self->geom.octaveLastSubdivision - self->geom.octaveFirstSubdivision + 1;
    memcpy(copy->octaves[o - self->geom.firstOctave],
           self->octaves[o - self->geom.firstOctave],
           ogeom.width * ogeom.height * totalNumLevels * sizeof(float)) ;
  }
  return copy ;

err_alloc_copy:
  /* todo: flag error */
  return NULL ;
}

/** @brief Clone the object but do not copy the data
 ** @param self ::VlScaleSpace object instance.
 ** @return the object copy, or @c NULL.
 **
 ** The function can fail if the memory is insufficient to hold the
 ** object copy. In this case, the function returns @c NULL.
 **/

VlScaleSpace *
vl_scalespace_clone_structure (VlScaleSpace* self)
{
  return vl_scalespace_new_with_geometry (self->geom) ;
}

double
vl_scalespace_get_level_sigma (VlScaleSpace const *self, vl_index o, vl_index s)
{
  return self->geom.sigma0 * pow(2.0, o + (double) s / self->geom.octaveResolution) ;
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
  VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self, o) ;

  for(s = self->geom.octaveFirstSubdivision + 1 ;
      s <= self->geom.octaveLastSubdivision ; ++s) {
    double sigma = vl_scalespace_get_level_sigma(self, o, s) ;
    double previousSigma = vl_scalespace_get_level_sigma(self, o, s - 1) ;
    double deltaSigma = sqrtf(sigma*sigma - previousSigma*previousSigma) ;

    float* level = vl_scalespace_get_level (self, o, s) ;
    float* previous = vl_scalespace_get_level (self, o, s-1) ;
    vl_imsmooth_f (level, ogeom.width,
                   previous, ogeom.width, ogeom.height, ogeom.width,
                   deltaSigma / ogeom.step, deltaSigma / ogeom.step) ;
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
  double sigma, imageSigma ;
  vl_index op ;

  assert(self) ;
  assert(image) ;
  assert(o >= self->geom.firstOctave) ;
  assert(o <= self->geom.lastOctave) ;

  /*
   * Copy the image to self->geom.octaveFirstSubdivision of octave o, upscaling or
   * downscaling as needed.
   */

  level = vl_scalespace_get_level(self, VL_MAX(0, o), self->geom.octaveFirstSubdivision) ;
  copy_and_downsample(level, image, self->geom.width, self->geom.height, VL_MAX(0, o)) ;

  for (op = -1 ; op >= o ; --op) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self, op + 1) ;
    float *succLevel = vl_scalespace_get_level(self, op + 1, self->geom.octaveFirstSubdivision) ;
    level = vl_scalespace_get_level(self, op, self->geom.octaveFirstSubdivision) ;
    copy_and_upsample(level, succLevel, ogeom.width, ogeom.height) ;
  }

  /*
   * Adjust the smoothing of the first level just initialised, accounting
   * for the fact that the input image is assumed to be a nominal scale
   * level self->sigman.
   */

  sigma = vl_scalespace_get_level_sigma(self, o, self->geom.octaveFirstSubdivision) ;
  imageSigma = self->geom.sigman ;

  if (sigma > imageSigma) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self, o) ;
    double deltaSigma = sqrt (sigma*sigma - imageSigma*imageSigma) ;
    level = vl_scalespace_get_level (self, o, self->geom.octaveFirstSubdivision) ;
    vl_imsmooth_f (level, ogeom.width,
                   level, ogeom.width, ogeom.height, ogeom.width,
                   deltaSigma / ogeom.step, deltaSigma / ogeom.step) ;
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
  double sigma, prevSigma ;
  float *level, *prevLevel ;
  vl_index prevLevelIndex ;
  VlScaleSpaceOctaveGeometry ogeom ;

  assert(self) ;
  assert(o > self->geom.firstOctave) ; /* must not be the first octave */
  assert(o <= self->geom.lastOctave) ;

  /*
   * From the previous octave pick the level which is closer to
   * self->geom.octaveFirstSubdivision in this octave.
   * The is self->geom.octaveFirstSubdivision + self->numLevels since there are
   * self->numLevels levels in an octave, provided that
   * this value does not exceed self->geom.octaveLastSubdivision.
   */

  prevLevelIndex = VL_MIN(self->geom.octaveFirstSubdivision
                          + (signed)self->geom.octaveResolution,
                          self->geom.octaveLastSubdivision) ;
  prevLevel = vl_scalespace_get_level (self, o - 1, prevLevelIndex) ;
  level = vl_scalespace_get_level (self, o, self->geom.octaveFirstSubdivision) ;
  ogeom = vl_scalespace_get_octave_geometry(self, o - 1) ;

  copy_and_downsample (level, prevLevel, ogeom.width, ogeom.height, 1) ;

  /*
   * Add remaining smoothing, if any.
   */

  sigma = vl_scalespace_get_level_sigma(self, o, self->geom.octaveFirstSubdivision) ;
  prevSigma = vl_scalespace_get_level_sigma(self, o - 1, prevLevelIndex) ;


  if (sigma > prevSigma) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self, o) ;
    double deltaSigma = sqrt (sigma*sigma - prevSigma*prevSigma) ;
    level = vl_scalespace_get_level (self, o, self->geom.octaveFirstSubdivision) ;
    vl_imsmooth_f (level, ogeom.width,
                   level, ogeom.width, ogeom.height, ogeom.width,
                   deltaSigma / ogeom.step, deltaSigma / ogeom.step) ;
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
vl_scalespace_put_image (VlScaleSpace *self, float const *image)
{
  vl_index o ;
  _vl_scalespace_start_octave_from_image(self, image, self->geom.firstOctave) ;
  _vl_scalespace_fill_octave(self, self->geom.firstOctave) ;
  for (o = self->geom.firstOctave + 1 ; o <= self->geom.lastOctave ; ++o) {
    _vl_scalespace_start_octave_from_previous_octave(self, o) ;
    _vl_scalespace_fill_octave(self, o) ;
  }
}
