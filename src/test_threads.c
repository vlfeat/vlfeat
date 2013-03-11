/** @file   test_threads.c
 ** @author Andrea Vedaldi
 ** @brief  Test threaded VLFeat client
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
Copyright (C) 2013 Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/generic.h>
#include <vl/random.h>

#include <stdio.h>

#if defined(VL_THREADS_POSIX)
#include <pthread.h>
#endif

#if defined(VL_THREADS_WIN)
#include <Windows.h>
#endif

#if defined(VL_THREADS_POSIX)

void *
testThread(void * args)
{
  int j ;
  int id = *(int*)args ;
  vl_tic() ;
  for (j = 0 ; j < 10 ; ++j) {
    printf("Thread %5d: %d\n",
      id, vl_rand_int31(vl_get_rand())) ;
    fflush(stdout) ;
  }
  printf("Thread %5d: elapsed time: %.2f s\n", id, vl_toc()) ;
  return NULL ;
}

#elif defined(VL_THREADS_WIN)

DWORD WINAPI
testThread(LPVOID args)
{
  int j ;
  int id = *(DWORD*)args ;
  vl_tic() ;
  for (j = 0 ; j < 10 ; ++j) {
    printf("Thread %5d: %d\n",
      id, vl_rand_int31(vl_get_rand())) ;
    fflush(stdout) ;
  }
  printf("Thread %5d: elapsed time: %.2f s\n", id, vl_toc()) ;
  return 0 ;
}

#endif

int
main(int argc VL_UNUSED, char** argv VL_UNUSED)
{
#ifndef VL_DISABLE_THREADS
#if defined(VL_THREADS_POSIX)
  vl_uindex i ;
  pthread_t threads [5] ;
  int threadIds [5] ;
  for (i = 0 ; i < sizeof(threads) / sizeof(threads[0]) ; ++i) {
    threadIds [i] = i ;
    pthread_create (threads + i, NULL, testThread, threadIds + i) ;
  }
  for (i = 0 ; i < sizeof(threads) / sizeof(threads[0]) ; ++i) {
    pthread_join (threads[i], NULL) ;
  }
#elif defined(VL_THREADS_WIN)
  int i ;
  DWORD threadIds [5] ;
  HANDLE threadHandles [5] ;
  for (i = 0 ; i < sizeof(threadHandles) / sizeof(threadHandles[0]) ; ++i) {
    threadHandles [i] = CreateThread (
      NULL, 0,
      testThread, threadIds + i,
      0, threadIds + i) ;
  }
  for (i = 0 ; i < sizeof(threadHandles) / sizeof(threadHandles[0]) ; ++i) {
    WaitForSingleObject (threadHandles[i], INFINITE);
  }
#endif
/* VL_DISABLE_THREADS */
#else
  printf("VLFeat was compiled without threading support\n") ;
#endif
  {
    char * string = vl_configuration_to_string_copy() ;
    printf("%s", string) ;
    vl_free(string) ;
  }
  return 0 ;
}
