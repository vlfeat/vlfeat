/** file: test_gauss_elimination.cpp */

#include <vl/random.h>
#include <vl/mathop.h>

#include <math.h>

int
main(int argc VL_UNUSED, char**argv VL_UNUSED)
{
  double A [] = { 1, 12, 3, 4 };
  double b [] = { 1, -2 } ;
  double x [2] ;
  double c [2] ;
  int i ;

  vl_solve_linear_system_2 (x, A, b) ;

  c[0] = A[0] * x[0] + A[2] * x[1] ;
  c[1] = A[1] * x[0] + A[3] * x[1] ;

  for (i = 0 ; i < 2 ; ++i) {
    VL_PRINTF("[A x]_%-4d = %4g, b_%-4d = %4g\n", i+1, c[i], i+1, b[i]) ;
  }

  return 0 ;
}
