/** @file    rodrigues.h
 ** @author  Andrea Vedaldi
 ** @brief   Rodrigues formulas
 **
 ** @section rodrigues Rodrigues formulas
 **
 ** - Use vl_rodrigues() to compute the Rodrigues formula and its derivative.
 ** - Use vl_irodrigues() to compute the inverse Rodrigues formula and
 **   its derivative.
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_RODRIGUES
#define VL_RODRIGUES

#include "generic.h"

VL_EXPORT void vl_rodrigues  (double* R_pt,  double* dR_pt, const double* om_pt) ;
VL_EXPORT void vl_irodrigues (double* om_pt, double* dom_pt, const double* R_pt) ;

/* VL_RODRIGUES */
#endif

