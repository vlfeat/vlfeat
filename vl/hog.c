/** @file hog.c
 ** @brief Histogram of Oriented Gradients (HOG) - Definition
 ** @author Andrea Vedaldi
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
*/

#include "hog.h"
#include "mathop.h"
#include <string.h>

/* ---------------------------------------------------------------- */
/** @brief Create a new HOG object
 ** @param variant HOG descriptor variant.
 ** @param numOrientations number of distinguished orientations.
 ** @param transposed wether images are transposed (column major).
 ** @return the new HOG object.
 **
 ** The function creates a new HOG object to extract descriptors of
 ** the prescribed @c variant. The angular resolution is set by
 ** @a numOrientations, which specifies the number of <em>undirected</em>
 ** orientations. The object can work with column major images
 ** by setting @a transposed to true.
 **/

VlHog *
vl_hog_new (VlHogVariant variant, vl_size numOrientations, vl_bool transposed)
{
  vl_index o, k ;
  VlHog * self = vl_calloc(1, sizeof(VlHog)) ;

  assert(numOrientations >= 1) ;

  self->variant = variant ;
  self->numOrientations = numOrientations ;
  self->glyphSize = 21 ;
  self->transposed = transposed ;
  self->orientationX = vl_malloc(sizeof(float) * self->numOrientations) ;
  self->orientationY = vl_malloc(sizeof(float) * self->numOrientations) ;

  /*
   Create a vector along the center of each orientation bin. These
   are used to map gradients to bins. If the image is transposed,
   then this can be adjusted here by swapping X and Y in these
   vectors.
   */
  for(o = 0 ; o < (signed)self->numOrientations ; ++o) {
    double angle = o * VL_PI / self->numOrientations ;
    if (!self->transposed) {
      self->orientationX[o] = (float) cos(angle) ;
      self->orientationY[o] = (float) sin(angle) ;
    } else {
      self->orientationX[o] = (float) sin(angle) ;
      self->orientationY[o] = (float) cos(angle) ;
    }
  }

  /*
   If the number of orientation is equal to 9, one gets:

   Uoccti:: 18 directed orientations + 9 undirected orientations + 4 texture
   DalalTriggs:: 9 undirected orientations x 4 blocks.
   */
  switch (self->variant) {
    case VlHogVariantUoctti:
      self->dimension = 3*self->numOrientations + 4 ;
      break ;

    case VlHogVariantDalalTriggs:
      self->dimension = 4*self->numOrientations ;
      break ;

    default:
      assert(0) ;
  }

  /*
   A permutation specifies how to permute elements in a HOG
   descriptor to flip it horizontally. Since the first orientation
   of index 0 points to the right, this must be swapped with orientation
   self->numOrientation that points to the left (for the directed case,
   and to itself for the undirected one).
   */

  self->permutation = vl_malloc(self->dimension * sizeof(vl_index)) ;
  switch (self->variant) {
    case VlHogVariantUoctti:
      for(o = 0 ; o < (signed)self->numOrientations ; ++o) {
        vl_index op = self->numOrientations - o ;
        self->permutation[o] = op ;
        self->permutation[o + self->numOrientations] = (op + self->numOrientations) % (2*self->numOrientations) ;
        self->permutation[o + 2*self->numOrientations] = (op % self->numOrientations) + 2*self->numOrientations ;
      }
      for (o = self->numOrientations * 3 ; o < (signed)self->dimension ; ++o) {
        self->permutation[o] = o ;
      }
      break ;

    case VlHogVariantDalalTriggs:
      for(k = 0 ; k < 4 ; ++k) {
        for(o = 0 ; o < (signed)self->numOrientations ; ++o) {
          vl_index op = self->numOrientations - o ;
          self->permutation[o + k*self->numOrientations] = (op % self->numOrientations) + k*self->numOrientations ;
        }
      }
      break ;

    default:
      assert(0) ;
  }

  /*
   Create glyphs for representing the HOG features/ filters. The glyphs
   are simple bars, oriented orthogonally to the gradients to represent
   image edges. If the object is configured to work on transposed image,
   the glyphs images are also stored in column-major.
   */
  self->glyphs = vl_calloc(self->glyphSize * self->glyphSize * self->numOrientations, sizeof(float)) ;
#define atglyph(x,y,k) self->glyphs[(x) + self->glyphSize * (y) + self->glyphSize * self->glyphSize * (k)]
  for (o = 0 ; o < (signed)self->numOrientations ; ++o) {
    double angle = fmod(o * VL_PI / self->numOrientations +  VL_PI/2, VL_PI) ;
    double x2 = self->glyphSize * cos(angle) / 2 ;
    double y2 = self->glyphSize * sin(angle) / 2 ;

    if (angle <= VL_PI / 4 || angle >= VL_PI * 3 / 4) {
      /* along horizontal direction */
      double slope = y2 / x2 ;
      double offset = (1 - slope) * (self->glyphSize - 1) / 2 ;
      vl_index skip = (1 - fabs(cos(angle))) / 2 * self->glyphSize ;
      vl_index i, j ;
      for (i = skip ; i < (signed)self->glyphSize - skip ; ++i) {
        j = round(slope * i + offset) ;
        if (! self->transposed) {
          atglyph(i,j,o) = 1 ;
        } else {
          atglyph(j,i,o) = 1 ;
        }
      }
    } else {
      /* along vertical direction */
      double slope = x2 / y2 ;
      double offset = (1 - slope) * (self->glyphSize - 1) / 2 ;
      vl_index skip = (1 - sin(angle)) / 2 * self->glyphSize ;
      vl_index i, j ;
      for (j = skip ; j < (signed)self->glyphSize - skip; ++j) {
        i = round(slope * j + offset) ;
        if (! self->transposed) {
          atglyph(i,j,o) = 1 ;
        } else {
          atglyph(j,i,o) = 1 ;
        }
      }
    }
  }
  return self ;
}

