/** @file    mexutils.h
 ** @brief   MEX utilities
 ** @author  Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef MEXUTILS_H
#define MEXUTILS_H

#include"mex.h"
#include<vl/generic.h>
#include<vl/array.h>
#include<vl/stringop.h>
#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdarg.h>

#ifdef VL_COMPILER_MSC
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#if (! defined(MX_API_VER) || (MX_API_VER < 0x07030000)) && \
  (! defined(HAVE_OCTAVE))
typedef vl_uint32 mwSize ;
typedef vl_int32 mwIndex ;
#endif

/** @brief Access MEX input argument */
#undef IN
#define IN(x) (in[IN_ ## x])

/** @brief Access MEX output argument */
#undef OUT
#define OUT(x) (out[OUT_ ## x])

#ifdef HAVE_OCTAVE
static void *
mxReallocOctaveWorkaround(void * ptr, size_t size)
{
  /*  mexPrintf("fixed realloc\n") ; */
  if (ptr) {
    return mxRealloc(ptr, size) ;
  } else {
    return mxMalloc(size) ;
  }
}
#define mxRealloc mxReallocOctaveWorkaround
static int
mxSetDimensionsOctaveWorkaround(mxArray * array, const mwSize  *dims, int ndims)
{
  mwSize * dims_ = mxMalloc(sizeof(mwSize)*ndims) ;
  int i ;
  for (i = 0 ; i < ndims ; ++i) dims_[i] = dims[i] ;
  mxSetDimensions(array,dims_,ndims) ;
  return 0 ;
}
#define mxSetDimensions mxSetDimensionsOctaveWorkaround

#endif

/** @brief Setup VLFeat to be used in a MEX file
 **
 ** This makes VLFeat use MATLAB version of the memory allocation and
 ** logging functions.
 **/

#define VL_USE_MATLAB_ENV \
  vl_set_alloc_func (mxMalloc, mxRealloc, mxCalloc, mxFree) ; \
  vl_set_printf_func ((printf_func_t)mexPrintf) ;


/** @file mexutils.h

 This header file provides helper functions for writing MATLAB MEX
 files.

 - @ref mexutils-env "VLFeat environment"
 - @ref mexutils-array-test "Array tests"
 - @ref mexutils-options "Parsing options"

 @section mexutils-env VLFeat environment

 When the VLFeat DLL is linked to a MATLAB MEX files, at run time the
 MEX file must configure VLFeat to use MATLAB memory allocation and
 logging functions. This can be obtained by calling the macro
 ::VL_USE_MATLAB_ENV as the first line of each MEX file which is
 linked to the VLFeat DLL.

 @section mexutils-array-test Array tests

 MATLAB supports a variety of array types. Most MEX file arguments are
 restricted to a few types and must be properly checked at run time.
 @ref mexutils.h provides some helper functions to make it simpler to
 check such arguments. MATLAB basic array types are:

 - Numeric array:
   @c mxDOUBLE_CLASS, @c mxSINGLE_CLASS,
   @c mxINT8_CLASS, @c mxUINT8_CLASS,
   @c mxINT16_CLASS, @c mxUINT16_CLASS,
   @c mxINT32_CLASS, @c mxUINT32_CLASS. Moreover:
   - all such types have a @e real component
   - all such types may have a @e imaginary component
   - @c mxDOUBLE_LCASS arrays with two dimensions can be @e sparse.
 - Logical array (@c mxLOGICAL_CLASS).
 - Character array (@c mxCHAR_CLASS).

 The other MATLAB array types are:

 - Struct array (@c mxSTRUCT_CLASS).
 - Cell array (@c mxCELL_CLASS).
 - Custom class array (@c mxCLASS_CLASS).
 - Unkown type array (@c mxUNKNOWN_CLASS).

 VLFeat defines a number of common classes of arrays and corresponding
 tests.

 - <b>Scalar array</b> is a non-sparse array with exactly one element.
   Note that the array may have an arbitrary number of dimensions, and
   be of any numeric or other type. All dimensions are singleton
   (which is implied by having exactly one element). Use ::vlmxIsScalar
   to test if an array is scalar.

 - <b>Vector array</b> is a non-sparse array which is either empty
   (empty vector) or has at most one non-singleton dimension. The
   array can be of any numeric or other type. The elements of such a
   MATLAB array are stored as a plain C array with a number of
   elements equal to the number of elements in the array (obtained
   with @c mxGetNumberOfElements). Use ::vlmxIsVector to test whether an
   array is a vector.

 - <b>Matrix array</b> is a non-sparse array for which all dimensions
   beyond the first two are singleton, or a non-sparse array which is
   empty and for which at least one of the first two dimensions is
   zero. The array can be of any numeric or other type.  The
   non-singleton dimensions can be zero (empty matrix), one, or
   more. The element of such a MATLAB array are stored as a C array in
   column major order and its dimensions can be obtained by @c mxGetM
   and @c mxGetN.  Use ::vlmxIsMatrix to test if an array is a matrix.

 - <b>Real array</b> is a numeric array (as for @c mxIsNumeric)
   without a complex component. Use ::vlmxIsReal to check if an array
   is real.

 - Use ::vlmxIsOfClass to check if an array is of a prescribed
   (storage) class, such as @c mxDOUBLE_CLASS.

 - <b>Plain scalar, vector, and matrix</b> are a scalar, vector, and
   matrix arrays which are <em>real</em> and of class @c
   mxDOUBLE_CLASS.  Use ::vlmxIsPlainScalar, ::vlmxIsPlainVector and
   ::vlmxIsPlainMatrix to check this.

 @section mexutils-options Parsing options

 It is common to pass optional arguments to a MEX file as option
 type-value pairs. Here type is a string identifying the option and
 value is a MATLAB array specifing its value. The function
 ::vlmxNextOption  can be used to simplify parsing a list of such
 arguments (similar to UNIX @c getopt). The functions ::vlmxError
 and ::vlmxWarning are shortcuts to specify VLFeat formatted errors.

 **/

