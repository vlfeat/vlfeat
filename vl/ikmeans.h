/** @file   ikmeans.h
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Integer K-Means clustering - Declaration
 **/

#ifndef VL_IKMEANS_H
#define VL_IKMEANS_H

#include "generic.h"

int       vl_ikmeans          (vl_int32 *centers, vl_uint* asgn, 
                               vl_uint8 *data, int M, int N, int K,
                               int max_passes) ;
void      vl_ikmeans_push     (vl_uint *asgn,
                               vl_int32 *centers, int K, vl_uint8 *data, int M, int N) ;
vl_uint   vl_ikmeans_push_one (vl_int32 *centers, int K, vl_uint8 *data, int M) ;

/* VL_IKMEANS_H */
#endif
