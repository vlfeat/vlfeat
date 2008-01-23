/** @file   aib.h
 ** @author Brian Fulkerson
 ** @brief  Agglomerative Information Bottleneck (AIB)
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_AIB_H
#define VL_AIB_H

#include "generic.h"

/** @brief AIB probability type */
typedef vl_double vl_aib_prob ;

/** @brief AIB node type */
typedef vl_uint vl_aib_node ;

vl_aib_node *vl_aib (vl_aib_prob * Pcx, 
                     vl_uint nlabels, 
                     vl_uint nvalues,
                     double ** cost) ;

vl_aib_node *vl_aib_ec (vl_aib_prob * Pcx, 
                        vl_uint nlabels, 
                        vl_uint nvalues,
                        double ** cost) ;

/* VL_AIB_H */
#endif
