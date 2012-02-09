/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
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
