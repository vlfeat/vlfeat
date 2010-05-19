/** @file     pegasos.c
 ** @brief    PEGASOS SVM - Definition
 ** @author   Andrea Vedaldi
 **/

/* AUTORIGHTS
 Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available under the terms of the
 GNU GPLv2, or (at your option) any later version.
 */

/** @file pegasos.h

@ref pegasos.h provides basic impelmentation of the PEGASOS [1] SVM solver.

- @ref pegasos-overview Overview
- @ref pegasos-bias Bias
- @ref pegasos-kernels Non-linear kernels
- @ref pegasos-algorithm Algorithm
- @ref pegasos-references References

<!-- ------------------------------------------------------------ --->
@section pegasos-overview Overview
<!-- ------------------------------------------------------------ --->

PEGASOS solves the <em>linear</em> SVM learning problem

@f[
 \min_{w} \frac{\lambda}{2} \|w\|^2 + \frac{1}{m} \sum_{i=1}^n
 \ell(w; (x_i,y_i))
@f]

where @f$ x_i @f$ are data vectors in @f$ \mathbb{R}^d @f$, @f$ y_i \in
\{-1,1\} @f$ are binary labels,  @f$ \lambda > 0 @f$ is the
regularization parameter, and

@f[
\ell(w;(x_i,y_i) = \max\{0, 1 - y_i \langle w,x_i\rangle\}.
@f]

is the <em>hinge loss</em>. The result of the optimization is a model
@f$ w \in \mathbb{R}^d @f$ that yields the decision function

@f[ F(x) = \operatorname{sign} \langle w, x\rangle. @f]

It is well known that the hinge loss is a convex upper bound of the
01-loss of the decision function:

@f[
 \ell(w;(x,y)) \geq \frac{1}{2}(1 - y F(x)).
@f]

PEGASOS is accessed by calling ::vl_pegasos_train_binary_svm_d or
 ::vl_pegasos_train_binary_svm_f, operating respectively on @c double
 or @c float data.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-bias Bias
<!-- ------------------------------------------------------------ --->

PEGASOS SVM formulation does not incorporate a bias. To learn an SVM
with bias, the vector @f$ x @f$ can be optionally extended by a
constant component @f$ B @f$. In this case, the model @f$ w @f$ is
@f$d + 1 @f$ dimensional and the decision function is @f$ F(x) =
\operatorname{sign} (\langle w_{1:d}, x\rangle+ w_{d+1} B) @f$. If
<em>B</em> is large enough, the weight @f$ w_{d+1} @f$ remains small
and it has small contribution in the regularization term @f$ \| w \|^2
@f$; however, a large @f$ B @f$ makes the optimization harder.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-kernels Non-linear kernels
<!-- ------------------------------------------------------------ --->

PEGASOS can be extended to work with kernels, but the algorithm is not
particularly efficient in this setting. A preferable solution may be
to compute an explicit feature map representing the kernel.

Let @f$ k(x,y) @f$ be a kernel. A <em>feature map</em> is a function
@f$ \Psi(x) @f$ such that @f$ k(x,y) = \langle \Psi(x), \Psi(y)
\rangle @f$. Using this representation, that exists for any positive
definite kernel as long as one accepts infinite dimensional feature
maps, non-linear SVM learning writes:

@f[
 \min_{w} \frac{\lambda}{2} \|w\|^2 + \frac{1}{m} \sum_{i=1}^n
 \ell(w; (\Psi(x)_i,y_i)).
@f]

Thus the only difference with the linear case is that @f$ \Psi(x) @f$
is used in place of @f$ x @f$.

The ability of computing a feature-map representation depends on the
kernel. A general solution is to take the (incomplete) Cholesky
decomposition @f$ V^\top V @f$ of the kernel matrix @f$ K =
[k(x_i,x_j)] @f$ (in this case @f$ \Psi(x_i) @f$ is the <em>i</em>-th
columns of <em>V</em>). Alternatively, for additive kernels
(e.g. intersection, Chi2) @ref homkermap.h can be used.

<!-- ------------------------------------------------------------ --->
@subsection pegasos-algorithm Algorithm
<!-- ------------------------------------------------------------ --->

PEGASOS is a stochastic subgradient optmizer with automatically tuned
learning rate. At the <em>t</em>-iteration, the algorithm:

- Samples uniformly at random as subset @f$ A_t@f$ of <em>k</em> of
  training pairs @f$(x,y)@f$ from the <em>m</em> pairs provided.
- Compute a subgradient @f$ \nabla_t @f$ of the function @f$ E_t(w) =
  \frac{1}{2}\|w\|^2 + \frac{1}{k} \sum_{(x,y) \in A_t} \ell(w;(x,y)) @f$.
- Do a step @f$ w_{t+1/2} = w_t - \alpha_t \nalba_t @f$ with learning
  rate @f$  \alpha_t = 1/(\eta t) @f$.
- Back project on the hypersphere of radius @f$ \sqrt{\lambda} @f$ to obtain
  the next model estimate @f$ w_{t+1} @f$:
  @f[
    w_t = \min\{1, \sqrt{\lambda}/\|w\|\} w_{t+1/2}
  @f]

<!-- ------------------------------------------------------------ --->
@section pegasos-references References
<!-- ------------------------------------------------------------ --->

[1] S. Shalev-Shwartz, Y. Singer, and N. Srebro.
    <em>Pegasos: Primal estimated sub-GrAdient SOlver for SVM.</em>
    In Proc. ICML, 2007.
*/

