/** @file fisher.h
 ** @brief Fisher encoding (@ref fisher)
 ** @author David Novotny
 ** @author Andrea Vedaldi
 ** @see @ref fisher
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_FISHER_H
#define VL_FISHER_H

#include "generic.h"

/** @name Fisher vector options
 ** @{ */
#define VL_FISHER_FLAG_SQUARE_ROOT          (0x1 << 0)
#define VL_FISHER_FLAG_NORMALIZED           (0x1 << 1)
#define VL_FISHER_FLAG_IMPROVED             (VL_FISHER_FLAG_NORMALIZED|VL_FISHER_FLAG_SQUARE_ROOT)
#define VL_FISHER_FLAG_FAST                 (0x1 << 2)

/** @def VL_FISHER_FLAG_SQUARE_ROOT
 ** @brief Use signed squared-root.
 **/

/** @def VL_FISHER_FLAG_NORMALIZED
 ** @brief Gobally normalize the Fisher vector in L2 norm.
 **/

/** @def VL_FISHER_FLAG_IMPROVED
 ** @brief Improved Fisher vector.
 ** This is the same as @c VL_FISHER_FLAG_SQUARE_ROOT|VL_FISHER_FLAG_NORMALIZED.
 **/

/** @def VL_FISHER_FLAG_FAST
 ** @brief Fast but more approximate calculations
 ** Clamps to zero data to cluster assignments that are very small.
 ** Typically this makes the computation much faster.
 **/

/** @} */

VL_EXPORT void vl_fisher_encode
(void * enc, vl_type dataType,
 void const * means, vl_size dimension, vl_size numClusters,
 void const * covariances,
 void const * priors,
 void const * data, vl_size numData,
 int flags) ;

/* VL_FISHER_H */
#endif
