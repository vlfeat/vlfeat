/** @file covdet.c
 ** @brief Covariant feature detectors - Definition
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

#include "covdet2.h"
#include <string.h>

/* ---------------------------------------------------------------- */
/*                                            Local extrema finding */
/* ---------------------------------------------------------------- */

/** @brief Reallocate buffer
 ** @param buffer
 ** @param bufferSize
 ** @param targetSize
 ** @return error code
 **/

static int
_vl_resize_buffer (void ** buffer, vl_size * bufferSize, vl_size targetSize) {
  if (*buffer == NULL) {
    *buffer = vl_malloc(targetSize) ;
    if (*buffer) {
      *bufferSize = targetSize ;
      return VL_ERR_OK ;
    } else {
      *bufferSize = 0 ;
      return VL_ERR_ALLOC ;
    }
  }
  void * newBuffer = vl_realloc(*buffer, targetSize) ;
  if (newBuffer) {
    *buffer = newBuffer ;
    *bufferSize = targetSize ;
    return VL_ERR_OK ;
  } else {
    return VL_ERR_ALLOC ;
  }
}

/** @brief Find extrema in 3D function
 ** @param extrema
 ** @param bufferSize
 ** @param map a 3D array representing the map.
 ** @param width of the map.
 ** @param height of the map.
 ** @param deepth of the map.
 ** @param threshold minumum extremum value.
 ** @return number of extrema found.
 **/

vl_size
vl_find_local_extrema_3 (vl_index ** extrema, vl_size * bufferSize,
                         float const * map,
                         vl_size width, vl_size height, vl_size depth,
                         double threshold)
{
  vl_index x, y, z ;
  vl_size const xo = 1 ;
  vl_size const yo = width ;
  vl_size const zo = width * height ;
  float const *pt = map + xo + yo + zo ;

  vl_size numExtrema = 0 ;
  vl_size requiredSize = 0 ;

#define CHECK_NEIGHBORS_3(v,CMP,SGN)     (\
v CMP ## = SGN threshold &&               \
v CMP *(pt + xo) &&                       \
v CMP *(pt - xo) &&                       \
v CMP *(pt + zo) &&                       \
v CMP *(pt - zo) &&                       \
v CMP *(pt + yo) &&                       \
v CMP *(pt - yo) &&                       \
\
v CMP *(pt + yo + xo) &&                  \
v CMP *(pt + yo - xo) &&                  \
v CMP *(pt - yo + xo) &&                  \
v CMP *(pt - yo - xo) &&                  \
\
v CMP *(pt + xo      + zo) &&             \
v CMP *(pt - xo      + zo) &&             \
v CMP *(pt + yo      + zo) &&             \
v CMP *(pt - yo      + zo) &&             \
v CMP *(pt + yo + xo + zo) &&             \
v CMP *(pt + yo - xo + zo) &&             \
v CMP *(pt - yo + xo + zo) &&             \
v CMP *(pt - yo - xo + zo) &&             \
\
v CMP *(pt + xo      - zo) &&             \
v CMP *(pt - xo      - zo) &&             \
v CMP *(pt + yo      - zo) &&             \
v CMP *(pt - yo      - zo) &&             \
v CMP *(pt + yo + xo - zo) &&             \
v CMP *(pt + yo - xo - zo) &&             \
v CMP *(pt - yo + xo - zo) &&             \
v CMP *(pt - yo - xo - zo) )

  for (z = 1 ; z < (signed)depth - 1 ; ++z) {
    for (y = 1 ; y < (signed)height - 1 ; ++y) {
      for (x = 1 ; x < (signed)width - 1 ; ++x) {
        float value = *pt ;
        if (CHECK_NEIGHBORS_3(value,>,+) || CHECK_NEIGHBORS_3(value,<,-)) {
          numExtrema ++ ;
          requiredSize += sizeof(vl_index) * 3 ;
          if (*bufferSize < requiredSize) {
            int err = _vl_resize_buffer((void**)extrema, bufferSize,
                                        requiredSize + 2000 * 3 * sizeof(vl_index)) ;
            assert(err == VL_ERR_OK) ;
          }
          (*extrema) [3 * (numExtrema - 1) + 0] = x ;
          (*extrema) [3 * (numExtrema - 1) + 1] = y ;
          (*extrema) [3 * (numExtrema - 1) + 2] = z ;
        }
        pt += xo ;
      }
      pt += 2*xo ;
    }
    pt += 2*yo ;
  }
  return numExtrema ;
}

