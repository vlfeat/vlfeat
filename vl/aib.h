/** @file   aib.h
 ** @author Brian Fulkerson
 ** @brief  Agglomerative Information Bottleneck (AIB) with speedups - Declaration
 **
 ** This provides an implementation of Agglomerative Information Bottleneck 
 ** (AIB) as first described in:
 ** 
 ** <em>N. Slonim and N. Tishby. Agglomerative information bottleneck. 
 ** In Proc. NIPS, 1999</em>
 **
 ** While our implemention provides identical results as the one described in 
 ** [Slonim], we have added some speedups which allow us to handle 
 ** much larger datasets.
 ** 
 ** The key idea is that we maintain a list of the best possible merge and the
 ** corresponding decrease in information and at each iteration and after a 
 ** merge, we only update those nodes whose minimum value could have changed.
 **
 ** Suppose that L is the number of rows which will be merged and C is the 
 ** number of columns (typically, L >> C). This speedup allows our
 ** algorithm to use O(L^2) time in many cases and O((C+2)*L) 
 ** space (as opposed to [Slonim] which takes O(L^3)) time and O(L^2) space).
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
