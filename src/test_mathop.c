#include <vl/generic.h>
#include <vl/mathop.h>
#include <math.h>
#include <float.h>

int
main (int argc VL_UNUSED, char** argv VL_UNUSED)
{
  int error = 0 ;

  /* -----------------------------------------------------------------
   *                                                  vl_fast_resqrt_*
   * -------------------------------------------------------------- */

  VL_PRINTF ("%20s %10s %10s %10s\n", "func", "elaps [s]", "eval/s", "chksum") ;

#define SFX f
#define SFX2 FLT
#define T float
#define SQRT sqrtf
#define ABS  fabsf
#define ONE 1.0F
#include "test_mathop_fast_resqrt.tc"
#undef ONE
#undef ABS
#undef SQRT
#undef T
#undef SFX2
#undef SFX

#define SFX d
#define SFX2 DBL
#define T float
#define SQRT sqrt
#define ABS  fabs
#define ONE 1.0
#include "test_mathop_fast_resqrt.tc"
#undef ONE
#undef ABS
#undef SQRT
#undef T
#undef SFX2
#undef SFX

  VL_PRINTF("\n") ;

  /* -----------------------------------------------------------------
   *                                                  vl_fast_sqrt_ui*
   * -------------------------------------------------------------- */

  VL_PRINTF ("%20s %10s %10s %10s\n", "func", "elaps [s]", "eval/s", "chksum") ;

#undef SFX
#undef T
#undef STEP
#define SFX 32
#define T vl_uint32
#define STEP 7
#include "test_mathop_fast_sqrt_ui.tc"

#undef SFX
#undef T
#undef STEP
#define SFX 16
#define T vl_uint16
#define STEP 0
#include "test_mathop_fast_sqrt_ui.tc"

#undef SFX
#undef T
#undef STEP
#define SFX 8
#define T vl_uint8
#define STEP 0
#include "test_mathop_fast_sqrt_ui.tc"

  return error ;
}
