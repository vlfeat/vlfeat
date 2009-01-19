/** @file    mathop.c
 ** @author  Andrea Vedaldi
 ** @brief   Math operations - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "mathop.h"

#define FLOAT_TYPE_FLOAT 1
#define FLOAT_TYPE_DOUBLE 2

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_DOUBLE


#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_FLOAT


float
vl_dist_l2_f (float * dist, int M, int NX, int NY,
              float const* x, float const* y) ;


