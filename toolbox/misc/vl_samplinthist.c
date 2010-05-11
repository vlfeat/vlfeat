/** @internal
 ** @file    samplinthist.c
 ** @author  Andrea Vedaldi
 ** @brief   Sample  an integral histogram - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/generic.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#undef T
#undef SFX
#define T vl_int32
#define SFX i
#include "samplinthist.tc"

#undef T
#undef SFX
#define T vl_uint32
#define SFX ui
#include "samplinthist.tc"

#undef T
#undef SFX
#define T double
#define SFX d
#include "samplinthist.tc"

#undef T
#undef SFX
#define T float
#define SFX f
#include "samplinthist.tc"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  mwSize dims [3] ;
  int numDims ;
  mwSize const * dimsPt = 0 ;

  void* histPt = 0 ;
  void const* intHistPt = 0 ;
  vl_uint32 const*  boxesPt = 0 ;
  size_t     numLabels = 0 ;
  mxClassID  histClass = mxUINT32_CLASS ;
  size_t     numBoxes  = 0 ;
  int width, height ;

  enum {IN_INTHIST = 0, IN_BOXES, IN_END} ;
  enum {OUT_HIST = 0} ;

  /* ------------------------------------------------------------------
  **                                                Check the arguments
  ** --------------------------------------------------------------- */
  if (nin != 2) {
    vlmxError(vlmxErrInvalidArgument,
             "Two arguments required.") ;
  } else if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument,
             "Too many output arguments.");
  }

  histClass = mxGetClassID(in[IN_INTHIST]) ;
  if (histClass != mxDOUBLE_CLASS &&
      histClass != mxUINT32_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
             "INTHIST must be of either class DOUBLE or UINT32.") ;
  }

  numDims = mxGetNumberOfDimensions(in[IN_INTHIST]) ;
  if (numDims > 3) {
    vlmxError(vlmxErrInvalidArgument,
             "INTHIST must be a MxNxK array.") ;
  }
  intHistPt = mxGetData(in[IN_INTHIST]) ;

  dimsPt = mxGetDimensions(in[IN_INTHIST]) ;
  height    = dimsPt [0] ;
  width     = dimsPt [1] ;
  numLabels = (numDims >= 3) ? dimsPt [2] : 1 ;

  numBoxes = mxGetNumberOfElements(in[IN_BOXES]) ;
  if (numBoxes % 4 != 0) {
    vlmxError(vlmxErrInvalidArgument,
             "The number of elements of BOXES must be a multiple of four.") ;
  }
  numBoxes /= 4 ;

  if (mxGetClassID(in[IN_BOXES]) != mxUINT32_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
             "BOXES must be of class UINT32.") ;
  }
  boxesPt = (vl_uint32*) mxGetData(in[IN_BOXES]) ;

  /* Allocate space for the result */
  dims [0] = numLabels ;
  dims [1] = numBoxes ;
  out [OUT_HIST] = mxCreateNumericArray(2, dims, histClass, mxREAL) ;
  histPt = mxGetData(out[OUT_HIST]) ;

  /* ------------------------------------------------------------------
   *                                          Sample integral histogram
   * --------------------------------------------------------------- */

#define PROCESS(SAMPLE, T)                      \
  SAMPLE (histPt,                               \
          intHistPt, height, width, numLabels,  \
          boxesPt, numBoxes) ;

  switch (histClass) {
  case mxDOUBLE_CLASS: { PROCESS(sample_d,  double)    } ; break ;
  case mxUINT32_CLASS: { PROCESS(sample_ui, vl_uint32) } ; break ;
  default:
    abort() ;
  }
}
