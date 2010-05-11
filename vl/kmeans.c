/** @file   kmeans.c
 ** @author Andrea Vedaldi
 ** @brief  K-means - Declaration
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include "kmeans.h"
#include "generic.h"
#include "mathop.h"
#include <string.h>

/** @file kmeans.h

 @section kmeans-overview Overview

 @ref kmeans.h implements a number of algorithm for k-means quantisation.
 It supports

 - data of type @c float or @c double;
 - @e l1 and @e l2 distances;
 - random selection and <code>k-means++</code> initialization methods;
 - basic Lloyd and accelerated Elkan optimization methods.

 @section kmeans-usage Usage

 To use @ref kmeans.h to learn clusters from some training data,
 instantiate a ::KMeans object, set the configuration
 parameters, initialise the cluster centers, and run the trainig code.
 For instance, to learn @c numCenters clusters from
 @c numData vectors of dimension @c dimension
 and storage type @c float using L2 distance
 and at most 100 Lloyd iterations of the Lloyd algorithm use:

 @code
 #include <vl/kmeans.h>

 VlKMeansAlgorithm algorithm = VlKMeansLLoyd ;
 VlVectorComparisonType distance = VlDistanceL2 ;
 KMeans * kmeans = vl_kmeans_new (algorithm, distance, VL_TYPE_FLOAT) ;
 vl_kmeans_init_centers_with_rand_data (kmeans, data, dimension, numData, numCenters) ;
 vl_kmeans_set_max_num_iterations (kmeans, 100) ;
 vl_kmeans_train (kmeans) ;
 @endcode

 Use ::vl_kmeans_get_energy to get the solution energy (or an upper
 bound for the Elkan algorithm) and ::vl_kmeans_get_centers
 to obtain the @c numCluster cluster centers. Use ::vl_kmeans_push
 to quantize new data points.

 @subsection kmeans-usage-init Initialization algorithms

 @ref kmeans.h supports the following cluster initialization algorithms:

 - <b>Random data points</b> (::vl_kmeans_init_centers_with_rand_data)
   initialize the centers from a random selection of the training data.
 - <b>k-means++</b> (::vl_kmeans_init_centers_plus_plus) initialize
   the centers from a random selection of the training data while
   attempting to obtain a good coverage of the dataset. This is
   the strategy from [1].

 @subsection kmeans-usage-optimizers Optimization algorithms

 @ref kmeans.h supports the following optimization algorithms:

 - <b>Lloyd</b> [2] (::VlKMeansLloyd). This is the standard k-means algorithm,
   alternating the estimation of the point-to-cluster memebrship and
   of the cluster centers (means in the Euclidean case). Estimating
   membership requires computing the distance of each point to all
   cluster centers, which can be extremely slow.
 - <b>Elkan</b> [3] (::VlKMeansElkan). This is a variation of [2] that uses
   the triangular inequality to avoid many distance calculations
   when assigning points to clusters and is typically much
   faster than [2]. However, it uses storage
   proportional to the square of the number of clusters, which
   makes it unpractical for a very large number of clusters.

 @section kmeans-tech Technical details

 Given data points @f$ x_1, \dots, x_n \in \mathbb{R}^d @f$, k-means
 searches for @f$ k @f$ vectors @f$ c_1, \dots, c_n \in \mathbb{R}^d @f$
 (cluster centers) and a function @f$ \pi : \{1, \dots, n\}
 \rightarrow \{1, \dots, k\} @f$ (cluster memberships)
 that minimize the objective:

 @f[
   E(c_1,\dots,c_n,\pi) = \sum_{i=1}^n d^2(x_i, c_{\pi(i)})
 @f]

 A simple procedure due to Lloyd [2] to locally optimize this objective
 alternates estimating the cluster centers and the membeship function.
 Specifically, given the membership function @f$ \pi @f$,
 the objective can be minimized independently for eac @f$ c_k @f$
 by minimizing

 @f[
   \sum_{i : \pi(i) = k} d^2(x_i, c_k)
 @f]

 For the Euclidean distance, the minimizer is simply the mean of the points
 assigned to that cluster. For other distances, the minimizer is
 a generalized average. For instance, for the @f$ l^1 @f$ distance,
 this is the median. Assuming that computing the average is linear
 in the number of points and the data dimension,
 this step requires @f$ O(nd) @f$ operations.

 Similarly, given the centers @f$ c_1, \dots, c_k @f$, the objective
 can be optimized independently for the membership @f$ \pi(i) @f$ of
 each point @f$ x_i @f$ by minimizing @f$ d^2(x_i, c_{\pi(i)}) @f$
 over @f$ \pi(i) \in \{1, \dots, k\} @f$. Assuming that computing
 a distance is @f$ O(d) @f$, this step requires @f$ O(ndk) @f$ operations
 and dominates the other.

 The algorithm usually starts by initializing the centers from a
 random selection of the data point.

 @subsection kmeans-tech-kmeanspp Initialization by k-means++

 [1] proposes a randomized initialization of the centers which improves
 upon random selection. The first center @f$ c_1 @f$ is selected at random
 from the data points @f$ x_1, \dots, x_n @f$ and
 the distance from this center to all points @f$ d^2(x_i, c_1) @f$ is
 computed. Then the second center @f$ c_2 @f$ is selected at random
 from the data points with probability proportional to the distance, and
 the procedure is repeated using the minimum distance to the centers
 collected so far.

 @subsection kmeans-tech-elkan Speeding up by using the triangular inequality

 [3] proposes to use the triangular inequality to avoid most
 distances calculations when computing point-to-cluster membership
 and the cluster centers did not change much from the previous iteration.

 This uses two key ideas:

 - If a point @f$ x_i @f$ is very close to its current
   center @f$  c_{\pi(i)} @f$ and this center is very far from another
   center @f$ c @f$, then the point cannot be assigned to @f$ c @f$.
   Specifically, if @f$ d(x_i, c_{\pi(i)}) \leq d(c_{\pi(i)}, c) / 2 @f$,
   then also @f$ d(x_i, c_{\pi(i)}) \leq d(x_i, c) @f$.

 - If a center @f$ c @f$ is updated to @f$ \hat c @f$, then the variation
   of the distance of the center to any point can be bounded by
   @f$ d(x, c) - d(c, \hat c) \leq d(x, \hat c) \leq d(x,c) + d(c, \hat c) @f$.

 The first idea is used by keeping track of the inter-center distances and
 exlcuding reassigments to centers too far away from the current assigned
 center. The second idea is used by keeping for each point an upper bound
 to the distance to the currently assigned center and a lower bound to the
 distance to all the other centers. Unless such bounds do not intersect,
 then a point need not to be reassigned. See [3] for details.

 @section kmeans-references References

 - [1] D. Arthur and S. Vassilvitskii.
   <em>k-means++: The advantages of careful seeding.</em>
   In Proc. ACM-SIAM Symp. on Discrete Algorithms, 2007.

 - [2] S. Lloyd.
   <em>Least square quantization in PCM.</em>
   IEEE Trans. on Information Theory, 28(2), 1982.

 - [3] C. Elkan.
   <em>Using the triangle inequality to accelerate k-means.</em>
   In Proc. ICML, 2003.

 */
