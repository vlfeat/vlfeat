/** @file   ikmeans.c
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Integer K-Means clustering - Definition
 **/

/* AUTORIGHTS */

/** @file ikmeans.h
 ** 
 ** Integer K-means is a basic implementation of K-means clustering on
 ** integer data. The main application is partitioning large visual
 ** features spaces, for which it is usually more important to proess
 ** a large amount of data (even if the data has lower accuracy).
 **
 ** The function ::vl_ikmeans() computes the clusters given some data,
 ** the function ::vl_ikmeans_push_one() projects a new datum on its
 ** cluster and the functon ::vl_ikmeans_push() does the same for
 ** multiple data.
 **
 ** The K-means algorithm is straightforward: Starting from K randomly
 ** chosen data point, the algorithm alternatively estimates the
 ** associations of each datum to the closest cluster center (in L2
 ** norm) and then the cluster centers as the mean of the data
 ** associated to them.
 **/

#include "ikmeans.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */

/* pairs are used to generate random permutations of data */
typedef struct
{
  int w;
  int j;
} pair_t;

int cmp_pair (void const *a, void const *b)
{
  pair_t *pa = (pair_t *) a;
  pair_t *pb = (pair_t *) b;
  return pa->w - pb->w;
}

/** @brief Integer K-means clusering
 **
 ** @param centers    cluster centers (output).
 ** @param asgn       data to clusters assignments (output).
 ** @param data       data.
 ** @param M          data dimensionality.
 ** @param N          number of data.
 ** @param K          number of clusters.
 ** @param max_passes maximum number of passes.
 **
 ** The function clusters the N D-dimensional data in K partitions and
 ** fills a MxK matrix with the cluster centers.  If @a asgn is not @c
 ** NULL, the function also writes to @a asgn the cluster index of
 ** each datum.
 **
 ** @return -1 if an overflow may have occured.
 **/
int 
vl_ikmeans (vl_int32 *centers, vl_uint* asgn, 
            vl_uint8 *data, int M, int N, int K,
            int max_passes)
{  
  /* random permutation */
  int err = 0 ;
  vl_uint i, j, k, pass;
  vl_int32 *counts = malloc (sizeof (vl_int32) * K);
  pair_t   *pairs     = (pair_t *) malloc (sizeof(pair_t) * N);
  
  /* we need this buffer anyways */
  vl_bool own_asgn = (asgn == NULL) ;
  if (own_asgn) asgn = malloc(sizeof(vl_uint)*N);

  /* -----------------------------------------------------------------
   *                                         Randomized initialization
   * -------------------------------------------------------------- */

  /* permute the data randomly */
  for (j = 0 ; j < N ; ++j) {
    pairs[j].j = j ;
    pairs[j].w = rand () ;
  }
  qsort (pairs, N, sizeof(pair_t), cmp_pair);

  /* initialize centers from random data points */
  for (k = 0; k < K; ++k)
    for (i = 0; i < M; ++i)
      centers[k * M + i] = data[pairs[k].j * M + i];

  /* -----------------------------------------------------------------
   *                                                  Calc. assigments
   * -------------------------------------------------------------- */

  for (pass = 0; 1; ++pass) {
    vl_bool done = 1 ;
    
    /* assign data to centers */
    for (j = 0; j < N; ++j) {
      vl_int32 best_dist = 0 ;
      vl_uint  best = (vl_uint)-1 ;
      
      for (k = 0; k < K; ++k) {
          vl_int32 dist = 0;
          
          /* compute distance with this center */
          for (i = 0; i < M; ++i) {
            vl_int32 delta =
              data [j * M + i] - centers [k * M + i] ;
            dist += delta * delta ;
          }
          
          /* compare with current best */
          if (best == (vl_uint)-1 || dist < best_dist) {
            best = k ;
            best_dist = dist ;
          }
      }
      if (asgn[j] != best) {
        asgn[j] = best ;
        done = 0 ;
      }
    }
    
    /* stopping condition */
    if (done || pass == max_passes) break ;

  /* -----------------------------------------------------------------
   *                                                     Calc. centers
   * -------------------------------------------------------------- */
    
    /* re-compute centers */
    memset (centers, 0, sizeof (vl_int32) * M * K);
    memset (counts,  0, sizeof (vl_int32) * K);
    for (j = 0; j < N; ++j) {
      int this_center = asgn [j] ;      
      ++ counts [this_center] ;      
      for (i = 0; i < M; ++i)
        centers [this_center * M + i] += data[j * M + i];
    }
    
    for (k = 0; k < K; ++k) {
      vl_int32 n = counts [k];
      if (n > 0xffffff) {
        err = 1 ;
      }
      if (n > 0) {
        for (i = 0; i < M; ++i)
          centers [k * M + i] /= n;
      } else {
        for (i = 0; i < M; ++i)
          centers [k * M + i] = data [pairs[k].j * M + i];
      }
    }
  }
  free (pairs) ;
  free (counts) ;
  if (own_asgn) free(asgn) ;
  return err ;
}

/** @brief Project one datum on integer K-means clusters
 **
 ** @param centers cluser centers.
 ** @param K       number of centers.
 ** @param data    datum to project.
 ** @param M       dimensionality of the datum
 **
 ** The function projects the datum on the integer K-kmeans clusters
 ** specified by the centers.
 **
 ** @return the cluster index.
 **/
VL_INLINE
vl_uint
vl_ikmeans_push_one (vl_int32  *centers, int K, vl_uint8* data, int M)
{
  int i,k ;
  
  /* assign data to centers */
  vl_int32 best_dist = 0 ;
  vl_uint  best      = (vl_uint)-1 ;
  
  for(k = 0 ; k < K ; ++k) {
    vl_int32 dist = 0 ;
    
    /* compute distance with this center */
    for(i = 0 ; i < M ; ++i)
      {
        vl_int32 delta = data[i] - centers[k*M + i] ;
        dist += delta*delta ;
      }
    
    /* compare with current best */
    if (best == (vl_uint)-1 || dist < best_dist) {
      best = k  ;
      best_dist = dist ;
    }
  }
  return best;
}


/** @brief Project data on integer K-means clusters
 **
 ** @param asgn    data to clusters assignments (output).
 ** @param centers cluser centers.
 ** @param K       number of centers.
 ** @param data    data to project.
 ** @param M       dimensionality of the data.
 ** @param N       number of data.
 **
 ** The function projects the data on the integer K-kmeans clusters
 ** specified by the centers.
 **/

void
vl_ikmeans_push (vl_uint *asgn,
                 vl_int32 *centers, int K, vl_uint8* data, int M, int N)
{
  int j ;
  for(j=0 ; j < N ; ++j) {
    asgn[j] = vl_ikmeans_push_one(centers, K, &data[j*M], M);
  }
}
