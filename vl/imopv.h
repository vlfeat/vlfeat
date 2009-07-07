/** @file   imopv.h
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations
 **/

/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#ifndef VL_IMOPV_H
#define VL_IMPOV_H

#include "generic.h"

/** @name Image convolution flags
 ** @{ */
#define VL_PAD_BY_ZERO       (0x0 << 0) /**< @brief Pad with zeroes. */
#define VL_PAD_BY_CONTINUITY (0x1 << 0) /**< @brief Pad by continuity. */
#define VL_PAD_MASK          (0x3)      /**< @brief Padding field selector. */
#define VL_TRANSPOSE         (0x1 << 2) /**< @brief Transpose result. */
/** @} */

/** @name Image convolution
 ** @{ */
VL_EXPORT
void vl_imconvcol_vf (float* dst, int dst_stride,
                      float const* src,
                      int src_width, int src_height, int src_stride,
                      float const* filt, int filt_begin, int filt_end,
                      int step, unsigned int flags) ;

VL_EXPORT
void vl_imconvcol_vd (double* dst, int dst_stride,
                      double const* src,
                      int src_width, int src_height, int src_stride,
                      double const* filt, int filt_begin, int filt_end,
                      int step, unsigned int flags) ;

VL_EXPORT
void vl_imconvcoltri_vf (float* dst, int dst_stride,
                         float const* src,
                         int src_width, int src_height, int src_stride,
                         int filt_size,
                         int step, unsigned int flags) ;

VL_EXPORT
void vl_imconvcoltri_vd (double* dst, int dst_stride,
                         double const* src,
                         int src_width, int src_height, int src_stride,
                         int filt_size,
                         int step, unsigned int flags) ;
/** @} */

/* VL_IMOPV_H */
#endif