/* ================================================================ */
#ifndef VL_KMEANS_INSTANTIATING


/** ------------------------------------------------------------------
 ** @brief Reset state
 **
 ** The function reset the state of the KMeans object. It deletes
 ** any stored centers, releasing the corresponding memory. This
 ** cancels the effect of seeding or setting the centers, but
 ** does not change the other configuration parameters.
 **/

VL_EXPORT void
vl_kmeans_reset (VlKMeans * self)
{
  self->numCenters = 0 ;
  self->dimension = 0 ;

  if (self->centers) vl_free(self->centers) ;
  if (self->centerDistances) vl_free(self->centerDistances) ;
  self->centers = NULL ;
  self->centerDistances = NULL ;
}

/** ------------------------------------------------------------------
 ** @brief Create a new KMeans object
 ** @param dataType type of data (::VL_TYPE_FLOAT or ::VL_TYPE_DOUBLE)
 ** @param distance distance.
 ** @return new KMeans object instance.
**/

VL_EXPORT VlKMeans *
vl_kmeans_new (vl_type dataType,
               VlVectorComparisonType distance)
{
  VlKMeans * self = vl_malloc(sizeof(VlKMeans)) ;

  self->algorithm = VlKMeansLLoyd ;
  self->distance = distance ;
  self->dataType = dataType ;

  self->verbosity = 0 ;
  self->maxNumIterations = 100 ;
  self->numRepetitions = 1 ;

  self->centers = NULL ;
  self->centerDistances = NULL ;

  vl_kmeans_reset (self) ;

  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Create a new KMeans object by copy
 ** @param kmeans KMeans object to copy.
 ** @return new copy.
 **/

VL_EXPORT VlKMeans *
vl_kmeans_new_copy (VlKMeans const * kmeans)
{
  VlKMeans * self = vl_malloc(sizeof(VlKMeans)) ;

  self->algorithm = kmeans->algorithm ;
  self->distance = kmeans->distance ;
  self->dataType = kmeans->dataType ;

  self->verbosity = kmeans->verbosity ;
  self->maxNumIterations = kmeans->maxNumIterations ;
  self->numRepetitions = kmeans->numRepetitions ;

  self->dimension = kmeans->dimension ;
  self->numCenters = kmeans->numCenters ;
  self->centers = NULL ;
  self->centerDistances = NULL ;

  if (kmeans->centers) {
    vl_size dataSize = vl_get_type_size(self->dataType) * self->dimension * self->numCenters ;
    self->centers = vl_malloc(dataSize) ;
    memcpy (self->centers, kmeans->centers, dataSize) ;
  }

  if (kmeans->centerDistances) {
    vl_size dataSize = vl_get_type_size(self->dataType) * self->numCenters * self->numCenters ;
    self->centerDistances = vl_malloc(dataSize) ;
    memcpy (self->centerDistances, kmeans->centerDistances, dataSize) ;
  }

  return self ;
}

/** ------------------------------------------------------------------
 ** @brief Deletes a KMeans object
 ** @param self KMeans object instance.
 **
 ** The function deletes the KMeans object instance created
 ** by ::vl_kmeans_new.
 **/

VL_EXPORT void
vl_kmeans_delete (VlKMeans * self)
{
  vl_kmeans_reset (self) ;
  vl_free (self) ;
}

/* an helper structure */
typedef struct _VlKMeansSortWrapper
{
  vl_uint32 * permutation ;
  void const * data ;
  vl_size stride ;
} VlKMeansSortWrapper ;

/* ---------------------------------------------------------------- */
/* Instantiate shuffle algorithm */

#define VL_SHUFFLE_type vl_uindex
#define VL_SHUFFLE_prefix _vl_kmeans
#include "shuffle-def.h"

/* #ifdef VL_KMEANS_INSTANTITATING */
#endif

/* ================================================================ */
#ifdef VL_KMEANS_INSTANTIATING

/* ---------------------------------------------------------------- */
/*                                                      Set centers */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_kmeans_set_centers_, SFX)
(VlKMeans * self,
 TYPE const * centers,
 vl_size dimension,
 vl_size numCenters)
{
  self->dimension = dimension ;
  self->numCenters = numCenters ;
  self->centers = vl_malloc (sizeof(TYPE) * dimension * numCenters) ;
  memcpy ((TYPE*)self->centers, centers,
          sizeof(TYPE) * dimension * numCenters) ;
}

/* ---------------------------------------------------------------- */
/*                                                   Random seeding */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_kmeans_seed_centers_with_rand_data_, SFX)
(VlKMeans * self,
 TYPE const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numCenters)
{
  vl_uindex i, j, k ;
  VlRand * rand = vl_get_rand () ;

  self->dimension = dimension ;
  self->numCenters = numCenters ;
  self->centers = vl_malloc (sizeof(TYPE) * dimension * numCenters) ;

  {
    vl_uindex * perm = vl_malloc (sizeof(vl_uindex) * numData) ;
#if (FLT == VL_TYPE_FLOAT)
    VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(self->distance) ;
#else
    VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(self->distance) ;
#endif
    TYPE * distances = vl_malloc (sizeof(TYPE) * numCenters) ;

    /* get a random permutation of the data point */
    for (i = 0 ; i < numData ; ++i) perm[i] = i ;
    _vl_kmeans_shuffle (perm, numData, rand) ;

    for (k = 0, i = 0 ; k < numCenters ; ++ i) {

      /* compare the next data point to all centers collected so far
       to detect duplicates (if there are enough left)
       */
      if (numCenters - k < numData - i) {
        vl_bool duplicateDetected = VL_FALSE ;
        VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)(distances,
                                                              dimension,
                                                              data + dimension * perm[i], 1,
                                                              (TYPE*)self->centers, k,
                                                              distFn) ;
        for (j = 0 ; j < k ; ++j) { duplicateDetected |= (distances[j] == 0) ; }
        if (duplicateDetected) continue ;
      }

      /* ok, it is not a duplicate so we can accept it! */
      memcpy ((TYPE*)self->centers + dimension * k,
              data + dimension * perm[i],
              sizeof(TYPE) * dimension) ;
      k ++ ;
    }
    vl_free(distances) ;
    vl_free(perm) ;
  }
}

