/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <vl/generic.h>
#include <vl/mathop.h>

int
main(int argc VL_UNUSED, char** argv VL_UNUSED)
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



