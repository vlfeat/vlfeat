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

/** 
 @file dhog.h
 
 This module implements a dense version of @ref sift.h "SIFT" where
 features of fixed scale and orientation are computed on a regular
 (and dense) grid.

 @remark This descriptor is <em>not</em> equivalent to N. Dalal and
 B. Triggs. <em>Histograms of Oriented Gradients for Human
 Detection.</em> CVPR 2005. It is instead just a dense version of
 SIFT.
 
 - @ref dhog-usage
 - @ref dhog-detector
 - @ref dhog-descriptor

 @section dhog-usage Usage

 <p>DHOG is implemented by a filter, i.e. an object which can be
 reused to process sequentially similar images. To use the <b>DHOG
 filter</b>:</p>
 
 - Initialize the DHOG filter with ::vl_dhog_new(). The
   filter can be reused if the image size does not change.
 - 
 - Delete the DHOG filter by ::vl_dhog_delete().

 To use o

 @section dhog-algo Algorithm

 The program decomposes a given image @em I(x,y) in a set of equally
 sized regions (feature frames) sampled on a regular grid and computes
 a SIFT descripor for each one of these.
 
 @subsection dhog-algo-sift The SIFT descriptor

 A SIFT descriptor of an image region is an histogram of the local
 image gradients. The gradient at each pixel is regarded as a sample
 of a three-dimensional elementary feature vector, comprising the
 pixel location and the gradient orientation, weighed by the gradient
 norm. The occurences of such elmenetary features are accumulated in a
 3-D histogram @em h, which (up to normalization and clamping) forms
 the SIFT descriptor of the region.

 Formally, for each pixel location @f$ (x,y) @f$ one has an occurence
 of the elementray feature vector @f$ (\angle \nabla I(x,y), x, y)
 @f$, composed of are the gradient orientation @f$ \angle \nalba
 I(x,y) @f$ and of the pixel location @f$ (x, y) @f$. Such occurences are
 weighed by the gradeint norm @f$ | I(x,y) | @f$ and accumulated in a
 three-dimensional histogram @f$ h(t,i,j) @f$ of size @f$ N_\theta
 \times N_x \times N_y @f$. Here @f$ N_\theta @f$ is the number of
 bins along the orientation dimension @$f \theta @$f and @f$ N_x @f$ and @f$ N_y @f$
 the number of bins along the @em x and @em y dimensions respectively
 (that is @f$ t \in [0, \dots, N_\theta-1] @f$, @f$ i \in [0, \dots,
 N_x-1] @f$, and @f$ j \in [0, \dots, N_y-1] @f$).

 The association of elementary gradient features to the bins of the
 histogram @f$ h(t,i,j) @f$ is given by the multilinear weight @f$
 w_\theta(\theta-\theta_t) w_x(x - x_c - x_i) w_y(y - y_c -
 y_j)@f$. The weight is a windowing function centered at each bin
 center @f$ (\theta_t, x_c + x_i, y_c + y_i) @f$, where @f$ (x_c, y_c)
 @f$ is the center of the region being processed and @f$(\theta_t,
 x_i, y_j)@f$ are offsets obtained next. Let @f$ \Delta_\theta,
 \Delta_x, \Delta_y @f$ be the (nominal) size of each bin support
 along each dimenson (here @f$ \Delta_\theta = 2\pi / N_\theta @f$ and
 @f$\Delta_x@f$ and @f$ \Delta_y @f$ are proportional to the size of
 the region). Each bin center is then given by the offsets
 
 @f[
  \theta_t = t \Delta_\theta, 
  \qquad
  x_i = \left(i - \frac{N_x + 1}{2} \right)\Delta_x,
  \qquad
  y_j = \left(j - \frac{N_y + 1}{2} \right)\Delta_y,
 @f]

 The weight components are choosen so that they decrease linearly
 moving towards the center of a neighboring bin. Formally, they are
 given by
 
 @f[ 
  w_\theta(\theta) = \max\left(0, 1 - 
  \mathrm{mod}(\theta, 2\pi) / \Delta_\theta \right),
  \quad
  w_x(x) = \max\left(0, 1 - |x|/\Delta_x \right),
 @f]

 and similarly for @f$ w_y(y) @f$. 

 And additional Gaussian weighing factor @f$
 g_{\sigma_x}(x)g_{\sigma_y}(y) @f$ is applied to diminish the
 impotrance of gradients far from the center of the region. The
 variances are set to @f$ \sigma_x = N_x \Delta_x / 2 @f$ and
 @f$\sigma_y = N_y \Delta_y / 2@f$).

 We can now calculate the SIFT histogram as

 @f[
   h(t,i,j) = 
   \sum_{x,y} g_{\sigma_x}(x-x_c) g_{\sigma_y}(y-y_c) 
   w_{\theta}(\angle \nabla I(x,y) - \theta_t) 
   w_x(x - x_c - x_i) 
   w_y(y - y_c - x_j) | \nabla I(x,y) | 
 @f]

 @subsection dhog-algo-dense Dense sampling of descriptors
 
 We are interested in computing SIFT descriptors (with the same
 geometry) for densely sampled image regions.  We can speed-up
 significantly the computation by reducing the calculation of the
 histograms to a number of separable convolutions:

 @f{eqnarray*}
   h(t,i,j ; x_c,y_c) &=& 
   \sum_{x,y} 
   J_t(x,y)
   k_{i}(x_c+x_i-x) k_{j}(y_c+y_i-y) 
   \\
   &=& (J_t * k_ik_j)(x_c+x_i,y_c+x_i),
   \\
   J_t(x,y) 
   &=&
   | \nabla I(x,y) | w_\theta(\angle \nabla I(x,y) - \theta_t),
   \\
   k_{i}(x)
   &=&   
   g_{\sigma_x}(x - x_i)
   w_x(x),
   \\
   k_{j}(y) 
   &=&
   g_{\sigma_y}(y - y_j)
   w_y(y) 
 @f}
 
 where we used the symmetry of the various kernels. In this way
 computing the value of the histogram bin @f$ h(t,i,j|x_c,y_c) @f$ for
 all possible region centers @f$ (x_c, y_c) @f$ reduces to one
 convolution, and computing all the bins reduces to @f$ N_\theta N_x
 N_y @f$ convolutions.

 The number of convolutions can be reduced to @f$ N_\theta @f$ by
 dropping the Gaussian window @f$ g_\sigma(x,y) @f$, as in this
 case we have

 @f{eqnarray*}
   h(t,i,j ; x_c,y_c) &=& 
   \sum_{x,y} 
   J_t(x,y)
   w_{x}(x_c+x_i - x) w_{y}(y_c+y_i - y)
   \\
   &=& (J_t * w_xw_y)(x_c+x_i,y_c+y_i),
   \\
   J_t(x,y) 
   &=&
   | \nabla I(x,y) | w_\theta(\angle \nabla I(x,y) - \theta_t),
 @f}

 An additional advantage is that the filter @f$ w_x(x) @f$ (and
 similarly @f$ w_y(y) @f$) is a triangular signal of support @f$
 [-\Delta_x+1, \Delta_x-1] @f$ and decomposes decomposes as the
 convolution of two rectangular signals @f$ w_x =
 \mathrm{rect}_{[-\Delta_x+1,0]} * \mathrm{rect}_{{[0, \Delta_x-1]}}
 @f$. Thus computing each of the required @f$ N_\theta @f$
 convolutions can be done by convolving the gradient image @f$
 J_t(x,y) @f$ along the column and the rows by rectangular
 filters. This can be done (by means of integral images) in time
 independent of the spatial extend @f$\Delta_x, \Delta_y@f$ of the
 bins.

 To avoid resampling and dealing with special boundary conditions, we
 impose some mild restrictions on the geometry of the descriptors that
 can be computed. In particular, we make sure that the all bin centers
 @f$ x_c + x_i @f$ and @f$ y_c + y_i @f$ are integer coordinates
 within the image domain.  It is easier to take as reference the
 coordinates @f$ (x_d,y_d) = (x_c + x_0, y_c + y_0) @f$ of the
 upper-left bin center rather the center of the descriptor. We have
 the conditions
 
 @f{eqnarray*}
   x_d &\in& \mathbb{Z} \cap [0, W - 1 - \Delta_x (N_x - 1)], \\
   y_d &\in& \mathbb{Z} \cap [0, H - 1 - \Delta_y (N_y - 1)], \\
   (x_c,y_c) &=& \left(x_d + \frac{N_x\Delta_x}{2},\ y_d + \frac{N_y\Delta_y}{2}\right)
 @f}

 where @f$ (W,H) @f$ is the widht and height of the image (in pixels).

 Usually features are sampled not every pixels, but on a subgrid with
 steps @f$ T_x, T_y @f$. In particular, we define
 
 @f[
   x_d = x_{d0} + pT_x, \qquad y_d = y_{d0} + qT_x
 @f]
 
 @section dhog-descriptor Descriptor
 
 Each DHOG descriptor is an histogram of the gradient orientations
 inside a square image patch, very similar to a @ref sift-descriptor
 "SIFT descriptor". The histogram has @f$N_b@f$ bins along each
 spatial direction @e x and @e y and @f$N_o@f$ bins along the
 orientation dimension. Each spatial bin corresponds a portion of image
 which has size equal to @f$\Delta@f$ pixels (along @e x and @e y),
 where @f$\Delta@f$ is an even number. In addition, bins are
 bilinearly interpolated and partially overlap. The following figure
 illustrates which pixels (tick marks along the axis) are associated
 to one of two bins, and with weights (triangular signals).
 
 @image html dhog-bins.png
 
 The top row shows bins of extension @f$\Delta=2@f$ whose center is
 aligned to a pixel. The bottom row shows the same bins, but whose
 center sits in between two adjacent pixels. Notice that while the bin
 extent is always the same, in the first case the pixels that actually
 contributes to a bin (i.e. the ones with weights greater than zero)
 are @f$2\Delta -1@f$, while in the second case they are
 @f$2\Delta@f$.
 
 While both arrangements could be used, this implementation uses only
 the first (centers aligned to pixels), which is valid also for the
 case @f$\Delta=1@f$.
 
 **/

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Initialize new convolution kernel
 **
 ** @param binSize
 ** @param numBins
 ** @param binIndex negative to use flat window.
 ** @return a pointer to new filter.
 **/

