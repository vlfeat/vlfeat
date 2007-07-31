/** @file   ikmeans.h
 ** @author Andrea Vedaldi
 ** @author Brian Fulkerson
 ** @brief  Integer K-Means Clustering - Declaration
 **/
#ifndef VL_IKMEANS_H
#define VL_IKMEANS_H

#include "generic.h"

typedef unsigned char data_t;
typedef int acc_t;
typedef unsigned int idx_t;

void ikmeans(data_t * data_pt, int M, int N, idx_t K,
             acc_t * centers_pt, idx_t * asgn_pt);

void vl_ikmeans_push(data_t * data_pt, acc_t * centers_pt, int M, int N, idx_t K,
                     idx_t * asgn_pt);
/* VL_IKMEANS_H */
#endif
