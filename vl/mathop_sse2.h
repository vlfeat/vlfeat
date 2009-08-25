/*
 *  untitled.h
 *  vlfeat
 *
 *  Created by Andrea Vedaldi on 12/07/2009.
 *  Copyright 2009 UCLA. All rights reserved.
 *
 */

#ifndef VL_MATHOP_SSE2_H
#define VL_MATHOP_SSE2_H

#ifdef VL_SUPPORT_SSE2

#undef FLT
#define FLT VL_TYPE_DOUBLE
#include "mathop_sse2.th"

#undef FLT
#define FLT VL_TYPE_FLOAT
#include "mathop_sse2.th"

/* VL_SUPPORT_SSE2 */
#endif

/* VL_MATHOP_SSE2_H */
#endif
