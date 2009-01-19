/** @internal
 ** @file     imopv_sse2.c
 ** @author   Andrea Vedaldi
 ** @brief    Vectorized image operations - SSE2 - Definition
 **/

#if defined(VL_SUPPORT_SSE2) & ! defined(__SSE2__)
#error "Compiling with VL_SUPPORT_SSE2, but no __SSE2__ defined"
#endif

#if defined(VL_SUPPORT_SSE2)

#include <emmintrin.h>
#include "imopv.h"
#include "imopv_sse2.h"

#define FLOAT_TYPE_FLOAT 1
#define FLOAT_TYPE_DOUBLE 2

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_FLOAT
#include "imopv_sse2.tc"

#undef FLOAT_TYPE
#define FLOAT_TYPE FLOAT_TYPE_DOUBLE
#include "imopv_sse2.tc"

#endif