/* these attributes suppress undefined symbols warning with GCC */
#ifdef VL_COMPILER_GNUC
#if (! defined(HAVE_OCTAVE))
EXTERN_C void __attribute__((noreturn))
mexErrMsgIdAndTxt (const char * identifier, const char * err_msg, ...) ;
#else
extern void __attribute__((noreturn))
mexErrMsgIdAndTxt (const char *id, const char *s, ...);
#endif
#endif

#define MEXUTILS_RAISE_HELPER_A \
  char const * errorString ; \
  char formattedErrorId [512] ; \
  char formattedErrorMessage [1024] ; \
  \
  switch (errorId) { \
    case vlmxErrAlloc : errorString = "outOfMemory" ; break ; \
    case vlmxErrInvalidArgument : errorString = "invalidArgument" ; break ; \
    case vlmxErrNotEnoughInputArguments : errorString = "notEnoughInputArguments" ; break ; \
    case vlmxErrTooManyInputArguments : errorString = "tooManyInputArguments" ; break ; \
    case vlmxErrNotEnoughOutputArguments : errorString = "notEnoughOutputArguments" ; break ; \
    case vlmxErrTooManyOutputArguments : errorString = "tooManyOutputArguments" ; break ; \
    case vlmxErrInvalidOption : errorString = "invalidOption" ; break ; \
    case vlmxErrInconsistentData : errorString = "inconsistentData" ; break ; \
    default : errorString = "undefinedError" ; break ; \
  } \
  \
  if (! errorMessage) { \
    switch (errorId) { \
      case vlmxErrAlloc: errorMessage = "Out of memory." ; break ; \
      case vlmxErrInvalidArgument: errorMessage = "Invalid argument." ; break ; \
      case vlmxErrNotEnoughInputArguments: errorMessage = "Not enough input arguments." ; break ; \
      case vlmxErrTooManyInputArguments: errorMessage = "Too many input arguments." ; break ; \
      case vlmxErrNotEnoughOutputArguments: errorMessage = "Not enough output arguments." ; break ; \
      case vlmxErrTooManyOutputArguments: errorMessage = "Too many output arguments." ; break ; \
      case vlmxErrInconsistentData: errorMessage = "Inconsistent data." ; break ; \
      case vlmxErrInvalidOption: errorMessage = "Invalid option." ; break ; \
      default: errorMessage = "Undefined error message." ; \
    } \
  }