float * _vl_dhog_new_kernel (int binSize, int numBins, int binIndex)
{  
  int filtLen = 2 * binSize - 1 ;
  float * ker = vl_malloc (sizeof(float) * filtLen) ;
  float * kerIter = ker ;
  float delta = binSize * (binIndex - 0.5F * (numBins + 1)) ;
  float sigma = 0.5F * (numBins * binSize) ;
  int k ;
  
  for (k = - binSize + 1 ; k <= + binSize - 1 ; ++k) {
    float z = (k - delta) / sigma ;
    *kerIter++ = (1.0f - fabsf(k) / binSize) * 
      ((binIndex >= 0) ? expf(- 0.5F * z*z) : 1.0f) ;
  }
  return ker ;
}

/** ------------------------------------------------------------------
 ** @internal@brief Normalize histogram
 **
 ** @param begin
 ** @param end
 **/

VL_INLINE float
_vl_dhog_normalize_histogram 
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
 ** @internal
 ** @brief Free internal buffers
 ** @param self DHOG filter.
 **/

void
_vl_dhog_free_buffers (VlDhogFilter* self)
{
  if (self->frames) { 
    vl_free(self->frames) ; 
    self->frames = NULL ; 
  }
  if (self->descrs) { 
    vl_free(self->descrs) ; 
    self->descrs = NULL ; 
  }
  if (self->grads) {
    int t ;
    for (t = 0 ; t < self->numGradAlloc ; ++t)
      if (self->grads[t]) vl_free(self->grads[t]) ;
    vl_free(self->grads) ;
    self->grads = NULL ;
  }
  self->numFrameAlloc = 0 ;
  self->numBinAlloc = 0 ;
  self->numGradAlloc = 0 ;
}


