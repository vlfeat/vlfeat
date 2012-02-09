/** @file   test_rand.c
 ** @author Andrea Vedaldi
 ** @breif  Test vl/random.h
 **/

#include <stdio.h>
#include <assert.h>

#include <vl/random.h>

int
main (int argc VL_UNUSED, char *argv[] VL_UNUSED)
{
  int i ;
  vl_uint32 init [4] = {0x123, 0x234, 0x345, 0x456} ;
  VlRand rand ;
  vl_rand_init (&rand) ;

  vl_rand_seed_by_array (&rand, init, sizeof(init)/sizeof(init[0])) ;

  printf("1000 outputs of vl_rand_uint32()\n");
  for (i=0; i<1000; i++) {
    printf("%10" VL_FL_INT32 "u ", vl_rand_uint32(&rand));
    if (i%5==4) printf("\n");
  }

  printf("\n1000 outputs of vl_rand_real2()\n");
  for (i=0; i<1000; i++) {
    printf("%10.8f ", vl_rand_real2(&rand));
    if (i%5==4) printf("\n");
  }

  return 0;
}
