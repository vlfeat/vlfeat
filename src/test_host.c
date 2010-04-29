/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <vl/generic.h>

int
main(int argc VL_UNUSED, char ** argv VL_UNUSED)
{
  char * string = vl_configuration_to_string_copy() ;
  assert (string) ;
  VL_PRINTF(string) ;
  vl_free(string) ;
  return 0 ;
}
