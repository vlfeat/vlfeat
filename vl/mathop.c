/** @file    mathop.c
 ** @brief   Math operations - Definition
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "mathop.h"
#include "mathop_sse2.h"

/** @file mathop.h
 **
 ** @fn vl_get_vector_comparison_function_f
 ** @brief Get float vector comparison function from comparison type
 ** @param type vector comparison type.
 ** @return comparison function.
 **/

/** @fn vl_get_vector_comparison_function_d
 ** @brief Get double vector comparison function from comparison type
 ** @param type vector comparison type.
 ** @return comparison function.
 **/

/** @{ */

/** @fn vl_eval_vector_comparison_on_all_pairs_d(double*,vl_size,
 **     double const*,vl_size,double const*,vl_size,VlDoubleVectorComparisonFunction)
 **/

/** @fn vl_eval_vector_comparison_on_all_pairs_f(float*,vl_size,
 **     float const*,vl_size,float const*,vl_size,VlFloatVectorComparisonFunction)
 **
 ** @param result comparison matrix (output).
 ** @param dimension number of vector components (rows of @a X and @a Y).
 ** @param X data matrix X.
 ** @param Y data matrix Y.
 ** @param numDataX number of vectors in @a X (columns of @a X)
 ** @param numDataY number of vectros in @a Y (columns of @a Y)
 ** @param comparison vector comparison function.
 **
 ** The function evaluates @a function on all pairs of columns
 ** from matrices @a X and @a Y, filling a @a numDataX by @a numDataY
 ** matrix.
 **
 ** If @a Y is a null pointer the function compares all columns from
 ** @a X with themselves.
 **/

/** @} */

#undef FLT
#define FLT VL_TYPE_FLOAT
#include "mathop.tc"

#undef FLT
#define FLT VL_TYPE_DOUBLE
#include "mathop.tc"


