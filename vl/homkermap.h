/** @file    homker.h
 ** @author  Andrea Vedaldi
 ** @brief   Homogeneous kernel map
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available under the terms of the
 GNU GPLv2, or (at your option) any later version.
 */

#ifndef VL_HOMKER_H
#define VL_HOMKER_H

#include "generic.h"

#include <math.h>

typedef enum {
  VlHomogeneousKernelIntersection = 0,
  VlHomogeneousKernelChi2
} VlHomogeneousKernelType ;

typedef struct _VlHomogeneousKernelMap
{
  VlHomogeneousKernelType kernelType ;
  vl_size order ;
  double step ;
  vl_uint numSubdivisions ;
  double subdivision  ;
  vl_int minExponent ;
  vl_int maxExponent ;
  double * table ;
} VlHomogeneousKernelMap ;


VL_EXPORT VlHomogeneousKernelMap *
vl_homogeneouskernelmap_new (VlHomogeneousKernelType kernelType, vl_size order, double step) ;

VL_EXPORT void
vl_homogeneouskernelmap_delete (VlHomogeneousKernelMap * self) ;

VL_INLINE void
vl_homogeneouskernelmap_evaluate_d (VlHomogeneousKernelMap const * self,
                                    double * destination,
                                    vl_size stride,
                                    double x)
{
  /* break value into exponent and mantissa */
  int exponent ;
  int unsigned j ;
  double mantissa = frexp(x, &exponent) ;
  mantissa *= 2 ;
  exponent -- ;

  /*VL_PRINTF("%f %f %d %f\n", x, mantissa, exponent, self->subdivision);*/

  if (mantissa == 0 ||
      exponent <= self->minExponent ||
      exponent >= self->maxExponent) {
    for (j = 0 ; j <= self->order ; ++j) {
      *destination = 0.0 ;
      destination += stride ;
    }
    return  ;
  }
  {
    vl_size featureDimension = 2*self->order + 1 ;
    double const * v1 = self->table +
    (exponent - self->minExponent) * self->numSubdivisions * featureDimension ;
    double const * v2 ;
    double f1, f2 ;

    mantissa -= 1.0 ;
    while (mantissa >= self->subdivision) {
      mantissa -= self->subdivision ;
      v1 += featureDimension ;
    }
    v2 = v1 + featureDimension ;
    for (j = 0 ; j < featureDimension ; ++j) {
      f1 = *v1++ ;
      f2 = *v2++ ;
      *destination = (f2 - f1) * (self->numSubdivisions * mantissa) + f1 ;
      destination += stride ;
    }
  }
}

/* VL_HOMKER_H */
#endif
