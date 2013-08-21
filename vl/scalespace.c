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
@page scalespace Gaussian Scale Space (GSS)
@author Karel Lenc
@author Andrea Vedaldi
@author Michal Perdoch
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref scalespace.h implements a Gaussian scale space, a data structure
representing an image at multiple resolutions
@cite{witkin83scale-space} @cite{koenderink84the-structure}
@cite{lindeberg94scale-space}. Scale spaces are used, for example, to
detect scale invariant features @cite{lindeberg98principles} such as
SIFT, Hessian-Affine, Harris-Affine, Harris-Laplace, etc.

@tableofcontents

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section scalespace-overview Overview
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

A *scale space* is representation of an image at multiple resolution
levels. An image is a function $\ell(x,y)$ of two coordinates $x$,
$y$; the scale space $\ell(x,y,\sigma)$ adds a third coordinate
$\sigma$ indexing the *scale*. @ref scalespace.h implements in
particular the Gaussian scale space, where the image
$\ell(x,y,\sigma)$ is obtained by smoothing $\ell(x,y)$ by a Gaussian
kernel of isotropic standard deviation $\sigma$.

In practice, the spatial coordiantes $(x,y)$ and the scale coordinate
$\sigma$ are sampled. The density of the spatial sampling is adjusted
as a function of the scale: intuitively, coarser resolution images can
be sampled more coarsely. Thus, the scale space has the structure of a
*pyramid*: a collection of images of progressively coarser resolution
and smaller size (in pixels).

The pyramid is organised in a number of *octaves*, indexed by a
parameter `o`, and further *sublevels* for each octave, indexed by a
parameter `s`. These are related to the scale $\sigma$ by

\[
  \sigma(s,o) = \sigma_o 2^{\displaystyle o + \frac{s}{\mathtt{octaveResolution}}}
\]

where `octaveResolution` is the resolution of the octave subsampling
$\sigma_0$ is the *base smoothing*.

Given an input image `image`, the following example uses the
::VlScaleSpace object to compute the image `level`, which is the
version of `image` at scales `(o,s)`:

@code
float* level ;
VlScaleSpace ss = vl_scalespace_new(imageWidth, imageHeight) ;
vl_scalespace_put_image(ss, image) ;
level = vl_scalespace_get_level(ss, o, s) ;
@endcode

Note that `level` has not necessarily the same dimensions or sampling
density of `image`. The geometry of `level` can be obtained as

@code
VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(ss, o) ;
ogeom.width \\ width of level (in number of pixels)
ogeom.height \\ height of level (in number of pixels)
ogeom.step \\ spatial sampling step
@endcode

This means that $\ell(x,y,\sigma) = \mathtt{level}[i,j]$, where
\[
 x = \mathtt{ogeom.step} \times  i,
 \quad
 0 \leq i < \mathtt{ogeom.width},
\]

and similarly for $y$. The other impotant information in order to use
scale space is to know the range of the paramerters `o` and `s`. This
can be obtained as

@code
VlScaleSpaceGeometry geom = vl_scalespace_get_geomerty(ss) ;
@endcode

So for example `o` varies in the range from `geom.firstOctave` to
`geom.lastOctave` and `s` in the range `geom.octaveFirstSubdivision`
to `geom.octaveLastSubdivision`.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection scalespace-finer Finer control
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

::VlScaleSpace offers more control on how the pyramid is constructed.
Two usage cases are the following:

- In feature detection it is often convenient to sample a few redudant
  sublevels per octave, effectively representing the same scale with
  two different spatial samplings. This simplifies detecting extrema
  in scales at boundary scale levels.

- Often there is a convenience in oversampling (at more than the
  Nyquist frequency) the finer scales. While oversampling does not add
  information to the image, it may simplify significantly the
  implementation of filers such as derivatives.

All this is possible by using a more advanced interface to
::VlScaleSpace.  For example, in oerder to start sampling at octave -1
and have two redundant subdivisions per octave, one can use:

@code
VlScaleSpaceGeometry geom = vl_scalespace_get_default_geometry(imageWidth, imageHeight) ;
geom.firstOctave = -1 ;
geom.octaveFirstSubdivision = -1 ;
geom.octaveLastSubdivision = geom.octaveResolution ;
VlScaleSpacae ss = vl_scalespace_new_with_geometry (geom) ;
@endcode

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section scalespace-algorithm Algorithm and limitations
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

Formally, the *Gaussian scale space* of an image $\ell(x,y)$ is
defined as

\[
   \ell(x,y,\sigma) =
   [g_{\sigma} * \ell](x,y,\sigma)
\]

where $g_\sigma$ denotes a 2D Gaussian kernel of isotropic standard
deviation $\sigma$:

\[
  g_{\sigma}(x,y) = \frac{1}{2\pi\sigma^2}
  \exp\left(
  - \frac{x^2 + y^2}{2\sigma^2}
  \right).
\]