/** @brief Find extrema in 3D function
 ** @param extrema
 ** @param bufferSize
 ** @param map a 3D array representing the map.
 ** @param width of the map.
 ** @param height of the map.
 ** @param deepth of the map.
 ** @param threshold minumum extremum value.
 ** @return number of extrema found.
 **/

vl_size
vl_find_local_extrema_2 (vl_index ** extrema, vl_size * bufferSize,
                         float const* map,
                         vl_size width, vl_size height,
                         double threshold)
{
  vl_index x, y ;
  vl_size const xo = 1 ;
  vl_size const yo = width ;
  float const *pt = map + xo + yo ;

  vl_size numExtrema = 0 ;
  vl_size requiredSize = 0 ;
#define CHECK_NEIGHBORS_2(v,CMP,SGN)     (\
v CMP ## = SGN threshold &&               \
v CMP *(pt + xo) &&                       \
v CMP *(pt - xo) &&                       \
v CMP *(pt + yo) &&                       \
v CMP *(pt - yo) &&                       \
\
v CMP *(pt + yo + xo) &&                  \
v CMP *(pt + yo - xo) &&                  \
v CMP *(pt - yo + xo) &&                  \
v CMP *(pt - yo - xo) )

  for (y = 1 ; y < (signed)height - 1 ; ++y) {
    for (x = 1 ; x < (signed)width - 1 ; ++x) {
      float value = *pt ;
      if (CHECK_NEIGHBORS_2(value,>,+) || CHECK_NEIGHBORS_2(value,<,-)) {
        numExtrema ++ ;
        requiredSize += sizeof(vl_index) * 2 ;
        if (*bufferSize < requiredSize) {
          int err = _vl_resize_buffer((void**)extrema, bufferSize,
                                      requiredSize + 2000 * 2 * sizeof(vl_index)) ;
          assert(err == VL_ERR_OK) ;
        }
        (*extrema) [3 * (numExtrema - 1) + 0] = x ;
        (*extrema) [2 * (numExtrema - 1) + 1] = y ;
      }
      pt += xo ;
    }
    pt += 2*xo ;
  }
  return numExtrema ;
}


/** @brief Refine extrema in 3D function
 ** @param refined refined extrema.
 ** @param map a 3D array representing the map.
 ** @param width of the map.
 ** @param height of the map.
 ** @param deepth of the map.
 ** @param x
 ** @param y
 ** @param z
 ** @return extrema refinement was stable.
 **/

VL_EXPORT vl_bool
vl_refine_local_extreum_3 (VlCovDetExtremum3 * refined,
                           float const * map,
                           vl_size width, vl_size height, vl_size depth,
                           vl_index x, vl_index y, vl_index z)
{
  vl_size const xo = 1 ;
  vl_size const yo = width ;
  vl_size const zo = width * height ;
  refined->xi = x ;
  refined->yi = y ;
  refined->zi = z ;
  refined->x = x ;
  refined->y = y ;
  refined->z = z ;
  refined->peakScore = *(map + xo*x + yo*y + zo*z) ;
  refined->edgeScore = 5.0 ;
  return VL_TRUE ;
}

/** @brief Refine extrema in 3D function
 ** @param refined refined extrema.
 ** @param map a 3D array representing the map.
 ** @param width of the map.
 ** @param height of the map.
 ** @param deepth of the map.
 ** @param x
 ** @param y
 ** @param z
 ** @return extrema refinement was stable.
 **/