/* ---------------------------------------------------------------- */
/** @brief Delete a HOG object
 ** @param self HOG object to delete.
 **/

void
vl_hog_delete (VlHog * self)
{
  if (self->orientationX) {
    vl_free(self->orientationX) ;
    self->orientationX = NULL ;
  }

  if (self->orientationY) {
    vl_free(self->orientationY) ;
    self->orientationY = NULL ;
  }

  if (self->glyphs) {
    vl_free(self->glyphs) ;
    self->glyphs = NULL ;
  }

  if (self->permutation) {
    vl_free(self->permutation) ;
    self->permutation = NULL ;
  }

  if (self->hog) {
    vl_free(self->hog) ;
    self->hog = NULL ;
  }

  if (self->hogNorm) {
    vl_free(self->hogNorm) ;
    self->hogNorm = NULL ;
  }

  vl_free(self) ;
}


/* ---------------------------------------------------------------- */
/** @brief Get HOG glyph size
 ** @param self HOG object.
 ** @return size (height and width) of a glyph.
 **/

vl_size
vl_hog_get_glyph_size (VlHog const * self)
{
  return self->glyphSize ;
}

/* ---------------------------------------------------------------- */
/** @brief Get HOG left-right flip permutation
 ** @param self HOG object.
 ** @return left-right permutation.
 **
 ** The function returns a pointer to an array @c permutation of ::vl_hog_get_dimension
 ** elements. Given a HOG descriptor (for a cell) @c hog, which is also
 ** a vector of ::vl_hog_get_dimension elements, the
 ** descriptor obtained for the same image flipped horizotnally is
 ** given by <code>flippedHog[i] = hog[permutation[i]]</code>.
 **/

vl_index const *
vl_hog_get_permutation (VlHog const * self)
{
  return self->permutation ;
}

/* ---------------------------------------------------------------- */
/** @brief Render a HOG descriptor to a glyph image
 ** @param self HOG object.
 ** @param image glyph image (output).
 ** @param descriptor HOG descriptor.
 ** @param width HOG descriptor width.
 ** @param height HOG descriptor height.
 **
 ** The function renders the HOG descriptor or filter
 ** @a descriptor as an image (for visualization) and stores the result in
 ** the buffer @a image. This buffer
 ** must be an array of dimensions @c width*glyphSize
 ** by @c height*glyphSize elements, where @c glyphSize is
 ** obtained from ::vl_hog_get_glyph_size and is the size in pixels
 ** of the image element used to represent the descriptor of one
 ** HOG cell.
 **/

