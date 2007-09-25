#include <vl/generic.h>
#include <stdio.h>

int
main(int argc, char** argv)
{
  printf("Double NaN : `%g'\n", VL_NAN_D      ) ;
  printf("Double Inf : `%g'\n", VL_INFINITY_D ) ;
  printf("Single NaN : `%g'\n", VL_NAN_F      ) ;
  printf("Signle Inf : `%g'\n", VL_INFINITY_F ) ;
  return 0 ;
}



