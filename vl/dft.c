/** @internal
 ** @file     dft.c
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (DFT) - Definition
 **/

/* AUTORIGHTS */

#include "dft.h"
#include "pgm.h"
#include "mathop.h"
#include <math.h>
#include <strings.h>

int const NBP = 4 ;
int const NBO = 8 ;

/** ------------------------------------------------------------------
 ** @internal@brief Normalize histogram
 **
 ** @param begin
 ** @param end
 ** @param step
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
 ** @internal@brief Convolve rows, downsample, transpose in sub-block
 **
 ** The filter window is in [-W W] (2W+1 pixels). @a begin and @a end
 ** are in this range.
 **
 ** @param begin start of filter sub-block.
 ** @param end   end of filter sub-block.
 ** @param step  downsample step.
 **/
static void
econvolve(float*       dst_pt,
          const float* src_pt,    int M, int N,
          const float* filter_pt, int W,
          int begin, int end, int step)
{
  int i,j ;
  int M_down = M / step ;
  for(j = 0 ; j < N ; ++j) { 
    for(i = 0 ; i+step-1 < M ; i+=step) {
      float        acc = 0.0 ;
      float const* g = filter_pt + (W + begin) ;
      float const* start = src_pt + (i + begin) ;
      float const* stop ;
      float        x ;
      
      if (i + begin < 0) {
        start = src_pt + (i + begin) ;
        stop  = src_pt + VL_MIN(i + end, -1) ;
        x = *src_pt ;
        while (start <= stop) { acc += (*g++) * x ; start++ ; }
      }
      
      start = src_pt + VL_MIN(VL_MAX(i + begin, 0), M - 1) ;
      stop  = src_pt + VL_MIN(VL_MAX(i + end,   0), M - 1) ;
      while (start <=  stop) acc += (*g++) * (*start++) ;

      if (i + end > M - 1) {
        start = src_pt + M ;
        stop  = src_pt + i + end ;
        x = *(src_pt + M - 1) ;
        while (start <= stop) { acc += (*g++) * x ; start++ ; }
      }

      /* save */
      *dst_pt = acc ; 
      dst_pt += N ;
    }
    /* next column */
    src_pt += M ;
    dst_pt -= M_down*N - 1 ;
  }
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Shape kernel for linear interpolation
 **
 ** @param ker
 ** @param b
 ** @param e
 ** @param gker
 ** @param r
 ** @param k
 **/

static void 
make_kernel (float *ker, int* b, int *e, 
             float const* gker, int r, int k)
{
  double center, rate ;
  int i, n ;
  
  if (r & 0x1) {
    n = NBP * r + r - 1 ;
  } else {
    n = NBP * r + r ;
  }  
   
  *b = k * r ;
  *e = k * r + r + r - (r & 0x1) - 1 ;
  
  center = (*e + *b) / 2.0f ;
  rate = r ; 
      
  for (i = *b ; i <= *e ; ++i) {
    float box = 1 - fabs ((i - center) / rate) ;
    ker [i] = gker [i] * box ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Allocate and initialize a new DFT filter
 **
 ** @param f
 ** @param width
 ** @param height
 ** @param step
 ** @param size
 ** @return new filter.
 **/

VlDftFilter* vl_dft_new (int width, int height, int step, int size)
{
  int t ;
  VlDftFilter* f = vl_malloc (sizeof(VlDftFilter)) ;
  
  f-> width  = width ;
  f-> height = height ;
  f-> dwidth = width / step ;
  f-> dheight= height / step ;
  f-> nkeys  = f->dwidth * f->dheight ;
  f-> step   = step ;
  f-> size   = size ;

  f-> descr = vl_malloc (sizeof(float) * 128 * f->nkeys) ;
  f-> keys  = vl_malloc (sizeof(VlDftKeypoint) * f->nkeys) ;
  f-> tmp   = vl_malloc (sizeof(float) * f->width * f->height) ;
  f-> tmp2  = vl_malloc (sizeof(float) * f->width * f->height) ;
  f-> hist  = vl_malloc (sizeof(float*) * NBO) ;
  for (t = 0 ; t < NBO ; ++t) {
    f->hist [t] = vl_malloc (sizeof(float) * f->width * f->height) ;
  }
  return f ;
}

/** ------------------------------------------------------------------
 ** @brief Delete DFT filter
 ** @param f filter to delete.
 **/

void vl_dft_delete (VlDftFilter *f)
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
    if (f->descr) vl_free (f->descr) ;
    vl_free(f) ;
  }
}


/** ------------------------------------------------------------------
 ** @internal@brief Process with Gaussian window
 ** @param f filter to delete.
 **/

VL_INLINE 
void with_gaussian_window (VlDftFilter* f)
{
  int n, W, x, y, i, t ;
  float del, u ;
  float *gker, *xker, *yker, acc, sigma ;
  
  /* Gaussian window */
  if (f->size & 0x1) {
    n = NBP * f->size + f->size - 1 ;
  } else {
    n = NBP * f->size + f->size ;
  }  
  del = - (n - 1) / 2.0f ;
  W = n / 2 ; 
  
  gker = vl_malloc (sizeof(float) * n) ;
  xker = vl_malloc (sizeof(float) * n) ;
  yker = vl_malloc (sizeof(float) * n) ;
  acc  = 0 ;
  sigma = (f->size * NBP) / 2 ;
  for (i = 0 ; i < n ; ++ i) {    
    u = (i + del) / sigma ;
    gker [i] = expf (- u*u) ;
    acc += gker [i] ;
  }
  
  /* compute dense descriptors */
  for (y = 0 ; y < NBP ; ++y) {
    int xb, xe, yb, ye ;
    bzero(yker, sizeof(float)*n) ;
    make_kernel (yker, &yb, &ye, gker, f->size, y) ;
    
    for (x = 0 ; x < NBP ; ++x) {      
      bzero(xker, sizeof(float)*n) ;
      make_kernel (xker, &xb, &xe, gker, f->size, x) ;
      
      for (t = 0 ; t < NBO ; ++t) {
        int xs, ys ;
        float *dst = f->descr + x * NBO + y * (NBP*NBO) + t ;
        float *src = f->tmp ;
        
        econvolve(f->tmp2, f->hist[t], 
                  f->width, f->height, 
                  xker, W, xb - W, xe - W, f->step) ;
        
        econvolve(f->tmp, f->tmp2, 
                  f->height, f->width/f->step, 
                  yker, W, yb - W, ye - W, f->step) ;
        
        for (ys = 0 ; ys < f->dheight ; ++ys) {
          for (xs = 0 ; xs < f->dwidth ; ++xs) {
            *dst = *src++ ;
            dst += NBP*NBP*NBO ;
          }
        }
        
      } /* for t */
    } /* for x */
  } /* for y */
  
  vl_free (gker) ;
  vl_free (xker) ;
  vl_free (yker) ;
}



/** ------------------------------------------------------------------
 ** @internal@brief Process with flat window.
 ** @param f filter to delete.
 **/

VL_INLINE 
void with_flat_window (VlDftFilter* f)
{
  int n, W, x, y, i, t ;
  float del ;
  float *ker ;
  
  /* Gaussian window */
  if (f->size & 0x1) {
    n = f->size + f->size - 1 ;
  } else {
    n = f->size + f->size ;
  }  
  del = - (n - 1) / 2.0f ;
  W = n / 2 ;
  
  ker = vl_malloc (sizeof(float) * n) ;
  {
    double center, rate ;
    center = (n - 1) / 2.0f ;
    rate = f->size ;    
    for (i = 0 ; i < n ; ++i) {
      ker [i] = 1 - fabs ((i - center) / rate) ;    
    }
  }
  
  for (t = 0 ; t < NBO ; ++t) {
    econvolve(f->tmp2, f->hist[t], 
              f->width, f->height, 
              ker, W, 0 - W, n - 1 - W, f->step) ;
    
    econvolve(f->hist[t], f->tmp2, 
              f->height, f->width/f->step, 
              ker, W, 0 - W, n - 1 - W, f->step) ;
  }
  
  /* compute dense descriptors */
  for (y = 0 ; y < NBP ; ++y) {
    for (x = 0 ; x < NBP ; ++x) {
      int xs, ys ;
      int xd = (int) (-((NBP - 1) * f->size) / 2.0f + x * f->size) / f->step ;    
      int yd = (int) (-((NBP - 1) * f->size) / 2.0f + y * f->size) / f->step ;
      
      float *dpt = f->descr + NBO * x + NBP*NBO * y ;
            
      for (ys = 0 ; ys < f->dheight ; ++ys) {
        for (xs = 0 ; xs < f->dwidth ; ++xs) {
          int xp = VL_MAX(0, VL_MIN(f->dwidth,  xs + xd)) ;
          int yp = VL_MAX(0, VL_MIN(f->dheight, ys + yd)) ;
          for (t = 0 ; t < NBO ; ++t) {            
            dpt [t] = *(f->hist[t] + xp + yp * f->dwidth) ;
          }
          dpt += NBP*NBP*NBO ;
        }
      }
    } /* for x */
  } /* for y */
  
  vl_free (ker) ;
}

/** ------------------------------------------------------------------
 ** @brief Compute Dense Feature Transform
 **
 ** @param f DFT filter. 
 ** @param im image data.
 **/

void vl_dft_process (VlDftFilter* f, float const* im, vl_bool fast)
{
  int t, x, y ;

  /* clear integral images */
  for (t = 0 ; t < NBO ; ++t)
    bzero (f->hist[t], f->width*f->height*sizeof(float)) ;
  
#undef at
#define at(u,v) (im[(v)*f->width+(u)])
  
  /* histograms */
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
      nt = vl_mod_2pi_f (angle) * (8 / (2*VL_PI)) ;
      bint = vl_floor_f (nt) ;
      rbint = nt - bint ;
      
      /* write it back */
      f->hist[bint%NBO]    [x + y * f->width] += (1 - rbint) * mod ;      
      f->hist[(bint+1)%NBO][x + y * f->width] += rbint * mod ;
    }
  }

  if (fast) {
    with_flat_window(f) ;
  } else {
    with_gaussian_window(f) ;
  }
    
  {
    VlDftKeypoint* k = f->keys ;
    int xs, ys,bin ;
    float *dpt = f->descr ;
    float adj = (f->size*NBP & 0x1) ? 0.0f : -0.5f ;
    for (ys = 0 ; ys < f->height/f->step ; ++ys) {
      for (xs = 0 ; xs < f->width/f->step ; ++xs) {
        k->x = xs*f->step + adj ;
        k->y = ys*f->step + adj ;
        normalize_histogram (dpt, dpt + NBP*NBP*NBO) ;
        for(bin = 0; bin < NBO*NBP*NBP ; ++ bin) {
          if (dpt[bin] > 0.2f) dpt[bin] = 0.2f ;
        }
        normalize_histogram (dpt, dpt + NBP*NBP*NBO) ;
        ++ k ;
        dpt += NBP*NBP*NBO ;        
      }
    }
  }
}
  
