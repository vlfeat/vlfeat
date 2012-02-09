/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
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