void
vl_hog_render (VlHog const * self,
               float * image,
               float const * descriptor,
               vl_size width,
               vl_size height)
{
  vl_index x, y, k, cx, cy ;
  vl_size hogStride = width * height ;

  assert(self) ;
  assert(image) ;
  assert(descriptor) ;
  assert(width > 0) ;
  assert(height > 0) ;

  for (y = 0 ; y < (signed)height ; ++y) {
    for (x = 0 ; x < (signed)width ; ++x) {
      float minWeight = 0 ;
      float maxWeight = 0 ;

      for (k = 0 ; k < (signed)self->numOrientations ; ++k) {
        float weight ;
        float const * glyph = self->glyphs + k * (self->glyphSize*self->glyphSize) ;
        float * glyphImage = image + self->glyphSize * x + y * width * (self->glyphSize*self->glyphSize) ;

        switch (self->variant) {
          case VlHogVariantUoctti:
            weight =
            descriptor[k * hogStride] +
            descriptor[(k + self->numOrientations) * hogStride] +
            descriptor[(k + 2 * self->numOrientations) * hogStride] ;
            break ;
          case VlHogVariantDalalTriggs:
            weight =
            descriptor[k * hogStride] +
            descriptor[(k + self->numOrientations) * hogStride] +
            descriptor[(k + 2 * self->numOrientations) * hogStride] +
            descriptor[(k + 3 * self->numOrientations) * hogStride] ;
            break ;
          default:
            assert(0) ;
        }
        maxWeight = VL_MAX(weight, maxWeight) ;
        minWeight = VL_MIN(weight, minWeight);

        for (cy = 0 ; cy < (signed)self->glyphSize ; ++cy) {
          for (cx = 0 ; cx < (signed)self->glyphSize ; ++cx) {
            *glyphImage++ += weight * (*glyph++) ;
          }
          glyphImage += (width - 1) * self->glyphSize  ;
        }
      } /* next orientation */

      {
        float * glyphImage = image + self->glyphSize * x + y * width * (self->glyphSize*self->glyphSize) ;
        for (cy = 0 ; cy < (signed)self->glyphSize ; ++cy) {
          for (cx = 0 ; cx < (signed)self->glyphSize ; ++cx) {
            float value = *glyphImage ;
            *glyphImage++ = VL_MAX(minWeight, VL_MIN(maxWeight, value)) ;
          }
          glyphImage += (width - 1) * self->glyphSize  ;
        }
      }

      ++ descriptor ;
    } /* next column of cells (x) */
  } /* next row of cells (y) */
}

/* ---------------------------------------------------------------- */
/** @brief Get the dimension of the HOG features
 ** @param self HOG object.
 **/

vl_size
vl_hog_get_dimension (VlHog const * self)
{
  return self->dimension ;
}

/* ---------------------------------------------------------------- */
/** @internal @brief Prepare internal buffers
 ** @param self HOG object.
 ** @param width image width.
 ** @param height image height.
 ** @param cellSize size of a HOG cell.
 **/

static void
vl_hog_prepare_buffers (VlHog * self, vl_size width, vl_size height, vl_size cellSize)
{
  vl_size hogWidth = (width + cellSize/2) / cellSize ;
  vl_size hogHeight = (height + cellSize/2) / cellSize ;

  assert(width > 3) ;
  assert(height > 3) ;
  assert(hogWidth > 0) ;
  assert(hogHeight > 0) ;

  if (self->hog &&
      self->hogWidth == hogWidth &&
      self->hogHeight == hogHeight) {
    /* a suitable buffer is already allocated */
    memset(self->hog, 0, sizeof(float) * hogWidth * hogHeight * self->numOrientations * 2) ;
    memset(self->hog, 0, sizeof(float) * hogWidth * hogHeight) ;
    return ;
  }

  if (self->hog) {
    vl_free(self->hog) ;
    self->hog = NULL ;
  }

  if (self->hogNorm) {
    vl_free(self->hogNorm) ;
    self->hogNorm = NULL ;
  }

  self->hog = vl_calloc(hogWidth * hogHeight * self->numOrientations * 2, sizeof(float)) ;
  self->hogNorm = vl_calloc(hogWidth * hogHeight, sizeof(float)) ;
  self->hogWidth = hogWidth ;
  self->hogHeight = hogHeight ;
}