#ifdef VL_COMPILER_LCC
#define MEXUTILS_RAISE_HELPER_B \
{ \
  va_list args ; \
  va_start(args, errorMessage) ; \
  sprintf(formattedErrorId, \
          "vl:%s", errorString) ; \
  vsprintf(formattedErrorMessage, \
           errorMessage, args) ; \
  va_end(args) ; \
}
#else
#define MEXUTILS_RAISE_HELPER_B \
{ \
  va_list args ; \
  va_start(args, errorMessage) ; \
  snprintf(formattedErrorId, \
           sizeof(formattedErrorId)/sizeof(char), \
           "vl:%s", errorString) ; \
  vsnprintf(formattedErrorMessage, \
            sizeof(formattedErrorMessage)/sizeof(char), \
            errorMessage, args) ; \
  va_end(args) ; \
}
#endif

#define MEXUTILS_RAISE_HELPER MEXUTILS_RAISE_HELPER_A MEXUTILS_RAISE_HELPER_B

/** @{
 ** @name Error handling
 **/

/** @brief VLFeat MEX errors */
typedef enum _VlmxErrorId {
  vlmxErrAlloc = 1,
  vlmxErrInvalidArgument,
  vlmxErrNotEnoughInputArguments,
  vlmxErrTooManyInputArguments,
  vlmxErrNotEnoughOutputArguments,
  vlmxErrTooManyOutputArguments,
  vlmxErrInvalidOption,
  vlmxErrInconsistentData
} VlmxErrorId ;


/** @brief Raise a MEX error with VLFeat format
 ** @param errorId error ID string.
 ** @param errorMessage error message C-style format string.
 ** @param ... format string arguments.
 **
 ** The function internally calls @c mxErrMsgTxtAndId, which causes
 ** the MEX file to abort.
 **/

#if defined(VL_COMPILER_GNUC) & ! defined(__DOXYGEN__)
static void __attribute__((noreturn))
#else
static void
#endif
vlmxError (VlmxErrorId errorId, char const * errorMessage, ...)
{
  MEXUTILS_RAISE_HELPER ;
  mexErrMsgIdAndTxt (formattedErrorId, formattedErrorMessage) ;
}

/** @brief Raise a MEX warning with VLFeat format
 ** @param errorId error ID string.
 ** @param errorMessage error message C-style format string.
 ** @param ... format string arguments.
 **
 ** The function internally calls @c mxWarnMsgTxtAndId.
 **/

static void
vlmxWarning (VlmxErrorId errorId, char const * errorMessage, ...)
{
  MEXUTILS_RAISE_HELPER ;
  mexWarnMsgIdAndTxt (formattedErrorId, formattedErrorMessage) ;
}

/** @} */

/** @name Check for array attributes
 ** @{ */

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is of a prescribed class
 ** @param array MATLAB array.
 ** @param classId prescribed class of the array.
 ** @return ::VL_TRUE if the class is of the array is of the prescribed class.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsOfClass (mxArray const * array, mxClassID classId)
{
  return mxGetClassID (array) == classId ;
}

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is real
 ** @param array MATLAB array.
 ** @return ::VL_TRUE if the array is real.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsReal (mxArray const * array)
{
  return mxIsNumeric (array) && ! mxIsComplex (array) ;
}

/** @} */

/** @name Check for scalar, vector and matrix arrays
 ** @{ */

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is scalar
 ** @param array MATLAB array.
 ** @return ::VL_TRUE if the array is scalar.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsScalar (mxArray const * array)
{
  return (! mxIsSparse (array)) && (mxGetNumberOfElements (array) == 1)  ;
}

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is a vector.
 ** @param array MATLAB array.
 ** @param numElements number of elements (negative for any).
 ** @return ::VL_TRUE if the array is a vecotr of the prescribed size.
 ** @sa @ref mexutils-array-test
 **/