/* ---------------------------------------------------------------- */
/*                                                 kmeans++ seeding */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_kmeans_seed_centers_plus_plus_, SFX)
(VlKMeans * self,
 TYPE const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numCenters)
{
  vl_uindex x, c ;
  VlRand * rand = vl_get_rand () ;
  TYPE * distances = vl_malloc (sizeof(TYPE) * numData) ;
  TYPE * minDistances = vl_malloc (sizeof(TYPE) * numData) ;
#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(self->distance) ;
#else
  VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(self->distance) ;
#endif

  self->dimension = dimension ;
  self->numCenters = numCenters ;
  self->centers = vl_malloc (sizeof(TYPE) * dimension * numCenters) ;

  for (x = 0 ; x < numData ; ++x) {
    minDistances[x] = (TYPE) VL_INFINITY_D ;
  }

  /* select the first point at random */
  x = vl_rand_uindex (rand, numData) ;
  c = 0 ;
  while (1) {
    TYPE energy = 0 ;
    TYPE acc = 0 ;
    TYPE thresh = (TYPE) vl_rand_real1 (rand) ;

    memcpy ((TYPE*)self->centers + c * dimension,
            data + x * dimension,
            sizeof(TYPE) * dimension) ;

    c ++ ;
    if (c == numCenters) break ;

    VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)
    (distances,
     dimension,
     (TYPE*)self->centers + (c - 1) * dimension, 1,
     data, numData,
     distFn) ;

    for (x = 0 ; x < numData ; ++x) {
      minDistances[x] = VL_MIN(minDistances[x], distances[x]) ;
      energy += minDistances[x] ;
    }

    for (x = 0 ; x < numData - 1 ; ++x) {
      acc += minDistances[x] ;
      if (acc >= thresh * energy) break ;
    }
  }

  vl_free(distances) ;
  vl_free(minDistances) ;
}

/* ---------------------------------------------------------------- */
/*                                                     Quantization */
/* ---------------------------------------------------------------- */

static void
VL_XCAT(_vl_kmeans_quantize_, SFX)
(VlKMeans * self,
 vl_uint32 * assignments,
 TYPE * distances,
 TYPE const * data,
 vl_size numData)
{
  vl_uindex i ;
#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(self->distance) ;
#else
  VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(self->distance) ;
#endif
  TYPE * distanceToCenters = vl_malloc (sizeof(TYPE) * self->numCenters) ;

  for (i = 0 ; i < numData ; ++i) {
    vl_size k ;
    TYPE bestDistance = (TYPE) VL_INFINITY_D ;
    VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)(distanceToCenters,
                                                          self->dimension,
                                                          data + self->dimension * i, 1,
                                                          (TYPE*)self->centers, self->numCenters,
                                                          distFn) ;
    for (k = 0 ; k < self->numCenters ; ++k) {
      if (distanceToCenters[k] < bestDistance) {
        bestDistance = distanceToCenters[k] ;
        assignments[i] = k ;
      }
    }

    if (distances) distances[i] = bestDistance ;
  }
  vl_free(distanceToCenters) ;
}

/* ---------------------------------------------------------------- */
/*                                                 Helper functions */
/* ---------------------------------------------------------------- */

/* The sorting routine is used to find increasing permutation of each
 * data dimension. This is used to quickly find the median for l1
 * distance clustering. */

VL_INLINE TYPE
VL_XCAT3(_vl_kmeans_, SFX, _qsort_cmp)
(VlKMeansSortWrapper * array, vl_uindex indexA, vl_uindex indexB)
{
  return
  ((TYPE*)array->data) [array->permutation[indexA] * array->stride]
  -
  ((TYPE*)array->data) [array->permutation[indexB] * array->stride] ;
}

VL_INLINE void
VL_XCAT3(_vl_kmeans_, SFX, _qsort_swap)
(VlKMeansSortWrapper * array, vl_uindex indexA, vl_uindex indexB)
{
  vl_uint32 tmp = array->permutation[indexA] ;
  array->permutation[indexA] = array->permutation[indexB] ;
  array->permutation[indexB] = tmp ;
}

#define VL_QSORT_prefix  VL_XCAT3(_vl_kmeans_, SFX, _qsort)
#define VL_QSORT_array   VlKMeansSortWrapper*
#define VL_QSORT_cmp     VL_XCAT3(_vl_kmeans_, SFX, _qsort_cmp)
#define VL_QSORT_swap    VL_XCAT3(_vl_kmeans_, SFX, _qsort_swap)
#include "qsort-def.h"

static void
VL_XCAT(_vl_kmeans_sort_data_helper_, SFX)
(VlKMeans * self, vl_uint32 * permutations, TYPE const * data, vl_size numData)
{
  vl_uindex d, x ;

  for (d = 0 ; d < self->dimension ; ++d) {
    VlKMeansSortWrapper array ;
    array.permutation = permutations + d * numData ;
    array.data = data + d ;
    array.stride = self->dimension ;
    for (x = 0 ; x < numData ; ++x) { array.permutation[x] = x ; }
    VL_XCAT3(_vl_kmeans_, SFX, _qsort_sort)(&array, numData) ;
  }
}

/* ---------------------------------------------------------------- */
/*                                                 Lloyd refinement */
/* ---------------------------------------------------------------- */

