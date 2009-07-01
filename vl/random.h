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

#include "generic.h"

/** @name Setting and reading the state
 **
 ** @{ */
VL_EXPORT void vl_rand_set_state (vl_uint32 const state [625]) ;
VL_EXPORT void vl_rand_get_state (vl_uint32 *state) ;
VL_EXPORT void vl_rand_seed (vl_uint32 s) ;
VL_EXPORT void vl_rand_seed_by_array (vl_uint32 const init_key [], int key_length) ;
/** @} */

/** @name Generate random numbers
 **
 ** @{ */
VL_EXPORT vl_uint32 vl_rand_uint32 () ;
VL_EXPORT vl_int32  vl_rand_int31 () ;
VL_EXPORT double    vl_rand_real1 () ;
VL_EXPORT double    vl_rand_real2 () ;
VL_EXPORT double    vl_rand_real3 () ;
VL_EXPORT double    vl_rand_res53 () ;
/** @} */

/* VL_RANDOM_H */
#endif
