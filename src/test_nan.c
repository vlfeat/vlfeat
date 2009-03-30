#include <vl/generic.h>
#include <vl/mathop.h>

int
main(int argc, char** argv)
{
  VL_PRINTF ("Double   NaN : `%g'\n",   VL_NAN_D     ) ;
  VL_PRINTF ("Double   Inf : `%g'\n",   VL_INFINITY_D) ;
  VL_PRINTF ("Double - Inf : `%g'\n", - VL_INFINITY_D) ;
  VL_PRINTF ("Single   NaN : `%g'\n",   VL_NAN_F     ) ;
  VL_PRINTF ("Single   Inf : `%g'\n",   VL_INFINITY_F) ;
  VL_PRINTF ("Single - Inf : `%g'\n", - VL_INFINITY_F) ;

  VL_PRINTF ("Double: 0.0 <   VL_INFINITY_D: %d\n", 0.0 <   VL_INFINITY_D) ;
  VL_PRINTF ("Double: 0.0 > - VL_INFINITY_D: %d\n", 0.0 > - VL_INFINITY_D) ;

  return 0 ;
}



