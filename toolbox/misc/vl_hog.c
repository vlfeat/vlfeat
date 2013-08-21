/** @file vl_hog.c
 ** @brief vl_hog MEX definition.
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/hog.h>

/* option codes */
enum {
  opt_verbose, opt_variant,
  opt_num_orientations,
  opt_directed_polar_field,
  opt_undirected_polar_field,
  opt_bilinear_orientations
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",              0,   opt_verbose                      },
  {"Variant",              1,   opt_variant                      },
  {"NumOrientations",      1,   opt_num_orientations             },
  {"DirectedPolarField",   0,   opt_directed_polar_field         },
  {"UndirectedPolarField", 0,   opt_undirected_polar_field       },
  {"BilinearOrientations", 0,   opt_bilinear_orientations        },
  {0,                      0,   0                                }
} ;

enum Mode {ExtractFeatures, Render, GetPermutation} ;
enum InputType {Image, DirectedPolarField, UndirectedPolarField} ;

static char const * inputTypeNames [] = {
  "Image",
  "DirectedPolarField",
  "UndirectedPolarField"
} ;

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  mxArray const * descriptor_array ;
  float const * descriptor = NULL ;
  float const * image = NULL ;
  vl_size width = 0, height = 0, numChannels = 0 ;
  vl_size cellSize = 16 ;
  vl_size numOrientations = 9 ;
  vl_bool bilinearOrientations = VL_FALSE ;
  VlHogVariant variant = VlHogVariantUoctti ;
  char const * variantName ;
  enum {IN_I = 0, IN_CELLSIZE, IN_END} ;
  enum {OUT_FEATURES = 0} ;

  vl_bool verbose = 0 ;
  int opt, next ;
  mxArray const *optarg ;

  enum Mode mode = ExtractFeatures ;
  enum InputType inputType = Image ;


  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  }
  if (nout > 1) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (vlmxIsString(IN(I), -1)) {
    mxArray const * string_array = IN(I) ;
    if (vlmxIsEqualToStringI(string_array, "permutation")) {
      /* perm = vl_hog('permutation') */
      mode = GetPermutation ;
      next = 1 ;

    } else if (vlmxIsEqualToStringI(string_array, "render")) {
      /* image = vl_hog('render', descriptor) */
      mode = Render ;
      descriptor_array = IN(CELLSIZE) ;
      next = 2 ;
      if (! mxIsNumeric(descriptor_array) ||
          ! vlmxIsReal(descriptor_array) ||
          mxGetClassID(descriptor_array) != mxSINGLE_CLASS) {
        vlmxError(vlmxErrInvalidArgument,
        "DESCRIPTOR is not a real numeric array of class SINGLE.") ;
      }
      if (mxGetNumberOfDimensions(descriptor_array) > 3) {
        vlmxError(vlmxErrInvalidArgument,
                  "DESCRIPTOR has more than three dimensions.") ;
      }
      if (mxGetNumberOfDimensions(descriptor_array) == 3) {
        numChannels = mxGetDimensions(descriptor_array)[2] ;
      } else {
        numChannels = 1 ;
      }
      descriptor = mxGetData(descriptor_array) ;
      height = mxGetDimensions(descriptor_array)[0] ;
      width = mxGetDimensions(descriptor_array)[1] ;
    } else {
      vlmxError(vlmxErrInvalidArgument,
                "The first argument is neither an image nor a recognized command.") ;
    }
  } else {
    /* descriptor = vl_hog(image, cellSize) */
    mode = ExtractFeatures ;
    next = 2 ;
    if (nin < 2) {
      vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
    }
    if (! mxIsNumeric(IN(I)) ||
        ! vlmxIsReal(IN(I)) ||
        mxGetClassID(IN(I)) != mxSINGLE_CLASS) {
      vlmxError(vlmxErrInvalidArgument,
                "I is not a real numeric array of class SINGLE.") ;
    }
    if (mxGetNumberOfDimensions(IN(I)) > 3) {
      vlmxError(vlmxErrInvalidArgument,
                "I has more than three dimensions.") ;
    }
    if (mxGetNumberOfDimensions(IN(I)) == 3) {
      numChannels = mxGetDimensions(IN(I))[2] ;
    } else {
      numChannels = 1 ;
    }
    if (! vlmxIsPlainScalar(IN(CELLSIZE))) {
      vlmxError(vlmxErrInvalidArgument,
                "CELLSIZE is not a plain scalar.") ;
    }
    if (mxGetScalar(IN(CELLSIZE)) < 1.0) {
      vlmxError(vlmxErrInvalidArgument,
                "CELLSIZE is less than 1.") ;
    }
    cellSize = (vl_size) mxGetScalar(IN(CELLSIZE)) ;
    image = mxGetData(IN(I)) ;
    height = mxGetDimensions(IN(I))[0] ;
    width = mxGetDimensions(IN(I))[1] ;
  }

  /* parse the options */
  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_variant :
        if (! vlmxIsString(optarg, -1)) {
          vlmxError(vlmxErrInvalidArgument, "VARIANT must be a string") ;
        }
        if (vlmxIsEqualToStringI(optarg, "uoctti")) {
          variant = VlHogVariantUoctti ;
        } else if (vlmxIsEqualToStringI(optarg, "dalaltriggs")) {
          variant = VlHogVariantDalalTriggs ;
        } else {
          vlmxError(vlmxErrInvalidArgument, "The option VARIANT has an unknown value.") ;
        }
        break ;

      case opt_num_orientations :
        if (! vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "NUMORIENTATIONS is not a plain scalar.") ;
        }
        if (mxGetScalar(optarg) < 1) {
          vlmxError(vlmxErrInvalidArgument, "NUMORIENTATIONS is smaller than one.") ;
        }
        numOrientations = mxGetScalar(optarg) ;
        break;

      case opt_directed_polar_field :
        inputType = DirectedPolarField ;
        break ;

      case opt_undirected_polar_field :
        inputType = UndirectedPolarField ;
        break ;

      case opt_bilinear_orientations :
        bilinearOrientations = VL_TRUE ;
        break ;

      case opt_verbose :
        ++ verbose ;
        break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  switch (variant) {
    case VlHogVariantUoctti : variantName = "UOCTTI" ; break ;
    case VlHogVariantDalalTriggs : variantName = "DalalTriggs" ; break ;
    default: abort() ; break ;
  }

  switch (mode) {
    case ExtractFeatures :
    {
      /* recall that MATLAB images are transposed */
      VlHog * hog = vl_hog_new (variant, numOrientations, VL_TRUE) ;
      mwSize dimensions [3] ;

      vl_hog_set_use_bilinear_orientation_assignments (hog, bilinearOrientations) ;

      if ((inputType == DirectedPolarField ||
           inputType == UndirectedPolarField) &&
          numChannels != 2) {
        vlmxError(vlmxErrInvalidArgument,
                  "NUMCHANNELS=%d is not equal to two with input of type %s.",
                  numChannels, inputTypeNames[inputType]) ;
      }

      switch (inputType) {
      case Image:
        vl_hog_put_image(hog, image, height, width, numChannels, cellSize) ;
        break ;
      case DirectedPolarField:
      case UndirectedPolarField:
        vl_hog_put_polar_field(hog, image, image + height*width,
                               inputType == DirectedPolarField,
                               height, width, cellSize) ;
          break ;
      default:
        abort() ;
      }

      dimensions[0] = vl_hog_get_width(hog) ;
      dimensions[1] = vl_hog_get_height(hog) ;
      dimensions[2] = vl_hog_get_dimension(hog) ;

      if (verbose) {
        mexPrintf("vl_hog: image: [%d x %d x %d]\n", height, width, numChannels) ;
        mexPrintf("vl_hog: descriptor: [%d x %d x %d]\n", dimensions[0], dimensions[1], dimensions[2]) ;
        mexPrintf("vl_hog: number of orientations: %d\n", numOrientations) ;
        mexPrintf("vl_hog: bilinear orientation assignments: %s\n", VL_YESNO(vl_hog_get_use_bilinear_orientation_assignments(hog))) ;
        mexPrintf("vl_hog: variant: %s\n", variantName) ;
        mexPrintf("vl_hog: input type: %s\n", inputTypeNames[inputType]) ;
      }

      OUT(FEATURES) = mxCreateNumericArray(3, dimensions, mxSINGLE_CLASS, mxREAL) ;
      vl_hog_extract (hog, mxGetData(OUT(FEATURES))) ;
      vl_hog_delete(hog) ;
      break ;
    }

    case GetPermutation :
    {
      VlHog * hog = vl_hog_new(variant, numOrientations, VL_TRUE) ;
      vl_size dimension = vl_hog_get_dimension(hog) ;
      vl_uindex k ;
      vl_uint32 * permutationOut ;
      vl_index const * permutation ;

      if (verbose) {
        mexPrintf("vl_hog: number of orientations: %d\n", numOrientations) ;
        mexPrintf("vl_hog: variant: %s\n", variantName) ;
      }

      OUT(FEATURES) = mxCreateNumericMatrix(dimension, 1, mxUINT32_CLASS, mxREAL) ;
      permutationOut = mxGetData(OUT(FEATURES)) ;
      permutation = vl_hog_get_permutation(hog) ;
      for (k = 0 ; k < dimension ; ++k) {
        permutationOut[k] = permutation[k] + 1 ;
      }
      break ;
    }

    case Render :
    {
      VlHog * hog = vl_hog_new(variant, numOrientations, VL_TRUE) ;
      vl_size glyphSize = vl_hog_get_glyph_size(hog) ;
      vl_size imageHeight = glyphSize * height ;
      vl_size imageWidth = glyphSize * width ;

      if (numChannels != vl_hog_get_dimension(hog)) {
        vlmxError(vlmxErrInvalidArgument,
                  "The third dimension of DESCRIPTOR is not equal to the dimension of a HOG descriptor.");
      }

      if (verbose) {
        mexPrintf("vl_hog: descriptor: [%d x %d x %d]\n", height, width, numChannels) ;
        mexPrintf("vl_hog: glyph image: [%d x %d]\n", imageHeight, imageWidth) ;
        mexPrintf("vl_hog: number of orientations: %d\n", numOrientations) ;
        mexPrintf("vl_hog: variant: %s\n", variantName) ;
      }

      OUT(FEATURES) = mxCreateNumericMatrix(imageHeight, imageWidth, mxSINGLE_CLASS, mxREAL) ;
      vl_hog_render(hog,
                    mxGetData(OUT(FEATURES)),
                    descriptor, height, width) ;
      break ;
    }
  }
}
