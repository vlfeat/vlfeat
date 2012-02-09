/** @file lbp.c
 ** @brief Local Binary Patterns - Definition
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/** @file lbp.h

::VlLbp implements the local binary pattern features.

An LBP feature [1] is a l2-normalized local histogram of quantized
local binary patterns (LBP).

@section lbp Local Binary Patterns

A LBP is a string of bit obtained by binarizing a local neighbourhood
of pixels with respect to the brightness of the central pixel.
::VlLbp implements only the case of 3x3 pixel neighbourhoods (this
setting perform best in applications).  Thus the LBP at location @f$
(x,y) @f$ is a string of eight bits. Each bit is equal to one if the
corresponding pixel is brighter than the central one.  Pixels are
scanned starting from the one to the right in anti-clockwise sense.
For example the first bit is one if, and only if, @f$ I(x+1,y) >
I(x,y), and the second bit if, and only if, @f$ I(x+1,y-1) > I(x,y).

@subsection lbp-quantization Quantized LBP

For a 3x3 neighborhood, an LBP is a string of eight bits and so there
are 256 possible LBPs. These are usually too many for a reliable
statistics (histogram) to be computed. Therefore the 256 patterns are
further quantized into a smaller number of patterns according to one
of the following rules:

- <b>Uniform</b> (::VlLbpUniform) There is one quantized pattern for
  each LBP that has exactly a transitions from 0 to 1 and one from 1
  to 0 when scanned in anti-clockwise order, plus one quantized
  pattern comprising the two uniform LBPs, and one quantized pattern
  comprising all the other LBPs. This yields a total of 58 quantized
  patterns.

The number of quantized LBPs, which depends on the quantization type,
can be obtained by ::vl_lbp_get_dimension.

@subsection lbp-histograms Histograms of LBPs

The quantized LBP patterns are further grouped into local
histograms. The image is divided into a number of cells of a
prescribed size (@c cellSize).  Then the quantized LBPs are aggregated
into histogram by using bilinear interpolation along the two spatial
dimensions (similar to HOG and SIFT).

@subsection lbp-usage Usage

First, one creates a ::VlLbp object instance by specifying the type of
quantization (this initializes some internal tables to speedup the
computation). Then, one obtains all the quantized LBPs histograms by
calling ::vl_lbp_process. This function expects as input a buffer
large enough to contain the computed features. If the image has size
@c width x @c height, there are exactly @c floor(width/cellSize) x @c
floor(height/cellSize) cells, each of which has a histogram of LBPs of
size @c dimension (as returned by ::vl_lbp_get_dimension). Thus the
required buffer has size @c floor(width/cellSize) x @c
floor(height/cellSize) x @c dimension.

::VlLbp supports computing transposed LPBs as well. A transposed LBP
is the LBP obtained by transposing the underlying image patch
(regarded as a matrix). This can be useful to compute the features
when the image is column major rather than row major.

@section lbp-references References

[1] T. Ojala, M. Pietikainen, and M. Maenpaa. "Multiresolution
    gray-scale and rotation invariant texture classification with
    local binary patterns". PAMI, 2010.

**/

#include "lbp.h"
#include "mathop.h"

/* ---------------------------------------------------------------- */
/*                                           Initialization helpers */
/* ---------------------------------------------------------------- */

static void
_vl_lbp_init_uniform(VlLbp * self)
{
  int i, j ;

  /* one bin for constant patterns, 8*7 for 2-uniform, one for rest */
  self->dimension = 58 ;

  /* all but selected patterns map to 57 */
  for (i = 0 ; i < 256 ; ++i) {
    self->mapping[i] = 57 ;
  }

  /* uniform patterns map to 56 */
  self->mapping[0x00] = 56 ;
  self->mapping[0xff] = 56 ;

  /* now uniform pattenrs, in order */
  /* locations: 0:E, 1:SE, 2:S, ..., 7:NE */
  for (i = 0 ; i < 8 ; ++i) { /* string[i-1]=0, string[i]=1 */
    for (j = 1 ; j <= 7 ; ++j) { /* length of sequence of ones */
      /* string starting with j ones */
      int unsigned string = (1 << j) - 1 ;
      /* put i zeroes in front */
      string <<= i ;
      /* wrap around 8 bit boundaries */
      string = (string | (string >> 8)) & 0xff ;

      /* optionally transpose the pattern */
      if (self->transposed) {
        int unsigned original = string;
        int k ;
        /* flip the string left-right */
        string = 0 ;
        for (k = 0 ; k < 8 ; ++k) {
          string <<= 1 ;
          string |= original & 0x1  ;
          original >>= 1 ;
        }
        /* rotate 90 degrees */
        string <<= 3 ;
        string = (string | (string >> 8)) & 0xff ;
      }

      self->mapping[string] = i * 7 + (j-1) ;
    }
  }
}

/* ---------------------------------------------------------------- */

