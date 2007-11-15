/** @file   ikmeans.c
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Integer K-Means clustering - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

/** @file ikmeans.h
 ** 
 ** Integer K-means is an implementation of K-means clustering on
 ** integer data. This is especially useful when clustering large
 ** datasets of visual featrues.
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
#include "assert.h"

static void    vl_ikm_init_lloyd      (VlIKMFilt*) ;
static void    vl_ikm_init_elkan      (VlIKMFilt*) ;
static int     vl_ikm_train_lloyd     (VlIKMFilt*, vl_ikm_data const*, int) ;
static int     vl_ikm_train_elkan     (VlIKMFilt*, vl_ikm_data const*, int) ;
static void    vl_ikm_push_lloyd      (VlIKMFilt*, vl_uint*, vl_ikm_data const*, int) ;
static void    vl_ikm_push_elkan      (VlIKMFilt*, vl_uint*, vl_ikm_data const*, int) ;

/** @brief Create a new IKM Filter
 **
 ** @param method  Clustering method.
 **
 ** @return new IKM filter.
 **/

VlIKMFilt *vl_ikm_new (int method)
{
  VlIKMFilt *f =  vl_malloc (sizeof(VlIKMFilt)) ;
  f -> centers = 0 ;
  f -> inter_dist = 0 ;

  f -> M = 0 ;
  f -> K = 0 ;
  f -> method     = method ;
  f -> max_niters = 200 ;
  f -> verb       = 0 ;

  return f ;
}

/** @brief Delete IKM filter 
 **
 ** @param f IKM filter.
 **/

void vl_ikm_delete (VlIKMFilt* f)
{
  if (f) {
    if (f-> centers)    vl_free (f-> centers) ;
    if (f-> inter_dist) vl_free (f-> inter_dist) ;
    vl_free (f) ;
  }
}

/** @brief Train clusters
 **
 ** @param f       IKM filter.
 ** @param data       data.
 ** @param N          number of data (@a N &ge; 1).
 **
 ** @return -1 if an overflow may have occured.
 **/

int vl_ikm_train (VlIKMFilt *f, vl_ikm_data const *data, int N)
{ 
  int err ;
  
  if (f-> verb) {
    VL_PRINTF ("ikm: training with %d data\n",  N) ;
    VL_PRINTF ("ikm: %d clusters\n",  f -> K) ;
  }
  
  switch (f -> method) {
  case VL_IKM_LLOYD : err = vl_ikm_train_lloyd (f, data, N) ; break ;
  case VL_IKM_ELKAN : err = vl_ikm_train_elkan (f, data, N) ; break ;
  default :
    assert (0) ;
  }
  return err ;
}

/** @brief Push data to cluster
 **
 ** @param f     IKM filter.
 ** @param asgn  Assigments (out).
 ** @param data  data.
 ** @param N     number of data (@a N &ge; 1).
 **
 ** The function projects the data on the integer K-kmeans clusters
 ** specified by the centers.
 **/

void
vl_ikm_push (VlIKMFilt *f, vl_uint *asgn, vl_ikm_data const *data, int N) {
  switch (f -> method) {
  case VL_IKM_LLOYD : vl_ikm_push_lloyd (f, asgn, data, N) ; break ;
  case VL_IKM_ELKAN : vl_ikm_push_elkan (f, asgn, data, N) ; break ;
  default :
    assert (0) ;
  }
}

/** @brief Push one datum to clusters
 **
 ** @param centers centers.
 ** @param data    datum to project.
 ** @param K       number of centers.
 ** @param M       dimensionality of the datum.
 **
 ** The function projects the datum on the integer K-kmeans clusters
 ** specified by the centers.
 **
 ** @return the cluster index.
 **/

vl_uint
vl_ikm_push_one (vl_ikm_acc const *centers, 
		 vl_ikm_data const *data, 
		 int M, int K)
{
  int i,k ;
  
  /* assign data to centers */
  vl_int32 best_dist = 0 ;
  vl_uint  best      = (vl_uint)-1 ;
  
  for(k = 0 ; k < K ; ++k) {
    vl_int32 dist = 0 ;
    
    /* compute distance with this center */
    for(i = 0 ; i < M ; ++i) {
      vl_int32 delta = data[i] - centers[k*M + i] ;
      dist += delta * delta ;
    }
    
    /* compare with current best */
    if (best == (vl_uint)-1 || dist < best_dist) {
      best = k  ;
      best_dist = dist ;
    }
  }
  return best;
}

#include "ikmeans_init.tc"
#include "ikmeans_lloyd.tc"
#include "ikmeans_elkan.tc"
