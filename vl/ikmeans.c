/** @file   ikmeans.c
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Integer K-Means clustering - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file ikmeans.h
 **
 ** Integer K-means (IKM) is an implementation of K-means clustering
 ** (or Vector Quantization, VQ) for integer data. This is
 ** particularly useful for clustering large collections of visual
 ** descriptors.
 **
 ** Use the function ::vl_ikm_new() to create a IKM
 ** quantizer. Initialize the IKM quantizer with @c K clusters by
 ** ::vl_ikm_init() or similar function. Use ::vl_ikm_train() to train
 ** the quantizer. Use ::vl_ikm_push() or ::vl_ikm_push_one() to
 ** quantize new data.
 **
 ** Given data @f$x_1,\dots,x_N\in R^d@f$ and a number of clusters
 ** @f$K@f$, the goal is to find assignments @f$a_i\in\{1,\dots,K\},@f$
 ** and centers @f$c_1,\dots,c_K\in R^d@f$ so that the <em>expected
 ** distortion</em>
 **
 ** @f[
 **   E(\{a_{i}, c_j\}) = \frac{1}{N} \sum_{i=1}^N d(x_i, c_{a_i})
 ** @f]
 **
 ** is minimized. Here @f$d(x_i, c_{a_i})@f$ is the
 ** <em>distortion</em>, i.e. the cost we pay for representing @f$ x_i
 ** @f$ by @f$ c_{a_i} @f$. IKM uses the squared distortion
 ** @f$d(x,y)=\|x-y\|^2_2@f$.
 **
 ** @section ikmeans-algo Algorithms
 **
 ** @subsection ikmeans-alg-init Initialization
 **
 ** Most K-means algorithms are iterative and needs an initialization
 ** in the form of an initial choice of the centers
 ** @f$c_1,\dots,c_K@f$. We include the following options:
 **
 ** - User specified centers (::vl_ikm_init);
 ** - Random centers (::vl_ikm_init_rand);
 ** - Centers from @c K randomly selected data points (::vl_ikm_init_rand_data).
 **
 ** @subsection ikmeans-alg-lloyd Lloyd
 **
 ** The Lloyd (also known as Lloyd-Max and LBG) algorithm iteratively:
 **
 ** - Fixes the centers, optimizing the assignments (minimizing by
 **   exhaustive search the association of each data point to the
 **   centers);
 ** - Fixes the assignments and optimizes the centers (by descending
 **   the distortion error function). For the squared distortion, this
 **   step is in closed form.
 **
 ** This algorithm is not particularly efficient because all data
 ** points need to be compared to all centers, for a complexity
 ** @f$O(dNKT)@f$, where <em>T</em> is the total number of iterations.
 **
 ** @subsection ikmeans-alg-elkan Elkan
 **
 ** The Elkan algorithm is an optimized variant of Lloyd. By making
 ** use of the triangle inequality, many comparisons of data points
 ** and centers are avoided, especially at later iterations.
 ** Usually 4-5 times less comparisons than Lloyd are preformed,
 ** providing a dramatic speedup in the execution time.
 **
 **/

#include "ikmeans.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */
#include "assert.h"

static void    vl_ikm_init_lloyd      (VlIKMFilt*) ;
static void    vl_ikm_init_elkan      (VlIKMFilt*) ;
static int     vl_ikm_train_lloyd     (VlIKMFilt*, vl_uint8 const*, int) ;
static int     vl_ikm_train_elkan     (VlIKMFilt*, vl_uint8 const*, int) ;
static void    vl_ikm_push_lloyd      (VlIKMFilt*, vl_uint*, vl_uint8 const*, int) ;
static void    vl_ikm_push_elkan      (VlIKMFilt*, vl_uint*, vl_uint8 const*, int) ;

/** @brief Create a new IKM quantizer
 **
 ** @param method Clustering algorithm.
 **
 ** The function allocates initializes a new IKM quantizer to
 ** operate based algorithm @a method.
 **
 ** @a method has values in the enumerations ::VlIKMAlgorithms.
 **
 ** @return new IKM quantizer.
 **/

VlIKMFilt *
vl_ikm_new (int method)
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

/** @brief Delete IKM quantizer
 **
 ** @param f IKM quantizer.
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
 ** @param f     IKM quantizer.
 ** @param data  data.
 ** @param N     number of data (@a N @c >= 1).
 **
 ** @return -1 if an overflow may have occurred.
 **/

VL_EXPORT
int vl_ikm_train (VlIKMFilt *f, vl_uint8 const *data, int N)
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
    abort() ;
  }
  return err ;
}

/** @brief Project data to clusters
 **
 ** @param f     IKM quantizer.
 ** @param asgn  Assignments (out).
 ** @param data  data.
 ** @param N     number of data (@a N @c >= 1).
 **
 ** The function projects the data @a data on the integer K-means
 ** clusters specified by the IKM quantizer @a f. Notice that the
 ** quantizer must be initialized.
 **/

VL_EXPORT
void
vl_ikm_push (VlIKMFilt *f, vl_uint *asgn, vl_uint8 const *data, int N) {
  switch (f -> method) {
  case VL_IKM_LLOYD : vl_ikm_push_lloyd (f, asgn, data, N) ; break ;
  case VL_IKM_ELKAN : vl_ikm_push_elkan (f, asgn, data, N) ; break ;
  default :
    abort() ;
  }
}

/** @brief Project one datum to clusters
 **
 ** @param centers centers.
 ** @param data    datum to project.
 ** @param K       number of centers.
 ** @param M       dimensionality of the datum.
 **
 ** The function projects the specified datum @a data on the clusters
 ** specified by the centers @a centers.
 **
 ** @return the cluster index.
 **/

VL_EXPORT
vl_uint
vl_ikm_push_one (vl_ikm_acc const *centers,
		 vl_uint8 const *data,
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
