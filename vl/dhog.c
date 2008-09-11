/** @internal
 ** @file   dhog.c
 ** @author Andrea Vedaldi
 ** @brief  Dense Histogram of Oriented Gradients (DHOG) - Definition
 **/

/* AUTORIGHTS */

#include "dhog.h"
#include "pgm.h"
#include "mathop.h"
#include "imopv.h"
#include <math.h>
#include <string.h>

int const NBP = 4 ;
int const NBO = 8 ;


/** 
 @file dhog.h
 
 This module implements the Dense Histogram of Gradient descriptor (DHOG).
 The DHOG descriptor is equivalent or similar to a
 @ref sift-descriptor "SIFT descriptor" computed
 at each pixel of an image, for a fixed scale and orientation.
 
 Each DHOG descriptor is an histogram of the gradient orientations inside
 a square image patch, very similar to a SIFT descriptor. The histogram has @f$N_b@f$ bins along each
 spatial direction @e x and @e y and @f$N_o@f$ bins along the orientation
 dimension. Each spatial bin correspond a portion of image which has
 size equal to @f$\Delta@f$ pixels (along @e x and @e y), where @f$\Delta@f$
 is an even number. In addition, bins are bilinearly interpolated and partially
 overlap. The following figure illustrates which pixels (tick marks along 
 the axis) are associated to one of two bins, and with weights (triangular
 signals).
 
 @image html dhog-bins.png
 
 The top row shows bins of extension @f$\Delta=2@f$ whose center is aligned
 to a pixel. The bottom row shows the same bins, but whose center sits
 in between two adjacent pixels. Notice that while the bin extent is always
 the same, in the first case
 the pixels that actually contributes to a bin (i.e. the ones with weights
 greater than zero) are @f$2\Delta -1@f$, while in the second case
 they are @f$2\Delta@f$.
 
 While both arrangements could be used, this implementation uses only
 the first (centers aligned to pixels), which is valid also for the case
 @f$\Delta=1@f$.
 
 @section dhog-grid Covering with descriptor and downsampling
 
 DHOG extracts a dense collection of descriptors, one each few pixels.
 Since most calculations use convolutions, for simplicity we require
 the centers of all descriptor bins to lie at integer coordinates within
 the image boundaries.
 
 The center of the top-left bin of the top-left descriptor has
 coordinates 
 @f[
 x_0(0) = 0,\qquad y_0(0) = 0 
 @f]
 The center of the top-left bin of any other descriptor is
 @f[
 x_0(i) = x_0(0) + i \delta,
 \qquad
 y_0(j) = y_0(0) + j \delta 
 @f]
 where @f$\delta@f$ is the sampling step (an integer not smaller than one).
 The center of the bottom-right bin is related to the center of the
 top-left one by
 @f[
 x_{N_s-1}(i) = x_0(i) + \Delta (N_s - 1),
 \qquad
 y_{N_s-1}(i) = y_0(i) + \Delta (N_s - 1).
 @f]
 Notice that if @f$N_s = 1@f$ the two bins coincide.
 Our conditions translate into @f$0 \leq x_0(0) \leq 
 x_{N_s-1}(i_{\mathrm{max}}) \leq \mathrm{width} - 1@f$. Hence
 @f[
  0 \leq i \leq 
 \lfloor 
 \frac{\mathrm{width} - 1 - \Delta (N_s - 1)}{\delta}
 \rfloor
 @f]
 and similarly for the @e y coordinate. Notice that the center of
 each descriptor is then given by
 @f[
 x_{(N_s-1)/2}(i) = \delta i + \frac{\Delta (N_s - 1)}{2},
 \qquad
 y_{(N_s-1)/2}(i) = \delta j + \frac{\Delta (N_s - 1)}{2}.
 @f]
 
 
 
 **/


/** ------------------------------------------------------------------
 ** @internal@brief Normalize histogram
 **
 ** @param begin
 ** @param end
 **/
 
VL_INLINE float
normalize_histogram 
(float *begin, float *end)
{
  float* iter ;
  float  norm = 0.0 ;
  
  for (iter = begin ; iter != end ; ++ iter)
    norm += (*iter) * (*iter) ;
  
  norm = vl_fast_sqrt_f (norm) + VL_EPSILON_F ;
  
  for (iter = begin; iter != end ; ++ iter)
    *iter /= norm ;
  
  return norm;
}

/** ------------------------------------------------------------------
 ** @brief Allocate and initialize a new DHOG filter
 **
 ** @param width
 ** @param height
 ** @param sampling_step step used to sample descriptors (must be 1 or a multiple of bin_size).
 ** @param bin_size
 ** @return new filter.
 **/