static double
VL_XCAT(_vl_kmeans_refine_centers_lloyd_, SFX)
(VlKMeans * self,
 TYPE const * data,
 vl_size numData)
{
  vl_size c, d, x, iteration ;
  vl_bool allDone ;
  double previousEnergy = VL_INFINITY_D ;
  double energy ;
  TYPE * distances = vl_malloc (sizeof(TYPE) * numData) ;
  vl_uint32 * assignments = vl_malloc (sizeof(vl_uint32) * numData) ;
  vl_size * clusterMasses = vl_malloc (sizeof(vl_size) * numData) ;
  vl_uint32 * permutations = NULL ;
  vl_size * numSeenSoFar = NULL ;

  if (self->distance == VlDistanceL1) {
    permutations = vl_malloc(sizeof(vl_uint32) * numData * self->dimension) ;
    numSeenSoFar = vl_malloc(sizeof(vl_size) * self->numCenters) ;
    VL_XCAT(_vl_kmeans_sort_data_helper_, SFX)(self, permutations, data, numData) ;
  }

  for (energy = VL_INFINITY_D,
       iteration = 0,
       allDone = VL_FALSE ;
       1 ;
       ++ iteration) {

    /* assign data to cluters */
    VL_XCAT(_vl_kmeans_quantize_, SFX)(self, assignments, distances, data, numData) ;

    /* compute energy */
    energy = 0 ;
    for (x = 0 ; x < numData ; ++x) energy += distances[x] ;
    if (self->verbosity) {
      VL_PRINTF("kmeans: Lloyd iter %d: energy = %g\n", iteration,
                energy) ;
    }

    /* check termination conditions */
    if (iteration >= self->maxNumIterations) {
      if (self->verbosity) {
        VL_PRINTF("kmeans: Lloyd terminating because maximum number of iterations reached\n") ;
      }
      break ;
    }
    if (energy == previousEnergy) {
      if (self->verbosity) {
        VL_PRINTF("kmeans: Lloyd terminating because the algorithm fully converged\n") ;
      }
      break ;
    }

    /* begin next iteration */
    previousEnergy = energy ;

    /* update clusters */
    memset(clusterMasses, 0, sizeof(vl_size) * numData) ;
    for (x = 0 ; x < numData ; ++x) {
      clusterMasses[assignments[x]] ++ ;
    }

    switch (self->distance) {
      case VlDistanceL2:
        memset(self->centers, 0, sizeof(TYPE) * self->dimension * self->numCenters) ;
        for (x = 0 ; x < numData ; ++x) {
          TYPE * cpt = (TYPE*)self->centers + assignments[x] * self->dimension ;
          TYPE const * xpt = data + x * self->dimension ;
          for (d = 0 ; d < self->dimension ; ++d) { cpt[d] += xpt[d] ; }
        }
        for (c = 0 ; c < self->numCenters ; ++c) {
          TYPE mass = clusterMasses[c] ;
          TYPE * cpt = (TYPE*)self->centers + c * self->dimension ;
          for (d = 0 ; d < self->dimension ; ++d) { cpt[d] /= mass ; }
        }
        break ;
      case VlDistanceL1:
        for (d = 0 ; d < self->dimension ; ++d) {
          vl_uint32 * perm = permutations + d * numData ;
          memset(numSeenSoFar, 0, sizeof(vl_size) * self->numCenters) ;
          for (x = 0; x < numData ; ++x) {
            c = assignments[perm[x]] ;
            if (2 * numSeenSoFar[c] < clusterMasses[c]) {
              ((TYPE*)self->centers) [d + c * self->dimension] =
              data [d + perm[x] * self->dimension] ;
            }
            numSeenSoFar[c] ++ ;
          }
        }
        break ;
      default:
        abort();
    } /* done compute centers */
  } /* next Lloyd iteration */

  if (permutations) { vl_free(permutations) ; }
  if (numSeenSoFar) { vl_free(numSeenSoFar) ; }
  vl_free(distances) ;
  vl_free(assignments) ;
  vl_free(clusterMasses) ;
  return energy ;
}

/* ---------------------------------------------------------------- */
/*                                                 Elkan refinement */
/* ---------------------------------------------------------------- */

static double
VL_XCAT(_vl_kmeans_update_center_distances_, SFX)
(VlKMeans * self)
{
#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(self->distance) ;
#else
  VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(self->distance) ;
#endif

  if (! self->centerDistances) {
    self->centerDistances = vl_malloc (sizeof(TYPE) *
                                       self->numCenters *
                                       self->numCenters) ;
  }
  VL_XCAT(vl_eval_vector_comparison_on_all_pairs_, SFX)(self->centerDistances,
                                                        self->dimension,
                                                        self->centers, self->numCenters,
                                                        NULL, 0,
                                                        distFn) ;
  return self->numCenters * (self->numCenters - 1) / 2 ;
}



