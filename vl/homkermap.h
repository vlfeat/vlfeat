/** @file    homkermap.h
 ** @author  Andrea Vedaldi
 ** @brief   Homogeneous kernel map
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_HOMKERMAP_H
#define VL_HOMKERMAP_H

#include "generic.h"

#include <math.h>

/** @brief Type of kernel */
typedef enum {
  VlHomogeneousKernelIntersection = 0, /**< intersection kernel */
  VlHomogeneousKernelChi2, /**< Chi2 kernel */
  VlHomogeneousKernelJS /**< Jensen-Shannon kernel */
} VlHomogeneousKernelType ;

/** @brief Type of spectral windowing function  */
typedef enum {
  VlHomogeneousKernelMapWindowUniform = 0, /**< uniform window */
  VlHomogeneousKernelMapWindowRectangular = 1, /**< rectangular window */
} VlHomogeneousKernelMapWindowType ;

/** @brief Homogeneous kernel map */
typedef struct _VlHomogeneousKernelMap
{
  VlHomogeneousKernelType kernelType ;
  double gamma ;
  VlHomogeneousKernelMapWindowType windowType ;
  vl_size order ;
  double period ;
  vl_size numSubdivisions ;
  double subdivision  ;
  vl_index minExponent ;
  vl_index maxExponent ;
  double * table ;
} VlHomogeneousKernelMap ;

VL_EXPORT VlHomogeneousKernelMap *
vl_homogeneouskernelmap_new (VlHomogeneousKernelType kernelType,
                             double gamma,
                             vl_size order,
                             double period,
                             VlHomogeneousKernelMapWindowType windowType) ;

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
/* VL_HOMKERMAP_H */
#endif