VL_EXPORT vl_bool
vl_refine_local_extreum_2 (VlCovDetExtremum2 * refined,
                           float const * map,
                           vl_size width, vl_size height,
                           vl_index x, vl_index y)
{
  vl_size const xo = 1 ;
  vl_size const yo = width ;
  refined->xi = x ;
  refined->yi = y ;
  refined->x = x ;
  refined->y = y ;
  refined->peakScore = *(map + xo*x + yo*y) ;
  refined->edgeScore = 5.0 ;
  return VL_TRUE ;
}

/* ---------------------------------------------------------------- */
/*                                            Local extrema finding */
/* ---------------------------------------------------------------- */

VlEnumerator vlCovdetMethods [VL_COVDET_METHOD_NUM] = {
  {"DoG" ,              (vl_index)VL_COVDET_METHOD_DOG              },
  {"Hessian",           (vl_index)VL_COVDET_METHOD_HESSIAN          },
  {"HessianLaplace",    (vl_index)VL_COVDET_METHOD_HESSIAN_LAPLACE  },
  {"HarrisLaplace",     (vl_index)VL_COVDET_METHOD_HARRIS_LAPLACE   },
  {"MultiscaleHessian", (vl_index)VL_COVDET_METHOD_HESSIAN_LAPLACE  },
  {"MultiscaleHarris",  (vl_index)VL_COVDET_METHOD_HARRIS_LAPLACE   },
  {0,                   0                                           }
} ;

/** @brief Create a new object instance
 ** @param method
 ** @return new covariant detector.
 **/
VlCovDet *
vl_covdet_new (VlCovDetMethod method)
{
  VlCovDet * self = vl_calloc(sizeof(VlCovDet),1) ;
  self->method = method ;
  self->octaveResolution = 3 ;
  self->firstOctave = -1 ;
  self->peakThreshold = 0.0001 ;
  self->edgeThreshold = 10.0 ;
  self->frames = NULL ;
  self->numFrames = 0 ;
  self->numFrameBufferSize = 0 ;
  return self ;
}

void
vl_covdet_reset (VlCovDet * self)
{
  if (self->frames) {
    vl_free(self->frames) ;
    self->frames = NULL ;
  }
  if (self->css) {
    vl_scalespace_delete(self->css) ;
    self->css = NULL ;
  }
  if (self->gss) {
    vl_scalespace_delete(self->gss) ;
    self->gss = NULL ;
  }
}

/** @brief Delete object instance
 ** @param self object to delete.
 **/
void
vl_covdet_delete (VlCovDet * self)
{
  vl_covdet_reset(self) ;
  vl_free(self) ;
}

/* ---------------------------------------------------------------- */
/*                                     Start processing a new image */
/* ---------------------------------------------------------------- */

/** @brief Detect features in an image
 ** @param method
 ** @return new covariant detector.
 **/
void
vl_covdet_put_image (VlCovDet * self,
                     float const * image,
                     vl_size width, vl_size height)
{
  vl_size const minOctaveSize = 16 ;
  vl_index lastOctave ;
  vl_index octaveFirstSubdivision ;
  vl_index octaveLastSubdivision ;
  vl_bool gssReady = VL_FALSE ;

  assert (self) ;
  assert (image) ;
  assert (width >= 1) ;
  assert (height >= 1) ;

  /* (minOctaveSize - 1) 2^lastOctave <= min(width,height) - 1 */
  lastOctave = vl_floor_d(log2(VL_MIN((double)width-1,(double)height-1) / (minOctaveSize - 1))) ;

  if (self->method == VL_COVDET_METHOD_DOG) {
    octaveFirstSubdivision = -2 ;
    octaveLastSubdivision = self->octaveResolution ;
  } else if (self->method == VL_COVDET_METHOD_HESSIAN) {
    octaveFirstSubdivision = -1 ;
    octaveLastSubdivision = self->octaveResolution ;
  } else {
    octaveFirstSubdivision = 0 ;
    octaveLastSubdivision = self->octaveResolution - 1 ;
  }

  if (self->gss) {
    VlScaleSpaceGeometry geom = vl_scalespace_get_geometry (self->gss);
    gssReady =
    geom.width == width &&
    geom.height == height &&
    geom.firstOctave == self->firstOctave &&
    geom.lastOctave == lastOctave &&
    geom.octaveResolution == self->octaveResolution &&
    geom.octaveFirstSubdivision == octaveFirstSubdivision &&
    geom.octaveLastSubdivision == octaveLastSubdivision ;
  }

  /* see if the gss must be re-allocated */
  if (! gssReady) {
    if (self->gss) vl_scalespace_delete(self->gss) ;
    self->gss = vl_scalespace_new(width, height,
                                  lastOctave - self->firstOctave + 1,
                                  self->firstOctave,
                                  self->octaveResolution,
                                  octaveFirstSubdivision, octaveLastSubdivision) ;
  }
  vl_scalespace_init (self->gss, image) ;
}