static double
VL_XCAT(_vl_kmeans_refine_centers_elkan_, SFX)
(VlKMeans * self,
 TYPE const * data,
 vl_size numData)
{
  vl_size d, iteration, x ;
  vl_uint32 c, j ;
  vl_bool allDone ;
  TYPE * distances = vl_malloc (sizeof(TYPE) * numData) ;
  vl_uint32 * assignments = vl_malloc (sizeof(vl_uint32) * numData) ;
  vl_size * clusterMasses = vl_malloc (sizeof(vl_size) * numData) ;

#if (FLT == VL_TYPE_FLOAT)
    VlFloatVectorComparisonFunction distFn = vl_get_vector_comparison_function_f(self->distance) ;
#else
    VlDoubleVectorComparisonFunction distFn = vl_get_vector_comparison_function_d(self->distance) ;
#endif

  TYPE * nextCenterDistances = vl_malloc (sizeof(TYPE) * self->numCenters) ;
  TYPE * pointToClosestCenterUB = vl_malloc (sizeof(TYPE) * numData) ;
  vl_bool * pointToClosestCenterUBIsStrict = vl_malloc (sizeof(vl_bool) * numData) ;
  TYPE * pointToCenterLB = vl_malloc (sizeof(TYPE) * numData * self->numCenters) ;
  TYPE * newCenters = vl_malloc(sizeof(TYPE) * self->dimension * self->numCenters) ;
  TYPE * centerToNewCenterDistances = vl_malloc (sizeof(TYPE) * self->numCenters) ;

  vl_uint32 * permutations = NULL ;
  vl_size * numSeenSoFar = NULL ;

  double energy ;

  vl_size totDistanceComputationsToInit = 0 ;
  vl_size totDistanceComputationsToRefreshUB = 0 ;
  vl_size totDistanceComputationsToRefreshLB = 0 ;
  vl_size totDistanceComputationsToRefreshCenterDistances = 0 ;
  vl_size totDistanceComputationsToNewCenters = 0 ;
  vl_size totDistanceComputationsToFinalize = 0 ;

  if (self->distance == VlDistanceL1) {
    permutations = vl_malloc(sizeof(vl_uint32) * numData * self->dimension) ;
    numSeenSoFar = vl_malloc(sizeof(vl_size) * self->numCenters) ;
    VL_XCAT(_vl_kmeans_sort_data_helper_, SFX)(self, permutations, data, numData) ;
  }

  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /*                          Initialization                        */
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

  /* An iteration is: get_new_centers + reassign + get_energy.
     This counts as iteration 0, where get_new_centers is assumed
     to be performed before calling the train function by
     the initialization function */

  /* update distances between centers */
  totDistanceComputationsToInit +=
  VL_XCAT(_vl_kmeans_update_center_distances_, SFX)(self) ;

  /* assigmen points to the initial centers and initialize bounds */
  memset(pointToCenterLB, 0, sizeof(TYPE) * self->numCenters *  numData) ;
  for (x = 0 ; x < numData ; ++x) {
    TYPE distance ;

    /* do the first center */
    assignments[x] = 0 ;
    distance = distFn(self->dimension,
                      data + x * self->dimension,
                      (TYPE*)self->centers + 0) ;
    pointToClosestCenterUB[x] = distance ;
    pointToClosestCenterUBIsStrict[x] = VL_TRUE ;
    pointToCenterLB[0 + x * self->numCenters] = distance ;
    totDistanceComputationsToInit += 1 ;

    /* do other centers */
    for (c = 1 ; c < self->numCenters ; ++c) {

      /* Can skip if the center assigned so far is twice as close
         as its distance to the center under consideration */

      if (((self->distance == VlDistanceL1) ? 2.0 : 4.0) *
          pointToClosestCenterUB[x] <=
          ((TYPE*)self->centerDistances)
          [c + assignments[x] * self->numCenters]) {
        continue ;
      }

      distance = distFn(self->dimension,
                        data + x * self->dimension,
                        (TYPE*)self->centers + c * self->dimension) ;
      pointToCenterLB[c + x * self->numCenters] = distance ;
      totDistanceComputationsToInit += 1 ;
      if (distance < pointToClosestCenterUB[x]) {
        pointToClosestCenterUB[x] = distance ;
        assignments[x] = c ;
      }
    }
  }

  /* compute UB on energy */
  energy = 0 ;
  for (x = 0 ; x < numData ; ++x) {
    energy += pointToClosestCenterUB[x] ;
  }

  if (self->verbosity) {
    VL_PRINTF("kmeans: Elkan iter 0: energy = %g, dist. calc. = %d\n",
              energy, totDistanceComputationsToInit) ;
  }

/* #define SANITY*/
#ifdef SANITY
  {
    int xx ; int cc ;
    TYPE tol = 1e-5 ;
    VL_PRINTF("inconsistencies after initial assignments:\n");
    for (xx = 0 ; xx < numData ; ++xx) {
      for (cc = 0 ; cc < self->numCenters ; ++cc) {
        TYPE a = pointToCenterLB[cc + xx * self->numCenters] ;
        TYPE b = distFn(self->dimension,
                        data + self->dimension * xx,
                        (TYPE*)self->centers + self->dimension * cc) ;
        if (cc == assignments[xx]) {
          TYPE z = pointToClosestCenterUB[xx] ;
          if (z+tol<b) VL_PRINTF("UB %d %d = %f < %f\n",
                             cc, xx, z, b) ;
        }
        if (a>b+tol) VL_PRINTF("LB %d %d = %f  > %f\n",
                           cc, xx, a, b) ;
      }
    }
  }
#endif

  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /*                          Iterations                            */
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

  for (iteration = 1 ; 1; ++iteration) {

    vl_size numDistanceComputationsToRefreshUB = 0 ;
    vl_size numDistanceComputationsToRefreshLB = 0 ;
    vl_size numDistanceComputationsToRefreshCenterDistances = 0 ;
    vl_size numDistanceComputationsToNewCenters = 0 ;

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /*                         Compute new centers                  */
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    memset(clusterMasses, 0, sizeof(vl_size) * numData) ;
    for (x = 0 ; x < numData ; ++x) {
      clusterMasses[assignments[x]] ++ ;
    }

    switch (self->distance) {
      case VlDistanceL2:
        memset(newCenters, 0, sizeof(TYPE) * self->dimension * self->numCenters) ;
        for (x = 0 ; x < numData ; ++x) {
          TYPE * cpt = newCenters + assignments[x] * self->dimension ;
          TYPE const * xpt = data + x * self->dimension ;
          for (d = 0 ; d < self->dimension ; ++d) { cpt[d] += xpt[d] ; }
        }
        for (c = 0 ; c < self->numCenters ; ++c) {
          TYPE mass = clusterMasses[c] ;
          TYPE * cpt = newCenters + c * self->dimension ;
          for (d = 0 ; d < self->dimension ; ++d) { cpt[d] /= mass ; }
        }
        break ;
      case VlDistanceL1:
        for (d = 0 ; d < self->dimension ; ++d) {
          vl_uint32 * perm = permutations + d * numData ;
          memset(numSeenSoFar, 0, sizeof(vl_size) * self->numCenters) ;
          for (x = 0; x < numData ; ++x) {
            c = assignments[perm[x]] ;
            if (2 * numSeenSoFar[c] < clusterMasses[c]) {
              newCenters [d + c * self->dimension] =
              data [d + perm[x] * self->dimension] ;
            }
            numSeenSoFar[c] ++ ;
          }
        }
        break ;
      default:
        abort();
    } /* done compute centers */

    /* compute the distance from the old centers to the new centers */
    for (c = 0 ; c < self->numCenters ; ++c) {
      TYPE distance = distFn(self->dimension,
                             newCenters + c * self->dimension,
                             (TYPE*)self->centers + c * self->dimension) ;
      centerToNewCenterDistances[c] = distance ;
      numDistanceComputationsToNewCenters += 1 ;
    }

    /* make the new centers current */
    {
      TYPE * tmp = self->centers ;
      self->centers = newCenters ;
      newCenters = tmp ;
    }

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /*                Reassign points to a centers                  */
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    /*
     Update distances between centers.
     */
    numDistanceComputationsToRefreshCenterDistances
    += VL_XCAT(_vl_kmeans_update_center_distances_, SFX)(self) ;

    for (c = 0 ; c < self->numCenters ; ++c) {
      nextCenterDistances[c] = (TYPE) VL_INFINITY_D ;
      for (j = 0 ; j < self->numCenters ; ++j) {
        if (j == c) continue ;
        nextCenterDistances[c] = VL_MIN(nextCenterDistances[c],
                                        ((TYPE*)self->centerDistances)
                                        [j + c * self->numCenters]) ;
      }
    }

    /*
     Update upper bounds on point-to-closest-center distances
     based on the center variation.
     */
    for (x = 0 ; x < numData ; ++x) {
      TYPE a = pointToClosestCenterUB[x] ;
      TYPE b = centerToNewCenterDistances[assignments[x]] ;
      if (self->distance == VlDistanceL1) {
        pointToClosestCenterUB[x] = a + b ;
      } else {
#if (FLT == VL_TYPE_FLOAT)
        TYPE sqrtab =  sqrtf (a * b) ;
#else
        TYPE sqrtab =  sqrt (a * b) ;
#endif
        pointToClosestCenterUB[x] = a + b + 2.0 * sqrtab ;
      }
      pointToClosestCenterUBIsStrict[x] = VL_FALSE ;
    }

    /*
     Update lower bounds on point-to-center distances
     based on the center variation.
     */
    for (x = 0 ; x < numData ; ++x) {
      for (c = 0 ; c < self->numCenters ; ++c) {
        TYPE a = pointToCenterLB[c + x * self->numCenters] ;
        TYPE b = centerToNewCenterDistances[c] ;
        if (a < b) {
          pointToCenterLB[c + x * self->numCenters] = 0 ;
        } else {
          if (self->distance == VlDistanceL1) {
             pointToCenterLB[c + x * self->numCenters]  = a - b ;
          } else {
#if (FLT == VL_TYPE_FLOAT)
            TYPE sqrtab =  sqrtf (a * b) ;
#else
            TYPE sqrtab =  sqrt (a * b) ;
#endif
             pointToCenterLB[c + x * self->numCenters]  = a + b - 2.0 * sqrtab ;
          }
        }
      }
    }

   #ifdef SANITY
    {
      int xx ; int cc ;
      TYPE tol = 1e-5 ;
      VL_PRINTF("inconsistencies before assignments:\n");
      for (xx = 0 ; xx < numData ; ++xx) {
        for (cc = 0 ; cc < self->numCenters ; ++cc) {
          TYPE a = pointToCenterLB[cc + xx * self->numCenters] ;
          TYPE b = distFn(self->dimension,
                          data + self->dimension * xx,
                          (TYPE*)self->centers + self->dimension * cc) ;
          if (cc == assignments[xx]) {
            TYPE z = pointToClosestCenterUB[xx] ;
            if (z+tol<b) VL_PRINTF("UB %d %d = %f < %f\n",
                                            cc, xx, z, b) ;
          }
          if (a>b+tol) VL_PRINTF("LB %d %d = %f  > %f (assign = %d)\n",
                                          cc, xx, a, b, assignments[xx]) ;
        }
      }
    }
#endif

    /*
     Scan the data and to the reassignments. Use the bounds to
     skip as many point-to-center distance calculations as possible.
     */
    for (allDone = VL_TRUE, x = 0 ; x < numData ; ++x) {
      /*
       A point x sticks with its current center assignmets[x]
       the UB to d(x, c[assigmnets[x]]) is not larger than half
       the distance of c[assigments[x]] to any other center c.
       */
      if (((self->distance == VlDistanceL1) ? 2.0 : 4.0) *
          pointToClosestCenterUB[x] <= nextCenterDistances[assignments[x]]) {
        continue ;
      }

      for (c = 0 ; c < self->numCenters ; ++c) {
        vl_uint32 cx = assignments[x] ;
        TYPE distance ;

        /* The point is not reassigned to a given center c
         if either:

         0 - c is already the assigned center
         1 - The UB of d(x, c[assignments[x]]) is smaller than half
             the distance of c[assigments[x]] to c, OR
         2 - The UB of d(x, c[assignmets[x]]) is smaller than the
             LB of the distance of x to c.
         */
        if (cx == c) {
          continue ;
        }
        if (((self->distance == VlDistanceL1) ? 2.0 : 4.0) *
            pointToClosestCenterUB[x] <= ((TYPE*)self->centerDistances)
            [c + cx * self->numCenters]) {
          continue ;
        }
        if (pointToClosestCenterUB[x] <= pointToCenterLB
            [c + x * self->numCenters]) {
          continue ;
        }

        /* If the UB is loose, try recomputing it and test again */
        if (! pointToClosestCenterUBIsStrict[x]) {
          distance = distFn(self->dimension,
                            data + self->dimension * x,
                            (TYPE*)self->centers + self->dimension * cx) ;
          pointToClosestCenterUB[x] = distance ;
          pointToClosestCenterUBIsStrict[x] = VL_TRUE ;
          pointToCenterLB[cx + x * self->numCenters] = distance ;
          numDistanceComputationsToRefreshUB += 1 ;

          if (((self->distance == VlDistanceL1) ? 2.0 : 4.0) *
              pointToClosestCenterUB[x] <= ((TYPE*)self->centerDistances)
              [c + cx * self->numCenters]) {
            continue ;
          }
          if (pointToClosestCenterUB[x] <= pointToCenterLB
              [c + x * self->numCenters]) {
            continue ;
          }
        }

        /*
         Now the UB is strict (equal to d(x, assignments[x])), but
         we still could not exclude that x should be reassigned to
         c. We therefore compute the distance, update the LB,
         and check if a reassigmnet must be made
         */
        distance = distFn(self->dimension,
                          data + x * self->dimension,
                          (TYPE*)self->centers + c *  self->dimension) ;
        numDistanceComputationsToRefreshLB += 1 ;
        pointToCenterLB[c + x * self->numCenters] = distance ;

        if (distance < pointToClosestCenterUB[x]) {
          assignments[x] = c ;
          pointToClosestCenterUB[x] = distance ;
          allDone = VL_FALSE ;
          /* the UB strict flag is already set here */
        }

      } /* assign center */
    } /* next data point */

    totDistanceComputationsToRefreshUB
    += numDistanceComputationsToRefreshUB ;

    totDistanceComputationsToRefreshLB
    += numDistanceComputationsToRefreshLB ;

    totDistanceComputationsToRefreshCenterDistances
    += numDistanceComputationsToRefreshCenterDistances ;

    totDistanceComputationsToNewCenters
    += numDistanceComputationsToNewCenters ;

#ifdef SANITY
    {
      int xx ; int cc ;
      TYPE tol = 1e-5 ;
      VL_PRINTF("inconsistencies after assignments:\n");
      for (xx = 0 ; xx < numData ; ++xx) {
        for (cc = 0 ; cc < self->numCenters ; ++cc) {
          TYPE a = pointToCenterLB[cc + xx * self->numCenters] ;
          TYPE b = distFn(self->dimension,
                          data + self->dimension * xx,
                          (TYPE*)self->centers + self->dimension * cc) ;
          if (cc == assignments[xx]) {
            TYPE z = pointToClosestCenterUB[xx] ;
            if (z+tol<b) VL_PRINTF("UB %d %d = %f < %f\n",
                               cc, xx, z, b) ;
          }
          if (a>b+tol) VL_PRINTF("LB %d %d = %f  > %f (assign = %d)\n",
                             cc, xx, a, b, assignments[xx]) ;
        }
      }
    }
#endif

    /* compute UB on energy */
    energy = 0 ;
    for (x = 0 ; x < numData ; ++x) {
      energy += pointToClosestCenterUB[x] ;
    }

    if (self->verbosity) {
      vl_size numDistanceComputations =
      numDistanceComputationsToRefreshUB +
      numDistanceComputationsToRefreshLB +
      numDistanceComputationsToRefreshCenterDistances +
      numDistanceComputationsToNewCenters ;
      VL_PRINTF("kmeans: Elkan iter %d: energy <= %g, dist. calc. = %d\n",
                iteration,
                energy,
                numDistanceComputations) ;
      if (self->verbosity > 1) {
        VL_PRINTF("kmeans: Elkan iter %d: total dist. calc. per type: "
                  "UB: %.1f%% (%d), LB: %.1f%% (%d), "
                  "intra_center: %.1f%% (%d), "
                  "new_center: %.1f%% (%d)\n",
                  iteration,
                  100.0 * numDistanceComputationsToRefreshUB / numDistanceComputations,
                  numDistanceComputationsToRefreshUB,
                  100.0 *numDistanceComputationsToRefreshLB / numDistanceComputations,
                  numDistanceComputationsToRefreshLB,
                  100.0 * numDistanceComputationsToRefreshCenterDistances / numDistanceComputations,
                  numDistanceComputationsToRefreshCenterDistances,
                  100.0 * numDistanceComputationsToNewCenters / numDistanceComputations,
                  numDistanceComputationsToNewCenters) ;
      }
    }

    /* check termination conditions */
    if (iteration >= self->maxNumIterations) {
      if (self->verbosity) {
        VL_PRINTF("kmeans: Elkan terminating because maximum number of iterations reached\n") ;
      }
      break ;
    }
    if (allDone) {
      if (self->verbosity) {
        VL_PRINTF("kmeans: Elkan terminating because the algorithm fully converged\n") ;
      }
      break ;
    }

  } /* next Elkan iteration */


  /* compute true energy */
  energy = 0 ;
  for (x = 0 ; x < numData ; ++ x) {
    vl_uindex cx = assignments [x] ;
    energy += distFn(self->dimension,
                     data + self->dimension * x,
                     (TYPE*)self->centers + self->dimension * cx) ;
    totDistanceComputationsToFinalize += 1 ;
  }

  {
    vl_size totDistanceComputations =
    totDistanceComputationsToInit +
    totDistanceComputationsToRefreshUB +
    totDistanceComputationsToRefreshLB +
    totDistanceComputationsToRefreshCenterDistances +
    totDistanceComputationsToNewCenters +
    totDistanceComputationsToFinalize ;

    double saving = (double)totDistanceComputations
    / (iteration * self->numCenters * numData) ;

    if (self->verbosity) {
      VL_PRINTF("kmeans: Elkan: total dist. calc.: %d (%.2f %% of Lloyd)\n",
                totDistanceComputations, saving * 100.0) ;
    }

    if (self->verbosity > 1) {
      VL_PRINTF("kmeans: Elkan: total dist. calc. per type: "
                "init: %.1f%% (%d), UB: %.1f%% (%d), LB: %.1f%% (%d), "
                "intra_center: %.1f%% (%d), "
                "new_center: %.1f%% (%d), "
                "finalize: %.1f%% (%d)\n",
                100.0 * totDistanceComputationsToInit / totDistanceComputations,
                totDistanceComputationsToInit,
                100.0 * totDistanceComputationsToRefreshUB / totDistanceComputations,
                totDistanceComputationsToRefreshUB,
                100.0 *totDistanceComputationsToRefreshLB / totDistanceComputations,
                totDistanceComputationsToRefreshLB,
                100.0 * totDistanceComputationsToRefreshCenterDistances / totDistanceComputations,
                totDistanceComputationsToRefreshCenterDistances,
                100.0 * totDistanceComputationsToNewCenters / totDistanceComputations,
                totDistanceComputationsToNewCenters,
                100.0 * totDistanceComputationsToFinalize / totDistanceComputations,
                totDistanceComputationsToFinalize) ;
    }
  }

  if (permutations) { vl_free(permutations) ; }
  if (numSeenSoFar) { vl_free(numSeenSoFar) ; }

  vl_free(distances) ;
  vl_free(assignments) ;
  vl_free(clusterMasses) ;

  vl_free(nextCenterDistances) ;
  vl_free(pointToClosestCenterUB) ;
  vl_free(pointToClosestCenterUBIsStrict) ;
  vl_free(pointToCenterLB) ;
  vl_free(newCenters) ;
  vl_free(centerToNewCenterDistances) ;

  return energy ;
}

