/** @file pegasos.c
 ** @brief PEGASOS - Definition
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/**
<!-- ------------------------------------------------------------- -->
@page pegasos PEGASOS SVM solver
@author Andrea Vedaldi
<!-- ------------------------------------------------------------- -->

@ref pegasos.h provides a basic implementation of the PEGASOS
@cite{shalev-shwartz07pegasos} linear SVM solver.

- @ref pegasos-overview "Overview"
  - @ref pegasos-algorithm "Algorithm"
  - @ref pegasos-bias "Bias"
  - @ref pegasos-restarting "Restarting"
  - @ref pegasos-kernels "Non-linear kernels"

<!-- ------------------------------------------------------------ --->
@section pegasos-overview Overview
<!-- ------------------------------------------------------------ --->

PEGASOS solves the <em>linear</em> SxVM learning problem

@f[
 \min_{w} \frac{\lambda}{2} \|w\|^2 + \frac{1}{m} \sum_{i=1}^n
 \ell(w; (x_i,y_i))
@f]

where @f$ x_i @f$ are data vectors in @f$ \mathbb{R}^d @f$, @f$ y_i
\in \{-1,1\} @f$ are binary labels, @f$ \lambda > 0 @f$ is the
regularization parameter, and

@f[
\ell(w;(x_i,y_i)) = \max\{0, 1 - y_i \langle w,x_i\rangle\}.
@f]

is the <em>hinge loss</em>. The result of the optimization is a model
@f$ w \in \mathbb{R}^d @f$ that yields the decision function

@f[ F(x) = \operatorname{sign} \langle w, x\rangle. @f]

It is well known that the hinge loss is a convex upper bound of the
i01-loss of the decision function:

@f[
 \ell(w;(x,y)) \geq \frac{1}{2}(1 - y F(x)).
@f]

PEGASOS is accessed by calling ::vl_pegasos_train_binary_svm_d or
 ::vl_pegasos_train_binary_svm_f, operating respectively on @c double
 or @c float data.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-algorithm Algorithm
<!-- ------------------------------------------------------------ --->

PEGASOS @cite{shalev-shwartz07pegasos} is a stochastic subgradient
optimizer. At the <em>t</em>-th iteration the algorithm:

- Samples uniformly at random as subset @f$ A_t@f$ of <em>k</em> of
  training pairs @f$(x,y)@f$ from the <em>m</em> pairs provided for
  training (this subset is called mini batch).
- Computes a subgradient @f$ \nabla_t @f$ of the function @f$ E_t(w) =
  \frac{1}{2}\|w\|^2 + \frac{1}{k} \sum_{(x,y) \in A_t} \ell(w;(x,y))
  @f$ (this is the SVM objective function restricted to the
  minibatch).
- Compute an intermediate weight vector @f$ w_{t+1/2} @f$ by doing a
  step @f$ w_{t+1/2} = w_t - \alpha_t \nalba_t @f$ with learning rate
  @f$ \alpha_t = 1/(\eta t) @f$ along the subgradient. Note that the
  learning rate is inversely proportional to the iteration numeber.
- Back projects the weight vector @f$ w_{t+1/2} @f$ on the
  hypersphere of radius @f$ \sqrt{\lambda} @f$ to obtain the next
  model estimate @f$ w_{t+1} @f$:
  @f[
     w_t = \min\{1, \sqrt{\lambda}/\|w\|\} w_{t+1/2}.
  @f]
  The hypersfere is guaranteed to contain the optimal weight vector
  @f$ w^* @f$ @cite{shalev-shwartz07pegasos}.

VLFeat implementation fixes to one the size of the mini batches @f$ k
@f$.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-bias Bias
<!-- ------------------------------------------------------------ --->

PEGASOS SVM formulation does not incorporate a bias. To learn an SVM
with bias, the each data vector @f$ x @f$ can be extended by a
constant component @f$ B @f$ (called @c biasMultiplier in the
code). In this case, the model @f$ w @f$ has dimension @f$ D + 1 @f$
and the SVM discriminat function is given by @f$ F(x) =
\operatorname{sign} (\langle w_{1:d}, x\rangle+ w_{d+1} B) @f$. If the
bias multiplier <em>B</em> is large enough, the weight @f$ w_{d+1} @f$
remains small and it has small contribution in the SVM regularization
term @f$ \| w \|^2 @f$, better approximating the case of an SVM with
bias. Unfortunately, setting the bias multiplier @f$ B @f$ to a large
value makes the optimization harder.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-restarting Restarting
<!-- ------------------------------------------------------------ --->

VLFeat PEGASOS implementation can be restatred after any given number
of iterations. This is useful to compute intermediate statistics or to
load new data from disk for large datasets.  The state of the
algorithm, which is required for restarting, is limited to the current
estimate @f$ w_t @f$ of the SVM weight vector and the iteration number
@f$ t @f$.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-permutation Permutation
<!-- ------------------------------------------------------------ --->

VLFeat PEGASOS can use a user-defined permutation to decide the order
in which data points are visited (instead of using random
sampling). By specifying a permutation the algorithm is guaranteed to
visit each data point exactly once in each loop. The permutation needs
not to be bijective. This can be used to visit certain data samples
more or less often than others, implicitly reweighting their relative
importance in the SVM objective function. This can be used to blanace
the data.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-kernels Non-linear kernels
<!-- ------------------------------------------------------------ --->

PEGASOS can be extended to non-linear kernels, but the algorithm is
not particularly efficient in this setting [1]. When possible, it may
be preferable to work with explicit feature maps.

Let @f$ k(x,y) @f$ be a positive definite kernel. A <em>feature
map</em> is a function @f$ \Psi(x) @f$ such that @f$ k(x,y) = \langle
\Psi(x), \Psi(y) \rangle @f$. Using this representation the non-linear
SVM learning objective function writes:

@f[
 \min_{w} \frac{\lambda}{2} \|w\|^2 + \frac{1}{m} \sum_{i=1}^n
 \ell(w; (\Psi(x)_i,y_i)).
@f]

Thus the only difference with the linear case is that the feature @f$
\Psi(x) @f$ is used in place of the data @f$ x @f$.

@f$ \Psi(x) @f$ can be learned off-line, for instance by using the
incomplete Cholesky decomposition @f$ V^\top V @f$ of the Gram matrix
@f$ K = [k(x_i,x_j)] @f$ (in this case @f$ \Psi(x_i) @f$ is the
<em>i</em>-th columns of <em>V</em>). Alternatively, for additive
kernels (e.g. intersection, Chi2) the explicit feature map computed by
@ref homkermap.h can be used.

*/