/* ---------------------------------------------------------------- */
/*                                           determinant of Hessian */
/* ---------------------------------------------------------------- */

/** @brief Scaled derminant of the Hessian filter
 ** @param hessian
 ** @param image
 ** @param width
 ** @param height
 ** @param sigma
 **/

static void
_vl_det_hessian_response (float * hessian,
                          float const * image,
                          vl_size width, vl_size height,
                          double sigma)
{
  float factor = (float) (sigma * sigma * sigma * sigma) ;
  vl_index const xo = 1 ; /* x-stride */
  vl_index const yo = width;  /* y-stride */
  vl_size r, c;

  float p11, p12, p13, p21, p22, p23, p31, p32, p33;

  /* setup input pointer to be centered at 0,1 */
  float const *in = image + yo ;

  /* setup output pointer to be centered at 1,1 */
  float *out = hessian + xo + yo;

  /* move 3x3 window and convolve */
  for (r = 1; r < height - 1; ++r)
  {
    /* fill in shift registers at the beginning of the row */
    p11 = in[-yo]; p12 = in[xo - yo];
    p21 = in[  0]; p22 = in[xo     ];
    p31 = in[+yo]; p32 = in[xo + yo];
    /* setup input pointer to (2,1) of the 3x3 square */
    in += 2;
    for (c = 1; c < width - 1; ++c)
    {
      float Lxx, Lyy, Lxy;
      /* fetch remaining values (last column) */
      p13 = in[-yo]; p23 = *in; p33 = in[+yo];

      /* Compute 3x3 Hessian values from pixel differences. */
      Lxx = (-p21 + 2*p22 - p23);
      Lyy = (-p12 + 2*p22 - p32);
      Lxy = ((p11 - p31 - p13 + p33)/4.0f);

      /* normalize and write out */
      *out = (Lxx * Lyy - Lxy * Lxy) * factor ;

      /* move window */
      p11=p12; p12=p13;
      p21=p22; p22=p23;
      p31=p32; p32=p33;

      /* move input/output pointers */
      in++; out++;
    }
    out += 2;
  }

  /* Copy the computed values to borders */
  in = hessian + yo + xo ;
  out = hessian + xo ;

  /* Top row without corners */
  memcpy(out, in, (width - 2)*sizeof(float));
  out--;
  in -= yo;

  /* Left border columns without last row */
  for (r = 0; r < height - 1; r++){
    *out = *in;
    *(out + yo - 1) = *(in + yo - 3);
    in += yo;
    out += yo;
  }

  /* Bottom corners */
  in -= yo;
  *out = *in;
  *(out + yo - 1) = *(in + yo - 3);

  /* Bottom row without corners */
  out++;
  memcpy(out, in, (width - 2)*sizeof(float));
}


/* ---------------------------------------------------------------- */
/*                                           Determinant of Hessian */
/* ---------------------------------------------------------------- */

/** @brief Scaled Harris response
 ** @param harris
 ** @param imagexx
 ** @param imageyy
 ** @param imagexy
 ** @param image
 ** @param width
 ** @param height
 ** @param sigma
 **/

