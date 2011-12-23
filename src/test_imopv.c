/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/generic.h>
#include <vl/pgm.h>
#include <vl/imopv.h>

int
main (int argc, char** argv)
{
  int width = 256 ;
  int height = 256 ;
  int const W = 7 ;

  float * image ;
  float * dest  ;
  float * dest2 ;
  float * filt ;

  int x, y ;

  if (argc < 2) {
    image = vl_malloc (sizeof(float) * width * height) ;
    for (y = 0 ; y < height ; ++y) {
      for (x = 0  ; x < width ; ++x) {
        image [x + width * y] =
        128.0f * (((x % 16) < 8) ^ ((y % 16) < 8)) ;
      }
    }
  } else {
    VlPgmImage im ;
    int err ;
    err = vl_pgm_read_new_f (argv[1], &im, &image) ;
    if (err) {
      VL_PRINTF("test_imopv: error: %s (%d)\n",
                vl_get_last_error_message(),
                vl_get_last_error()) ;
      return -1 ;
    }
    width = im.width ;
    height = im.height ;
  }

  VL_PRINTF("test_imopv: width: %d, height: %d\n", width, height);

  dest  = vl_malloc (sizeof(float) * width * height) ;
  dest2 = vl_malloc (sizeof(float) * width * height) ;
  filt  = vl_malloc (sizeof(float) * (2*W+1)) ;

  for (x = 0 ; x < 2*W+1 ; ++ x) {
    filt [x] = 1.0f / (2*W+1) ;
  }

#if 1
  vl_set_simd_enabled (0) ;
  vl_tic() ;
  {
    int trial = 0 ;
    for (trial = 0 ; trial < 1000 ; ++trial)
      vl_imconvcol_vf (dest, height,
                       image, width, height, width,
                       filt, -W, W, 1,
                       VL_TRANSPOSE|VL_PAD_BY_CONTINUITY) ;
  }
  VL_PRINTF ("Elapsed time no SIMD: %f [s]\n", vl_toc()) ;

  vl_set_simd_enabled (1) ;
  vl_tic() ;
  {
    int trial = 0 ;
    for (trial = 0 ; trial < 1000 ; ++trial)
      vl_imconvcol_vf (dest2, height,
                       image, width, height, width,
                       filt, -W, W, 1,
                       VL_TRANSPOSE|VL_PAD_BY_CONTINUITY) ;
  }
  VL_PRINTF ("Elapsed time with SIMD: %f [s]\n", vl_toc()) ;

#else

  vl_imconvcoltri_vf (dest, height,
                      image, width, height, width,
                      5, 1, VL_TRANSPOSE|VL_PAD_BY_CONTINUITY) ;

  vl_imconvcoltri_vf (dest2, height,
                      image, width, height, width,
                      5, 1, VL_TRANSPOSE|VL_PAD_BY_ZERO) ;
#endif

  vl_pgm_write_f("/tmp/src.pgm", image, width, height) ;
  vl_pgm_write_f("/tmp/test.pgm", dest, height, width) ;
  vl_pgm_write_f("/tmp/test2.pgm", dest2, height, width) ;


  vl_free(image) ;
  vl_free(filt) ;
  vl_free(dest) ;
  vl_free(dest2) ;

  return 0 ;
}
