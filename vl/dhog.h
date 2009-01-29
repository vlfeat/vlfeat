/** @file     dhog.h
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (DHOG)
 **/

/* AUTORIGHTS
 Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
 
 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#ifndef VL_DHOG_H 
#define VL_DHOG_H 

#include "generic.h"

/** @brief DHOG keypoint */
typedef struct VlDhogKeypoint_
{
  double x ; /**< x coordinate */
  double y ; /**< y coordinate */
  double s ; /**< scale */
  double norm ; /**< norm */
} VlDhogKeypoint ;

/** @brief SIFT descriptor geometry */
typedef struct VlDhogDescriptorGeometry_
{
  int numBinT ;
  int numBinX ;
  int numBinY ;
  int binSizeX ;
  int binSizeY ;
} VlDhogDescriptorGeometry ;

/** @brief DHOG filter */
typedef struct VlDhogFilter_ 
{  
  int imWidth ;            /**< image width */
  int imHeight ;           /**< image height */

  int stepX ;
  int stepY ;

  int boundMinX ;
  int boundMinY ;
  int boundMaxX ;
  int boundMaxY ;
   
  VlDhogDescriptorGeometry geom ;
  int useFlatWindow ;

  int numFrames ;
  int descrSize ;
  VlDhogKeypoint *frames ; 
  float *descrs ;

  int numBinAlloc ;  
  int numFrameAlloc ;
  int numGradAlloc ;

  float **grads ;
  float *convTmp1 ;
  float *convTmp2 ;
}  VlDhogFilter ;

VL_EXPORT VlDhogFilter *vl_dhog_new (int width, int height) ;
VL_EXPORT VlDhogFilter *vl_dhog_new_basic (int width, int height, int step, int binSize) ;
VL_EXPORT void vl_dhog_delete (VlDhogFilter *self) ;
VL_EXPORT void vl_dhog_process (VlDhogFilter *self, float const* im) ;
VL_INLINE void vl_dhog_transpose_descriptor (float* dst, 
                                             float const* src,                                             
                                             int numBinT,
                                             int numBinX,
                                             int numBinY) ;

/** @name Setting parameters
 ** @{
 **/
VL_INLINE void vl_dhog_set_steps (VlDhogFilter *self,
                                  int stepX,
                                  int stepY) ;
VL_INLINE void vl_dhog_set_bounds (VlDhogFilter *self,
                                   int minX,
                                   int minY,
                                   int maxX,
                                   int maxY) ;
VL_INLINE void vl_dhog_set_geometry (VlDhogFilter *self,
                                     VlDhogDescriptorGeometry const* geom) ;
VL_INLINE void vl_dhog_set_flat_window (VlDhogFilter *self, vl_bool flatWindow) ;
/** @} */

/** @name Retrieving data and parameters
 ** @{
 **/
VL_INLINE float const    *vl_dhog_get_descriptors     (VlDhogFilter const *self) ;
VL_INLINE int             vl_dhog_get_descriptor_size (VlDhogFilter const *self) ;
VL_INLINE int             vl_dhog_get_keypoint_num    (VlDhogFilter const *self) ;
VL_INLINE VlDhogKeypoint const *vl_dhog_get_keypoints (VlDhogFilter const *self) ;
VL_INLINE void            vl_dhog_get_bounds          (VlDhogFilter const *self,
                                                       int* minX,
                                                       int* minY,
                                                       int* maxX,
                                                       int* maxY) ;
VL_INLINE void            vl_dhog_get_steps           (VlDhogFilter const*self, 
                                                       int* stepX, 
                                                       int* stepY) ;
VL_INLINE VlDhogDescriptorGeometry const* vl_dhog_get_geometry (VlDhogFilter const *self) ;
VL_INLINE vl_bool         vl_dhog_get_flat_window     (VlDhogFilter const *self) ;
/** @} */

VL_EXPORT
void _vl_dhog_update_buffers (VlDhogFilter *self) ;

/** ------------------------------------------------------------------
 ** @brief Get descriptor size.
 ** @param self DHOG filter.
 ** @return size of a descriptor.
 **/

int
vl_dhog_get_descriptor_size (VlDhogFilter const *self) 
{
  return self->descrSize ;
}

/** ------------------------------------------------------------------
 ** @brief Get descriptors.
 ** @param f DHOG filter.
 ** @return descriptors.
 **/

float const *
vl_dhog_get_descriptors (VlDhogFilter const *self)
{
  return self->descrs ;
}

/** ------------------------------------------------------------------
 ** @brief Get keypoints
 ** @param self DHOG filter.
 **/

VlDhogKeypoint const *
vl_dhog_get_keypoints (VlDhogFilter const *self)
{
  return self->frames ;
}

/** ------------------------------------------------------------------
 ** @brief Get number of keypoints
 ** @param self DHOG filter.
 **/

int
vl_dhog_get_keypoint_num (VlDhogFilter const *self)
{
  return self->numFrames ;
}