static void
_vl_harris_response (float * harris,
                     float * imagexx,
                     float * imageyy,
                     float * imagexy,
                     float const * image,
                     vl_size width, vl_size height,
                     double sigma)
{


}

/* ---------------------------------------------------------------- */
/*                                          Difference of Gaussians */
/* ---------------------------------------------------------------- */

/** @brief Scaled Harris response
 ** @param harris
 ** @param imagexx
 ** @param imageyy
 ** @param imagexy
 ** @param image
 ** @param width
 ** @param height
 ** @param sigma
 **/

static void
_vl_dog_response (float * dog,
                  float const * level1,
                  float const * level2,
                  vl_size width, vl_size height)
{
  vl_index k ;
  for (k = 0 ; k < (signed)(width*height) ; ++k) {
    dog[k] = level2[k] - level1[k] ;
  }
}

/* ---------------------------------------------------------------- */
/*                                        Detect scale-space points */
/* ---------------------------------------------------------------- */

/** @brief Append a frame to the internal buffer
 ** @param method
 ** @return new covariant detector.
 **/
static int
_vl_covdet_append_feature (VlCovDet * self, VlCovDetFeature const * feature)
{
  vl_size requiredSize ;
  assert(self) ;
  assert(feature) ;
  self->numFrames ++ ;
  requiredSize = self->numFrames * sizeof(VlCovDetFeature) ;
  if (requiredSize > self->numFrameBufferSize) {
    int err = _vl_resize_buffer((void**)&self->frames, &self->numFrameBufferSize,
                                (self->numFrames + 1000) * sizeof(VlCovDetFeature)) ;
    if (err) return err ;
  }
  self->frames[self->numFrames - 1] = *feature ;
  return VL_ERR_OK ;
}

/** @brief Detect scale-space features
 ** @param method
 ** @return new covariant detector.
 **/

