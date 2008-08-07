/** @file   imop.c
 ** @author Andrea Vedaldi
 ** @brief  Image operations - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "imop.h"

#include <assert.h>
#include <string.h>

/** @fn ::vl_convtransp_f(
 ** float*,float const*,float const*,int,int,int,int)
 **
 ** @brief Convolve along columns and take transpose
 **
 ** The function convolves the columns of the matrix @a src by the
 ** kernel @a filt and writes the transpose of the
 ** result to the buffer @a dst.
 **
 ** @remark Here `columns' correspond to the fastest varying
 ** index. Often image are stored in row-major order, so the `columns'
 ** are actually the `rows' of the image.
 **
 ** @remark To convolve an image by a 2-D separable filter just call
 ** this function twice.
 **
 ** @remark The function can be used to process N-dimensional arrays
 ** by stacking them properly.
 **
 ** @param dst        output image buffer.
 ** @param src        input  image buffer.
 ** @param filt       filter buffer.
 ** @param width      width  of the image.
 ** @param height     height of the image.
 ** @param filt_width filter half width (total width = 2 @c filt_width + 1).
 ** @param mode       mode flags.
 **/

/** @fn ::vl_imsmooth_f(
 ** float*,float*,float const*,int,int,double)
 **
 ** @brief Smooth image by Gaussian kernel
 **
 ** The function convolves the image @a src by a Gaussian kernel of
 ** variance @a sigma and write the result to the buffer @a dst. The
 ** functions also need a scratch buffer @a dst of the same size of
 ** the buffers @a src and @a dst.
 **
 ** The various functions differ by the data type of the image pixels
 ** that they process.
 **
 ** @param dst    output image buffer.
 ** @param temp   scratch image buffer.
 ** @param src    input image buffer.
 ** @param width  width of the buffers.
 ** @param height height of the buffers.
 ** @param sigma  standard deviation of the Gaussian kernel.
 **/

#define PIX float /**< pixel type. @internal */
#define FLT float /**< float type. @internal */
#define SFX f         /**< suffix.     @internal */
#include "imop.tc"
#undef PIX
#undef SFX
#undef FLT

#define PIX double
#define FLT double
#define SFX d
#include "imop.tc"
#undef PIX
#undef SFX
#undef FLT