/** ------------------------------------------------------------------
 ** @brief Get SIFT descriptor geometry
 ** @param self DHOG filter.
 ** @param numBinT
 ** @param numBinX
 ** @parma numBinY
 ** @param binSizeX
 ** @param binSizeY
 **/

VlDhogDescriptorGeometry const* vl_dhog_get_geometry (VlDhogFilter const *self)
{
  return &self->geom ;
}

/** ------------------------------------------------------------------
 ** @brief Get bounds
 ** @param self DHOG filter.
 ** @param minX bounding box minimum X coordinate.
 ** @parma minY bounding box minimum Y coordinate.
 ** @param maxX bounding box maximum X coordinate.
 ** @param maxY bounding box maximum Y coordinate.
 **/

void
vl_dhog_get_bounds (VlDhogFilter const* self, 
                    int *minX, int *minY, int *maxX, int *maxY)
{
  *minX = self->boundMinX ;
  *minY = self->boundMinY ;
  *maxX = self->boundMaxX ;
  *maxY = self->boundMaxY ;
}

/** ------------------------------------------------------------------
 ** @brief Get flat window flag
 ** @param self DHOG filter.
 ** @return @c TRUE if the DHOG filter uses a flat window.
 **/

int
vl_dhog_get_flat_window (VlDhogFilter const* self)
{
  return self->useFlatWindow ;
}

/** ------------------------------------------------------------------
 ** @brief Get steps
 ** @param self DHOG filter.
 ** @param stepX sampling step along X.
 ** @param stepY sampling step along Y.
 **/

void
vl_dhog_get_steps (VlDhogFilter const* self, 
                   int* stepX,
                   int* stepY)
{
  *stepX = self->stepX ;
  *stepY = self->stepY ;
}

/** ------------------------------------------------------------------
 ** @brief Set steps
 ** @param self DHOG filter.
 ** @param stepX sampling step along X.
 ** @param stepY sampling step along Y.
 **/

void
vl_dhog_set_steps (VlDhogFilter* self, 
                   int stepX,
                   int stepY)
{
  self->stepX = stepX ;
  self->stepY = stepY ;
  _vl_dhog_update_buffers(self) ;
}

/** ------------------------------------------------------------------
 ** @brief Set bounds
 ** @param self DHOG filter.
 ** @param minX bounding box minimum X coordinate.
 ** @parma minY bounding box minimum Y coordinate.
 ** @param maxX bounding box maximum X coordinate.
 ** @param maxY bounding box maximum Y coordinate.
 **/

void
vl_dhog_set_bounds (VlDhogFilter* self, 
                    int minX, int minY, int maxX, int maxY)
{
  self->boundMinX = minX ;
  self->boundMinY = minY ;
  self->boundMaxX = maxX ;
  self->boundMaxY = maxY ;
  _vl_dhog_update_buffers(self) ;
}

/** ------------------------------------------------------------------
 ** @brief Set SIFT descriptor geometry
 ** @param self DHOG filter.
 ** @param geom descriptor geometry parameters.
 **/

void
vl_dhog_set_geometry (VlDhogFilter *self, 
                      VlDhogDescriptorGeometry const *geom)
{
  self->geom = *geom ;
  _vl_dhog_update_buffers(self) ;
}

/** ------------------------------------------------------------------
 ** @brief Set flat window flag
 ** @param self DHOG filter.
 ** @param useFilatWindow TRUE if the DHOG filter uses a flat window.
 **/

void
vl_dhog_set_flat_window (VlDhogFilter* self, 
                         int useFlatWindow)
{
  self->useFlatWindow = useFlatWindow ;
}

/** ------------------------------------------------------------------
 ** @brief Transpose descriptor
 **
 ** @param dst destination buffer.
 ** @param src source buffer.
 ** @param numBinT 
 ** @param numBinX
 ** @param numBinY
 **
 ** The function writes to @a dst the transpose of the SIFT descriptor
 ** @a src. Let <code>I</code> be an image. The transpose operator
 ** satisfies the equation <code>transpose(dhog(I,x,y)) =
 ** dhog(transpose(I),y,x)</code>
 **/

VL_INLINE void 
vl_dhog_transpose_descriptor (float* dst, 
                              float const* src,
                              int numBinT,
                              int numBinX,
                              int numBinY)
{
  int t, x, y ;
    
  for (y = 0 ; y < numBinY ; ++y) {
    for (x = 0 ; x < numBinX ; ++x) {
      int offset  = numBinT * (x + y * numBinX) ;
      int offsetT = numBinT * (y + x * numBinY) ;
      
      for (t = 0 ; t < numBinT ; ++t) {
        int tT = numBinT / 4 - t ;
        dst [offsetT + (tT + numBinT) % numBinT] = src [offset + t] ;
      }
    }
  }
}

/*  VL_DHOG_H */
#endif