static vl_bool
vlmxIsVector (mxArray const * array, vl_index numElements)
{
  vl_size numDimensions = (unsigned) mxGetNumberOfDimensions (array) ;
  mwSize const * dimensions = mxGetDimensions (array) ;
  vl_uindex di ;

  /* check that it is not sparse */
  if (mxIsSparse (array)) {
    return VL_FALSE ;
  }

  /* check that the number of elements is the prescribed one */
  if ((numElements >= 0) && ((unsigned) mxGetNumberOfElements (array) !=
                             (unsigned) numElements)) {
    return VL_FALSE ;
  }

  /* check that all but at most one dimension is singleton */
  for (di = 0 ;  di < numDimensions ; ++ di) {
    if (dimensions[di] != 1) break ;
  }
  for (++ di ; di < numDimensions ; ++di) {
    if (dimensions[di] != 1) return VL_FALSE ;
  }
  return VL_TRUE ;
}

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is a matrix.
 ** @param array MATLAB array.
 ** @param M number of rows (negative for any).
 ** @param N number of columns (negative for any).
 ** @return ::VL_TRUE if the array is a matrix of the prescribed size.
 ** @sa @ref mexutils-array-test
 **/

static vl_bool
vlmxIsMatrix (mxArray const * array, vl_index M, vl_index N)
{
  vl_size numDimensions = (unsigned) mxGetNumberOfDimensions (array) ;
  mwSize const * dimensions = mxGetDimensions (array) ;
  vl_uindex di ;

  /* check that it is not sparse */
  if (mxIsSparse (array)) {
    return VL_FALSE ;
  }

  /* check that the number of elements is the prescribed one */
  if ((M >= 0) && ((unsigned) mxGetM (array) != (unsigned) M)) {
    return VL_FALSE;
  }
  if ((N >= 0) && ((unsigned) mxGetN (array) != (unsigned) N)) {
    return VL_FALSE;
  }

  /* ok if empty and either M = 0 or N = 0 */
  if ((mxGetNumberOfElements (array) == 0) && (mxGetM (array) == 0 || mxGetN (array) == 0)) {
    return VL_TRUE ;
  }

  /* ok if any dimension beyond the first two is singleton */
  for (di = 2 ; ((unsigned)dimensions[di] == 1) && di < numDimensions ; ++ di) ;
  return di == numDimensions ;
}


/** ------------------------------------------------------------------
 ** @brief Check if the MATLAB array has the specified dimensions.
 ** @param array array to check.
 ** @param numDimensions number of dimensions.
 ** @param dimensions dimensions.
 ** @return true the test succeeds.
 **
 ** The test is true if @a numDimensions < 0. If not, it is false if
 ** the array has not @a numDimensions. Otherwise it is true is @a
 ** dimensions is @c NULL or if each entry of @a dimensions is
 ** either negative or equal to the corresponding array dimension.
 **/

static vl_bool
vlmxIsArray (mxArray const * array, vl_index numDimensions, vl_index* dimensions)
{
  if (numDimensions >= 0) {
    vl_index d ;
    mwSize const * actualDimensions = mxGetDimensions (array) ;

    if ((unsigned) mxGetNumberOfDimensions (array) != (unsigned) numDimensions) {
      return VL_FALSE ;
    }

    if(dimensions != NULL) {
      for(d = 0 ; d < numDimensions ; ++d) {
        if (dimensions[d] >= 0 && (unsigned) dimensions[d] != (unsigned) actualDimensions[d])
          return VL_FALSE ;
      }
    }
  }
  return VL_TRUE ;
}

/** @} */

/** @name Check for plain arrays
 ** @{ */

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is plain
 ** @param array MATLAB array.
 ** @return ::VL_TRUE if the array is plain.
 ** @sa @ref mexutils-array-test
 **/

 VL_INLINE vl_bool
 vlmxIsPlain (mxArray const * array)
 {
 return
   vlmxIsReal (array) &&
   vlmxIsOfClass (array, mxDOUBLE_CLASS) ;
 }


/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is plain scalar
 ** @param array MATLAB array.
 ** @return ::VL_TRUE if the array is plain scalar.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsPlainScalar (mxArray const * array)
{
  return vlmxIsPlain (array) && vlmxIsScalar (array) ;
}

