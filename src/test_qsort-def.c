/** @file   test_heap-def.c
 ** @brief  Test heap-def.h
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#define VL_QSORT_prefix my_qsort
#define VL_QSORT_type   float
#include <vl/qsort-def.h>

#include <stdio.h>

int
main (int argc VL_UNUSED, char ** argv VL_UNUSED)
{
  float values [] = {.4, 12.3, 44.0, 1.2, 3.4, 5.6} ;
  vl_size n = sizeof(values) / sizeof(values[0]) ;
  vl_uindex i ;

  for (i = 0 ; i < n ; ++i) printf("%5.2f ", values [i]) ;
  printf("\n") ;

  my_qsort_sort (values, n) ;

  for (i = 0 ; i < n ; ++i) printf("%5.2f ", values [i]) ;
  printf("\n") ;

  return 0 ;
}
