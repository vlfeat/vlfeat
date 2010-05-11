/** @file    homker.h
 ** @author  Andrea Vedaldi
 ** @brief   Homogeneous kernel map
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_HOMKER_H
#define VL_HOMKER_H

#include "generic.h"

#include <math.h>

typedef enum {
  VlHomogeneousKernelIntersection = 0,
  VlHomogeneousKernelChi2,
  VlHomogeneousKernelJS
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

VL_EXPORT void
vl_homogeneouskernelmap_evaluate_d (VlHomogeneousKernelMap const * self,
                                    double * destination,
                                    vl_size stride,
                                    double x) ;

VL_EXPORT void
vl_homogeneouskernelmap_evaluate_f (VlHomogeneousKernelMap const * self,
                                    float * destination,
                                    vl_size stride,
                                    double x) ;
/* VL_HOMKER_H */
#endif