/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is a plain vector.
 ** @param array MATLAB array.
 ** @param numElements number of elements (negative for any).
 ** @return ::VL_TRUE if the array is a plain vecotr of the prescribed size.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsPlainVector (mxArray const * array, vl_index numElements)
{
  return vlmxIsPlain (array) && vlmxIsVector (array, numElements) ;
}


/** ------------------------------------------------------------------
 ** @brief Check if a MATLAB array is a plain matrix.
 ** @param array MATLAB array.
 ** @param M number of rows (negative for any).
 ** @param N number of columns (negative for any).
 ** @return ::VL_TRUE if the array is a plain matrix of the prescribed size.
 ** @sa @ref mexutils-array-test
 **/

VL_INLINE vl_bool
vlmxIsPlainMatrix (mxArray const * array, vl_index M, vl_index N)
{
  return vlmxIsPlain (array) && vlmxIsMatrix (array, M, N) ;
}

/** ------------------------------------------------------------------
 ** @brief Check if the array is a string
 ** @param array array to test.
 ** @param length string length.
 ** @return true if the array is a string of the specified length
 **
 ** The array @a array satisfies the test if:
 ** - its storage class is CHAR;
 ** - it has two dimensions but only one row;
 ** - @a length < 0 or the array has @a length columns.
 **/

static int
vlmxIsString (const mxArray* array, vl_index length)
{
  mwSize M = (mwSize) mxGetM (array) ;
  mwSize N = (mwSize) mxGetN (array) ;

  return
    mxIsChar(array) &&
    mxGetNumberOfDimensions(array) == 2 &&
    (M == 1 || (M == 0 && N == 0)) &&
    (length < 0 || (signed)N == length) ;
}


/** @} */

/** ------------------------------------------------------------------
 ** @brief Create a MATLAB array which is a plain scalar
 ** @param x scalar value.
 ** @return the new array.
 **/

static mxArray *
vlmxCreatePlainScalar (double x)
{
  mxArray * array = mxCreateDoubleMatrix (1,1,mxREAL) ;
  *mxGetPr(array) = x ;
  return array ;
}

/** ------------------------------------------------------------------
 ** @brief Create a MATLAB array from a VlArray
 ** @param x VlArray instance.
 ** @return the new array.
 **/

static mxArray *
vlmxCreateArrayFromVlArray (VlArray const * x)
{
  mwSize dimensions [VL_ARRAY_MAX_NUM_DIMENSIONS] ;
  mxArray * array = NULL ;
  mxClassID classId = (mxClassID)0 ;
  vl_uindex d ;
  vl_size numElements = vl_array_get_num_elements(x) ;
  vl_size numDimensions  = vl_array_get_num_dimensions(x) ;
  vl_size const * xdimensions = vl_array_get_dimensions(x) ;
  vl_type type = vl_array_get_data_type(x) ;
  vl_size typeSize = vl_get_type_size(type) ;

  for (d = 0 ; d < numDimensions ; ++d) {
    dimensions[d] = (mwSize) xdimensions[d] ;
  }

  switch (type) {
    case VL_TYPE_FLOAT   : classId = mxSINGLE_CLASS ; break ;
    case VL_TYPE_DOUBLE  : classId = mxDOUBLE_CLASS ; break ;
    case VL_TYPE_INT8    : classId = mxINT8_CLASS ; break ;
    case VL_TYPE_INT16   : classId = mxINT16_CLASS ; break ;
    case VL_TYPE_INT32   : classId = mxINT32_CLASS ; break ;
    case VL_TYPE_INT64   : classId = mxINT64_CLASS ; break ;
    case VL_TYPE_UINT8   : classId = mxUINT8_CLASS ; break ;
    case VL_TYPE_UINT16  : classId = mxUINT16_CLASS ; break ;
    case VL_TYPE_UINT32  : classId = mxUINT32_CLASS ; break ;
    case VL_TYPE_UINT64  : classId = mxUINT64_CLASS ; break ;
    default: assert(VL_FALSE) ;
  }

  array = mxCreateNumericArray(numDimensions,
                               dimensions,
                               classId,
                               mxREAL) ;

  if (array == NULL) return NULL ;

  memcpy(mxGetData(array), vl_array_get_data(x), typeSize * numElements) ;

  return array ;
}