void
vl_covdet_detect (VlCovDet * self)
{
  assert (self) ;
  assert (self->gss) ;
  VlScaleSpaceGeometry geom = vl_scalespace_get_geometry(self->gss) ;
  VlScaleSpaceGeometry cgeom ;
  vl_bool cssReady = VL_FALSE ;
  float * levelxx = NULL ;
  float * levelyy = NULL ;
  float * levelxy = NULL ;
  vl_index o, s ;

  /* clear previous detections if any */
  self->numFrames = 0 ;

  /* prepare buffers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  cgeom = geom ;
  if (self->method == VL_COVDET_METHOD_DOG) {
    cgeom.octaveFirstSubdivision = -1 ;
  }
  if (self->css) {
    VlScaleSpaceGeometry cssGeom = vl_scalespace_get_geometry(self->css) ;
    cssReady =
    cssGeom.width == cgeom.width &&
    cssGeom.height == cgeom.height &&
    cssGeom.firstOctave == cgeom.firstOctave &&
    cssGeom.lastOctave == cgeom.lastOctave &&
    cssGeom.octaveResolution == cgeom.octaveResolution &&
    cssGeom.octaveFirstSubdivision == cgeom.octaveFirstSubdivision &&
    cssGeom.octaveLastSubdivision == cgeom.octaveLastSubdivision ;
  }
  if (! cssReady) {
    if (self->css) vl_scalespace_delete(self->css) ;
    self->css = vl_scalespace_new(cgeom.width, cgeom.height,
                                  cgeom.lastOctave - cgeom.firstOctave + 1,
                                  cgeom.firstOctave,
                                  cgeom.octaveResolution,
                                  cgeom.octaveFirstSubdivision,
                                  cgeom.octaveLastSubdivision) ;
  }
  if (self->method == VL_COVDET_METHOD_HARRIS_LAPLACE ||
      self->method == VL_COVDET_METHOD_MULTISCALE_HARRIS) {
    vl_size width = vl_scalespace_get_octave_width(self->gss, geom.firstOctave) ;
    vl_size height = vl_scalespace_get_octave_width(self->gss, geom.firstOctave) ;
    levelxx = vl_malloc(width*height*sizeof(float)) ;
    levelyy = vl_malloc(width*height*sizeof(float)) ;
    levelxy = vl_malloc(width*height*sizeof(float)) ;
  }

  /* compute cornerness ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  for (o = cgeom.firstOctave ; o <= cgeom.lastOctave ; ++o) {
    for (s = cgeom.octaveFirstSubdivision ; s <= cgeom.octaveLastSubdivision ; ++s) {
      float * level = vl_scalespace_get_octave(self->gss, o, s) ;
      float * clevel = vl_scalespace_get_octave(self->css, o, s) ;
      vl_size width = vl_scalespace_get_octave_width(self->css, o) ;
      vl_size height = vl_scalespace_get_octave_height(self->css, o) ;
      double sigma = vl_scalespace_get_sigma_for_scale(self->css, o, s) ;
      switch (self->method) {
        case VL_COVDET_METHOD_DOG:
          _vl_dog_response(clevel, level,
                           vl_scalespace_get_octave(self->gss, o, s-1),
                           width, height) ;
          break ;

        case VL_COVDET_METHOD_HARRIS_LAPLACE:
        case VL_COVDET_METHOD_MULTISCALE_HARRIS:
          _vl_harris_response(clevel, levelxx, levelyy, levelxy,
                              level, width, height, sigma) ;
          break ;

        case VL_COVDET_METHOD_HESSIAN:
        case VL_COVDET_METHOD_HESSIAN_LAPLACE:
        case VL_COVDET_METHOD_MULTISCALE_HESSIAN:
          _vl_det_hessian_response(clevel, level, width, height, sigma) ;
          break ;

        default:
          assert(0) ;
      }
    }
  }

  /* find and refine local maxima ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  {
    vl_index * extrema = NULL ;
    vl_size extremaBufferSize = 0 ;
    vl_size numExtrema ;
    vl_size index ;
    for (o = cgeom.firstOctave ; o <= cgeom.lastOctave ; ++o) {
      vl_size width = vl_scalespace_get_octave_width(self->css, o) ;
      vl_size height = vl_scalespace_get_octave_height(self->css, o) ;
      vl_size depth = cgeom.octaveLastSubdivision - cgeom.octaveFirstSubdivision + 1 ;
      double step = vl_scalespace_get_octave_sampling_step (self->css, o) ;

      switch (self->method) {
        case VL_COVDET_METHOD_DOG:
        case VL_COVDET_METHOD_HESSIAN:
        {
          /* scale-space extrema */
          float const * octave =
          vl_scalespace_get_octave(self->css,o,cgeom.octaveFirstSubdivision) ;
          numExtrema = vl_find_local_extrema_3(&extrema, &extremaBufferSize,
                                               octave,
                                               width, height, depth,
                                               0.8 * self->peakThreshold);
          for (index = 0 ; index < numExtrema ; ++index) {
            VlCovDetExtremum3 refined ;
            VlCovDetFeature feature ;
            vl_bool ok = vl_refine_local_extreum_3(&refined,
                                                   octave, width, height, depth,
                                                   extrema[3*index+0],
                                                   extrema[3*index+1],
                                                   extrema[3*index+2]) ;
            if (ok) {
              double sigma = self->css->sigma0 * pow(2.0, o + refined.z / cgeom.octaveResolution) ;
              feature.frame.x = refined.x * step ;
              feature.frame.y = refined.y * step ;
              feature.frame.a11 = sigma ;
              feature.frame.a12 = 0.0 ;
              feature.frame.a21 = 0.0 ;
              feature.frame.a22 = sigma ;
              feature.peakScore = refined.peakScore ;
              feature.edgeScore = refined.edgeScore ;
              _vl_covdet_append_feature(self, &feature) ;
            }
          }
          break ;
        }

        default:
        {
          for (s = cgeom.octaveFirstSubdivision ; s < cgeom.octaveLastSubdivision ; ++s) {
            /* space extrema */
            float const * level = vl_scalespace_get_octave(self->css,o,s) ;
            numExtrema = vl_find_local_extrema_2(&extrema, &extremaBufferSize,
                                                 level,
                                                 width, height,
                                                 0.8 * self->peakThreshold);
            for (index = 0 ; index < numExtrema ; ++index) {
              VlCovDetExtremum2 refined ;
              VlCovDetFeature feature ;
              vl_bool ok = vl_refine_local_extreum_2(&refined,
                                                     level, width, height,
                                                     extrema[2*index+0],
                                                     extrema[2*index+1]);
              if (ok) {
                double sigma = self->css->sigma0 * pow(2.0, o + (double)s / cgeom.octaveResolution) ;
                feature.frame.x = refined.x * step ;
                feature.frame.y = refined.y * step ;
                feature.frame.a11 = sigma ;
                feature.frame.a12 = 0.0 ;
                feature.frame.a21 = 0.0 ;
                feature.frame.a22 = sigma ;
                feature.peakScore = refined.peakScore ;
                feature.edgeScore = refined.edgeScore ;
                _vl_covdet_append_feature(self, &feature) ;
              }
            }
          }
          break ;
        }
      }
    }
    if (extrema) { vl_free(extrema) ; extrema = 0 ; }
  }

  if (levelxx) vl_free(levelxx) ;
  if (levelyy) vl_free(levelyy) ;
  if (levelxy) vl_free(levelxy) ;
}


