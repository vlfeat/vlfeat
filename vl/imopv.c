/** @file   imopv.c
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations - Definition
 **/

/* AUTORIGHTS */

#include "imopv.h"

#ifdef __SSE3__
#include <pmmintrin.h>
#endif

/** @file imopv.h
 ** 
 ** This module includes vectorized image operations.
 **/

#undef  VL_IMCONVCOL
#define VL_IMCONVCOL vl_imconvcol_vf
#undef  FLT
#define FLT float
#undef  VSIZE
#define VSIZE 4
#undef  VTYPE
#define VTYPE __m128
#undef  VMUL
#define VMUL _mm_mul_ps
#undef  VADD
#define VADD _mm_add_ps
#undef  VSTZ
#define VSTZ _mm_setzero_ps
#undef  VLD1 
#define VLD1 _mm_load1_ps
#include "imopv.tc"

#undef  VL_IMCONVCOL
#define VL_IMCONVCOL vl_imconvcol_vd
#undef  FLT
#define FLT double
#undef  VSIZE
#define VSIZE 2
#undef  VTYPE
#define VTYPE __m128d
#undef  VMUL
#define VMUL _mm_mul_pd
#undef  VADD
#define VADD _mm_add_pd
#undef  VSTZ
#define VSTZ _mm_setzero_pd
#undef  VLD1 
#define VLD1 _mm_load1_pd
#include "imopv.tc"
