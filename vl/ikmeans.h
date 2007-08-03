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

/* Returns the centers and optionally assigns points if asgn_pt is not null */
acc_t * vl_ikmeans(data_t * data, int M, int N, idx_t K, idx_t * asgn);

/* Returns the assignments */
idx_t * vl_ikmeans_push(acc_t * centers, idx_t K, data_t * data, int M, int N);
/* VL_IKMEANS_H */
#endif