/* ---------------------------------------------------------------- */
static double
VL_XCAT(_vl_kmeans_refine_centers_, SFX)
(VlKMeans * self,
 TYPE const * data,
 vl_size numData)
{
  switch (self->algorithm) {
    case VlKMeansLLoyd:
      return
      VL_XCAT(_vl_kmeans_refine_centers_lloyd_, SFX)(self, data, numData) ;
      break ;
    case VlKMeansElkan:
      return
      VL_XCAT(_vl_kmeans_refine_centers_elkan_, SFX)(self, data, numData) ;
      break ;
    default:
      abort() ;
  }
}

/* VL_KMEANS_INSTANTIATING */
#else

#define FLT VL_TYPE_FLOAT
#define TYPE float
#define SFX f
#define VL_KMEANS_INSTANTIATING
#include "kmeans.c"

#define FLT VL_TYPE_DOUBLE
#define TYPE double
#define SFX d
#define VL_KMEANS_INSTANTIATING
#include "kmeans.c"

/* VL_KMEANS_INSTANTIATING */
#endif

/* ================================================================ */
#ifndef VL_KMEANS_INSTANTIATING

/** ------------------------------------------------------------------
 ** @brief Set centers
 ** @param self KMeans object.
 ** @param centers centers to copy.
 ** @param dimension data dimension.
 ** @param numCenters number of centers.
 **/

