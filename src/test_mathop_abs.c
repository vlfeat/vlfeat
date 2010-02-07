/*
 *  test_mathop_abs.c
 *  vlfeat
 *
 *  Created by Andrea Vedaldi on 16/07/2009.
 *  Copyright 2009 UCLA. All rights reserved.
 *
 */

#include <vl/random.h>
#include <vl/mathop.h>

#include <math.h>

int
main(int argc VL_UNUSED, char**argv VL_UNUSED)
{
  VlRand * rand = vl_get_rand() ;
  vl_size numData = 100000000 ;
  vl_size i ;
  float * X = vl_malloc(sizeof(float) * numData) ;
  float * Y = vl_malloc(sizeof(float) * numData) ;
  float * X_end = X + numData ;

  for (i = 0 ; i < numData ; ++ i) {
    X[i] = vl_rand_real1(rand) - .5 ;
    Y[i] = 0 ;
  }

  {
    float * Xi = X ;
    float * Yi = Y ;
    vl_tic() ;
    while (Xi < X_end) {
      *Yi++ = fabsf (*Xi++) ;
    }
    VL_PRINTF("fabsf: %.3f\n", vl_toc()) ;
  }

  for (i = 0 ; i < numData ; ++ i) {
    X[i] = vl_rand_real1(rand) - .5 ;
    X[i] = 0 ;
  }

  {
    float * Xi = X ;
    float * Yi = Y ;
    vl_tic() ;
    while (Xi < X_end) {
      *Yi++ = vl_abs_f (*Xi++) ;
    }
    VL_PRINTF("vl_abs_f: %.3f\n", vl_toc()) ;
  }

  vl_free (Y) ;
  vl_free (X) ;

  return 0 ;
}
