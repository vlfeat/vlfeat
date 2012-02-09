/** @file lbp.h
 ** @brief Local Binary Patterns
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include "generic.h"

/** @brief Type of quantization for LBP features */
typedef enum _VlLbpMappingType
{
  VlLbpUniform     /**< Uniform patterns */
} VlLbpMappingType ;

/** @brief Local Binary Pattern extractor */
typedef struct VlLbp_
{
  vl_size dimension ;
  vl_uint8 mapping [256] ;
  vl_bool transposed ;
} VlLbp ;

VL_EXPORT VlLbp * vl_lbp_new(VlLbpMappingType type, vl_bool transposed) ;
VL_EXPORT void vl_lbp_delete(VlLbp * self) ;
VL_EXPORT void vl_lbp_process (VlLbp * self,
                               float * features,
                               float * image, vl_size width, vl_size height,
                               vl_size cellSize) ;


/** @brief Get the dimension of the LBP histograms
 ** @return dimension of the LBP histograms.
 ** The dimension depends on the type of quantization used.
 ** @see ::vl_lbp_new().
 **/

VL_INLINE vl_size vl_lbp_get_dimension(VlLbp * self)
{
  return self->dimension ;
}
