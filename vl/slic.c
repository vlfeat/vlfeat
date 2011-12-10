/** @file slic.c
 ** @author Andrea Vedaldi
 ** @brief SLIC superpixels - Definition
 **/

/* AUTORIGHTS
 Copyright (C) 2012 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available under the terms of the
 GNU GPLv2, or (at your option) any later version.
 */

/** @file slic.h

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section slic SLIC superpixels
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref slic.h implements SLIC superpixels, as described by
@cite{achanta10slic}.

- @ref slic-overview Overview
- @ref slic-tech Technical details

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection slic-overview Overview
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

SLIC superpixels @cite{achanta10slic} are obtained by a spatially
localized version of k-means clustering. Similar to mean shift or
quick shift (@ref quickshift.h), each pixel is associated to a feature
vector

@f[
\Psi(x,y) =
\left[
\begin{array}{c}
\lambda x \\
\lambda y \\
I(x,y)
\end{array}
\right]
@f]

and then k-means clustering is run on those. As discussed below, the
coefficient @f$ \lambda @f$ balances the spatial and appearance
components of the feature vectors, imposing a degree of spatial
regularization.

SLIC takes two parameters: the nominal size of the regions
(superpixels) @c regionSize and the strength of the spatial
regularization @c regularizer. The image is first divided into a grid
with step @c regionSize. The center of each grid tile is then used to
initialize a corresponding k-means (up to a small shift to avoid
image eges). Finally, the k-means centers and clusters are refined by
using the Lloyd algorithm, yielding segmenting the image. As a
further restriction and simplification, during the k-means iterations
each pixel can be assigned to only the <em>2&times;2</em> centers
corresponding to grid tiles adjecent to the pixel.

The parameter @c regularizer sets the trade-off between clustering
appearance and spatial regularization. This is obtained by setting

@f[
 \lambda = \frac{\mathtt{regularizer}}{\mathtt{regionSize}}
@f]

in the definition of the feature @f$ \psi(x,y) @f$.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@subsection slic-tech Technical details
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The image is divided into a regular grid with @f$ M, N @f$ elements,
where

@f[
   M = \lceil \frac{\mathtt{imageWidth}}{\mathtt{regionSize}} \rceil,
   \quad
   N = \lceil \frac{\mathtt{imageHeight}}{\mathtt{regionSize}} \rceil.
@f]

A cluster center is initialized from each grid center

@f[
  x_i = \operatorname{round} i \frac{\mathtt{imageWidth}}{\mathtt{regionSize}}
  \quad
  y_j = \operatorname{round} j \frac{\mathtt{imageWidth}}{\mathtt{regionSize}}.
@f]

In order to avoid placing segment centers on top of image
discontinuities, the centers are then moved in a 3&times;3
neighbourohood to minimize the edge strength

@f[
   \operatorname{edge}(x,y) =
   \| I(x+1,y) - I(x-1,y) \|_2^2 +
   \| I(x,y+1) - I(x,y-1) \|_2^2
@f]

Then superpixels are obtained by using k-means clustering starting
from the collection of k-means centers

@f[
  C = \{ \Psi(x_i,y_j), i=0,1,\dots,M-1\ j=0,1,\dots,N-1 \}
@f]

where the features have been defined above. K-means uses the standard
LLoyd algorithm alternating computing new assignments of pixels to
centers and re-estimating the centers as the mean of the assigned
pixel feature vectors. The only difference compared to standard
k-means is that each pixel is assigned only

*/

#include "slic.h"
#include "mathop.h"
#include <math.h>
#include <string.h>

/** @brief SLIC superpixel segmentation
 ** @param segmentation segmentation.
 ** @param image image to segment.
 ** @param width image width.
 ** @param height image height.
 ** @param numChannels number of image channels (depth).
 ** @param regionSize nominal size of the regions.
 ** @param regularization trade-off between appearance and spatial terms.
 **
 ** The function computes the SLIC superpixels of the specified image @a image.
 ** @param image is a pointer to an @c width by @c height by @c by numChannles array of @c float.
 ** @param segmentation is a pointer to a @c width by @c height array of @c vl_uint32.
 ** @param segmentation contain the labels of each image pixels, from 0 to
 ** the number of regions minus one.
 **/

