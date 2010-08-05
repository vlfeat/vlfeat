/** @internal
 ** @file     imopv_sse2.h
 ** @author   Andrea Vedaldi
 ** @brief    Vectorized image operations - SSE2
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_IMOPV_SSE2_H
#define VL_IMOPV_SSE2_H

#include "generic.h"

#ifndef VL_DISABLE_SSE2

VL_EXPORT
void _vl_imconvcol_vf_sse2 (float* dst, int dst_stride,
                            float const* src,
                            int src_width, int src_height, int src_stride,
                            float const* filt, int filt_begin, int filt_end,
                            int step, unsigned int flags) ;

VL_EXPORT
void _vl_imconvcol_vd_sse2 (double* dst, int dst_stride,
                            double const* src,
                            int src_width, int src_height, int src_stride,
                            double const* filt, int filt_begin, int filt_end,
                            int step, unsigned int flags) ;

/*
VL_EXPORT
void _vl_imconvcoltri_vf_sse2 (float* dst, int dst_stride,
                               float const* src,
                               int src_width, int src_height, int src_stride,
                               int filt_size,
                               int step, unsigned int flags) ;

VL_EXPORT
void _vl_imconvcoltri_vd_sse2 (double* dst, int dst_stride,
                               double const* src,
                               int src_width, int src_height, int src_stride,
                               int filt_size,
                               int step, unsigned int flags) ;
*/

#endif

/* VL_IMOPV_SSE2_H */
#endif
