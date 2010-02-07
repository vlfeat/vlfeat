/** @file   test_vec_comp.c
 ** @brief  Test vector computations
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS */

#include <vl/random.h>
#include <vl/mathop.h>

void
init_data (vl_size numDimensions, vl_size numSamples, float ** X, float ** Y)
{
  VlRand * rand = vl_get_rand() ;
  vl_uindex i ;
  float * Xi = *X = vl_malloc(sizeof(float) * numDimensions * numSamples) ;
  float * Yi = *Y = vl_malloc(sizeof(float) * numDimensions * numSamples) ;
  for (i = 0 ; i < numDimensions * numSamples ; ++ i) {
    *Xi++ = vl_rand_real1(rand) ;
    *Yi++ = vl_rand_real1(rand) ;
  }
}

int
main (int argc VL_UNUSED, char** argv VL_UNUSED)
{
  float * X ;
  float * Y ;
  vl_size numDimensions = 1000 ;
  vl_size numSamples    = 2000 ;
  float * result = vl_malloc (sizeof(float) * numSamples * numSamples) ;
  VlFloatVectorComparisonFunction f ;

  init_data (numDimensions, numSamples, &X, &Y) ;

  X+=1 ;
  Y+=1 ;

  vl_set_simd_enabled (VL_FALSE) ;
  f = vl_get_vector_comparison_function_f (VlDistanceL2) ;
  vl_tic () ;
  vl_eval_vector_comparison_on_all_pairs_f (result, numDimensions, X, numSamples, Y, numSamples, f) ;
  VL_PRINTF("Float L2 distnace: %.3f s\n", vl_toc ()) ;

  vl_set_simd_enabled (VL_TRUE) ;
  f = vl_get_vector_comparison_function_f (VlDistanceL2) ;
  vl_tic () ;
  vl_eval_vector_comparison_on_all_pairs_f (result, numDimensions, X, numSamples, Y, numSamples, f) ;
  VL_PRINTF("Float L2 distance (SIMD): %.3f s\n", vl_toc ()) ;

  X-- ;
  Y-- ;

  vl_free (X) ;
  vl_free (Y) ;
  vl_free (result) ;

  return 0 ;
}