An important detail is that the algorithm is not fed with the dieal
infinite resolution image $\ell(x,y)$, but with a pre-smoothed and
sampled version measured by the CCD. This is modelled by assuming that
the input is in fact the sampled image $\ell(x,y,\sigma_n)$, where
$\sigma_n$ is a *nominal smoothing*, usually taken to be 0.5 (half a
pixel standard deviation). This also means that $\sigma = \sigma_n =
0.5$ is the *finest scale* that can actually be computed.

Given $\ell(x,y,\sigma_n)$, any of a vast number digitial filtering
techniques can be used to compute the succesive scales. Presently,
::VlScaleSpace uses a basic FIR implementation of the Gaussian
filters.

The FIR implementation is obtained by sampling the Gaussian function
and re-normalizing it to have unit norm. As a rule of thumb, such a
filter works sufficiently well for, say, feature detection if the
standard deviation $\sigma$ is at least 1.6 times the spatial sampling
step. The same filter works better still the input image is
oversampled.

The limitations on the FIR filters have impliciations on the pyramid
itself, as the latter is constructed by *incremental smoothing*: each
successive level is obtained from the previous one by adding the
needed amount of smoothing. In this manner, the size of the FIR
filters remains small; however, if the scale sublevels are too packed,
the incremental smoothing is so small that the FIR implementation may
break.
*/

#include "scalespace.h"
#include "mathop.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

/** @file scalespace.h
 ** @struct VlScaleSpace
 ** @brief Scale space class
 **
 ** This is an opaque class used to compute the scale space of an
 ** image.
 **/

struct _VlScaleSpace
{
  VlScaleSpaceGeometry geom ; /**< Geometry of the scale space */
  float **octaves ; /**< Data */
} ;

/* ---------------------------------------------------------------- */
/** @brief Get the default geometry for a given image size.
 ** @param width image width.
 ** @param height image height.
 ** @return the default scale space geometry.
 **
 ** Both @a width and @a height must be at least one pixel wide.
 **/

VlScaleSpaceGeometry
vl_scalespace_get_default_geometry (vl_size width, vl_size height)
{
  VlScaleSpaceGeometry geom ;
  assert(width >= 1) ;
  assert(height >= 1) ;
  geom.width = width ;
  geom.height = height ;
  geom.firstOctave = 0 ;
  geom.lastOctave = VL_MAX(floor(vl_log2_d(VL_MIN(width, height))) - 3, 0) ;
  geom.octaveResolution= 3 ;
  geom.octaveFirstSubdivision = 0 ;
  geom.octaveLastSubdivision = geom.octaveResolution - 1 ;
  geom.baseScale = 1.6 * pow(2.0, 1.0 / geom.octaveResolution) ;
  geom.nominalScale = 0.5 ;
  return geom ;
}

#define is_valid_geometry(geom) (\
geom.firstOctave <= geom.lastOctave && \
geom.octaveResolution >= 1 && \
geom.octaveFirstSubdivision <= geom.octaveLastSubdivision && \
geom.baseScale >= 0.0 && \
geom.nominalScale >= 0.0)

/** @brief Check scale space geometries for equality
 ** @param a first geometry.
 ** @param b second geometry.
 ** @return true if equal.
 **/

vl_bool
vl_scalespacegeometry_is_equal (VlScaleSpaceGeometry a,
                                VlScaleSpaceGeometry b)
{
  return
  a.width == b.width &&
  a.height == b.height &&
  a.firstOctave == b.firstOctave &&
  a.lastOctave == b.lastOctave &&
  a.octaveResolution == b.octaveResolution &&
  a.octaveFirstSubdivision == b.octaveLastSubdivision &&
  a.baseScale == b.baseScale &&
  a.nominalScale == b.nominalScale ;
}

/** @brief Get the geometry of the scale space.
 ** @param self object.
 ** @return the scale space geometry.
 **/

VlScaleSpaceGeometry
vl_scalespace_get_geometry (VlScaleSpace const * self)
{
  return self->geom ;
}

/** @brief Get the geometry of an octave of the scalespace.
 ** @param self object.
 ** @param o octave index.
 ** @return the geometry of octave @a o.
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

/** @brief Get the data of a scale space level
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
vl_scalespace_get_level (VlScaleSpace *self, vl_index o, vl_index s)
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


/** @brief Get the data of a scale space level (const)
 ** @param self object.
 ** @param o octave index.
 ** @param s level index.
 ** @return pointer to the data for octave @a o, level @a s.
 **
 ** This function is the same as ::vl_scalespce_get_level but reutrns
 ** a @c const pointer to the data.
 **/

float const *
vl_scalespace_get_level_const (VlScaleSpace const * self, vl_index o, vl_index s)
{
  return vl_scalespace_get_level((VlScaleSpace*)self, o, s) ;
}


/** ------------------------------------------------------------------
 ** @brief Get the scale of a given octave and sublevel
 ** @param self object.
 ** @param o octave index.
 ** @param s sublevel index.
 **
 ** The function returns the scale $\sigma(o,s)$ as a function of the
 ** octave index @a o and sublevel @a s.
 **/