/** @fn vl_pegasos_train_binary_svm_d(double*,double const*,vl_size,vl_size,vl_int8 const*,double,vl_size,double,VlRand*)
 ** @param model (out) the learned model.
 ** @param data training vectors.
 ** @param dimension data dimension.
 ** @param numSamples number of training data vectors.
 ** @param labels lables of the training vetctors.
 ** @param regularizer value of @f$ \lambda @f$.
 ** @param firstIteration number of the fist iteration.
 ** @param lastIteration number of the last iteration.
 ** @param biasMultiplier value of @f$ B @f$.
 ** @param randomGenerator random number generator.
 **
 ** The function runs PEGASOS on the specified data. The vector @a
 ** model must have either dimension equal to @a dimension if @a
 ** biasMultiplier is zero, or @a dimension + 1 if @a biasMultiplier
 ** is larger than zero.
 **
 ** The function runs PEGASOS for iterations <em>t</em> in the
 ** interval [@a fistIteration, @a lastIteration]. Together with the
 ** fact that the inital model can be set arbitrarily, this enable
 ** restaring PEGASOS from any point.
 **
 ** PEGASOS select the next point for computing the gradient at
 ** random. If @a randomGenerator is @c NULL, the default random
 ** generator (as returned by ::vl_get_rand()) is used.
 **
 ** See the @ref pegasos-overview overview for details.
 **/

/** @fn vl_pegasos_train_binary_svm_f(float*,float const*,vl_size,vl_size,vl_int8 const*,double,vl_size,double,VlRand*)
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
                                          VlRand * randomGenerator)
{
  vl_uindex iteration ;
  vl_uindex i ;
  T const * x ;
  T acc, eta, y, scale = 1 ;
  double lambda = regularizer ;
  double sqrtLambda = sqrt(lambda) ;


#if (FLT == VL_TYPE_FLOAT)
  VlFloatVectorComparisonFunction dotFn =
#else
  VlDoubleVectorComparisonFunction dotFn =
#endif
  VL_XCAT(vl_get_vector_comparison_function_,SFX)(VlKernelL2) ;

  if (randomGenerator == NULL) randomGenerator = vl_get_rand() ;

  assert(startingIteration >= 1) ;

  /*
     The model is stored as scale*model[]. When a sample does not violate
     the margin, only scale needs to be updated.
   */

  for (iteration = startingIteration ;
       iteration < startingIteration + numIterations ;
       ++ iteration) {
    /* pick a sample  */
    vl_uindex k = vl_rand_uindex(randomGenerator, numSamples) ;
    x = data + dimension * k ;
    y = labels[k] ;

    /* project on the weight vector */
    acc = dotFn(dimension, x, model) ;
    if (biasMultiplier) acc += biasMultiplier * model[dimension] ;
    acc *= scale ;

    /* learning rate */
    eta = 1.0 / (iteration * lambda) ;

    if (y * acc < (T) 1.0) {
      /* margin violated */
      T a = scale * (1 - eta * lambda)  ;
      T b = y * eta ;

      acc = 0 ;
      for (i = 0 ; i < dimension ; ++i) {
        model[i] = a * model[i] + b * x[i] ;
        acc += model[i] * model[i] ;
      }
      if (biasMultiplier) {
        model[dimension] = a * model[dimension] + b * biasMultiplier ;
        acc += model[dimension] * model[dimension] ;
      }
      scale = VL_MIN((T)1.0 / (sqrtLambda * sqrt(acc + VL_EPSILON_D)), (T)1.0) ;
    } else {
      /* margin not violated */
      scale *= 1 - eta * lambda ;
    }
  }

  /* denormalize representation */
  for (i = 0 ; i < dimension + (biasMultiplier ? 1 : 0) ; ++i) {
    model[i] *= scale ;
  }
}

/* VL_PEGAOS_INSTANTIATING */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif

