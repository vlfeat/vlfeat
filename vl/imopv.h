/** @file   imopv.h
 ** @author Andrea Vedaldi
 ** @brief  Vectorized image operations
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
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
void vl_imconvcoltri_f (float * dest, vl_size destStride,
                        float const * image,
                        vl_size imageWidth, vl_size imageHeight, vl_size imageStride,
                        vl_size filterSize,
                        vl_size step, int unsigned flags) ;

VL_EXPORT
void vl_imconvcoltri_d (double * dest, vl_size destStride,
                        double const * image,
                        vl_size imageWidth, vl_size imageHeight, vl_size imageStride,
                        vl_size filterSize,
                        vl_size step, int unsigned flags) ;
/** @} */

/** @name Integral image
 ** @{ */
VL_EXPORT
void vl_imintegral_f (float * integral,  vl_size integralStride,
                      float const * image,
                      vl_size imageWidth, vl_size imageHeight, vl_size imageStride) ;

VL_EXPORT
void vl_imintegral_d (double * integral,  vl_size integralStride,
                      double const * image,
                      vl_size imageWidth, vl_size imageHeight, vl_size imageStride) ;

VL_EXPORT
void vl_imintegral_i32 (vl_int32 * integral,  vl_size integralStride,
                        vl_int32 const * image,
                        vl_size imageWidth, vl_size imageHeight, vl_size imageStride) ;

VL_EXPORT
void vl_imintegral_ui32 (vl_uint32 * integral,  vl_size integralStride,
                         vl_uint32 const * image,
                         vl_size imageWidth, vl_size imageHeight, vl_size imageStride) ;
/** @} */

/** @name Distance transform */
/** @{ */

VL_EXPORT void
vl_image_distance_transform_d (double const * image,
                               vl_size numColumns,
                               vl_size numRows,
                               vl_size columnStride,
                               vl_size rowStride,
                               double * distanceTransform,
                               vl_uindex * indexes,
                               double coeff,
                               double offset) ;

VL_EXPORT void
vl_image_distance_transform_f (float const * image,
                               vl_size numColumns,
                               vl_size numRows,
                               vl_size columnStride,
                               vl_size rowStride,
                               float * distanceTransform,
                               vl_uindex * indexes,
                               float coeff,
                               float offset) ;

/** @} */

/* VL_IMOPV_H */
#endif