/** @brief Create a new LBP object
 ** @param type type of LBP features.
 ** @param transposed if @c true, then transpose each LBP pattern.
 ** @return new VlLbp object instance.
 **/

VlLbp *
vl_lbp_new(VlLbpMappingType type, vl_bool transposed)
{
  VlLbp * self = vl_malloc(sizeof(VlLbp)) ;
  if (self == NULL) {
    vl_set_last_error(VL_ERR_ALLOC, NULL) ;
    return NULL ;
  }
  self->transposed = transposed ;
  switch (type) {
    case VlLbpUniform: _vl_lbp_init_uniform(self) ; break ;
    default: exit(1) ;
  }
  return self ;
}

/** @brief Delete VlLbp object
 ** @param self object to delete.
 **/

void
vl_lbp_delete(VlLbp * self) {
  vl_free(self) ;
}


/* ---------------------------------------------------------------- */

/** @brief Extract LBP features
 ** @param self LBP object.
 ** @param features buffer to write the features to.
 ** @param image image.
 ** @param width image width.
 ** @param height image height.
 ** @param cellSize size of the LBP cells.
 **
 ** @a features is a  @c numColumns x @c numRows x @c dimension where
 ** @c dimension is the dimension of a LBP feature obtained from ::vl_lbp_get_dimension,
 ** @c numColumns is equal to @c floor(width / cellSize), and similarly
 ** for @c numRows.
 **/

VL_EXPORT void
vl_lbp_process (VlLbp * self,
                float * features,
                float * image, vl_size width, vl_size height,
                vl_size cellSize)
{
  vl_size cwidth = width / cellSize;
  vl_size cheight = height / cellSize ;
  vl_size cstride = cwidth * cheight ;
  vl_size cdimension = vl_lbp_get_dimension(self) ;
  vl_index x,y,cx,cy,k,bin ;

#define at(u,v) (*(image + width * (v) + (u)))
#define to(u,v,w) (*(features + cstride * (w) + cwidth * (v) + (u)))

  /* accumulate pixel-level measurements into cells */
  for (y = 1 ; y < (signed)height - 1 ; ++y) {
    float wy1 = (y + 0.5f) / (float)cellSize - 0.5f ;
    int cy1 = (int) vl_floor_f(wy1) ;
    int cy2 = cy1 + 1 ;
    float wy2 = wy1 - (float)cy1 ;
    wy1 = 1.0f - wy2 ;
    if (cy1 >= (signed)cheight) continue ;

    for (x = 1 ; x < (signed)width - 1; ++x) {
      float wx1 = (x + 0.5f) / (float)cellSize - 0.5f ;
      int cx1 = (int) vl_floor_f(wx1) ;
      int cx2 = cx1 + 1 ;
      float wx2 = wx1 - (float)cx1 ;
      wx1 = 1.0f - wx2 ;
      if (cx1 >= (signed)cwidth) continue ;

      {
        int unsigned bitString = 0 ;
        float center = at(x,y) ;
        if(at(x+1,y+0) > center) bitString |= 0x1 << 0; /*  E */
        if(at(x+1,y+1) > center) bitString |= 0x1 << 1; /* SE */
        if(at(x+0,y+1) > center) bitString |= 0x1 << 2; /* S  */
        if(at(x-1,y+1) > center) bitString |= 0x1 << 3; /* SW */
        if(at(x-1,y+0) > center) bitString |= 0x1 << 4; /*  W */
        if(at(x-1,y-1) > center) bitString |= 0x1 << 5; /* NW */
        if(at(x+0,y-1) > center) bitString |= 0x1 << 6; /* N  */
        if(at(x+1,y-1) > center) bitString |= 0x1 << 7; /* NE */
        bin = self->mapping[bitString] ;
      }

      if ((cx1 >= 0) & (cy1 >=0)) {
        to(cx1,cy1,bin) += wx1 * wy1;
      }
      if ((cx2 < (signed)cwidth)  & (cy1 >=0)) {
        to(cx2,cy1,bin) += wx2 * wy1 ;
      }
      if ((cx1 >= 0) & (cy2 < (signed)cheight)) {
        to(cx1,cy2,bin) += wx1 * wy2 ;
      }
      if ((cx2 < (signed)cwidth) & (cy2 < (signed)cheight)) {
        to(cx2,cy2,bin) += wx2 * wy2 ;
      }
    } /* x */
  } /* y */

  /* normalize cells */
  for (cy = 0 ; cy < (signed)cheight ; ++cy) {
    for (cx = 0 ; cx < (signed)cwidth ; ++ cx) {
      float norm = 0 ;
      for (k = 0 ; k < (signed)cdimension ; ++k) {
        norm += features[k * cstride] ;
      }
      norm = sqrtf(norm) + 1e-10f; ;
      for (k = 0 ; k < (signed)cdimension ; ++k) {
        features[k * cstride] = sqrtf(features[k * cstride]) / norm  ;
      }
      features += 1 ;
    }
  } /* next cell to normalize */
}
