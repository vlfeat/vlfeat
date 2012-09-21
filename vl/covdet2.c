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

/** @brief Enlarge buffer
 ** @param buffer
 ** @param bufferSize
 ** @param targetSize
 ** @return error code
 **/

static int
_vl_enlarge_buffer (void ** buffer, vl_size * bufferSize, vl_size targetSize) {
  if (*bufferSize >= targetSize) return VL_ERR_OK ;
  return _vl_resize_buffer(buffer,bufferSize,targetSize) ;
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
        (*extrema) [2 * (numExtrema - 1) + 0] = x ;
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

  double Dx=0,Dy=0,Dz=0,Dxx=0,Dyy=0,Dzz=0,Dxy=0,Dxz=0,Dyz=0 ;
  double A [3*3], b [3] ;

#define at(dx,dy,dz) (*(pt + (dx)*xo + (dy)*yo + (dz)*zo))
#define Aat(i,j) (A[(i)+(j)*3])

  float const * pt ;
  vl_index dx = 0 ;
  vl_index dy = 0 ;
  /*vl_index dz = 0 ;*/
  vl_index iter ;
  int err ;

  assert (map) ;
  assert (1 <= x && x <= (signed)width - 2) ;
  assert (1 <= y && y <= (signed)height - 2) ;
  assert (1 <= z && z <= (signed)depth - 2) ;

  for (iter = 0 ; iter < 5 ; ++iter) {
    x += dx ;
    y += dy ;
    pt = map + x*xo + y*yo + z*zo ;

    /* compute the gradient */
    Dx = 0.5 * (at(+1,0,0) - at(-1,0,0)) ;
    Dy = 0.5 * (at(0,+1,0) - at(0,-1,0));
    Dz = 0.5 * (at(0,0,+1) - at(0,0,-1)) ;

    /* compute the Hessian */
    Dxx = (at(+1,0,0) + at(-1,0,0) - 2.0 * at(0,0,0)) ;
    Dyy = (at(0,+1,0) + at(0,-1,0) - 2.0 * at(0,0,0)) ;
    Dzz = (at(0,0,+1) + at(0,0,-1) - 2.0 * at(0,0,0)) ;

    Dxy = 0.25 * (at(+1,+1,0) + at(-1,-1,0) - at(-1,+1,0) - at(+1,-1,0)) ;
    Dxz = 0.25 * (at(+1,0,+1) + at(-1,0,-1) - at(-1,0,+1) - at(+1,0,-1)) ;
    Dyz = 0.25 * (at(0,+1,+1) + at(0,-1,-1) - at(0,-1,+1) - at(0,+1,-1)) ;

    /* solve linear system */
    Aat(0,0) = Dxx ;
    Aat(1,1) = Dyy ;
    Aat(2,2) = Dzz ;
    Aat(0,1) = Aat(1,0) = Dxy ;
    Aat(0,2) = Aat(2,0) = Dxz ;
    Aat(1,2) = Aat(2,1) = Dyz ;

    b[0] = - Dx ;
    b[1] = - Dy ;
    b[2] = - Dz ;

    err = vl_solve_linear_system_3(b, A, b) ;

    if (err != VL_ERR_OK) {
      b[0] = 0 ;
      b[1] = 0 ;
      b[2] = 0 ;
      break ;
    }

    /* Keep going if there is sufficient translation */

    dx = (b[0] > 0.6 && x < (signed)width - 2 ?  1 : 0)
    + (b[0] < -0.6 && x > 1 ? -1 : 0) ;

    dy = (b[1] > 0.6 && y < (signed)height - 2 ?  1 : 0)
    + (b[1] < -0.6 && y > 1 ? -1 : 0) ;

    if (dx == 0 && dy == 0) break ;
  }

  /* check threshold and other conditions */
  {
    double peakScore = at(0,0,0)
    + 0.5 * (Dx * b[0] + Dy * b[1] + Dz * b[2]) ;
    double alpha = (Dxx+Dyy)*(Dxx+Dyy) / (Dxx*Dyy - Dxy*Dxy) ;
    double edgeScore = (0.5*alpha - 1) + sqrt(VL_MAX(0.25*alpha - 1,0)*alpha) ;

    refined->xi = x ;
    refined->yi = y ;
    refined->zi = z ;
    refined->x = x + b[0] ;
    refined->y = y + b[1] ;
    refined->z = z + b[2] ;
    refined->peakScore = peakScore ;
    refined->edgeScore = edgeScore ;

    return
    err == VL_ERR_OK &&
    vl_abs_d(b[0]) < 1.5 &&
    vl_abs_d(b[1]) < 1.5 &&
    vl_abs_d(b[2]) < 1.5 &&
    0 <= refined->x && refined->x <= (signed)width - 1 &&
    0 <= refined->y && refined->y <= (signed)height - 1 &&
    0 <= refined->z && refined->z <= (signed)depth - 1 ;
  }
#undef Aat
#undef at
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

  double Dx=0,Dy=0,Dxx=0,Dyy=0,Dxy=0;
  double A [2*2], b [2] ;

#define at(dx,dy) (*(pt + (dx)*xo + (dy)*yo ))
#define Aat(i,j) (A[(i)+(j)*2])

  float const * pt ;
  vl_index dx = 0 ;
  vl_index dy = 0 ;
  vl_index iter ;
  int err ;

  assert (map) ;
  assert (1 <= x && x <= (signed)width - 2) ;
  assert (1 <= y && y <= (signed)height - 2) ;

  for (iter = 0 ; iter < 5 ; ++iter) {
    x += dx ;
    y += dy ;
    pt = map + x*xo + y*yo  ;

    /* compute the gradient */
    Dx = 0.5 * (at(+1,0) - at(-1,0)) ;
    Dy = 0.5 * (at(0,+1) - at(0,-1));

    /* compute the Hessian */
    Dxx = (at(+1,0) + at(-1,0) - 2.0 * at(0,0)) ;
    Dyy = (at(0,+1) + at(0,-1) - 2.0 * at(0,0)) ;
    Dxy = 0.25 * (at(+1,+1) + at(-1,-1) - at(-1,+1) - at(+1,-1)) ;

    /* solve linear system */
    Aat(0,0) = Dxx ;
    Aat(1,1) = Dyy ;
    Aat(0,1) = Aat(1,0) = Dxy ;

    b[0] = - Dx ;
    b[1] = - Dy ;

    err = vl_solve_linear_system_2(b, A, b) ;

    if (err != VL_ERR_OK) {
      b[0] = 0 ;
      b[1] = 0 ;
      break ;
    }

    /* Keep going if there is sufficient translation */

    dx = (b[0] > 0.6 && x < (signed)width - 2 ?  1 : 0)
    + (b[0] < -0.6 && x > 1 ? -1 : 0) ;

    dy = (b[1] > 0.6 && y < (signed)height - 2 ?  1 : 0)
    + (b[1] < -0.6 && y > 1 ? -1 : 0) ;

    if (dx == 0 && dy == 0) break ;
  }

  /* check threshold and other conditions */
  {
    double peakScore = at(0,0) + 0.5 * (Dx * b[0] + Dy * b[1]) ;
    double alpha = (Dxx+Dyy)*(Dxx+Dyy) / (Dxx*Dyy - Dxy*Dxy) ;
    double edgeScore = (0.5*alpha - 1) + sqrt(VL_MAX(0.25*alpha - 1,0)*alpha) ;

    refined->xi = x ;
    refined->yi = y ;
    refined->x = x + b[0] ;
    refined->y = y + b[1] ;
    refined->peakScore = peakScore ;
    refined->edgeScore = edgeScore ;

    return
    err == VL_ERR_OK &&
    vl_abs_d(b[0]) < 1.5 &&
    vl_abs_d(b[1]) < 1.5 &&
    0 <= refined->x && refined->x <= (signed)width - 1 &&
    0 <= refined->y && refined->y <= (signed)height - 1 ;
  }
#undef Aat
#undef at
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
  self->peakThreshold = VL_COVDET_DEF_PEAK_THRESHOLD ;
  self->edgeThreshold = VL_COVDET_DEF_EDGE_THRESHOLD ;
  self->frames = NULL ;
  self->numFrames = 0 ;
  self->numFrameBufferSize = 0 ;
  self->patch = NULL ;
  self->patchBufferSize = 0 ;
  self->referenceAngle = VL_COVDET_DEF_REFERENCE_ANGLE ;
  self->transposed = VL_FALSE ;

  {
    vl_index const w = VL_COVDET_AA_PATCH_RESOLUTION ;
    vl_index i,j ;
    double step = (2.0 * VL_COVDET_AA_PATCH_EXTENT) / (2*w+1) ;
    double sigma = VL_COVDET_AA_RELATIVE_INTEGRATION_SIGMA ;
    for (j = -w ; j <= w ; ++j) {
      for (i = -w ; i <= w ; ++i) {
        double dx = i*step/sigma ;
        double dy = j*step/sigma ;
        self->aaMask[(i+w) + (2*w+1)*(j+w)] = exp(-0.5*(dx*dx+dy*dy)) ;
      }
    }
  }

  {
    /*
     Covers one octave of Laplacian filters, from sigma=1 to sigma=2.
     The spatial sampling step is 0.5.
     */
    vl_index s, k ;
    for (s = 0 ; s < VL_COVDET_LAP_NUM_LEVELS ; ++s) {
      double sigmaLap = pow(2.0, -0.5 + 0.5 * s / (VL_COVDET_LAP_NUM_LEVELS - 1)) ;
      double const sigmaIm = 1.0 / sqrt(2.0) ;
      double const sigmaDelta = sqrt(sigmaLap*sigmaLap - sigmaIm*sigmaIm) ;
      double const step = 0.5 * sigmaIm ;
      double mass = 0 ;
      vl_size const w = VL_COVDET_LAP_PATCH_RESOLUTION ;
      vl_size const num = 2 * w + 1  ;
      float * pt = self->laplacians + s * (num * num) ;

      memset(pt, 0, num * num * sizeof(float)) ;

#define at(x,y) pt[(x+w)+(y+w)*(2*w+1)]
      at(0,0) = - 4.0 ;
      at(-1,0) = 1.0 ;
      at(+1,0) = 1.0 ;
      at(0,1) = 1.0 ;
      at(0,-1) = 1.0 ;
#undef at

      vl_imsmooth_f(pt, num,
                    pt, num, num, num,
                    sigmaDelta / step, sigmaDelta / step) ;

      for (k = 0 ; k < (signed)(num * num) ; ++k) mass += fabs(pt[k]) ;
      for (k = 0 ; k < (signed)(num * num) ; ++k) pt[k] /= mass ;

#if 0
      {
        char name [200] ;
        snprintf(name, 200, "/Users/vedaldi/Desktop/bla/%f-lap.pgm", sigmaDelta) ;
        vl_pgm_write_f(name, pt, num, num) ;
      }
#endif

    }
  }

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
  if (self->patch) vl_free (self->patch) ;
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
  vl_scalespace_put_image(self->gss, image) ;
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
                          double step, double sigma)
{
  float factor = (float) (sigma * sigma * sigma * sigma) / (step*step) ;
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
                     float const * image,
                     vl_size width, vl_size height,
                     double step, double sigma,
                     double sigmaI, double alpha)
{
  float factor = (float) (sigma * sigma) / (step * step) ;
  vl_index k ;

  float * LxLx ;
  float * LyLy ;
  float * LxLy ;

  LxLx = vl_malloc(sizeof(float) * width * height) ;
  LyLy = vl_malloc(sizeof(float) * width * height) ;
  LxLy = vl_malloc(sizeof(float) * width * height) ;

  vl_imgradient_f (LxLx, LyLy, width, 1, image, width, height, width) ;

  for (k = 0 ; k < (signed)(width * height) ; ++k) {
    float dx = LxLx[k] ;
    float dy = LyLy[k] ;
    LxLx[k] = dx*dx ;
    LyLy[k] = dy*dy ;
    LxLy[k] = dx*dy ;
  }

  vl_imsmooth_f(LxLx, width, LxLx, width, height, width,
                sigmaI / step, sigmaI / step) ;

  vl_imsmooth_f(LyLy, width, LyLy, width, height, width,
                sigmaI / step, sigmaI / step) ;

  vl_imsmooth_f(LxLy, width, LxLy, width, height, width,
                sigmaI / step, sigmaI / step) ;

  for (k = 0 ; k < (signed)(width * height) ; ++k) {
    float a = LxLx[k] ;
    float b = LyLy[k] ;
    float c = LxLy[k] ;

    float determinant = a * b - c * c ;
    float trace = a + b ;

    harris[k] = factor * (determinant - alpha * (trace * trace)) ;
  }

  vl_free(LxLy) ;
  vl_free(LyLy) ;
  vl_free(LxLx) ;
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
int
vl_covdet_append_feature (VlCovDet * self, VlCovDetFeature const * feature)
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
    VlScaleSpaceOctaveGeometry oct = vl_scalespace_get_octave_geometry(self->gss, geom.firstOctave) ;
    levelxx = vl_malloc(oct.width * oct.height * sizeof(float)) ;
    levelyy = vl_malloc(oct.width * oct.height * sizeof(float)) ;
    levelxy = vl_malloc(oct.width * oct.height * sizeof(float)) ;
  }

  /* compute cornerness ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  for (o = cgeom.firstOctave ; o <= cgeom.lastOctave ; ++o) {
    VlScaleSpaceOctaveGeometry oct = vl_scalespace_get_octave_geometry(self->css, o) ;

    for (s = cgeom.octaveFirstSubdivision ; s <= cgeom.octaveLastSubdivision ; ++s) {
      float * level = vl_scalespace_get_level(self->gss, o, s) ;
      float * clevel = vl_scalespace_get_level(self->css, o, s) ;
      double sigma = vl_scalespace_get_level_sigma(self->css, o, s) ;
      switch (self->method) {
        case VL_COVDET_METHOD_DOG:
          _vl_dog_response(clevel, level,
                           vl_scalespace_get_level(self->gss, o, s - 1),
                           oct.width, oct.height) ;
          break ;

        case VL_COVDET_METHOD_HARRIS_LAPLACE:
        case VL_COVDET_METHOD_MULTISCALE_HARRIS:
          _vl_harris_response(clevel,
                              level, oct.width, oct.height, oct.step,
                              sigma, 1.4 * sigma, 0.05) ;
          break ;

        case VL_COVDET_METHOD_HESSIAN:
        case VL_COVDET_METHOD_HESSIAN_LAPLACE:
        case VL_COVDET_METHOD_MULTISCALE_HESSIAN:
          _vl_det_hessian_response(clevel, level, oct.width, oct.height, oct.step, sigma) ;
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
      VlScaleSpaceOctaveGeometry octgeom = vl_scalespace_get_octave_geometry(self->css, o) ;
      double step = octgeom.step ;
      vl_size width = octgeom.width ;
      vl_size height = octgeom.height ;
      vl_size depth = cgeom.octaveLastSubdivision - cgeom.octaveFirstSubdivision + 1 ;

      switch (self->method) {
        case VL_COVDET_METHOD_DOG:
        case VL_COVDET_METHOD_HESSIAN:
        {
          /* scale-space extrema */
          float const * octave =
          vl_scalespace_get_level(self->css, o, cgeom.octaveFirstSubdivision) ;
          numExtrema = vl_find_local_extrema_3(&extrema, &extremaBufferSize,
                                               octave, width, height, depth,
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
              double sigma = cgeom.sigma0 *
              pow(2.0, o + (refined.z - cgeom.octaveFirstSubdivision)
                  / cgeom.octaveResolution) ;
              feature.frame.x = refined.x * step ;
              feature.frame.y = refined.y * step ;
              feature.frame.a11 = sigma ;
              feature.frame.a12 = 0.0 ;
              feature.frame.a21 = 0.0 ;
              feature.frame.a22 = sigma ;
              feature.peakScore = refined.peakScore ;
              feature.edgeScore = refined.edgeScore ;
              vl_covdet_append_feature(self, &feature) ;
            }
          }
          break ;
        }

        default:
        {
          for (s = cgeom.octaveFirstSubdivision ; s < cgeom.octaveLastSubdivision ; ++s) {
            /* space extrema */
            float const * level = vl_scalespace_get_level(self->css,o,s) ;
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
                double sigma = cgeom.sigma0 *
                pow(2.0, o + (double)s / cgeom.octaveResolution) ;
                feature.frame.x = refined.x * step ;
                feature.frame.y = refined.y * step ;
                feature.frame.a11 = sigma ;
                feature.frame.a12 = 0.0 ;
                feature.frame.a21 = 0.0 ;
                feature.frame.a22 = sigma ;
                feature.peakScore = refined.peakScore ;
                feature.edgeScore = refined.edgeScore ;
                vl_covdet_append_feature(self, &feature) ;
              }
            }
          }
          switch (self->method) {
            case VL_COVDET_METHOD_HARRIS_LAPLACE :
            case VL_COVDET_METHOD_HESSIAN_LAPLACE :
              vl_covdet_extract_laplacian_scales (self) ;
              break ;
            default:
              break ;
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
/*                                                    Extract patch */
/* ---------------------------------------------------------------- */

vl_bool
vl_covdet_extract_patch_for_frame (VlCovDet * self,
                                   float * patch,
                                   vl_size resolution,
                                   double extent,
                                   double sigma,
                                   VlFrameOrientedEllipse frame)
{

  vl_index o, s ;
  double factor ;
  double sigma_ ;
  float const * level ;
  vl_size width, height ;
  double step ;

  double U [2*2] ;
  double V [2*2] ;
  double D [2*2] ;
  double A [2*2] = {frame.a11, frame.a21, frame.a12, frame.a22} ;
  double T[2] = {frame.x, frame.y} ;
  VlScaleSpaceGeometry geom = vl_scalespace_get_geometry(self->css) ;
  VlScaleSpaceOctaveGeometry oct ;


  /* Starting from a pre-smoothed image at scale sigma_
     because of the mapping A the resulting smoothing in
     the warped patch is S, where

        sigma_^2 I = A S A',

        S = sigma_^2 inv(A) inv(A)' = sigma_^2 V D^-2 V',

        A = U D V'.

     Thus we rotate A by V to obtain an axis-aligned smoothing:

        A = U*D,

        S = sigma_^2 D^2.

     Then we search the scale-space for the best sigma_ such
     that the target smoothing is approximated from below:

        max sigma_(o,s) :    simga_(o,s) factor <= sigma,
        factor = max{abs(D11), abs(D22)}.
   */

  vl_svd2(D, U, V, A) ;
  factor = 1.0 / VL_MIN(D[0], D[3]) ;

  /*
  A[0] = U[0] * D[0] ;
  A[1] = U[1] * D[0] ;
  A[2] = U[2] * D[3] ;
  A[3] = U[3] * D[3] ;
   */

  /*
   Determine the best level (o,s) such that sigma_(o,s) factor <= sigma.
   This can be obtained by scanning octaves from smalles to largest
   and stopping when no level in the octave satisfies the relation.

   Given the range of octave availables, do the best you can.
   */
  for (o = geom.firstOctave + 1 ; o <= geom.lastOctave - 1; ++o) {
    s = vl_floor_d(log2(sigma / (factor * geom.sigma0)) - o) ;
    s = VL_MAX(s, geom.octaveFirstSubdivision) ;
    s = VL_MIN(s, geom.octaveLastSubdivision) ;
    sigma_ = geom.sigma0 * pow(2.0, o + (double)s / geom.octaveResolution) ;
    if (factor * sigma_ > sigma) {
      o -- ;
      break ;
    }
  }
  s = vl_floor_d(log2(sigma / (factor * geom.sigma0)) - o) ;
  s = VL_MAX(s, geom.octaveFirstSubdivision) ;
  s = VL_MIN(s, geom.octaveLastSubdivision) ;
  sigma_ = geom.sigma0 * pow(2.0, o + (double)s / geom.octaveResolution) ;

  /* VL_PRINTF("%d %d %g %g %g %g\n", o, s, factor, sigma_, factor * sigma_, sigma) ; */

  /*
   Now the scale space level to be used for this warping has been
   determined.

   If the patch is partially or completely out of the image boundary,
   create a padded copy of the required region first.
   */

  level = vl_scalespace_get_level(self->gss, o, s) ;
  oct = vl_scalespace_get_octave_geometry(self->gss, o) ;
  width = oct.width ;
  height = oct.height ;
  step = oct.step ;

  A[0] /= step ;
  A[1] /= step ;
  A[2] /= step ;
  A[3] /= step ;
  T[0] /= step ;
  T[1] /= step ;

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
    double boxx [4] = {extent, extent, -extent, -extent} ;
    double boxy [4] = {-extent, extent, extent, -extent} ;
    int i ;
    for (i = 0 ; i < 4 ; ++i) {
      double x = A[0] * boxx[i] + A[2] * boxy[i] + T[0] ;
      double y = A[1] * boxx[i] + A[3] * boxy[i] + T[1] ;
      x0 = VL_MIN(x0, x) ;
      x1 = VL_MAX(x1, x) ;
      y0 = VL_MIN(y0, y) ;
      y1 = VL_MAX(y1, y) ;
    }

    /* Leave one pixel border for bilinear interpolation. */
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
      vl_index xi, yi ;

      /* compute the amount of l,r,t,b padding needed to complete the patch */
      vl_index padx0 = VL_MAX(0, - x0i) ;
      vl_index pady0 = VL_MAX(0, - y0i) ;
      vl_index padx1 = VL_MAX(0, x1i - ((signed)width - 1)) ;
      vl_index pady1 = VL_MAX(0, y1i - ((signed)height - 1)) ;

      /* make enough room for the patch */
      vl_index patchWidth = x1i - x0i + 1 ;
      vl_index patchHeight = y1i - y0i + 1 ;
      vl_size patchBufferSize = patchWidth * patchHeight * sizeof(float) ;
      if (patchBufferSize > self->patchBufferSize) {
        int err = _vl_resize_buffer((void**)&self->patch, &self->patchBufferSize, patchBufferSize) ;
        if (err) return vl_set_last_error(VL_ERR_ALLOC, NULL) ;
      }

      if (pady0 < patchHeight - pady1) {
        /* start by filling the central horizontal band */
        for (yi = y0i + pady0 ; yi < y0i + patchHeight - pady1 ; ++ yi) {
          float *dst = self->patch + (yi - y0i) * patchWidth ;
          float const *src = level + yi * width + VL_MIN(VL_MAX(0, x0i),width-1) ;
          for (xi = x0i ; xi < x0i + padx0 ; ++xi) *dst++ = *src ;
          for ( ; xi < x0i + patchWidth - padx1 - 2 ; ++xi) *dst++ = *src++ ;
          for ( ; xi < x0i + patchWidth ; ++xi) *dst++ = *src ;
        }
        /* now extend the central band up and down */
        for (yi = 0 ; yi < pady0 ; ++yi) {
          memcpy(self->patch + yi * patchWidth,
                 self->patch + pady0 * patchWidth,
                 patchWidth * sizeof(float)) ;
        }
        for (yi = patchHeight - pady1 ; yi < patchHeight ; ++yi) {
          memcpy(self->patch + yi * patchWidth,
                 self->patch + (patchHeight - pady1 - 1) * patchWidth,
                 patchWidth * sizeof(float)) ;
        }
      } else {
        /* should be handled better! */
        memset(self->patch, 0, self->patchBufferSize) ;
      }
#if 0
      {
        char name [200] ;
        snprintf(name, 200, "/Users/vedaldi/Desktop/bla/%20.0f-ext.pgm", 1e10*vl_get_cpu_time()) ;
        vl_pgm_write_f(name, self->patch, selfPatchWidth, selfPatchHeight) ;
      }
#endif

      level = self->patch ;
      width = patchWidth ;
      height = patchHeight ;
      T[0] -= x0i ;
      T[1] -= y0i ;
    }
  }

  /*
   Resample by using bilinear interpolation.
   */
  {
    float * pt = patch ;
    double yhat = -extent ;
    vl_index xxi ;
    vl_index yyi ;
    double stephat = (2*extent) / (2 * resolution + 1) ;

    for (yyi = 0 ; yyi < 2 * (signed)resolution + 1 ; ++yyi) {
      double xhat = -extent ;
      double rx = A[2] * yhat + T[0] ;
      double ry = A[3] * yhat + T[1] ;
      for (xxi = 0 ; xxi < 2 * (signed)resolution + 1 ; ++xxi) {
        double x = A[0] * xhat + rx ;
        double y = A[1] * xhat + ry ;
        vl_index xi = vl_floor_d(x) ;
        vl_index yi = vl_floor_d(y) ;
        double i00 = level[yi * width + xi] ;
        double i10 = level[yi * width + xi + 1] ;
        double i01 = level[(yi + 1) * width + xi] ;
        double i11 = level[(yi + 1) * width + xi + 1] ;
        double wx = x - xi ;
        double wy = y - yi ;

        assert(xi >= 0 && xi <= (signed)width - 1) ;
        assert(yi >= 0 && yi <= (signed)height - 1) ;

        *pt++ =
        (1.0 - wy) * ((1.0 - wx) * i00 + wx * i10) +
        wy * ((1.0 - wx) * i01 + wx * i11) ;

        xhat += stephat ;
      }
      yhat += stephat ;
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

  return VL_ERR_OK ;
}

/* ---------------------------------------------------------------- */
/*                                             Extract affine shape */
/* ---------------------------------------------------------------- */

int
vl_covdet_extract_affine_shape_for_frame (VlCovDet * self,
                                          VlFrameOrientedEllipse * adapted,
                                          VlFrameOrientedEllipse frame)
{
  vl_index iter = 0 ;

  double U [2*2] ;
  double V [2*2] ;
  double D [2*2] ;
  double M [2*2] ;
  double P [2*2] ;
  double P_ [2*2] ;
  double Q [2*2] ;
  double factor ;
  double anisotropy ;
  double referenceScale ;
  VlScaleSpaceGeometry geom = vl_scalespace_get_geometry(self->css) ;
  vl_size const size = 2*VL_COVDET_AA_PATCH_RESOLUTION + 1 ;
  double A [2*2] = {frame.a11, frame.a21, frame.a12, frame.a22} ;

  *adapted = frame ;

  while (1) {
    double lxx = 0, lxy = 0, lyy = 0 ;
    vl_index k ;
    int err ;

    /* A = U D V' */
    vl_svd2(D, U, V, A) ;
    anisotropy = VL_MAX(D[0]/D[3], D[3]/D[0]) ;

    if (anisotropy > VL_COVDET_AA_MAX_ANISOTROPY) {
      /* diverged, give up with current solution */
      break ;
    }

    /* make sure that the smallest singluar value stays fixed
       after the first iteration */
    if (iter == 0) {
      referenceScale = VL_MIN(D[0], D[3]) ;
      factor = 1.0 ;
    } else {
      factor = referenceScale / VL_MIN(D[0],D[3]) ;
    }

    A[0] = U[0] * D[0] * factor ;
    A[1] = U[1] * D[0] * factor ;
    A[2] = U[2] * D[3] * factor ;
    A[3] = U[3] * D[3] * factor ;

    adapted->a11 = A[0] ;
    adapted->a21 = A[1] ;
    adapted->a12 = A[2] ;
    adapted->a22 = A[3] ;

    if (++iter >= VL_COVDET_AA_MAX_NUM_ITERATIONS) break ;

    err = vl_covdet_extract_patch_for_frame(self,
                                            self->aaPatch,
                                            VL_COVDET_AA_PATCH_RESOLUTION,
                                            VL_COVDET_AA_PATCH_EXTENT,
                                            1.0,
                                            *adapted) ;

    /* compute second moment matrix */
    vl_imgradient_f (self->aaPatchX, self->aaPatchY, 1, size,
                     self->aaPatch, size, size, size) ;

    for (k = 0 ; k < (signed)(size*size) ; ++k) {
      double lx = self->aaPatchX[k] ;
      double ly = self->aaPatchY[k] ;
      lxx += lx * lx * self->aaMask[k] ;
      lyy += ly * ly * self->aaMask[k] ;
      lxy += lx * ly * self->aaMask[k] ;
    }
    M[0] = lxx ;
    M[1] = lxy ;
    M[2] = lxy ;
    M[3] = lyy ;

    if (lxx == 0 || lyy == 0) {
      *adapted = frame ;
      break ;
    }

    /* decompose M = P * Q * P' */
    vl_svd2 (Q, P, P_, M) ;

    /*
     Setting A <- A * dA results in M to change approximatively as

     M --> dA'  M dA = dA' P Q P dA

     To make this proportional to the identity, we set

     dA ~= P Q^1/2

     we also make it so the smallest singular value of A is unchanged.
     */

    if (Q[3]/Q[0] < VL_COVDET_AA_CONVERGENCE_THRESHOLD &&
        Q[0]/Q[3] < VL_COVDET_AA_CONVERGENCE_THRESHOLD) {
      break ;
    }

    {
      double Ap [4] ;
      double q0 = sqrt(Q[0]) ;
      double q1 = sqrt(Q[3]) ;
      Ap[0] = (A[0] * P[0] + A[2] * P[1]) / q0 ;
      Ap[1] = (A[1] * P[0] + A[3] * P[1]) / q0 ;
      Ap[2] = (A[0] * P[2] + A[2] * P[3]) / q1 ;
      Ap[3] = (A[1] * P[2] + A[3] * P[3]) / q1 ;
      memcpy(A,Ap,4*sizeof(double)) ;
    }

  } /* next iteration */

  /*
   Shape adaptation does not estimate rotation. This is fixed by default
   so that a selected axis is not rotated at all (usually this is the
   vertical axis for upright features). To do so, the frame is rotated
   as follows.
   */
#if 1
  {
    double A [2*2] = {adapted->a11, adapted->a21, adapted->a12, adapted->a22} ;
    double ref [2] ;
    double ref_ [2] ;
    double angle ;
    double angle_ ;
    double dangle ;
    double r1, r2 ;

    if (self->transposed) {
      /* up is the x axis */
      ref[0] = 1 ;
      ref[1] = 0 ;
    } else {
      /* up is the y axis */
      ref[0] = 0 ;
      ref[1] = 1 ;
    }

    vl_solve_linear_system_2 (ref_, A, ref) ;
    angle = atan2(ref[1], ref[0]) ;
    angle_ = atan2(ref_[1], ref_[0]) ;
    dangle = angle_ - angle ;
    r1 = cos(dangle) ;
    r2 = sin(dangle) ;
    adapted->a11 = + A[0] * r1 + A[2] * r2 ;
    adapted->a21 = + A[1] * r1 + A[3] * r2 ;
    adapted->a12 = - A[0] * r2 + A[2] * r1 ;
    adapted->a22 = - A[1] * r2 + A[3] * r1 ;
  }
#endif
  return VL_ERR_OK ;
}

void
vl_covdet_extract_affine_shape (VlCovDet * self)
{
  vl_index i, j = 0 ;
  vl_size numFrames = vl_covdet_get_num_features(self) ;
  VlCovDetFeature * feature = vl_covdet_get_features(self);
  for (i = 0 ; i < (signed)numFrames ; ++i) {
    int status ;
    VlFrameOrientedEllipse adapted ;
    status = vl_covdet_extract_affine_shape_for_frame(self, &adapted, feature[i].frame) ;
    if (status == VL_ERR_OK) {
      feature[j] = feature[i] ;
      feature[j].frame = adapted ;
      ++ j ;
    }
  }
  self->numFrames = j ;
}

/* ---------------------------------------------------------------- */
/*                                             Extract orientations */
/* ---------------------------------------------------------------- */

double * vl_covdet_extract_orientations_for_frame (VlCovDet * self,
                                                   vl_size * numOrientations,
                                                   VlFrameOrientedEllipse frame)
{
  assert(self);
  assert(numOrientations) ;
  int err ;
  vl_index k, i ;
  vl_index iter ;
  vl_size size = 2 * VL_COVDET_AA_PATCH_RESOLUTION + 1  ;
  vl_size const numBins = VL_COVDET_OR_NUM_ORIENTATION_HISTOGAM_BINS ;
  double hist [VL_COVDET_OR_NUM_ORIENTATION_HISTOGAM_BINS] ;
  double const step = 2 * VL_PI / VL_COVDET_OR_NUM_ORIENTATION_HISTOGAM_BINS ;
  double const inverseStep = 1.0 / step ;
  double maxValue ;
  double const fraction = VL_COVDET_OR_ADDITIONAL_PEAKS_RELATIVE_SIZE ;

  err = vl_covdet_extract_patch_for_frame(self,
                                self->aaPatch,
                                VL_COVDET_AA_PATCH_RESOLUTION,
                                VL_COVDET_AA_PATCH_EXTENT,
                                1.0,
                                frame) ;

  vl_imgradient_polar_f (self->aaPatchX, self->aaPatchY, 1, size,
                         self->aaPatch, size, size, size) ;

  memset (hist, 0, sizeof(double) * VL_COVDET_OR_NUM_ORIENTATION_HISTOGAM_BINS) ;

  /* accumulate histogram */
  for (k = 0 ; k < (signed)(size*size) ; ++k) {
    double modulus = self->aaPatchX[k] ;
    double angle = self->aaPatchY[k] ;
    double weight = self->aaMask[k] ;

    double x = angle * inverseStep ;
    vl_index bin = vl_floor_d(x) ;
    double w2 = x - bin ;
    double w1 = 1.0 - w2 ;

    hist[(bin + numBins) % numBins] += w1 * (modulus * weight) ;
    hist[(bin + numBins + 1) % numBins] += w2 * (modulus * weight) ;
  }

  /* smooth histogram */
  for (iter = 0; iter < 6; iter ++) {
    double prev = hist [numBins - 1] ;
    double first = hist [0] ;
    vl_index i ;
    for (i = 0; i < (signed)numBins - 1; ++i) {
      double curr = (prev + hist[i] + hist[(i + 1) % numBins]) / 3.0 ;
      prev = hist[i] ;
      hist[i] = curr ;
    }
    hist[i] = (prev + hist[i] + first) / 3.0 ;
  }

  /* find the histogram maximum */
  maxValue = 0 ;
  for (i = 0 ; i < (signed)numBins ; ++i) {
    maxValue = VL_MAX (maxValue, hist[i]) ;
  }

  /* find peaks within 80% from max */
  *numOrientations = 0 ;
  for(i = 0 ; i < (signed)numBins ; ++i) {
    double h0 = hist [i] ;
    double hm = hist [(i - 1 + numBins) % numBins] ;
    double hp = hist [(i + 1 + numBins) % numBins] ;

    /* is this a peak? */
    if (h0 > fraction * maxValue && h0 > hm && h0 > hp) {
      /* quadratic interpolation */
      double di = - 0.5 * (hp - hm) / (hp + hm - 2 * h0) ;
      double th = step * (i + di) ;
      if (self->transposed) {
        /* the axis to the right is y, measure orientations from this */
        th = VL_PI/2 - th ;
      }
      self->orientations [*numOrientations] = th ;
      *numOrientations += 1 ;
      if (*numOrientations >= VL_COVDET_MAX_NUM_ORIENTATIONS) break ;
    }
  }

  return self->orientations ;
}

void
vl_covdet_extract_orientations (VlCovDet * self)
{
  vl_index i, j  ;
  vl_size numFrames = vl_covdet_get_num_features(self) ;
  for (i = 0 ; i < (signed)numFrames ; ++i) {
    vl_size numOrientations ;
    VlCovDetFeature feature = self->frames[i] ;
    double const * angles =
    vl_covdet_extract_orientations_for_frame(self, &numOrientations, feature.frame) ;

    for (j = 0 ; j < (signed)numOrientations ; ++j) {
      double A [2*2] = {
        feature.frame.a11,
        feature.frame.a21,
        feature.frame.a12,
        feature.frame.a22} ;
      double r1 = cos(angles[j]) ;
      double r2 = sin(angles[j]) ;
      VlFrameOrientedEllipse * oriented ;

      if (j == 0) {
        oriented = & self->frames[i].frame ;
      } else {
        vl_covdet_append_feature(self, &feature) ;
        oriented = & self->frames[self->numFrames -1].frame ;
      }

      oriented->a11 = + A[0] * r1 + A[2] * r2 ;
      oriented->a21 = + A[1] * r1 + A[3] * r2 ;
      oriented->a12 = - A[0] * r2 + A[2] * r1 ;
      oriented->a22 = - A[1] * r2 + A[3] * r1 ;
    }
  }
}


/* ---------------------------------------------------------------- */
/*                                    Extract scales with Laplacian */
/* ---------------------------------------------------------------- */

double * vl_covdet_extract_laplacian_scales_for_frame (VlCovDet * self,
                                                       vl_size * numScales,
                                                       VlFrameOrientedEllipse frame)
{
  assert(self) ;
  assert(numScales) ;

  int err ;
  double sigmaImage = 1.0 / sqrt(2.0) ;
  double step = 0.5 / sqrt(2.0) ;
  vl_size const w = VL_COVDET_LAP_PATCH_RESOLUTION ;
  vl_size const num = 2*w+1;
  double extent = step * w ;
  vl_index k ;
  float const * pt ;
  double scores [VL_COVDET_LAP_NUM_LEVELS] ;

  err = vl_covdet_extract_patch_for_frame(self,
                                          self->lapPatch,
                                          w,
                                          extent,
                                          sigmaImage,
                                          frame) ;

  pt = self->laplacians ;
  for (k = 0 ; k < VL_COVDET_LAP_NUM_LEVELS ; ++k) {
    vl_index q ;
    double score = 0 ;
    for (q = 0 ; q < (signed)(num * num) ; ++q) {
      score += (*pt++) * self->lapPatch[q] ;
    }
    scores[k] = score ;
  }

  /* find and interpolate maxima */
  *numScales = 0 ;
  for (k = 1 ; k < VL_COVDET_LAP_NUM_LEVELS - 1 ; ++k) {
    double a = scores[k-1] ;
    double b = scores[k] ;
    double c = scores[k+1] ;
    if ((b > a && b > c) || (b < a && b < c)) {
      double dk = - 0.5 * (c - a) / (c + a - 2 * b) ;
      double scale = pow(2.0, (k + dk) / (VL_COVDET_MAX_NUM_LAPLACIAN_SCALES - 1) - 0.5) ;
      if (*numScales < VL_COVDET_MAX_NUM_LAPLACIAN_SCALES) {
        self->scales[*numScales++] = scale ;
      }
    }
  }

  return self->scales ;
}

void
vl_covdet_extract_laplacian_scales (VlCovDet * self)
{
  vl_index i, j  ;
  vl_size numFrames = vl_covdet_get_num_features(self) ;
  for (i = 0 ; i < (signed)numFrames ; ++i) {
    vl_size numScales ;
    VlCovDetFeature feature = self->frames[i] ;
    double const * scales =
    vl_covdet_extract_laplacian_scales_for_frame(self, &numScales, feature.frame) ;

    for (j = 0 ; j < (signed)numScales ; ++j) {
      VlFrameOrientedEllipse * scaled ;

      if (j == 0) {
        scaled = & self->frames[i].frame ;
      } else {
        vl_covdet_append_feature(self, &feature) ;
        scaled = & self->frames[self->numFrames -1].frame ;
      }

      scaled->a11 *= scales[j] ;
      scaled->a21 *= scales[j] ;
      scaled->a12 *= scales[j] ;
      scaled->a22 *= scales[j] ;
    }
  }
}

/* ---------------------------------------------------------------- */
/*                                     Setter, getter, and all that */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/** @brief Get wether images are passed in transposed
 ** @param self ::VlCovDet object.
 ** @return whether images are transposed.
 **/
vl_bool
vl_covdet_get_transposed (VlCovDet const  * self)
{
  return self->transposed ;
}

/** @brief Set the index of the first octave
 ** @param self ::VlCovDet object.
 ** @param t whether images are transposed.
 **/
void
vl_covdet_set_transposed (VlCovDet * self, vl_bool t)
{
  self->transposed = t ;
}

/* ---------------------------------------------------------------- */
/** @brief Get the edge threshold
 ** @param self ::VlCovDet object.
 ** @return the edge threshold.
 **/
double
vl_covdet_get_edge_threshold (VlCovDet const * self)
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

/* ---------------------------------------------------------------- */
/** @brief Get the peak threshold
 ** @param self ::VlCovDet object.
 ** @return the peak threshold.
 **/
double
vl_covdet_get_peak_threshold (VlCovDet const * self)
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
vl_covdet_get_first_octave (VlCovDet const * self)
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

/* ---------------------------------------------------------------- */
/** @brief Get the octave resolution.
 ** @param self ::VlCovDet object.
 ** @return octave resolution.
 **/

vl_size
vl_covdet_get_octave_resolution (VlCovDet const * self)
{
  return self->octaveResolution ;
}

/** @brief Set the octave resolutuon.
 ** @param self ::VlCovDet object.
 ** @param octave resoltuion.
 **
 ** Calling this function resets the detector.
 **/
void
vl_covdet_set_octave_resolution (VlCovDet * self, vl_size r)
{
  self->octaveResolution = r ;
  vl_covdet_reset(self) ;
}

/* ---------------------------------------------------------------- */
/** @brief Get number of stored frames
 ** @return number of frames stored in the detector.
 **/
vl_size
vl_covdet_get_num_features (VlCovDet const * self)
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