VL_EXPORT void
vl_kmeans_set_centers
(VlKMeans * self,
 void const * centers,
 vl_size dimension,
 vl_size numCenters)
{
  vl_kmeans_reset (self) ;

  switch (self->dataType) {
    case VL_TYPE_FLOAT :
      _vl_kmeans_set_centers_f
      (self, (float const *)centers, dimension, numCenters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_kmeans_set_centers_d
      (self, (double const *)centers, dimension, numCenters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief
 ** @param self
 ** @param self KMeans object.
 ** @param centers centers to copy.
 ** @param dimension data dimension.
 ** @param numData nmber of data points.
 ** @param numCenters number of centers.
 **
 ** The function seeds the KMeans centers by randomly sampling
 ** the data @a data.
 **/

VL_EXPORT void
vl_kmeans_seed_centers_with_rand_data
(VlKMeans * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numCenters)
{
  vl_kmeans_reset (self) ;

  switch (self->dataType) {
    case VL_TYPE_FLOAT :
      _vl_kmeans_seed_centers_with_rand_data_f
      (self, (float const *)data, dimension, numData, numCenters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_kmeans_seed_centers_with_rand_data_d
      (self, (double const *)data, dimension, numData, numCenters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief
 ** @param self
 ** @param data
 ** @param dimension
 ** @param numData
 ** @param numCenters
 **/

VL_EXPORT void
vl_kmeans_seed_centers_plus_plus
(VlKMeans * self,
 void const * data,
 vl_size dimension,
 vl_size numData,
 vl_size numCenters)
{
  vl_kmeans_reset (self) ;

  switch (self->dataType) {
    case VL_TYPE_FLOAT :
      _vl_kmeans_seed_centers_plus_plus_f
      (self, (float const *)data, dimension, numData, numCenters) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_kmeans_seed_centers_plus_plus_d
      (self, (double const *)data, dimension, numData, numCenters) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Quantize new data
 ** @param self KMeans object.
 ** @param assignments data to centers assignments.
 ** @param distances data to closes center distance/
 ** @param data data to quantize.
 ** @param numCenters number of data to quantize.
 **/

VL_EXPORT void
vl_kmeans_quantize
(VlKMeans * self,
 vl_uint32 * assignments,
 void * distances,
 void const * data,
 vl_size numData)
{
  switch (self->dataType) {
    case VL_TYPE_FLOAT :
      _vl_kmeans_quantize_f
      (self, assignments, distances, (float const *)data, numData) ;
      break ;
    case VL_TYPE_DOUBLE :
      _vl_kmeans_quantize_d
      (self, assignments, distances, (double const *)data, numData) ;
      break ;
    default:
      abort() ;
  }
}

/** ------------------------------------------------------------------
 ** @brief Refine center locations.
 ** @param self KMeans object.
 ** @param data data to quantize.
 ** @param numData number of data points.
 ** @return K-means energy at the end of optimization.
 **
 ** The function calls the underlying K-means quantization algorithm
 ** (@ref VlKMeansAlgorithm) to quantize the specified data @a data.
 ** The function assumes that the cluster centers have already
 ** been assigned by using one of the seeding functions, or by
 ** setting them.
 **/

VL_EXPORT double
vl_kmeans_refine_centers
(VlKMeans * self,
 void const * data,
 vl_size numData)
{
  assert (self->centers) ;

  switch (self->dataType) {
    case VL_TYPE_FLOAT :
      return
      _vl_kmeans_refine_centers_f
      (self, (float const *)data, numData) ;
    case VL_TYPE_DOUBLE :
      return
      _vl_kmeans_refine_centers_d
      (self, (double const *)data, numData) ;
    default:
      abort() ;
  }
}


/** ------------------------------------------------------------------
 ** @brief Cluster data.
 ** @param self KMeans object.
 ** @param data data to quantize.
 ** @param numData number of data points.
 ** @return K-means energy at the end of optimization.
 **
 ** The function calls the underlying K-means quantization algorithm
 ** (@ref VlKMeansAlgorithm) to quantize the specified data @a data.
 ** The function assumes that the cluster centers have already
 ** been assigned by using one of the seeding functions, or by
 ** setting them.
 **/

VL_EXPORT double
vl_kmeans_cluster (VlKMeans * self,
                   void const * data,
                   vl_size dimension,
                   vl_size numData,
                   vl_size numCenters)
{
  vl_uindex repetition ;
  double bestEnergy = VL_INFINITY_D ;
  void * bestCenters = NULL ;

  for (repetition = 0 ; repetition < self->numRepetitions ; ++ repetition) {
    double energy ;
    double timeRef ;

    if (self->verbosity) {
      VL_PRINTF("kmeans: repetition %d of %d\n", repetition + 1, self->numRepetitions) ;
    }

    timeRef = vl_get_cpu_time() ;
    switch (self->initialization) {
      case VlKMeansRandomSelection :
        vl_kmeans_seed_centers_with_rand_data (self,
                                               data, dimension, numData,
                                               numCenters) ;
        break ;
      case VlKMeansPlusPlus :
        vl_kmeans_seed_centers_plus_plus (self,
                                          data, dimension, numData,
                                          numCenters) ;
        break ;
      default:
        abort() ;
    }

    if (self->verbosity) {
      VL_PRINTF("kmeans: K-means initialized in %.2f s\n",
                vl_get_cpu_time() - timeRef) ;
    }

    timeRef = vl_get_cpu_time () ;
    energy = vl_kmeans_refine_centers (self, data, numData) ;
    if (self->verbosity) {
      VL_PRINTF("kmeans: K-means termineted in %.2f s with energy %g\n",
                vl_get_cpu_time() - timeRef, energy) ;
    }

    /* copy centers to output if current solution is optimal */
    if (energy < bestEnergy) {
      void * temp ;
      bestEnergy = energy ;

      if (bestCenters == NULL) {
        bestCenters = vl_malloc(vl_get_type_size(self->dataType) *
                                self->dimension *
                                self->numCenters) ;
      }

      /* swap buffers */
      temp = bestCenters ;
      bestCenters = self->centers ;
      self->centers = temp ;
    } /* better energy */
  } /* next repetition */

  vl_free (self->centers) ;
  self->centers = bestCenters ;
  return bestEnergy ;
}

/* VL_KMEANS_INSTANTIATING */
#endif

#undef SFX
#undef TYPE
#undef FLT
#undef VL_KMEANS_INSTANTIATING