void vl_slic_segment (vl_uint32 * segmentation,
                      float const * image,
                      vl_size width,
                      vl_size height,
                      vl_size numChannels,
                      vl_size regionSize,
                      float regularization)
{
  vl_index i, x, y, u, v, k, region ;
  vl_uindex iter ;
  vl_size numRegionsX = (vl_size) ceil((double) width / regionSize) ;
  vl_size numRegionsY = (vl_size) ceil((double) height / regionSize) ;
  vl_size numRegions = numRegionsX * numRegionsY ;
  float * centers ;
  float * edgeMap ;
  float previousEnergy = VL_INFINITY_F ;
  float startingEnergy ;
  vl_uint32 * masses ;
  vl_size const maxNumIterations = 100 ;

  assert(segmentation) ;
  assert(image) ;
  assert(width >= 1) ;
  assert(height >= 1) ;
  assert(numChannels >= 1) ;
  assert(regionSize >= 1) ;
  assert(regularization >= 0) ;

#define atimage(x,y,k) image[(x)+(y)*width+(k)*width*height]
#define atEdgeMap(x,y) edgeMap[(x)+(y)*width]

  edgeMap = vl_calloc(width * height, sizeof(float)) ;
  masses = vl_malloc(sizeof(vl_uint32) * width * height) ;
  centers = vl_malloc(sizeof(float) * (2 + numChannels) * numRegions) ;

  /* compute edge map (gradient strength) */
  for (k = 0 ; k < (signed)numChannels ; ++k) {
    for (y = 1 ; y < (signed)height-1 ; ++y) {
      for (x = 1 ; x < (signed)width-1 ; ++x) {
        float a = atimage(x-1,y,k) ;
        float b = atimage(x+1,y,k) ;
        float c = atimage(x,y+1,k) ;
        float d = atimage(x,y-1,k) ;
        atEdgeMap(x,y) += (a - b)  * (a - b) + (c - d) * (c - d) ;
      }
    }
  }

  /* initialize K-means centers */
  i = 0 ;
  for (v = 0 ; v < (signed)numRegionsY ; ++v) {
    for (u = 0 ; u < (signed)numRegionsX ; ++u) {
      vl_index xp ;
      vl_index yp ;
      vl_index centerx ;
      vl_index centery ;
      float minEdgeValue = VL_INFINITY_F ;

      x = (vl_index) floor(regionSize * (u - 0.5)) ;
      y = (vl_index) floor(regionSize * (v - 0.5)) ;

      x = VL_MAX(VL_MIN(x, (signed)width-1),0) ;
      y = VL_MAX(VL_MIN(y, (signed)height-1),0) ;

      /* search in a 3x3 neighbourhood the smallest edge response */
      for (yp = VL_MAX(0, y-1) ; yp <= VL_MIN((signed)height-1, y+1) ; ++ yp) {
        for (xp = VL_MAX(0, x-1) ; xp <= VL_MIN((signed)width-1, x+1) ; ++ xp) {
          float thisEdgeValue = atEdgeMap(xp,yp) ;
          if (thisEdgeValue < minEdgeValue) {
            minEdgeValue = thisEdgeValue ;
            centerx = xp ;
            centery = yp ;
          }
        }
      }

      /* initialize the new center at this location */
      centers[i++] = (float) centerx ;
      centers[i++] = (float) centery ;
      for (k  = 0 ; k < (signed)numChannels ; ++k) {
        centers[i++] = atimage(centerx,centery,k) ;
      }
    }
  }

  /* run k-means iterations */
  for (iter = 0 ; iter < maxNumIterations ; ++iter) {
    float factor = regularization / (regionSize * regionSize) ;
    float energy = 0 ;

    /* assign pixels to centers */
    for (y = 0 ; y < (signed)height ; ++y) {
      for (x = 0 ; x < (signed)width ; ++x) {
        vl_index u = floor((double)x / regionSize - 0.5) ;
        vl_index v = floor((double)y / regionSize - 0.5) ;
        vl_index up, vp ;
        float minDistance = VL_INFINITY_F ;

        for (vp = VL_MAX(0, v) ; vp <= VL_MIN((signed)numRegionsY-1, v+1) ; ++vp) {
          for (up = VL_MAX(0, u) ; up <= VL_MIN((signed)numRegionsX-1, u+1) ; ++up) {
            vl_index region = up  + vp * numRegionsX ;
            float centerx = centers[(2 + numChannels) * region + 0]  ;
            float centery = centers[(2 + numChannels) * region + 1] ;
            float spatial = (x - centerx) * (x - centerx) + (y - centery) * (y - centery) ;
            float appearance = 0 ;
            for (k = 0 ; k < (signed)numChannels ; ++k) {
              float centerz = centers[(2 + numChannels) * region + k + 2]  ;
              float z = atimage(x,y,k) ;
              appearance += (z - centerz) * (z - centerz) ;
            }
            float distance = appearance + factor * spatial ;
            if (minDistance > distance) {
              minDistance = distance ;
              segmentation[x + y * width] = region ;
            }
          }
        }
        energy += minDistance ;
      }
    }
    /*
     VL_PRINTF("vl:slic: iter %d: energy: %g\n", iter, energy) ;
    */

    /* check energy termination conditions */
    if (iter == 0) {
      startingEnergy = energy ;
    } else {
      if ((previousEnergy - energy) < 1e-5 * (startingEnergy - energy)) {
        break ;
      }
    }
    previousEnergy = energy ;

    /* recompute centers */
    memset(masses, 0, sizeof(vl_uint32) * width * height) ;
    memset(centers, 0, sizeof(float) * (2 + numChannels) * numRegions) ;

    for (y = 0 ; y < (signed)height ; ++y) {
      for (x = 0 ; x < (signed)width ; ++x) {
        vl_index pixel = x + y * width ;
        vl_index region = segmentation[pixel] ;
        masses[region] ++ ;
        centers[region * (2 + numChannels) + 0] += x ;
        centers[region * (2 + numChannels) + 1] += y ;
        for (k = 0 ; k < (signed)numChannels ; ++k) {
          centers[region * (2 + numChannels) + k + 2] += atimage(x,y,k) ;
        }
      }
    }

    for (region = 0 ; region < (signed)numRegions ; ++region) {
      float mass = VL_MAX(masses[region], 1e-8) ;
      for (i = (2 + numChannels) * region ;
           i < (signed)(2 + numChannels) * (region + 1) ;
           ++i) {
        centers[i] /= mass ;
      }
    }
  }

  vl_free(masses) ;
  vl_free(centers) ;
  vl_free(edgeMap) ;
}
