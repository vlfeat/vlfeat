/** @file    random.h
 ** @author  Andrea Vedaldi
 ** @brief   Random
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_RANDOM_H
#define VL_RANDOM_H

#include <vl/generic.h>

/** @name Setting and reading the state
 **
 ** @{ */
void vl_rand_set_state (vl_uint32 const state [625]) ;
void vl_rand_get_state (vl_uint32 *state) ;
void vl_rand_seed (vl_uint32 s) ;
void vl_rand_seed_by_array (vl_uint32 const init_key [], int key_length) ;
/** @} */

/** @name Generate random numbers
 **
 ** @{ */
vl_uint32 vl_rand_uint32 () ;
vl_int32  vl_rand_int31 () ;
double    vl_rand_real1 () ;
double    vl_rand_real2 () ;
double    vl_rand_real3 () ;
double    vl_rand_res53 () ;
/** @} */

/* VL_RANDOM_H */
#endif