/* ---------------------------------------------------------------- */
/*                                              Detect orientations */
/* ---------------------------------------------------------------- */



/* ---------------------------------------------------------------- */
/*                                                    Extract patch */
/* ---------------------------------------------------------------- */


/* ---------------------------------------------------------------- */
/*                                     Setter, getter, and all that */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/** @brief Get the edge threshold
 ** @param self ::VlCovDet object.
 ** @return the edge threshold.
 **/
double
vl_covdet_get_edge_threshold (VlCovDet * self)
{
  return self->edgeThreshold ;
}

/** @brief Set the edge threshold
 ** @param self ::VlCovDet object.
 ** @param the edge threshold.
 **
 ** The edge threshold must be non-negative.
 **/
void
vl_covdet_set_edge_threshold (VlCovDet * self, double edgeThreshold)
{
  assert(edgeThreshold >= 0) ;
  self->edgeThreshold = edgeThreshold ;
}

/** @brief Get the peak threshold
 ** @param self ::VlCovDet object.
 ** @return the peak threshold.
 **/
double
vl_covdet_get_peak_threshold (VlCovDet * self)
{
  return self->peakThreshold ;
}

/** @brief Set the peak threshold
 ** @param self ::VlCovDet object.
 ** @param the peak threshold.
 **
 ** The peak threshold must be non-negative.
 **/
void
vl_covdet_set_peak_threshold (VlCovDet * self, double peakThreshold)
{
  assert(peakThreshold >= 0) ;
  self->peakThreshold = peakThreshold ;
}

/* ---------------------------------------------------------------- */
/** @brief Get the index of the first octave
 ** @param self ::VlCovDet object.
 ** @return index of the first octave.
 **/
vl_index
vl_covdet_get_first_octave (VlCovDet * self)
{
  return self->firstOctave ;
}

/** @brief Set the index of the first octave
 ** @param self ::VlCovDet object.
 ** @param index of the first octave.
 **
 ** Calling this function resets the detector.
 **/
void
vl_covdet_set_first_octave (VlCovDet * self, vl_index o)
{
  self->firstOctave = o ;
  vl_covdet_reset(self) ;
}

/** @brief Get number of stored frames
 ** @return number of frames stored in the detector.
 **/
vl_size
vl_covdet_get_num_features (VlCovDet * self)
{
  return self->numFrames ;
}


/** @brief Get the stored frames
 ** @return frames stored in the detector.
 **/
void *
vl_covdet_get_features (VlCovDet * self)
{
  return self->frames ;
}