/** @fn vl_pegasos_train_binary_svm_d(double*,double const*,vl_size,vl_size,vl_int8 const*,double,double,vl_uindex,vl_size,VlRand*,vl_uint32 const*,vl_size,double const*)
 ** @param model (out) the learned model.
 ** @param data training vectors.
 ** @param dimension data dimension.
 ** @param numSamples number of training data vectors.
 ** @param labels labels of the training vectors.
 ** @param regularizer value of the regularizer coefficient @f$ \lambda @f$.
 ** @param biasMultiplier value of the bias multiplier @f$ B @f$.
 ** @param startingIteration number of the first iteration.
 ** @param numIterations number of iterations to perform.
 ** @param randomGenerator random number generator.
 ** @param permutation order in which the data is accessed.
 ** @param permutationSize length of @c permutation.
 ** @param preconditioner diagonal precoditioner.
 **
 ** The function runs PEGASOS on the specified data. The vector @a
 ** model must have either dimension equal to @a dimension if @a
 ** biasMultiplier is zero, or @a dimension + 1 if @a biasMultiplier
 ** is larger than zero.
 **
 ** The function runs PEGASOS for iterations <em>t</em> in the
 ** interval [@a fistIteration, @a lastIteration]. Together with the
 ** fact that the initial model can be set arbitrarily, this enable
 ** restarting PEGASOS from any point.
 **
 ** PEGASOS select the next point for computing the gradient at
 ** random. If @a randomGenerator is @c NULL, the default random
 ** generator (as returned by ::vl_get_rand()) is used.
 **
 ** Alternatively, if @a permutation is not @c NULL, then points are
 ** sampled in the order specified by this vector of indexes (this is
 ** cycled through). In this way It is an error to set both @a
 ** randomGenerator and @a permutation to non-null values.
 **
 ** @c preconditioner specifies a diagonal preconditioner for the
 ** minimization problem (it is often useful to slow down the steps
 ** for the bias term, if the latter is used). Set @c preconditioner to NULL
 ** to avoid using a preconditioner. The precodnitioner should have the
 ** same dimension of the model, plus one if an SVM with bias is learned.
 **
 ** See the @ref pegasos-overview overview for details.
 **/

