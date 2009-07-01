/** @file   test_heap.c
 ** @brief  Test heap-t.h
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#define NAME vl_heap_float
#define T float
#define CMP(x,y) ((*x)-(*y))
#include <vl/heap-t.h>
#undef NAME
#undef T
#undef CMP

#include <stdio.h>

int
main (int argc, char** argv)
{
  int i ;
  int unsigned numNodes = 0 ;
  float data [] = {1.01, 5.02, 8, 0.1, 100, 3, 9, 4, 1.02} ;

  VL_PRINTF("Pushing heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d: %f\n", i, data[i]) ;
    vl_heap_float_push (data, &numNodes) ;
  }

  VL_PRINTF("Popping heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d: %f\n", i, *vl_heap_float_pop (data, &numNodes)) ;
  }

  VL_PRINTF("Refilling, updating fourth element, and popping again\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    vl_heap_float_push (data, &numNodes) ;
  }
  VL_PRINTF("%f -> %f\n", data[3], 9.01) ;
  data [3] = 9.01 ;
  vl_heap_float_update (data, numNodes, 3) ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d:  %f\n", i, *vl_heap_float_pop (data, &numNodes)) ;
  }

  return 0 ;
}

