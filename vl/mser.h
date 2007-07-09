/** @file     mser.h
 ** @brief    Maximally Stable Extremal Regions (MSER) - Declaration
 ** @author   Andrea Vedaldi
 ** 
 ** Running the MSER filter usually involves the following steps:
 **
 ** - Initialize the MSER filter by ::vl_mser_new(). The
 **   filter can be reused if the image size does not change.
 ** - Call ::vl_mser_process() to compute the MSERs.
 ** - Delete the MSER filter by ::vl_mser_delete().
 **
 ** @section mser-volumetric Volumetric images
 **
 ** The code supports images of arbitrary dimension. For instance, it
 ** is possible to find the MSER regions of volumetric images.
 **/

/* AUTORIGHTS */

#ifndef VL_MSER
#define VL_MSER

#include "generic.h"

/** @brief MSER image data type 
 ** 
 ** This is the data type of the image pixels. It has to be an
 ** integer.
 **/
typedef vl_uint8 vl_mser_pix ;

/** @brief MSER Filter */
typedef struct _VlMserFilt VlMserFilt ;

VlMserFilt*      vl_mser_new     (int ndims, int const* dims) ;
void             vl_mser_delete  (VlMserFilt *filt) ;
void             vl_mser_process (VlMserFilt *filt, 
                                  vl_mser_pix const *im) ;
void             vl_mser_fit_ell (VlMserFilt *filt) ;

vl_uint          vl_mser_get_num_regions  (VlMserFilt const *filt) ;
vl_uint const*   vl_mser_get_regions      (VlMserFilt const *filt) ;
vl_single const* vl_mser_get_ell          (VlMserFilt const *filt) ;
vl_uint          vl_mser_get_num_ell      (VlMserFilt const *filt) ;
vl_uint          vl_mser_get_dof_ell      (VlMserFilt const *filt) ;

void  vl_mser_set_delta (VlMserFilt *filt,  vl_mser_pix x) ;
vl_mser_pix  vl_mser_get_delta (VlMserFilt const *filt) ;

/* VL_MSER */
#endif
