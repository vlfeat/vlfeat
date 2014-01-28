/** @file test_sqrti.c
 ** @brief Test integer square root algorithm
 ** @author Andrea Vedaldi
 **/

/*
 Copyright (C) 2014 Andrea Vedaldi.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#include <vl/mathop.h>
#include <math.h>
#include <stdio.h>

int
main (int argc VL_UNUSED, char** argv VL_UNUSED)
{
  vl_index i ;
  for (i = 0 ; i < 0xff ; ++i) {
    vl_uint8 x = (vl_uint8)i ;
    vl_uint8 r = vl_fast_sqrt_ui8(x) ;
    vl_uint8 r_ = (vl_uint8) floor(sqrt((double)x)) ;
    if (r != r_) abort() ;
    /* printf("sqrt(%5d) = %5d -- %5d\n", i, r, r_) ; */
  }

  for (i = 0 ; i < 0xffffffff ; ++i) {
    vl_uint16 x = (vl_uint16)i ;
    vl_uint16 r = vl_fast_sqrt_ui16(x) ;
    vl_uint16 r_ = (vl_uint8) floor(sqrt((double)x)) ;
    if (r != r_) abort() ;
    /* printf("sqrt(%5d) = %5d -- %5d\n", i, r, r_) ; */
  }

  return 0 ;
}