/** ------------------------------------------------------------------
 ** @internal
 **/
void _vl_dhog_update_buffers (VlDhogFilter *self) 
{
  int x1 = self->boundMinX ;
  int x2 = self->boundMaxX ;
  int y1 = self->boundMinY ;
  int y2 = self->boundMaxY ;
  
  int rangeX = x2 - x1 - (self->geom.numBinX - 1) * self->geom.binSizeX ;
  int rangeY = y2 - y1 - (self->geom.numBinY - 1) * self->geom.binSizeY ;
  
  int numFramesX = (rangeX >= 0) ? rangeX / self->stepX + 1 : 0 ;
  int numFramesY = (rangeY >= 0) ? rangeY / self->stepY + 1 : 0 ;
  
  self->numFrames = numFramesX * numFramesY ;
  self->descrSize = self->geom.numBinT * 
                    self->geom.numBinX *
                    self->geom.numBinY ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Allocate internal buffers
 ** @param self DHOG filter.
 **
 ** The function (re)allocates the internal buffers in accordance with
 ** the current image and descriptor geometry.
 **/

void
_vl_dhog_alloc_buffers (VlDhogFilter* self)
{
  _vl_dhog_update_buffers (self) ;  
  {
    int numFrameAlloc = vl_dhog_get_keypoint_num (self) ;   
    int numBinAlloc   = vl_dhog_get_descriptor_size (self) ;
    int numGradAlloc  = self->geom.numBinT ;
    
    /* see if we need to update the buffers */
    if (numBinAlloc != self->numBinAlloc || 
        numGradAlloc != self->numGradAlloc ||
        numFrameAlloc != self->numFrameAlloc) {
      
      int t ;
      
      _vl_dhog_free_buffers(self) ;
      
      self->frames = vl_malloc(sizeof(VlDhogKeypoint) * numFrameAlloc) ;
      self->descrs = vl_malloc(sizeof(float) * numBinAlloc * numFrameAlloc) ;
      self->grads = vl_malloc(sizeof(float*) * numGradAlloc) ;
      for (t = 0 ; t < numGradAlloc ; ++t) {
        self->grads[t] = 
          vl_malloc(sizeof(float) * self->imWidth * self->imHeight) ;
      }
      self->numBinAlloc = numBinAlloc ;
      self->numGradAlloc = numGradAlloc ;
      self->numFrameAlloc = numFrameAlloc ;
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief Create a new DHOG filter
 **
 ** @param width width of the image.
 ** @param height height of the image
 **
 ** @return new filter.
 **/
  
VL_EXPORT
VlDhogFilter* 
vl_dhog_new (int imWidth, int imHeight)
{
  VlDhogFilter* self = vl_malloc (sizeof(VlDhogFilter)) ;    
  self->imWidth  = imWidth ;
  self->imHeight = imHeight ;

  self->stepX = 5 ;
  self->stepY = 5 ;

  self->boundMinX = 0 ;
  self->boundMinY = 0 ;
  self->boundMaxX = imWidth - 1 ;
  self->boundMaxY = imHeight - 1 ;

  self->geom.numBinX = 4 ;
  self->geom.numBinY = 4 ;
  self->geom.numBinT = 8 ;
  self->geom.binSizeX = 5 ;
  self->geom.binSizeY = 5 ;

  self->useFlatWindow = VL_FALSE ;

  self->convTmp1 = vl_malloc(sizeof(float) * self->imWidth * self->imHeight) ;
  self->convTmp2 = vl_malloc(sizeof(float) * self->imWidth * self->imHeight) ;

  self->numBinAlloc = 0 ;
  self->numFrameAlloc = 0 ;
  self->numGradAlloc = 0 ;

  self->descrSize = 0 ;
  self->numFrames = 0 ;
  self->grads = NULL ;
  self->frames = NULL ;
  self->descrs = NULL ;
  
  _vl_dhog_update_buffers(self) ;  
  return self ;
}
 
/** ------------------------------------------------------------------
 ** @brief Create a new DHOG filter (basic interface)
 **
 ** @param width width of the image.
 ** @param height height of the image.
 ** @param step sampling step.
 ** @param binSize bin size.
 **
 ** @return new filter.
 **/
 
VL_EXPORT
VlDhogFilter* 
vl_dhog_new_basic (int imWidth, int imHeight, int step, int binSize) 
{
  VlDhogFilter* self = vl_dhog_new(imWidth, imHeight) ;
  VlDhogDescriptorGeometry geom = *vl_dhog_get_geometry(self) ;
  geom.binSizeX = binSize ;
  geom.binSizeY = binSize ;
  vl_dhog_set_geometry(self, &geom) ;
  vl_dhog_set_steps(self, step, step) ;
  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Delete DHOG filter
 ** @param f filter to delete.
 **/

VL_EXPORT
void
vl_dhog_delete (VlDhogFilter *self)
{
  _vl_dhog_free_buffers (self) ;
  if (self->convTmp2) vl_free(self->convTmp2) ;
  if (self->convTmp1) vl_free(self->convTmp1) ;
  vl_free (self) ;
}


/** ------------------------------------------------------------------
 ** @internal@brief Process with Gaussian window
 ** @param f filter to delete.
 **/

VL_INLINE 
void _vl_dhog_with_gaussian_window (VlDhogFilter* self)
{
  int binx, biny, bint ;
  int framex, framey ;
  float *xker, *yker ;

  int Wx = self->geom.binSizeX - 1 ;
  int Wy = self->geom.binSizeY - 1 ;
    
  for (biny = 0 ; biny < self->geom.numBinY ; ++biny) {

    yker = _vl_dhog_new_kernel(self->geom.binSizeY,
                               self->geom.numBinY, 
                               biny) ;
    
    for (binx = 0 ; binx < self->geom.numBinX ; ++binx) {

      xker = _vl_dhog_new_kernel(self->geom.binSizeX,
                                 self->geom.numBinX,
                                 binx) ;
      
      for (bint = 0 ; bint < self->geom.numBinT ; ++bint) {

        vl_imconvcol_vf (self->convTmp1, self->imHeight,
                         self->grads [bint], self->imWidth, self->imHeight, 
                         self->imWidth,
                         yker, -Wy, +Wy, 1,
                         VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
        
        vl_imconvcol_vf (self->convTmp2, self->imWidth,
                         self->convTmp1, self->imHeight, self->imWidth,
                         self->imHeight,
                         xker, -Wx, +Wx, 1,
                         VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
        
        {
          float *dst = self->descrs 
            + bint
            + binx * self->geom.numBinT
            + biny * (self->geom.numBinX * self->geom.numBinT)  ;
          
          float *src = self->convTmp2 ;
          
          int frameSizeX = self->geom.binSizeX * (self->geom.numBinX - 1) + 1 ;
          int frameSizeY = self->geom.binSizeY * (self->geom.numBinY - 1) + 1 ;
          int descrSize = vl_dhog_get_descriptor_size (self) ;

          for (framey  = self->boundMinY ;
               framey <= self->boundMaxY - frameSizeY + 1 ;
               framey += self->stepY) {
            for (framex  = self->boundMinX ;
                 framex <= self->boundMaxX - frameSizeX + 1 ;
                 framex += self->stepX) {
              *dst = src [(framex + binx * self->geom.binSizeX) * 1 +
                          (framey + biny * self->geom.binSizeY) * self->imWidth]  ;
              dst += descrSize ;
            } /* framex */
          } /* framey */
        }
        
      } /* for bint */
      vl_free (xker) ;
    } /* for binx */
    vl_free (yker) ;
  } /* for biny */  
}

/** ------------------------------------------------------------------
 ** @internal@brief Process with flat window.
 ** @param f filter to delete.
 **/

VL_INLINE 
void _vl_dhog_with_flat_window (VlDhogFilter* self)
{
  int binx, biny, bint ;
  int framex, framey ;

  /* for each orientation bin */
  for (bint = 0 ; bint < self->geom.numBinT ; ++bint) {
    
    vl_imconvcoltri_vf (self->convTmp1, self->imHeight,
                        self->grads [bint], self->imWidth, self->imHeight,
                        self->imWidth,
                        self->geom.binSizeY - 1, /* filt size */
                        1, /* subsampling step */
                        VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
    
    vl_imconvcoltri_vf (self->convTmp2, self->imWidth,
                        self->convTmp1, self->imHeight, self->imWidth, 
                        self->imHeight,
                        self->geom.binSizeX - 1,
                        1,
                        VL_PAD_BY_CONTINUITY|VL_TRANSPOSE) ;
        
    for (biny = 0 ; biny < self->geom.numBinY ; ++biny) {
      for (binx = 0 ; binx < self->geom.numBinX ; ++binx) {
        
        float *dst = self->descrs 
          + bint
          + binx * self->geom.numBinT
          + biny * (self->geom.numBinX * self->geom.numBinT)  ;

        float *src = self->convTmp2 ;
        
        int frameSizeX = self->geom.binSizeX * (self->geom.numBinX - 1) + 1 ;
        int frameSizeY = self->geom.binSizeY * (self->geom.numBinY - 1) + 1 ;
        int descrSize = vl_dhog_get_descriptor_size (self) ;
        
        for (framey  = self->boundMinY ;
             framey <= self->boundMaxY - frameSizeY + 1 ;
             framey += self->stepY) {
          for (framex  = self->boundMinX ;
               framex <= self->boundMaxX - frameSizeX + 1 ;
               framex += self->stepX) {
            *dst = src [(framex + binx * self->geom.binSizeX) * 1 +
                        (framey + biny * self->geom.binSizeY) * self->imWidth]  ;
            dst += descrSize ;
          } /* framex */
        } /* framey */
      } /* binx */
    } /* biny */
  } /* bint */
}

/** ------------------------------------------------------------------
 ** @brief Compute Dense Feature Transform
 **
 ** @param f    DHOG filter. 
 ** @param im   image data.
 ** @param fast speed up computation by replacing a gaussian window with
 **             a flat one 
 **/

void vl_dhog_process (VlDhogFilter* self, float const* im)
{
  int t, x, y ;

  /* update buffers */
  _vl_dhog_alloc_buffers (self) ;

  /* clear integral images */
  for (t = 0 ; t < self->geom.numBinT ; ++t)
    memset (self->grads[t], 0, 
            sizeof(float) * self->imWidth * self->imHeight) ;
  
#undef at
#define at(u,v) (im[(v)*self->imWidth+(u)])
  
  /*  */
  for (y = 0 ; y < self->imHeight ; ++y) {
    for (x = 0 ; x < self->imWidth ; ++x) {      
      float gx, gy ;
      float angle, mod, nt, rbint ;
      int bint ;
      
      /* y derivative */
      if (y == 0) {
        gy = at(x,y+1) - at(x,y) ;
      } else if (y == self->imHeight - 1) {
        gy = at(x,y) - at(x,y-1) ;
      } else {
        gy = 0.5F * (at(x,y+1) - at(x,y-1)) ;
      }
      
      /* x derivative */
      if (x == 0) {
        gx = at(x+1,y) - at(x,y) ;
      } else if (x == self->imWidth - 1) {
        gx = at(x,y) - at(x-1,y) ;
      } else {
        gx = 0.5F * (at(x+1,y) - at(x-1,y)) ;
      }
      
      /* angle and modulus */
      angle = vl_fast_atan2_f (gy,gx) ;
      mod = vl_fast_sqrt_f (gx*gx + gy*gy) ;
            
      /* quantize angle */
      nt = vl_mod_2pi_f (angle) * (self->geom.numBinT / (2*VL_PI)) ;
      bint = vl_floor_f (nt) ;
      rbint = nt - bint ;
      
      /* write it back */
      self->grads [(bint    ) % self->geom.numBinT][x + y * self->imWidth] = (1 - rbint) * mod ;
      self->grads [(bint + 1) % self->geom.numBinT][x + y * self->imWidth] = (    rbint) * mod ;
    }
  }

  if (self->useFlatWindow) {
    _vl_dhog_with_flat_window(self) ;
  } else {
    _vl_dhog_with_gaussian_window(self) ;
  }
  
  {
    VlDhogKeypoint* frameIter = self->frames ;
    float * descrIter = self->descrs ;
    int framex, framey, bint ;
    
    int frameSizeX = self->geom.binSizeX * (self->geom.numBinX - 1) + 1 ;
    int frameSizeY = self->geom.binSizeY * (self->geom.numBinY - 1) + 1 ;
    int descrSize = vl_dhog_get_descriptor_size (self) ;
    
    float deltaCenterX = 0.5F * self->geom.binSizeX * (self->geom.numBinX - 1) ;
    float deltaCenterY = 0.5F * self->geom.binSizeY * (self->geom.numBinY - 1) ;
    
    float normConstant = frameSizeX * frameSizeY ;

    if (self->useFlatWindow) {
      /* invoncoltri is normalized */
      normConstant /= self->geom.binSizeX * self->geom.binSizeY ;
    }
    
    for (framey  = self->boundMinY ;
         framey <= self->boundMaxY - frameSizeY + 1 ;
         framey += self->stepY) {

      for (framex  = self->boundMinX ;
           framex <= self->boundMaxX - frameSizeX + 1 ;
           framex += self->stepX) {
        
        frameIter->x    = framex + deltaCenterX ;
        frameIter->y    = framey + deltaCenterY ;

        /* mass */
        {
          float mass = 0 ;
          for (bint = 0 ; bint < descrSize ; ++ bint)
            mass += descrIter[bint] ;
          mass /= normConstant ;
          frameIter->norm = mass ;
        }

        /* L2 normalize */
        _vl_dhog_normalize_histogram (descrIter, descrIter + descrSize) ;

        /* clamp */
        for(bint = 0 ; bint < descrSize ; ++ bint)
          if (descrIter[bint] > 0.2F) descrIter[bint] = 0.2F ;
        
        frameIter ++ ;
        descrIter += descrSize ;
      } /* for framex */
    } /* for framey */
  }
}