/* ---------------------------------------------------------------- */
/** @brief Compute HOG features
 ** @param self HOG object.
 ** @param features HOG features (output).
 ** @param image image to process.
 ** @param width image width.
 ** @param height image height.
 ** @param numChannels number of image channles.
 ** @param cellSize size of a HOG cell.
 **
 ** The buffer @c hog must be a three-dimensional array.
 ** The first two dimensions are @c (width + cellSize/2)/cellSize and
 ** @c (height + cellSize/2)/cellSize, where divisions are integer.
 ** This is approximately @c width/cellSize and @c height/cellSize,
 ** adjusted so that the last cell is at least half contained in the
 ** image.
 **
 ** The image @c width and @c height must be not smaller than three
 ** pixels and not smaller than @c cellSize.
 **/

void
vl_hog_process (VlHog * self,
                float * features,
                float const * image,
                vl_size width, vl_size height, vl_size numChannels,
                vl_size cellSize)
{
  vl_size channelStride = width * height ;
  vl_index x, y ;
  vl_uindex k ;
  vl_size hogStride ;

  assert(self) ;
  assert(image) ;
  assert(features) ;

#define at(x,y,k) (self->hog[(x) + (y) * self->hogWidth + (k) * hogStride])
#define atNorm(x,y) (self->hogNorm[(x) + (y) * self->hogWidth])

  /* clear features */
  vl_hog_prepare_buffers(self, width, height, cellSize) ;
  hogStride = self->hogWidth * self->hogHeight ;

  /* compute gradients and map the to HOG cells by bilinear interpolation */
  for (y = 1 ; y < (signed)height - 1 ; ++y) {
    for (x = 1 ; x < (signed)width - 1 ; ++x) {
      float gradx ;
      float grady ;
      float grad ;
      float orientationScore = 0;
      vl_index orientation = 0 ;
      float hx, hy, wx1, wx2, wy1, wy2 ;
      vl_index binx, biny ;

      /*
       Compute the gradient at (x,y). The image channel with
       the maximum gradient at each location is selected.
      */
      {
        float const * iter = image + y * width + x ;
        float grad2 = 0 ;
        for (k = 0 ; k < numChannels ; ++k) {
          float gradx_ = *(iter + 1) - *(iter - 1) ;
          float grady_ = *(iter + width)  - *(iter - width) ;
          float grad2_ = gradx_ * gradx_ + grady_ * grady_ ;
          if (grad2_ > grad2) {
            gradx = gradx_ ;
            grady = grady_ ;
            grad2 = grad2_ ;
          }
          iter += channelStride ;
        }
        grad = sqrtf(grad2) ;
      }

      /*
       Map the gradient to the closest orientation bin. There are
       numOrientations orientation in the interval [0,2pi).
       The next numOriantations are the symmetric ones, for a total
       of 2*numOrientation directed orientations.
       */
      for (k = 0 ; k < self->numOrientations ; ++k) {
        float orientationScore_
        = gradx * self->orientationX[k] +  grady * self->orientationY[k] ;
        if (orientationScore_ > orientationScore) {
          orientation = k ;
          orientationScore = orientationScore_ ;
        } else if (-orientationScore_ > orientationScore) {
          orientation = k + self->numOrientations ;
          orientationScore = - orientationScore_ ;
        }
      }

      /*
       Accumulate the gradient. hx is the distance of the
       pixel x to the cell center at its left, in units of cellSize.
       With this parametrixation, a pixel on the cell center
       has hx = 0, which gradually increases to 1 moving to the next
       center.
       */
      hx = (x + 0.5) / cellSize - 0.5 ;
      hy = (y + 0.5) / cellSize - 0.5 ;
      binx = vl_floor_f(hx) ;
      biny = vl_floor_f(hy) ;
      wx2 = hx - binx ;
      wy2 = hy - biny ;
      wx1 = 1.0 - wx2 ;
      wy1 = 1.0 - wy2 ;

      if (binx >= 0 && biny >=0) {
        at(binx,biny,orientation) += grad * wx1 * wy1 ;
      }
      if (binx < (signed)self->hogWidth - 1 && biny >=0) {
        at(binx+1,biny,orientation) += grad * wx2 * wy1 ;
      }
      if (binx < (signed)self->hogWidth - 1 && biny < (signed)self->hogHeight - 1) {
        at(binx+1,biny+1,orientation) += grad * wx2 * wy2 ;
      }
      if (binx >= 0 && biny < (signed)self->hogHeight - 1) {
        at(binx,biny+1,orientation) += grad * wx1 * wy2 ;
      }
    } /* next x */
  } /* next y */

  /*
   Computes the squared L2 norm of each HOG cell. This is the norm of the
   undirected orientation histogram, counting only numOrientations. This
   histogram is obtained by folding the 2*numOrientations directed
   orientations that are computed.
   */
  {
    float const * iter = self->hog ;
    for (k = 0 ; k < self->numOrientations ; ++k) {
      float * niter = self->hogNorm ;
      float * niterEnd = self->hogNorm + self->hogWidth * self->hogHeight ;
      while (niter != niterEnd) {
        float h1 = *iter ;
        float h2 = *(iter + self->hogWidth*self->hogHeight*self->numOrientations) ;
        float h = h1 + h2 ;
        *niter += h * h ;
        niter++ ;
        iter++ ;
      }
    }
  }

  /*
   HOG block-normalisation. For each cell, there are four 2x2 blocks
   covering it. For example, the cell number 5 in the following scheme
   is covered by the four blocks 1245, 2356, 4578, 5689.

                           +---+---+---+
                           | 1 | 2 | 3 |
                           +---+---+---+
                           | 4 | 5 | 6 |
                           +---+---+---+
                           | 7 | 8 | 9 |
                           +---+---+---+

   In the Dalal-Triggs implementation, one forms all possible 2x2 blocks
   of cells, computes a descriptor vector for each by stacking the corresponding
   2x2 HOG cells, and L2 normalizes (and truncates) the result.

   Each of the cell HOG are used in four blocks. These are then decomposed
   again to produce descriptors for each cell. Each descriptor is simply
   the stacking of the portion of each block descriptor that arised
   from that cell. This process result in a descriptor
   of each cell which contains four copies of the original HOG,
   with four different normalization factors.

   @remark By stacking together the cell descriptors for a large retangular block
   of cells, one effectively stacks together the block descriptors of
   an equal number of blocks (except for the boundaries, for which
   blocks are only partially included). Since blocks are L2 normalized
   (up to truncation), this implies that the L2 norm of the resulting
   vector is approximately equal to the area of the region.

   */
  {
    float const * iter = self->hog ;
    for (y = 0 ; y < (signed)self->hogHeight ; ++y) {
      for (x = 0 ; x < (signed)self->hogWidth ; ++x) {

        /*
        */

        /* norm of upper-left, upper-right, ... blocks */
        vl_index xm = VL_MAX(x - 1, 0) ;
        vl_index xp = VL_MIN(x + 1, (signed)self->hogWidth - 1) ;
        vl_index ym = VL_MAX(y - 1, 0) ;
        vl_index yp = VL_MIN(y + 1, (signed)self->hogHeight - 1) ;

        float norm1 = atNorm(xm,ym) ;
        float norm2 = atNorm(x,ym) ;
        float norm3 = atNorm(xp,ym) ;
        float norm4 = atNorm(xm,y) ;
        float norm5 = atNorm(x,y) ;
        float norm6 = atNorm(xp,y) ;
        float norm7 = atNorm(xm,yp) ;
        float norm8 = atNorm(x,yp) ;
        float norm9 = atNorm(xp,yp) ;

#if 0
        float factor1 = 1.0f / VL_MAX(sqrtf(norm1 + norm2 + norm4 + norm5), 1e-10f) ;
        float factor2 = 1.0f / VL_MAX(sqrtf(norm2 + norm3 + norm5 + norm6), 1e-10f) ;
        float factor3 = 1.0f / VL_MAX(sqrtf(norm4 + norm5 + norm7 + norm8), 1e-10f) ;
        float factor4 = 1.0f / VL_MAX(sqrtf(norm5 + norm6 + norm8 + norm9), 1e-10f) ;
#else
        float factor1 = 1.0f / sqrtf(norm1 + norm2 + norm4 + norm5 + 1e-4) ;
        float factor2 = 1.0f / sqrtf(norm2 + norm3 + norm5 + norm6 + 1e-4) ;
        float factor3 = 1.0f / sqrtf(norm4 + norm5 + norm7 + norm8 + 1e-4) ;
        float factor4 = 1.0f / sqrtf(norm5 + norm6 + norm8 + norm9 + 1e-4) ;
#endif
        float t1 = 0 ;
        float t2 = 0 ;
        float t3 = 0 ;
        float t4 = 0 ;

        float * oiter = features + x + self->hogWidth * y ;

        for (k = 0 ; k < self->numOrientations ; ++k) {
          float ha = iter[hogStride * k] ;
          float hb = iter[hogStride * (k + self->numOrientations)] ;
          float hc ;

          float ha1 = factor1 * ha ;
          float ha2 = factor2 * ha ;
          float ha3 = factor3 * ha ;
          float ha4 = factor4 * ha ;

          float hb1 = factor1 * hb ;
          float hb2 = factor2 * hb ;
          float hb3 = factor3 * hb ;
          float hb4 = factor4 * hb ;

          float hc1 = ha1 + hb1 ;
          float hc2 = ha2 + hb2 ;
          float hc3 = ha3 + hb3 ;
          float hc4 = ha4 + hb4 ;

          ha1 = VL_MIN(0.2f, ha1) ;
          ha2 = VL_MIN(0.2f, ha2) ;
          ha3 = VL_MIN(0.2f, ha3) ;
          ha4 = VL_MIN(0.2f, ha4) ;

          hb1 = VL_MIN(0.2f, hb1) ;
          hb2 = VL_MIN(0.2f, hb2) ;
          hb3 = VL_MIN(0.2f, hb3) ;
          hb4 = VL_MIN(0.2f, hb4) ;

          hc1 = VL_MIN(0.2f, hc1) ;
          hc2 = VL_MIN(0.2f, hc2) ;
          hc3 = VL_MIN(0.2f, hc3) ;
          hc4 = VL_MIN(0.2f, hc4) ;

          t1 += hc1 ;
          t2 += hc2 ;
          t3 += hc3 ;
          t4 += hc4 ;

          switch (self->variant) {
            case VlHogVariantUoctti :
              ha = 0.5f * (ha1 + ha2 + ha3 + ha4) ;
              hb = 0.5f * (hb1 + hb2 + hb3 + hb4) ;
              hc = 0.5f * (hc1 + hc2 + hc3 + hc4) ;
              *oiter = ha ;
              *(oiter + hogStride * self->numOrientations) = hb ;
              *(oiter + 2 * hogStride * self->numOrientations) = hc ;
              break ;

            case VlHogVariantDalalTriggs :
              *oiter = hc1 ;
              *(oiter + hogStride * self->numOrientations) = hc2 ;
              *(oiter + 2 * hogStride * self->numOrientations) = hc3 ;
              *(oiter + 3 * hogStride * self->numOrientations) = hc4 ;
              break ;
          }
          oiter += hogStride ;

        } /* next orientation */

        switch (self->variant) {
          case VlHogVariantUoctti :
            oiter += 2 * hogStride * self->numOrientations ;
            *oiter = (1.0f/sqrtf(18.0f)) * t1 ; oiter += hogStride ;
            *oiter = (1.0f/sqrtf(18.0f)) * t2 ; oiter += hogStride ;
            *oiter = (1.0f/sqrtf(18.0f)) * t3 ; oiter += hogStride ;
            *oiter = (1.0f/sqrtf(18.0f)) * t4 ; oiter += hogStride ;
            break ;

          case VlHogVariantDalalTriggs :
            break ;
        }
        ++iter ;
      } /* next x */
    } /* next y */
  } /* block normalization */
}