/** ------------------------------------------------------------------
 ** @brief Envelope a MATLAB array in a VlArray instance
 ** @param v VlArray instance (out)
 ** @param x MATALB array.
 ** @return @c v.
 **/

static VlArray *
vlmxEnvelopeArrayInVlArray (VlArray * v, mxArray * x)
{
  vl_size numDimensions = mxGetNumberOfDimensions(x) ;
  mwSize const * dimensions = mxGetDimensions(x) ;
  mxClassID classId = mxGetClassID(x) ;
  vl_size vdimensions [VL_ARRAY_MAX_NUM_DIMENSIONS] ;
  vl_type type ;
  vl_uindex d ;

  for (d = 0 ; d < numDimensions ; ++d) {
    vdimensions[d] = dimensions[d] ;
  }

  switch (classId) {
    case mxSINGLE_CLASS: type =  VL_TYPE_FLOAT  ; break ;
    case mxDOUBLE_CLASS: type =  VL_TYPE_DOUBLE ; break ;
    case mxINT8_CLASS  : type =  VL_TYPE_INT8   ; break ;
    case mxINT16_CLASS : type =  VL_TYPE_INT16  ; break ;
    case mxINT32_CLASS : type =  VL_TYPE_INT32  ; break ;
    case mxINT64_CLASS : type =  VL_TYPE_INT64  ; break ;
    case mxUINT8_CLASS : type =  VL_TYPE_UINT8  ; break ;
    case mxUINT16_CLASS: type =  VL_TYPE_UINT16 ; break ;
    case mxUINT32_CLASS: type =  VL_TYPE_UINT32 ; break ;
    case mxUINT64_CLASS: type =  VL_TYPE_UINT64 ; break ;
    default: assert(VL_FALSE) ; abort() ;
  }

  vl_array_init_envelope(v, mxGetData(x), type, numDimensions, vdimensions) ;
  return v ;
}

/** ------------------------------------------------------------------
 ** @brief Case insensitive string comparison
 ** @param s1 first string.
 ** @param s2 second string.
 ** @return comparison result.
 **
 ** The comparison result is equal to 0 if the strings are equal, >0
 ** if the first string is greater than the second (in lexicographical
 ** order), and <0 otherwise.
 **/

static int
vlmxCompareStringsI(const char *s1, const char *s2)
{
  /*
   Since tolower has an int argument, characters must be unsigned
   otherwise will be sign-extended when converted to int.
   */
  while (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))
  {
    if (*s1 == 0) return 0 ; /* implies *s2 == 0 */
    s1++;
    s2++;
  }
  return tolower((unsigned char)*s1) - tolower((unsigned char)*s2) ;
}

/** ------------------------------------------------------------------
 ** @brief Case insensitive string comparison with array
 ** @param array first string (as a MATLAB array).
 ** @param string second string.
 ** @return comparison result.
 **
 ** The comparison result is equal to 0 if the strings are equal, >0
 ** if the first string is greater than the second (in lexicographical
 ** order), and <0 otherwise.
 **/

static int
vlmxCompareToStringI(mxArray const * array, char const  * string)
{
  mxChar const * s1 = (mxChar const *) mxGetData(array) ;
  char unsigned const * s2 = (char unsigned const*) string ;
  vl_size n = mxGetNumberOfElements(array) ;

  /*
   Since tolower has an int argument, characters must be unsigned
   otherwise will be sign-extended when converted to int.
   */
  while (n && tolower((unsigned)*s1) == tolower(*s2)) {
    if (*s2 == 0) return 1 ; /* s2 terminated on 0, but s1 did not terminate yet */
    s1 ++ ;
    s2 ++ ;
    n -- ;
  }
  return tolower(n ? (unsigned)*s1 : 0) - tolower(*s2) ;
}

/** ------------------------------------------------------------------
 ** @brief Case insensitive string equality test with array
 ** @param array first string (as a MATLAB array).
 ** @param string second string.
 ** @return true if the strings are equal.
 **/

static int
vlmxIsEqualToStringI(mxArray const * array, char const  * string)
{
  return vlmxCompareToStringI(array, string) == 0 ;
}

