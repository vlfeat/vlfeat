/** @file   aib.h
 ** @author Brian Fulkerson
 ** @brief  Agglomerative Information Bottleneck - Declaration
 **/

#ifndef VL_AIB_H
#define VL_AIB_H

#include "generic.h"

/** @brief AIB probability type */
typedef vl_double vl_prob ;

/** @brief AIB node type */
typedef vl_uint32 vl_node ;

vl_node * vl_aib (vl_prob * Pic, vl_node nrows, vl_node ncols) ;

/* VL_AIB_H */
#endif
