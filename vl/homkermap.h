/** @file homkermap.h
 ** @brief Homogeneous kernel map (@ref homkermap)
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
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
