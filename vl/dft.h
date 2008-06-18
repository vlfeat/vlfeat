/** @file     dft.h
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (DFT)
 **/

/* AUTORIGHTS
 Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
 
 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#ifndef VL_DFT_H 
#define VL_DFT_H 

#include "generic.h"

/** @brief DFT keypoint */
typedef struct VlDftKeypoint_
{
  double x ; /**< x coordinate */
  double y ; /**< y coordinate */
  double s ; /**< scale */
} VlDftKeypoint ;

/** @brief DFT filter */
typedef struct VlDftFilter_ 
{  
  int width ;            /**< image width */
  int height ;           /**< image height */
  int step ;             /**< downsampling step */
  int size ;             /**< spatial bin size (in pixels) */
  int fast ;             /**< be fast but more approximated */

  int dwidth ;           /**< downsampled width */
  int dheight ;          /**< downsampled height */
  
  float **hist ;         /**< dense histogram */
  float *tmp ;           /**< temporary buffer */
  float *tmp2 ;          /**< temporary buffer */
  float *descr ;         /**< descriptors */
  VlDftKeypoint *keys ;  /**< keypoints */
  int nkeys ;            /**< number of keypoints */
}  VlDftFilter ;

VL_EXPORT VlDftFilter *vl_dft_new (int width, int height, int step, int size) ;
VL_EXPORT void vl_dft_delete (VlDftFilter *f) ;
VL_EXPORT void vl_dft_process (VlDftFilter *f, float const* im, vl_bool fast) ;

/** @name Retrieve data and parameters
 ** @{
 **/
VL_INLINE float         *vl_dft_get_descriptors   (VlDftFilter *f) ;
VL_INLINE int            vl_dft_get_keypoint_num  (VlDftFilter *f) ;
VL_INLINE VlDftKeypoint *vl_dft_get_keypoints     (VlDftFilter *f) ;
VL_INLINE void           vl_dft_transpose_descriptor (float* dst, float* src) ;

/** @} */

/** ------------------------------------------------------------------
 ** @brief Get descriptors.
 ** @param DFT filter.
 **/

float *
vl_dft_get_descriptors (VlDftFilter *f)
{
  return f->descr ;
}

/** ------------------------------------------------------------------
 ** @brief Get keypoints
 ** @param f DFT filter.
 **/

VlDftKeypoint *
vl_dft_get_keypoints (VlDftFilter *f)
{
  return f->keys ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of keypoints
 ** @param f DFT filter.
 **/

int
vl_dft_get_keypoint_num (VlDftFilter *f)
{
  return f->nkeys ;
}


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Transpose descriptor
 **
 ** @param dst destination buffer.
 ** @param src source buffer.
 **
 ** The function writes to @a dst the transpose of the SIFT descriptor
 ** @a src. The transpose is defined as the descriptor that one
 ** obtains from computing the normal descriptor on the transposed
 ** image.
 **/

VL_INLINE void 
vl_dft_transpose_descriptor (float* dst, float* src) 
{
  int const BO = 8 ;  /* number of orientation bins */
  int const BP = 4 ;  /* number of spatial bins     */
  int i, j, t ;
  
  for (j = 0 ; j < BP ; ++j) {
    int jp = BP - 1 - j ;
    for (i = 0 ; i < BP ; ++i) {
      int o  = BO * i + BP*BO * j  ;
      int op = BO * i + BP*BO * jp ;      
      dst [op] = src[o] ;      
      for (t = 1 ; t < BO ; ++t) 
        dst [BO - t + op] = src [t + o] ;
    }
  }
}

/*  VL_DFT_H */
#endif
