/** @file    homker.h
 ** @author  Andrea Vedaldi
 ** @brief   Homogeneous kernel map - Definition
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available under the terms of the
 GNU GPLv2, or (at your option) any later version.
 */

/* @file homker.h

 @overview homker-tech Technical details

 The

 */

#include "homkermap.h"
#include "mathop.h"

#include <math.h>

/** ------------------------------------------------------------------
 ** @brief Create new map object
 ** @param kernelType type of homogeneous kernel.
 ** @parma order approximation order.
 ** @return the new kernel map, or NULL if memory is exhausted.
 **
 ** The function intializes a new homogeneous kernel map for the
 ** specified kernel and order.
 **/

VlHomogeneousKernelMap *
vl_homogeneouskernelmap_new (VlHomogeneousKernelType kernelType, vl_size order, double step)
{
  VlHomogeneousKernelMap * self = vl_malloc(sizeof(VlHomogeneousKernelMap)) ;
  if (! self) return NULL ;
  self->kernelType = kernelType ;
  self->order = order ;
  self->step = step ;
  self->numSubdivisions = 8  ;
  self->subdivision = 1.0 / self->numSubdivisions ;
  self->minExponent = -20 ;
  self->maxExponent = 8 ;

  assert(kernelType == VlHomogeneousKernelIntersection ||
         kernelType == VlHomogeneousKernelChi2) ;

  self->table = vl_malloc (sizeof(double) *
                           (2*self->order+1) *
                           self->numSubdivisions *
                           (self->maxExponent - self->minExponent + 1)) ;
  if (! self->table) {
    vl_free(self) ;
    return NULL ;
  }

  {
    /* Tabulate the function for

       x = mantissa 2^exponent

     where minExponent <= exponent <= maxExponent and
     1 <= mantissa < 2.

     The exponent is integer and the mantissa is sampled at points

     mantissa = 1 + i / numSubdivisions, i = 0, ..., numSubdivisions - 1

     */
    vl_int exponent ;
    double * tablep  = self->table ;
    for (exponent  = self->minExponent ;
         exponent <= self->maxExponent ; ++ exponent) {

      double L = self->step ;
      double x, Lx, sqrtLx, Llogx, lambda ;
      double kappa, sqrtkappa0, sqrt2kappa ;
      double mantissa = 1.0 ;
      vl_uint i, j ;

      for (i = 0 ; i < self->numSubdivisions ;
           ++i, mantissa += self->subdivision) {
        x = scalbn(mantissa, exponent) ;

        Lx = self-> step * x ;
        sqrtLx = sqrt(Lx) ;

        switch (self->kernelType) {
          case VlHomogeneousKernelIntersection:
            sqrtkappa0 = sqrt(2.0 / VL_PI) ;
            break ;
          case VlHomogeneousKernelChi2:
            sqrtkappa0 = 1.0 ;
            break ;
          default:
            assert(0) ;
        }
        *tablep++ = sqrtkappa0 * sqrtLx ;

        for (j = 1 ; j <= self->order ; ++j) {
          lambda = j * L ;
          Llogx = L * log(x) ;
          switch (kernelType) {
            case VlHomogeneousKernelIntersection:
              kappa = (2.0 / VL_PI) / (1 + 4 * lambda*lambda) ;
              break ;
            case VlHomogeneousKernelChi2:
              kappa = 2.0 / (exp(VL_PI * lambda) + exp(-VL_PI * lambda)) ;
              break ;
            default:
              assert(0) ;
          }
          sqrt2kappa = sqrt(2.0 * kappa) ;
          *tablep++ = sqrt2kappa * sqrtLx * cos(j * Llogx) ;
          *tablep++ = sqrt2kappa * sqrtLx * sin(j * Llogx) ;
        }
      } /* next mantissa */
    } /* next exponent */
  } /* initialize table */
  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Delete a map object
 ** @param self map object.
 ** @parma order approximation order.
 ** @return the new kernel map, or NULL if memory is exhausted.
 **/

VL_EXPORT void
vl_homogeneouskernelmap_delete (VlHomogeneousKernelMap * self)
{
  vl_free(self->table) ;
  self->table = NULL ;
  vl_free(self) ;
}
