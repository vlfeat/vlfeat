/*
 *  mathop_sse2.c
 *  vlfeat
 *
 *  Created by Andrea Vedaldi on 12/07/2009.
 *  Copyright 2009 UCLA. All rights reserved.
 *
 */

#include "mathop.h"
#include "mathop_sse2.h"

#ifdef VL_SUPPORT_SSE2

#undef FLT
#define FLT VL_TYPE_DOUBLE
#include "mathop_sse2.tc"

#undef FLT
#define FLT VL_TYPE_FLOAT
#include "mathop_sse2.tc"

/* VL_SUPPORT_SSE2 */
#endif