VL_EXPORT
VlDhogFilter* 
vl_dhog_new (int width, int height, int sampling_step, int bin_size)
{
  int t ;
  VlDhogFilter* f = vl_malloc (sizeof(VlDhogFilter)) ;
  
  f-> width   = width ;
  f-> height  = height ;
  f-> dwidth  = (width  - 1 - (NBP - 1)*bin_size) / sampling_step + 1;
  f-> dheight = (height - 1 - (NBP - 1)*bin_size) / sampling_step + 1;
  f-> nkeys  = f->dwidth * f->dheight ;
  f-> sampling_step = sampling_step ;
  f-> bin_size = bin_size ;

  f-> descr = vl_malloc (sizeof(float) * 128 * f->nkeys) ;
  f-> keys  = vl_malloc (sizeof(VlDhogKeypoint) * f->nkeys) ;
  f-> tmp   = vl_malloc (sizeof(float) * f->width * f->height) ;
  f-> tmp2  = vl_malloc (sizeof(float) * f->width * f->height) ;
  f-> hist  = vl_malloc (sizeof(float*) * NBO) ;
  for (t = 0 ; t < NBO ; ++t) {
    f->hist [t] = vl_malloc (sizeof(float) * f->width * f->height) ;
  }
  return f ;
}

/** ------------------------------------------------------------------
 ** @brief Delete DHOG filter
 ** @param f filter to delete.
 **/

VL_EXPORT
void
vl_dhog_delete (VlDhogFilter *f)
{
  if (f) {
    int t ;
    if (f->hist) {      
      for (t = 0 ; t < NBO ; ++t)
        if (f->hist[t]) vl_free (f->hist[t]) ;
      vl_free (f->hist) ;
    }
    if (f->tmp2) vl_free (f->tmp2) ;
    if (f->tmp) vl_free (f->tmp) ;
    if (f->descr) vl_free (f->keys) ;
    if (f->descr) vl_free (f->descr) ;
    vl_free(f) ;
  }
}


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Initialize new convolution kernel
 ** @param f DHOG filter.
 ** @param i bin index.
 ** @return Filter.
 **/
float * _vl_dhog_new_kernel (VlDhogFilter *f, int i)
{
  int filt_len = 2*f->bin_size - 1 ;
  float * ker = vl_malloc (sizeof(float) * filt_len) ;
  float delta = f->bin_size * (NBP / 2.0f - i) ;
  float * kerit = ker ;
  float sigma = (NBP * f->bin_size) / 2.0f ;
  int k ;
  
  for (k  = - f->bin_size + 1 ;
       k <= + f->bin_size - 1 ;
       ++ k) {
    float z = (k - delta) / sigma ;
    *kerit++ = (1.0f - fabsf(k) / f->bin_size) * 
               (i >= 0) ? expf(-0.5f * z*z) : 1.0f ;
  }
  return ker ;
}

/** ------------------------------------------------------------------
 ** @internal@brief Process with Gaussian window
 ** @param f filter to delete.
 **/

