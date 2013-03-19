/** @file vlad.c
 ** @brief VLAD - Declaration
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page vlad VLAD encoding
@author David Novotny
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref vlad.h implements the calculation of VLAD encoding @cite{jegou10vlad}.

To get the VLAD vector siply run the function ::vl_vlad_encode
with correct parameters. The function can be applied to both
@c float or @c double data types.

@section vlad-tech Technical details

The VLAD (Vector of Linearly Aggregated Descriptors) encoding is bound
to estimation of a gaussian mixture model (see @ref gmm.h) or to
kmeans clustering (see @ref kmeans.h). Using the obtained soft (GMM) or hard (KMeans)
assignments and means one can encode a bunch of feature vectors with respect to the
estimated means and assignments.

Having a set of @f$ D @f$ dimensional features
@f$ x_1 ... x_N @f$,
a set of assignments
@f$ q_{1,1} ... q_{N,K}  @f$
and a set of means
@f$ \mu_1 ... \mu_K  @f$
the components @f$ v_{j} @f$ of VLAD encoding are defined as:
@f[
  v_{j} = \sum_{i=1}^{N} {  q_{i,j} (x_{i} - \mu_{j}) }
@f]

the <b>VLAD encoding</b> is an concatenation vector @f$ V @f$ of these components.
Thus the size of this encoding is @f$ D @f$ x @f$ K @f$.

@f[
  V = [ v_{1} ... v_{K} ]
@f]

However the size of each cluster could have a negative imapact on the
appearance of the vector @f$ V @f$ and so, the normalization of each
aggregate of vectors could be used:

@f[
  v_{j} = { \sum_{i=1}^{N} {q_{i,j} x_{i}}  \over { \sum_{i=1}^{N} {q_{i,j}} } } - \mu_{j}
@f]

This normalization is controlled by the last argument of ::vl_vlad_encode.


*/

#include "vlad.h"
#include "mathop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VL_VLAD_INSTANTIATING

#endif

#ifdef VL_VLAD_INSTANTIATING

static void
VL_XCAT(_vl_vlad_encode_, SFX)
(TYPE const * data,
 TYPE const * means,
 TYPE const * assignments,
 TYPE * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters,
 vl_bool normalize)
{
  vl_size dim;
  vl_size i_cl, i_d;
  TYPE * clusterMasses;

  clusterMasses = vl_malloc(numClusters*sizeof(TYPE));
  memset(clusterMasses,0,numClusters*sizeof(TYPE));

  for(i_cl = 0; i_cl < numClusters; i_cl++) {
    for(i_d = 0; i_d < numData; i_d++) {
      clusterMasses[i_cl] += assignments[i_d*numClusters+i_cl];
    }
  }

  memset(enc,0,sizeof(TYPE) * dimension * numClusters);

  for(i_cl = 0; i_cl < numClusters; i_cl++) {
    for(i_d = 0; i_d < numData; i_d++) {
      if(assignments[i_d*numClusters + i_cl] != 0) {
        for(dim = 0; dim < dimension; dim++) {
//          TYPE diff = assignments[i_cl*numData + i_d] *
//                      data [i_d  * dimension + dim] -
//                      means[i_cl * dimension + dim];
//          enc[i_cl*dimension + dim] += diff;
          enc[i_cl*dimension + dim] += assignments[i_d*numClusters + i_cl] *
                                       data [i_d  * dimension + dim];
        }
      }
    }

    if(clusterMasses[i_cl]!=0){
      if (normalize) {
        for(dim = 0; dim < dimension; dim++) {
          enc[i_cl*dimension + dim] /= clusterMasses[i_cl];
          enc[i_cl*dimension + dim] -= means[i_cl*dimension+dim];
        }
      } else {
        for(dim = 0; dim < dimension; dim++) {
          enc[i_cl*dimension + dim] -= clusterMasses[i_cl] * means[i_cl*dimension+dim];
        }
      }
    }
  }
}

/* VL_FISHER_INSTANTIATING */
#else

#ifndef __DOXYGEN__
#define FLT VL_TYPE_FLOAT
#define TYPE float
#define SFX f
#define VL_VLAD_INSTANTIATING
#include "vlad.c"

#define FLT VL_TYPE_DOUBLE
#define TYPE double
#define SFX d
#define VL_VLAD_INSTANTIATING
#include "vlad.c"
#endif

#endif

/* ================================================================ */
#ifndef VL_VLAD_INSTANTIATING

/** @brief Calculates vlad vector for given dataset
 ** @param dataType the type of the input data(VL_TYPE_DOUBLE or VL_TYPE_FLOAT)
 ** @param assignments probabilities of data points belonging to a specific cluster
 ** @param enc output VLAD encoding
 ** @param dimension dimensionality of the data
 ** @param numData number of data vectors
 ** @param numClusters number of gaussians in the mixture
 **
 ** The format of assignments is
 ** [ p(c1|x1), p(c1|x2), ... p(c1|xN),
 **   p(c2|x1) ...            p(c2|xN),
 **   ...
 **   p(cK|x1), ...           p(cK|xN) ]
 **
 ** where p(c_i|x_i) stands for a probability of point x_i belonging to a cluster c_i
 ** if hard assignments are used simply put to the corresponding p(c_i|x_i) fields
 ** 1 (assigned) or 0 (not assigned)
 **
 **/

VL_EXPORT void
vl_vlad_encode
(vl_type dataType,
 void const * data,
 void const * means,
 void const * assignments,
 void * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters,
 vl_bool normalize)
{
  switch(dataType) {
    case VL_TYPE_FLOAT:
      _vl_vlad_encode_f
      ((float const *) data,
       (float const *) means,
       (float const *) assignments,
       (float *) enc,
       dimension,
       numData,
       numClusters,
       normalize);
      break;
    case VL_TYPE_DOUBLE:
      _vl_vlad_encode_d
      ((double const *) data,
       (double const *) means,
       (double const *) assignments,
       (double *) enc,
       dimension,
       numData,
       numClusters,
       normalize);
      break;
    default:
      abort();
  }
}

#endif

#undef SFX
#undef TYPE
#undef FLT
#undef VL_VLAD_INSTANTIATING