double
vl_scalespace_get_level_sigma (VlScaleSpace const *self, vl_index o, vl_index s)
{
  return self->geom.baseScale * pow(2.0, o + (double) s / self->geom.octaveResolution) ;
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

/* ---------------------------------------------------------------- */
/** @brief Create a new scale space object
 ** @param width image width.
 ** @param height image height.
 ** @return new scale space object.
 **
 ** This function is the same as ::vl_scalespace_new_with_geometry()
 ** but it uses ::vl_scalespace_get_default_geometry to initialise
 ** the geometry of the scale space from the image size.
 **
 ** @sa ::vl_scalespace_new_with_geometry(), ::vl_scalespace_delete().
 **/

VlScaleSpace *
vl_scalespace_new (vl_size width, vl_size height)
{
  VlScaleSpaceGeometry geom ;
  geom = vl_scalespace_get_default_geometry(width, height) ;
  return vl_scalespace_new_with_geometry(geom) ;
}

/** ------------------------------------------------------------------
 ** @brief Create a new scale space with the specified geometry
 ** @param geom scale space geomerty.
 ** @return new scale space object.
 **
 ** If the geometry is not valid (see ::VlScaleSpaceGeometry), the
 ** result is unpredictable.
 **
 ** The function returns `NULL` if it was not possible to allocate the
 ** object because of an out-of-memory condition.
 **
 ** @sa ::VlScaleSpaceGeometry, ::vl_scalespace_delete().
 **/

VlScaleSpace *
vl_scalespace_new_with_geometry (VlScaleSpaceGeometry geom)
{

  vl_index o ;
  vl_size numSublevels = geom.octaveLastSubdivision - geom.octaveFirstSubdivision + 1 ;
  vl_size numOctaves = geom.lastOctave - geom.firstOctave + 1 ;
  VlScaleSpace *self ;

  assert(is_valid_geometry(geom)) ;
  numOctaves = geom.lastOctave - geom.firstOctave + 1 ;
  numSublevels = geom.octaveLastSubdivision - geom.octaveFirstSubdivision + 1 ;

  self = vl_calloc(1, sizeof(VlScaleSpace)) ;
  if (self == NULL) goto err_alloc_self ;
  self->geom = geom ;
  self->octaves = vl_calloc(numOctaves, sizeof(float*)) ;
  if (self->octaves == NULL) goto err_alloc_octave_list ;
  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self,o) ;
    vl_size octaveSize = ogeom.width * ogeom.height * numSublevels ;
    self->octaves[o - self->geom.firstOctave] = vl_malloc(octaveSize * sizeof(float)) ;
    if (self->octaves[o - self->geom.firstOctave] == NULL) goto err_alloc_octaves;
  }
  return self ;

err_alloc_octaves:
  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    if (self->octaves[o - self->geom.firstOctave]) {
      vl_free(self->octaves[o - self->geom.firstOctave]) ;
    }
  }
err_alloc_octave_list:
  vl_free(self) ;
err_alloc_self:
  return NULL ;
}

/* ---------------------------------------------------------------- */
/** @brief Create a new copy of the object
 ** @param self object to copy from.
 **
 ** The function returns `NULL` if the copy cannot be made due to an
 ** out-of-memory condition.
 **/

VlScaleSpace *
vl_scalespace_new_copy (VlScaleSpace* self)
{
  vl_index o  ;
  VlScaleSpace * copy = vl_scalespace_new_shallow_copy(self) ;
  if (copy == NULL) return NULL ;

  for (o = self->geom.firstOctave ; o <= self->geom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry ogeom = vl_scalespace_get_octave_geometry(self,o) ;
    vl_size numSubevels = self->geom.octaveLastSubdivision - self->geom.octaveFirstSubdivision + 1;
    memcpy(copy->octaves[o - self->geom.firstOctave],
           self->octaves[o - self->geom.firstOctave],
           ogeom.width * ogeom.height * numSubevels * sizeof(float)) ;
  }
  return copy ;
}

/* ---------------------------------------------------------------- */
/** @brief Create a new shallow copy of the object
 ** @param self object to copy from.
 **
 ** The function works like ::vl_scalespace_new_copy() but only allocates
 ** the scale space, without actually copying the data.
 **/

VlScaleSpace *
vl_scalespace_new_shallow_copy (VlScaleSpace* self)
{
  return vl_scalespace_new_with_geometry (self->geom) ;
}

/* ---------------------------------------------------------------- */
/** @brief Delete object
 ** @param self object to delete.
 ** @sa ::vl_scalespace_new()
 **/

void
vl_scalespace_delete (VlScaleSpace * self)
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

/** ------------------------------------------------------------------
 ** @internal @brief Fill octave starting from the first level
 ** @param self object instance.
 ** @param o octave to process.
 **
 ** The function takes the first sublevel of octave @a o (the one at
 ** sublevel `octaveFirstLevel` and iteratively
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
  imageSigma = self->geom.nominalScale ;

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

    /* todo: this may fail due to an out-of-memory condition */
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
