#include <vl/generic.h>

int
main(int argc, char** argv)
{
  VL_PRINTF ("Double NaN : `%g'\n", VL_NAN_D     ) ;
  VL_PRINTF ("Double Inf : `%g'\n", VL_INFINITY_D) ;
  VL_PRINTF ("Single NaN : `%g'\n", VL_NAN_F     ) ;
  VL_PRINTF ("Single Inf : `%g'\n", VL_INFINITY_F) ;
  return 0 ;
}



