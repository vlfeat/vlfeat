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
      VL_PRINTF("test_imopv: error: %s (%d)\n", vl_err_msg, vl_err_no) ;
      return -1 ;
    }
    width = im.width ;
    height = im.height ;
  }
  
  VL_PRINTF("test_imopv: width: %d, height: %d\n", width, height);
  
  dest  = vl_malloc (sizeof(float) * width * height) ;
  filt  = vl_malloc (sizeof(float) * (2*W+1)) ;
  
  for (x = 0 ; x < 2*W+1 ; ++ x) {
    filt [x] = 1.0f / (2*W+1) ;
  }

  vl_tic() ;
  {
    int trial = 0 ;
    for (trial = 0 ; trial < 1000 ; ++trial)
      vl_imconvcol_vf (dest, height, 
                       image, width, height, width,
                       filt, -W, W, 1,
                       VL_TRANSPOSE|VL_PAD_BY_CONTINUITY|VL_NO_SIMD) ;
  }
  VL_PRINTF ("Elapsed time (NO SIMD+TRANSPOSE): %f [s]\n", vl_toc()) ;
  
  vl_tic() ;
  {
    int trial = 0 ;
    for (trial = 0 ; trial < 1000 ; ++trial)
      vl_imconvcol_vf (dest, height,
                       image, width, height, width, 
                       filt, -W, W, 1,
                       VL_TRANSPOSE|VL_PAD_BY_CONTINUITY) ;
  }
  VL_PRINTF ("Elapsed time (SIMD+TRANSPOSE): %f [s]\n", vl_toc()) ;
  
  vl_pgm_write_f("/tmp/src.pgm", image, width, height) ;
  vl_pgm_write_f("/tmp/test.pgm", dest, height, width) ;
  
  vl_free(image) ;
  vl_free(filt) ;
  vl_free(dest) ;
  
  return 0 ;
}

