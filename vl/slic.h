/** @file slic.h
 ** @brief SLIC superpixels (see @ref slic)
 ** @author Andrea Vedaldi
 **/

#ifndef VL_SLIC
#define VL_SLIC

#include "generic.h"

VL_EXPORT
void vl_slic_segment (vl_uint32 * segmentation,
                      float const * image,
                      vl_size width,
                      vl_size height,
                      vl_size numChannels,
                      vl_size regionSize,
                      float regularization,
                      vl_size minRegionSize) ;

/* VL_SLIC */
#endif