/* ---------------------------------------------------------------- */
/*                        Options handling                          */
/* ---------------------------------------------------------------- */

/** @brief MEX option */

struct _vlmxOption
{
  const char *name ; /**< option name */
  int has_arg ;      /**< has argument? */
  int val ;          /**< value to return */
} ;

/** @brief MEX option type */

typedef struct _vlmxOption vlmxOption  ;

/** ------------------------------------------------------------------
 ** @brief Parse the next option
 ** @param args     MEX argument array.
 ** @param nargs    MEX argument array length.
 ** @param options  List of option definitions.
 ** @param next     Pointer to the next option (input and output).
 ** @param optarg   Pointer to the option optional argument (output).
 ** @return the code of the next option, or -1 if there are no more options.
 **
 ** The function parses the array @a args for options. @a args is
 ** expected to be a sequence alternating option names and option
 ** values, in the form of @a nargs instances of @c mxArray. The
 ** function then scans the option starting at position @a next in the
 ** array.  The option name is matched (case insensitive) to the table
 ** of options @a options, a pointer to the option value is stored in
 ** @a optarg, @a next is advanced to the next option, and the option
 ** code is returned.
 **
 ** The function is typically used in a loop to parse all the available
 ** options. @a next is initialized to zero, and then the function
 ** is called until the special code -1 is returned.
 **
 ** If the option name cannot be matched to the available options,
 ** either because the option name is not a string array or because
 ** the name is unknown, the function exits the MEX file with an
 ** error.
 **/

static int
vlmxNextOption (mxArray const *args[], int nargs,
                vlmxOption  const *options,
                int *next,
                mxArray const **optarg)
{
  char name [1024] ;
  int opt = -1, i;

  if (*next >= nargs) {
    return opt ;
  }

  /* check the array is a string */
  if (! vlmxIsString (args [*next], -1)) {
    vlmxError (vlmxErrInvalidOption,
               "The option name is not a string (argument number %d)",
               *next + 1) ;
  }

  /* retrieve option name */
  if (mxGetString (args [*next], name, sizeof(name))) {
    vlmxError (vlmxErrInvalidOption,
               "The option name is too long (argument number %d)",
               *next + 1) ;
  }

  /* advance argument list */
  ++ (*next) ;

  /* now lookup the string in the option table */
  for (i = 0 ; options[i].name != 0 ; ++i) {
    if (vlmxCompareStringsI(name, options[i].name) == 0) {
      opt = options[i].val ;
      break ;
    }
  }

  /* unknown argument */
  if (opt < 0) {
    vlmxError (vlmxErrInvalidOption, "Unknown option '%s'.", name) ;
  }

  /* no argument */
  if (! options [i].has_arg) {
    if (optarg) *optarg = 0 ;
    return opt ;
  }

  /* argument */
  if (*next >= nargs) {
    vlmxError(vlmxErrInvalidOption,
              "Option '%s' requires an argument.", options[i].name) ;
  }

  if (optarg) *optarg = args [*next] ;
  ++ (*next) ;
  return opt ;
}

/** @brief Get an emumeration member by name
 ** @param enumeration the enumeration to decode.
 ** @param name_array member name as a MATLAB string array.
 ** @param caseInsensitive if @c true match the string case-insensitive.
 ** @return the corresponding enumeration member, or @c NULL if any.
 **/

static VlEnumerator *
vlmxDecodeEnumeration (mxArray const *name_array,
                       VlEnumerator const *enumeration,
                       vl_bool caseInsensitive)
{
  char name [1024] ;

  /* check the array is a string */
  if (! vlmxIsString (name_array, -1)) {
    vlmxError (vlmxErrInvalidArgument, "The array is not a string.") ;
  }

  /* retrieve option name */
  if (mxGetString (name_array, name, sizeof(name))) {
    vlmxError (vlmxErrInvalidArgument, "The string array is too long.") ;
  }

  if (caseInsensitive) {
    return vl_enumeration_get_casei(enumeration, name) ;
  } else {
    return vl_enumeration_get(enumeration, name) ;
  }
}

/* MEXUTILS_H */
#endif
