/** @file    random.h
 ** @brief   Random
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_RANDOM_H
#define VL_RANDOM_H

#include "host.h"

/** @brief Random numbber generator state */
typedef struct _VlRand {
  vl_uint32 mt [624] ;
  vl_size mti ;
} VlRand ;

/** @name Setting and reading the state
 **
 ** @{ */
VL_EXPORT void vl_rand_init (VlRand * self) ;
VL_EXPORT void vl_rand_seed (VlRand * self, vl_uint32 s) ;
VL_EXPORT void vl_rand_seed_by_array (VlRand * self,
                                      vl_uint32 const key [],
                                      vl_size keySize) ;
/** @} */

/** @name Generate random numbers
 **
 ** @{ */
VL_EXPORT vl_uint32 vl_rand_uint32 (VlRand * self) ;
VL_EXPORT vl_int32  vl_rand_int31  (VlRand * self) ;
VL_EXPORT double    vl_rand_real1  (VlRand * self) ;
VL_EXPORT double    vl_rand_real2  (VlRand * self) ;
VL_EXPORT double    vl_rand_real3  (VlRand * self) ;
VL_EXPORT double    vl_rand_res53  (VlRand * self) ;
/** @} */

/* VL_RANDOM_H */
#endif
