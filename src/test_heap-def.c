/** @file   test_heap-def.c
 ** @brief  Test heap-def.h
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#define VL_HEAP_prefix vl_heap_float
#define VL_HEAP_type   float
#include <vl/heap-def.h>

#include <stdio.h>

typedef struct _S { int x ; } S ;
int s_cmp (S const * v, vl_uindex a, vl_uindex b)
{
  return v[a].x - v[b].x ;
}
void s_swap (S * v, vl_uindex a, vl_uindex b)
{
  S t = v[a] ;
  v[a] = v[b] ;
  v[b] = t ;
  printf("Swapping %" VL_FMT_UINDEX "x with %" VL_FMT_UINDEX "\n", a, b) ;
}

#define VL_HEAP_prefix  s_heap
#define VL_HEAP_type    S
#define VL_HEAP_cmp     s_cmp
#include <vl/heap-def.h>

#define VL_HEAP_prefix  track_s_heap
#define VL_HEAP_type    S
#define VL_HEAP_cmp     s_cmp
#define VL_HEAP_swap    s_swap
#include <vl/heap-def.h>

typedef struct _H {
  vl_size numNodes ;
  int* array ;
} H ;
int h_cmp (H const * h, vl_uindex a, vl_uindex b) {
  return h->array[a] - h->array[b] ;
}
void h_swap (H * h, vl_uindex a, vl_uindex b) {
  int t = h->array[a] ;
  h->array[a] = h->array[b] ;
  h->array[b] = t ;
}
#define VL_HEAP_prefix      h_heap
#define VL_HEAP_array       H*
#define VL_HEAP_array_const H const*
#define VL_HEAP_swap        h_swap
#define VL_HEAP_cmp         h_cmp
#include <vl/heap-def.h>

int
main (int argc VL_UNUSED, char** argv VL_UNUSED)
{
  vl_uindex i ;
  vl_size numNodes = 0 ;
  float data [] = {1.01, 5.02, 8, 0.1, 100, 3, 9, 4, 1.02} ;
  S data_s [] = {{5}, {7}, {9}, {1}} ;
  S data_s_track [] = {{5}, {7}, {9}, {1}} ;
  int data_h [] = {5, 7, 9, 1} ;
  H h ;
  h.numNodes = 0 ;
  h.array = data_h ;

  printf("Pushing heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    printf ("%5" VL_FMT_UINDEX ": %f\n", i, data[i]) ;
    vl_heap_float_push (data, &numNodes) ;
  }

  printf("Popping heap\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    printf ("%" VL_FMT_UINDEX ": %f\n", i, data[vl_heap_float_pop (data, &numNodes)]) ;
  }

  printf("Refilling, updating fourth element, and popping again\n") ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    vl_heap_float_push (data, &numNodes) ;
  }
  printf("%f -> %f\n", data[3], 9.01) ;
  data [3] = 9.01 ;
  vl_heap_float_update (data, numNodes, 3) ;
  for (i = 0 ; i < sizeof(data) / sizeof(data[0]) ; ++i) {
    printf ("%" VL_FMT_UINDEX ":  %f\n", i, data[vl_heap_float_pop (data, &numNodes)]) ;
  }

  printf("Pushing heap of structures\n") ;
  numNodes = 0 ;
  for (i = 0 ; i < sizeof(data_s) / sizeof(data_s[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, data_s[i].x) ;
    s_heap_push (data_s, &numNodes) ;
  }

  printf("Popping heap of structures\n") ;
  for (i = 0 ; i < sizeof(data_s) / sizeof(data_s[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, data_s[s_heap_pop (data_s, &numNodes)].x) ;
  }

  printf("Pushing heap of structures with custom swap\n") ;
  numNodes = 0 ;
  for (i = 0 ; i < sizeof(data_s) / sizeof(data_s[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, data_s_track[i].x) ;
    track_s_heap_push (data_s_track, &numNodes) ;
  }

  printf("Popping heap of structures with custom swap\n") ;
  for (i = 0 ; i < sizeof(data_s) / sizeof(data_s[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, data_s_track
               [track_s_heap_pop (data_s_track, &numNodes)].x) ;
  }

  printf("Pushing heap of structures with custom container\n") ;
  numNodes = 0 ;
  for (i = 0 ; i < sizeof(data_h) / sizeof(data_h[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, h.array[i]) ;
    h_heap_push (&h, &h.numNodes) ;
  }

  printf("Popping heap of structures with custom container\n") ;
  for (i = 0 ; i < sizeof(data_h) / sizeof(data_h[0]) ; ++i) {
    printf ("s[%" VL_FMT_UINDEX "].x = %d\n", i, h.array
               [h_heap_pop (&h, &h.numNodes)]) ;
  }

  return 0 ;
}