/** @fn vl_pegasos_train_binary_svm_f(float*,float const*,vl_size,vl_size,vl_int8 const*,double,double,vl_uindex,vl_size,VlRand*,vl_uint32 const*,vl_size,float const*)
 ** @see ::vl_pegasos_train_binary_svm_d
 **/

#ifndef VL_PEGASOS_INSTANTIATING

#include "pegasos.h"
#include "mathop.h"
#include <math.h>

#define FLT VL_TYPE_FLOAT
#define VL_PEGASOS_INSTANTIATING
#include "pegasos.c"

#define FLT VL_TYPE_DOUBLE
#define VL_PEGASOS_INSTANTIATING
#include "pegasos.c"

/* VL_PEGAOS_INSTANTIATING */
#else

#include "float.th"

VL_EXPORT void
VL_XCAT(vl_pegasos_train_binary_svm_,SFX)(T *  model,
                                          T const * data,
                                          vl_size dimension,
                                          vl_size numSamples,
                                          vl_int8 const * labels,
                                          double regularizer,
                                          double biasMultiplier,
                                          vl_uindex startingIteration,
                                          vl_size numIterations,
                                          VlRand * randomGenerator,
                                          vl_uint32 const * permutation,
                                          vl_size permutationSize,
                                          T const * preconditioner)
{
  vl_uindex iteration ;
  vl_uindex iteration0 ;
  vl_uindex i ;
  T const * x ;
  T acc, learningRate, y ;
  double lambda = regularizer ;
  vl_size const regularizationPeriod = 10 ;

#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction dotFn =
#else
  VlDoubleVectorComparisonFunction dotFn =
#endif
  VL_XCAT(vl_get_vector_comparison_function_,SFX)(VlKernelL2) ;

  if (randomGenerator == NULL && permutation == NULL) {
    randomGenerator = vl_get_rand() ;
  }

  assert(lambda > 0.0) ;
  assert(randomGenerator == NULL || permutation == NULL) ;
  assert(startingIteration >= 1) ;

  /*
   Choose iteration0 to start with small enoguh steps. Recall that
   the learning rate is

   learningRate = 1 / (lambda * (iteration + iteration0))

   */

  iteration0 = (vl_uindex) 1.0 / lambda ;

  /*
   The model is stored as scale*model[]. When a sample does not violate
   the margin, only scale needs to be updated.
   */

  for (iteration = startingIteration ;
       iteration < startingIteration + numIterations ;
       ++ iteration) {
    /* pick a sample  */
    vl_uindex k ;
    if (permutation == NULL) {
      k = vl_rand_uindex(randomGenerator, numSamples) ;
    } else {
      k = permutation[iteration % permutationSize] ;
      assert(k < numSamples) ;
    }

    x = data + dimension * k ;
    y = labels[k] ;

    /* compute learning rate */
    learningRate = 1.0 / ((iteration + iteration0) * lambda) ;

    /* regularizer step ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    if (iteration % regularizationPeriod == 0) {
      T eta = (T) learningRate * regularizationPeriod * lambda ;
      if (preconditioner) {
        for (i = 0 ; i < dimension + (biasMultiplier != 0) ; ++i) {
          model[i] -= eta * preconditioner[i] * model[i] ;
        }
      } else {
        for (i = 0 ; i < dimension + (biasMultiplier != 0) ; ++i) {
          model[i] -= eta * model[i] ;
        }
      }
    }

    /* loss step ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    acc = dotFn(dimension, x, model) ;
    if (biasMultiplier) acc += biasMultiplier * model[dimension] ;

    if (y * acc < (T) 1.0) {
      T eta = y * learningRate ;

      acc = 0 ;
      if (preconditioner) {
        for (i = 0 ; i < dimension ; ++i) {
          model[i] += eta * preconditioner[i] * x[i] ;
        }
        if (biasMultiplier) {
          model[dimension] += eta * preconditioner[dimension] * biasMultiplier ;
        }
      } else {
        for (i = 0 ; i < dimension ; ++i) {
          model[i] += eta * x[i] ;
        }
        if (biasMultiplier) {
          model[dimension] += eta * biasMultiplier ;
        }
      }
    }
  }
}

/* VL_PEGAOS_INSTANTIATING */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif
