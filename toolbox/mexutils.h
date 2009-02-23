/** @file   mexutils.h
 ** @author Andrea Vedaldi
 ** @brief  MEX helper functions
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include"mex.h"
#include<vl/generic.h>
#include<ctype.h>
#include<stdio.h>
#include<stdarg.h>

#ifdef VL_COMPILER_MSC
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#if !defined(MX_API_VER) | (MX_API_VER < 0x07030000) 
typedef int mwSize ;
typedef int mwIndex ;
#endif

/** @file mexutils.h 

This header file provides utility function that help writing MATLAB
MEX files.

@subsection mexutils-plain-matrix

- It is numeric.
- It has DOUBLE storage class.
- It has only a real component.
- It is full.

 **/

/** ------------------------------------------------------------------
 ** @brief Let VLFeat use MATLAB memory allocation/logging facilities
 **
 ** This makes VLFeat use MATLAB version of the memory allocation and
 ** logging functions.
 **
 **/
#define VL_USE_MATLAB_ENV                                       \
  vl_set_alloc_func (mxMalloc, mxRealloc, mxCalloc, mxFree) ;   \
  vl_set_printf_func (mexPrintf) ;

/** ------------------------------------------------------------------
 ** @brief Create array with pre-allocated data
 **
 ** @param ndim    number of dimensions.
 ** @param dims    dimensions.
 ** @param classid storage class ID.
 ** @param data    pre-allocated data.
 **
 ** If @a data is set to NULL, the data is allocated from the heap.
 ** If @a data is a buffer allocated by @a mxMalloc, then this buffer
 ** is used as data.
 **
 ** @return new array.
 **/

static mxArray *
uCreateNumericArray (mwSize ndim, const mwSize * dims, 
                     mxClassID classid, void * data)
{
  mxArray *A ;

  if  (data) {
    mwSize dims_ [2] = {0, 0} ;
    A = mxCreateNumericArray (2, dims_, classid, mxREAL) ;
    mxSetData (A, data) ;
    mxSetDimensions (A, dims, ndim) ;
  } else {
    A = mxCreateNumericArray (ndim, dims, classid, mxREAL) ;
  }
  
  return A ;
}

/** ------------------------------------------------------------------
 ** @brief Create an array with pre-allocated data
 **
 ** @param M       number of rows.
 ** @param N       number of columns.
 ** @param classid class ID.
 ** @param data    pre-allocated data.
 **
 ** If @a data is set to NULL, the data is allocated from the heap.
 ** If @a data is a buffer allocated by @a mxMalloc, then this buffer
 ** is used as data.
 **
 ** @return new array.
 **/

static mxArray *
uCreateNumericMatrix (int M, int N, mxClassID classid, void * data)
{
  mxArray *A ;

  if  (data) {
    A = mxCreateNumericMatrix (0, 0, classid, mxREAL) ;
    mxSetData (A, data) ;
    mxSetM(A, M) ;
    mxSetN(A, N) ;
  } else {
    A = mxCreateNumericMatrix (M, N, classid, mxREAL) ;
  }

  return A ;
}

/** ------------------------------------------------------------------
 ** @brief Create a plain scalar
 **
 ** @param x inital value.
 **
 ** @return new array.
 **/

static mxArray *
uCreateScalar (double x)
{
  mxArray *A = mxCreateDoubleMatrix(1,1,mxREAL) ;
  *mxGetPr(A) = x ;
  return A ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array a numeric scalar?
 **
 ** @param A array to test.
 **
 ** An array is <em>numeric and scalar</em> if:
 ** - It is numeric.
 ** - It as exactly one element.
 **
 ** @return test result.
 **/

static int
uIsScalar(const mxArray* A)
{
  return 
    mxIsNumeric (A) && mxGetNumberOfElements(A) == 1 ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array plain matrix?
 **
 ** @param A array to test.
 ** @param M number of rows.
 ** @param N number of columns.
 **
 ** The array @a A satisfies the test if:
 **
 ** - It is a @ref mexutils-plain-matrix "plain matrix"
 ** - @a M < 0 or the number of rows is equal to @a M.
 ** - @a N < 0 or the number of columns is equal to @a N.
 **
 ** @return test result.
 **/

static vl_bool 
uIsPlainArray (const mxArray* A)
{
  return
    mxGetClassID(A) == mxDOUBLE_CLASS &&
    ! mxIsComplex(A) &&
    ! mxIsSparse(A) ;
}

static vl_bool 
uIsPlainMatrix (const mxArray* A, int M, int N)
{
  return
    uIsPlainArray(A) &&
    mxGetNumberOfDimensions(A) == 2 &&
    (M < 0 || mxGetM(A) == M) &&
    (N < 0 || mxGetN(A) == N) ;   
}

static vl_bool 
uIsPlainVector (const mxArray* A, int M)
{
  return
    uIsPlainArray(A) &&
    mxGetNumberOfDimensions(A) == 2 &&
    (mxGetM(A) == 1 || mxGetN(A) == 1) &&
    (M < 0 || (mxGetM(A) == M || mxGetN(A) == M)) ;
}

static vl_bool 
uIsPlainScalar (const mxArray* A)
{
  return
    uIsPlainArray(A) &&
    mxGetNumberOfElements(A) == 1 ;
}


/** ------------------------------------------------------------------
 ** @brief Is the array a numeric matrix?
 **
 ** @param A array to test.
 ** @param M number of rows.
 ** @param N number of columns.
 **
 ** The array @a A satisfies the test if:
 ** - It is numeric.
 ** - It as two dimensions.
 ** - @a M < 0 or the number of rows is equal to @a M.
 ** - @a N < 0 or the number of columns is equal to @a N.
 **
 ** @return test result.
 **/

static int
uIsMatrix (const mxArray* A, int M, int N)
{
  return
    mxIsNumeric(A) &&
    mxGetNumberOfDimensions(A) == 2 &&
    (M < 0 || mxGetM(A) == M) &&
    (N < 0 || mxGetN(A) == N) ;   
}

/** ------------------------------------------------------------------
 ** @brief Is the array a vector?
 **
 ** @param A array to test.
 ** @param N number of elements.
 **
 ** The array @a A satisfies the test if
 ** - It is a matrix (see ::uIsMatrix()).
 ** - It has a singleton dimension.
 ** - @c N < 0 or the other dimension is equal to @c N.
 **
 ** @return test result.
 **/

static int
uIsVector(const mxArray* A, int N)
{
  return  
    uIsMatrix(A, 1, N) || uIsMatrix(A, N, 1) ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array real?
 **
 ** @param A array to test.
 **
 ** An array satisfies the test if:
 ** - The storage class is DOUBLE.
 ** - There is no imaginary part.
 **
 ** @return test result.
 **/

static int
uIsReal (const mxArray* A)
{
  return 
    mxIsDouble(A) && 
    ! mxIsComplex(A) ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array real and scalar?
 **
 ** @param A array to test.
 **
 ** An array is <em>real and scalar</em> if:
 ** - It is real (see ::uIsReal()).
 ** - It as only one element.
 **
 ** @return test result.
 **/

static int
uIsRealScalar(const mxArray* A)
{
  return 
    uIsReal (A) && mxGetNumberOfElements(A) == 1 ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array a real matrix?
 **
 ** @param A array to test.
 ** @param M number of rows.
 ** @param N number of columns.
 **
 ** The array @a A satisfies the test if:
 ** - It is real (see ::uIsReal()).
 ** - It as two dimensions.
 ** - @a M < 0 or the number of rows is equal to @a M.
 ** - @a N < 0 or the number of columns is equal to @a N.
 **
 ** @return test result.
 **/

static int
uIsRealMatrix(const mxArray* A, int M, int N)
{
  return  
    mxIsDouble(A) &&
    !mxIsComplex(A) &&
    mxGetNumberOfDimensions(A) == 2 &&
    (M < 0 || mxGetM(A) == M) &&
    (N < 0 || mxGetN(A) == N) ;   
}

/** ------------------------------------------------------------------
 ** @brief Is the array a real vector?
 **
 ** @param A array to test.
 ** @param N number of elements.
 **
 ** The array @a A satisfies the test if
 ** - It is a real matrix (see ::uIsRealMatrix()).
 ** - It has a singleton dimension.
 ** - @c N < 0 or the other dimension is equal to @c N.
 **
 ** @return test result.
 **/

static int
uIsRealVector(const mxArray* A, int N)
{
  return  
    uIsRealMatrix(A, 1, N) || uIsRealMatrix(A, N, 1) ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array real with specified dimensions?
 **
 ** @param A array to check.
 ** @param D number of dimensions.
 ** @param dims dimensions.
 **
 ** The array @a A satisfies the test if:
 ** - It is real (see ::uIsReal()).
 ** - @a ndims < 0 or it has @a ndims dimensions and
 **   - for each element of @a dims, either that element is negative
 **     or it is equal to the corresponding dimension of the array.
 **
 ** @return test result.
 **/

static int
uIsRealArray(const mxArray* A, int D, int* dims)
{
  if(!mxIsDouble(A) || mxIsComplex(A))
    return 0 ;

  if(D >= 0) {
    int d ;
    mwSize const * actual_dims = mxGetDimensions(A) ;

    if(mxGetNumberOfDimensions(A) != D)
      return 0 ;

    return 1 ;
    
    if(dims != NULL) {
      for(d = 0 ; d < D ; ++d) {
        if(dims[d] >= 0 && dims[d] != actual_dims[d])
          return 0 ;
      }
    }
  }
  return 1 ;
}

/** ------------------------------------------------------------------
 ** @brief Is the array a string?
 **
 ** @param A array to test.
 ** @param L string length.
 **
 ** The array @a A satisfies the test if:
 ** - its storage class is CHAR;
 ** - it has two dimensions;
 ** - it has one row;
 ** - @a L < 0 or it has @a L columns.
 **
 ** @return test result.
 **/

static int
uIsString(const mxArray* A, int L)
{
  int M = mxGetM(A) ;
  int N = mxGetN(A) ;

  return 
    mxIsChar(A) &&
    mxGetNumberOfDimensions(A) == 2 &&
    M == 1 &&
    (L < 0 || N == L) ;
}

/** ------------------------------------------------------------------
 ** @brief Formatted @c mexErrMsgTxt() 
 **
 ** @param args     Format string (for sprintf).
 ** @param ...      format string arguments.
 **/

void
uErrMsgTxt(char const * format, ...)
{
  enum { buffLen = 1024 } ;
  char buffer [buffLen] ;
  va_list args;
  va_start (args, format) ;
#ifdef VL_COMPILER_LCC
  vsprintf(buffer, format, args) ;
#else
  vsnprintf (buffer, buffLen, format, args) ;
#endif
  va_end (args) ;
  mexErrMsgTxt (buffer) ;
}


/** -------------------------------------------------------------------
 ** @brief MEX option 
 **/

struct _uMexOption
{
  const char *name ; /**< option name */
  int has_arg ;      /**< has argument? */
  int val ;          /**< value to return */
} ;

/** @brief MEX option type
 ** @see ::_uMexOption
 **/
typedef struct _uMexOption uMexOption ;

/** ------------------------------------------------------------------
 ** @brief Case insensitive string comparison
 **
 ** @param s1 first string.
 ** @param s2 second string.
 **
 ** @return 0 if the strings are equal, >0 if the first string is
 ** greater (in lexicographical order) and <0 otherwise.
 **/

static int
uStrICmp(const char *s1, const char *s2)
{
  while (tolower((unsigned char)*s1) == 
         tolower((unsigned char)*s2))
  {
    if (*s1 == 0)
      return 0;
    s1++;
    s2++;
  }
  return 
    (int)tolower((unsigned char)*s1) - 
    (int)tolower((unsigned char)*s2) ;
}

/** ------------------------------------------------------------------
 ** @brief Process next option
 **
 ** @param args     MEX argument array.
 ** @param nargs    MEX argument array length.
 ** @param options  List of option definitions.
 ** @param next     Pointer to the next option (in and out).
 ** @param optarg   Pointer to the option optional argument (out).
 **
 ** The function scans the MEX driver arguments array @a args of @a
 ** nargs elements for the next option starting at location @a next.
 **
 ** This argument is supposed to be the name of an option (case
 ** insensitive). The option is looked up in the option table @a
 ** options and decoded as the value uMexOption::val. Furthermore, if
 ** uMexOption::has_arg is true, the next entry in the array @a args
 ** is assumed to be argument of the option and stored in @a
 ** optarg. Finally, @a next is advanced to point to the next option.
 **
 ** @return the code of the option, or -1 if the argument list is
 ** exhausted. In case of an error (e.g. unknown option) the function
 ** prints an error message and quits the MEX file.
 **/

static int uNextOption(mxArray const *args[], int nargs, 
                       uMexOption const *options, 
                       int *next, 
                       mxArray const **optarg)
{
  char err_msg [1024] ;
  char name    [1024] ;
  int opt = -1, i, len ;

  if (*next >= nargs) {
    return opt ;
  }
  
  /* check the array is a string */
  if (! uIsString (args [*next], -1)) {
    snprintf(err_msg, sizeof(err_msg),
             "The option name is not a string (argument number %d).",
             *next + 1) ;
    mexErrMsgTxt(err_msg) ;
  }

  /* retrieve option name */
  len = mxGetNumberOfElements (args [*next]) ;
  
  if (mxGetString (args [*next], name, sizeof(name))) {
    snprintf(err_msg, sizeof(err_msg),
             "The option name is too long (argument number %d).", 
             *next + 1) ;
    mexErrMsgTxt(err_msg) ;    
  }
  
  /* advance argument list */
  ++ (*next) ;
        
  /* now lookup the string in the option table */
  for (i = 0 ; options[i].name != 0 ; ++i) {    
    if (uStrICmp(name, options[i].name) == 0) {
      opt = options[i].val ;
      break ;
    }
  }
  
  /* unknown argument */
  if (opt < 0) {
    snprintf(err_msg, sizeof(err_msg),
             "Unknown option '%s'.", name) ;
    mexErrMsgTxt(err_msg) ;
  }

  /* no argument */
  if (! options [i].has_arg) {
    if (optarg) *optarg = 0 ;
    return opt ;
  }
  
  /* argument */
  if (*next >= nargs) {
    snprintf(err_msg, sizeof(err_msg),
             "Option '%s' requires an argument.", options[i].name) ;
    mexErrMsgTxt(err_msg) ;        
  }

  if (optarg) *optarg = args [*next] ;
  ++ (*next) ;
  return opt ;  
}
