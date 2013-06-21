/** @file   vl_fisher.c
 ** @brief  vl_fisher MEX definition.
 ** @author Andrea Vedaldi
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/fisher.h>
#include <mexutils.h>
#include <string.h>
#include <stdio.h>

enum
{
  opt_verbose,
  opt_multithreading
} ;


vlmxOption  options [] =
{
  {"Verbose",           0,   opt_verbose             },
  {"Multithreading",    1,   opt_multithreading      }
} ;

/* driver */
void
mexFunction (int nout VL_UNUSED, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_MEANS, IN_SIGMAS, IN_WEIGHTS, IN_END} ;
  enum {OUT_ENC} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;
  
  vl_size numClusters = 10;
  vl_size dimension ;
  vl_size numData ;

  void * sigmas = NULL;
  void * means = NULL;
  void * weights = NULL;
  void * data = NULL ;
  void * enc = NULL;

  int verbosity = 0 ;

  vl_type dataType ;
  mxClassID classID ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 4)
  {
    vlmxError (vlmxErrInvalidArgument,
               "At least four arguments required.");
  }

  classID = mxGetClassID (IN(DATA)) ;
  switch (classID)
  {
  case mxSINGLE_CLASS:
    dataType = VL_TYPE_FLOAT ;
    break ;
  case mxDOUBLE_CLASS:
    dataType = VL_TYPE_DOUBLE ;
    break ;
  default:
    vlmxError (vlmxErrInvalidArgument,
               "DATA must be of class SINGLE or DOUBLE") ;
    abort() ;
  }

  if( mxGetClassID (IN(MEANS)) != classID ||
      mxGetClassID (IN(SIGMAS)) != classID ||
      mxGetClassID (IN(WEIGHTS)) != classID) {
    vlmxError (vlmxErrInvalidArgument,
               "MEANS, SIGMAS, WEIGHTS must be of same class as DATA.") ;
  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;
  numClusters = mxGetN (IN(MEANS)) ;

  if (dimension == 0)
  {
    vlmxError (vlmxErrInvalidArgument, "SIZE(DATA,1) is zero") ;
  }

  if(mxGetM(IN(DATA)) != mxGetM(IN(MEANS)) || mxGetM(IN(SIGMAS)) != mxGetM(IN(DATA))) {
    vlmxError (vlmxErrInvalidArgument,"MEANS, SIGMAS, must have the same dimensionality as DATA.") ;
  }

  if(mxGetM(IN(WEIGHTS)) != mxGetN(IN(MEANS)) || mxGetM(IN(WEIGHTS)) != mxGetN(IN(SIGMAS))) {
    vlmxError (vlmxErrInvalidArgument,"WEIGHTS mus have the same number of rows as MEANS and SIGMAS columns.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0)
  {
    switch (opt)
    {
      case opt_verbose :
        ++ verbosity ;
        break ;
      default :
        abort() ;
        break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  data = mxGetPr(IN(DATA)) ;
  means = mxGetPr(IN(MEANS)) ;
  sigmas = mxGetPr(IN(SIGMAS)) ;
  weights = mxGetPr(IN(WEIGHTS)) ;

  if (verbosity)
  {
      mexPrintf("Fisher encoding ...\n") ;
  }

  /* -------------------------------------------------------------- */
  /*                                                       Encoding */
  /* -------------------------------------------------------------- */

  switch(dataType){
      case VL_TYPE_FLOAT:
          enc = (void*)vl_malloc(sizeof(float) * dimension * numClusters * 2);
          break;
      case VL_TYPE_DOUBLE:
          enc = (void*)vl_malloc(sizeof(double) * dimension * numClusters * 2);
          break;
      default:
          abort();
          break;
  }

  vl_fisher_encode (dataType,
                    data,
                    means,
                    sigmas,
                    weights,
                    enc,
                    dimension,
                    numData,
                    numClusters) ;

  OUT(ENC) = mxCreateNumericMatrix (dimension * numClusters * 2, 1, classID, mxREAL) ;

  /* copy encodings */
  memcpy (mxGetData(OUT(ENC)),
          enc,
          vl_get_type_size (dataType) * 2 * dimension * numClusters) ;


}
