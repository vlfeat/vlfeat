/** @file   test_heap.c
 ** @author Andrea Vedaldi
 ** @brief  Test heap.h
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <vl/heap.h>
#include <stdio.h>

int float_compare (void const * a, void const * b)
{
  float xa = *(float*)a ;
  float xb = *(float*)b ;
  if (xa < xb) return -1 ;
  if (xa > xb) return +1 ;
  return 0 ;
}

int
main (int argc, char** argv)
{
  int i ;
  float data [] = {1.01, 5.02, 8, 0.1, 100, 3, 9, 4, 1.02} ;
  VlHeap * heap = vl_heap_new (data, sizeof(float), float_compare) ;

  VL_PRINTF("Pushing heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d: %f\n", i, data[i]) ;
    vl_heap_push (heap) ;
  }

  VL_PRINTF("Popping heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d: %f\n", i, *(float*)vl_heap_pop (heap)) ;
  }

  VL_PRINTF("Refilling, updating fourth element, and popping again\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    vl_heap_push (heap) ;
  }
  VL_PRINTF("%f -> %f\n", data[3], 9.01) ;
  data [3] = 9.01 ;
  vl_heap_update (heap, 3) ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    VL_PRINTF ("%5d:  %f\n", i, *(float*)vl_heap_pop (heap)) ;
  }

  return 0 ;
}