VL_INLINE 
void _vl_dhog_with_gaussian_window (VlDhogFilter* f)
{
  int binx, biny, bint, centx, centy ;
  float *xker, *yker ;
  int W = f->bin_size - 1 ;
    
  for (biny = 0 ; biny < NBP ; ++biny) {
    yker = _vl_dhog_new_kernel(f, biny) ;
    
    for (binx = 0 ; binx < NBP ; ++binx) {
      xker = _vl_dhog_new_kernel(f, binx) ;
      
      for (bint = 0 ; bint < NBO ; ++bint) {
        float *dst = f->descr + binx * NBO + biny * (NBP*NBO) + bint ;
        float *src = f->tmp ;
        
        vl_imconvcol_vf (f->tmp2, f->height,
                         f->hist [bint], f->width, f->height, f->width,
                         yker, -W, +W, 1,
                         VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
        
        vl_imconvcol_vf (f->tmp, f->width,
                         f->tmp2, f->height, f->width, f->height,
                         xker, -W, +W, 1,
                         VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
        
        {
          int A = f->bin_size * (NBP - 1) ;
          int d = f->sampling_step ;
          
          for (centy = 0 ;
               centy <= (f->height - 1 - A) / d ;
               ++ centy) {
            for (centx = 0 ;
                 centx <= (f->width - 1 - A) / d ;
                 ++ centx) {
              *dst = src [(centx + f->width*centy)*d +
                          (binx + f->width*biny)*f->bin_size] ;
              dst += NBP*NBP*NBO ;
            }
          }
        }
      } /* for t */
      vl_free (xker) ;
    } /* for x */
    vl_free (yker) ;
  } /* for biny */  
}

/** ------------------------------------------------------------------
 ** @internal@brief Process with flat window.
 ** @param f filter to delete.
 **/

VL_INLINE 
void _vl_dhog_with_flat_window (VlDhogFilter* f)
{
  int binx, biny, bint, centx, centy ;
/*  float *ker = _vl_dhog_new_kernel(f, -1);*/
  int W = f->bin_size - 1 ;
    
  for (bint = 0 ; bint < NBO ; ++bint) {

    /*
    vl_imconvcol_vf (f->tmp2, f->height,
                     f->hist [bint], f->width, f->height, f->width,
                     ker, -W, +W, 1,
                     VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
    
    vl_imconvcol_vf (f->tmp, f->width,
                     f->tmp2, f->height, f->width, f->height,
                     ker, -W, +W, 1,
                     VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
    */
    
    vl_imconvcoltri_vf (f->tmp2, f->height,
                        f->hist [bint], f->width, f->height, f->width,
                        W, 1,
                        VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
    
    vl_imconvcoltri_vf (f->tmp, f->width,
                        f->tmp2, f->height, f->width, f->height,
                        W, 1,
                        VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
    
    
    for (biny = 0 ; biny < NBP ; ++biny) {
      for (binx = 0 ; binx < NBP ; ++binx) {
        
        float *dst = f->descr + binx * NBO + biny * (NBP*NBO) + bint ;
        float *src = f->tmp ;
        int A = f->bin_size * (NBP - 1) ;
        int d = f->sampling_step ;
        
        for (centy = 0 ;
             centy <= (f->height - 1 - A) / d ;
             ++ centy) {
          for (centx = 0 ;
               centx <= (f->width - 1 - A) / d ;
               ++ centx) {
            *dst = src [(centx + f->width*centy)*d +
                        (binx + f->width*biny)*f->bin_size] ;
            dst += NBP*NBP*NBO ;
          } /* centx */
        } /* centy */
      } /* binx */
    } /* biny */
  } /* bint */
/*  vl_free (ker) ;*/
}

/** ------------------------------------------------------------------
 ** @brief Compute Dense Feature Transform
 **
 ** @param f    DHOG filter. 
 ** @param im   image data.
 ** @param fast speed up computation by replacing a gaussian window with
 **             a flat one 
 **/

void vl_dhog_process (VlDhogFilter* f, float const* im, vl_bool fast)
{
  int t, x, y ;

  /* clear integral images */
  for (t = 0 ; t < NBO ; ++t)
    memset (f->hist[t], 0, f->width*f->height*sizeof(float)) ;
  
#undef at
#define at(u,v) (im[(v)*f->width+(u)])
  
  /*  */
  for (y = 0 ; y < f->height ; ++y) {
    for (x = 0 ; x < f->width ; ++x) {      
      float gx, gy ;
      float angle, mod, nt, rbint ;
      int bint ;
      
      /* y derivative */
      if (y == 0) {
        gy = at(x,y+1) - at(x,y) ;
      } else if (y == f->height-1) {
        gy = at(x,y) - at(x,y-1) ;
      } else {
        gy = 0.5f*(at(x,y+1) - at(x,y-1)) ;
      }
      
      /* x derivative */
      if (x == 0) {
        gx = at(x+1,y) - at(x,y) ;
      } else if (x == f->width-1) {
        gx = at(x,y) - at(x-1,y) ;
      } else {
        gx = 0.5f*(at(x+1,y) - at(x-1,y)) ;
      }
      
      /* angle and modulus */
      angle = vl_fast_atan2_f (gy,gx) ;
      mod = vl_fast_sqrt_f (gx*gx + gy*gy) ;
            
      /* quantize angle */
      nt = vl_mod_2pi_f (angle) * (NBO / (2*VL_PI)) ;
      bint = vl_floor_f (nt) ;
      rbint = nt - bint ;
      
      /* write it back */
      f->hist[bint%NBO]    [x + y * f->width] += (1 - rbint) * mod ;      
      f->hist[(bint+1)%NBO][x + y * f->width] += rbint * mod ;
    }
  }

  if (fast) {
    _vl_dhog_with_flat_window(f) ;
  } else {
    _vl_dhog_with_gaussian_window(f) ;
  }
  
  {
    VlDhogKeypoint* k = f->keys ;
    int centx, centy, bint ;
    int A = f->bin_size * (NBP - 1) ;
    double A_ = f->bin_size * (NBP - 1) / 2.0f ;
    int d = f->sampling_step ;
    float *dpt = f->descr ;
    
    for (centy = 0 ;
         centy <= (f->height - 1 - A) / d ;
         ++ centy) {
      for (centx = 0 ;
           centx <= (f->width - 1 - A) / d ;
           ++ centx) {
        k->x = centx*d + A_ ;
        k->y = centy*d + A_ ;
        
        k->norm = normalize_histogram (dpt, dpt + NBP*NBP*NBO) ;
        for(bint = 0; bint < NBO*NBP*NBP ; ++ bint) {
          if (dpt[bint] > 0.2f) dpt[bint] = 0.2f ;
        }
        k->norm *= normalize_histogram (dpt, dpt + NBP*NBP*NBO) ;
        ++ k ;
        dpt += NBP*NBP*NBO ;
      } /* centx */
    } /* centy */
  }
}
  
