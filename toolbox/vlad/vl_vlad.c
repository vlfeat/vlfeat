/** @file   vl_vlad.c
 ** @brief  vl_vlad MEX definition.
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/vlad.h>
#include <mexutils.h>
#include <string.h>
#include <stdio.h>

enum {
  opt_verbose,
  opt_normalize,
  opt_multithreading
} ;


vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"Normalize",         0,   opt_normalize           },
  {"Multithreading",    0,   opt_multithreading      }
} ;

/* driver */
void
mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_MEANS, IN_ASSIGN, IN_END} ;
  enum {OUT_ENC} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  mxArray const * means_array =      in[IN_MEANS] ;
  mxArray const * data_array =       in[IN_DATA] ;
  mxArray const * assign_array =     in[IN_ASSIGN] ;

  vl_size numClusters ;
  vl_size dimension ;
  vl_size numData ;
  vl_bool normalize = VL_FALSE;
  VlVLADMultithreading multithreading = VlVLADParallel;

  void * means = NULL;
  void * assignments = NULL;
  void * data = NULL ;
  void * enc = NULL;

  int verbosity = 0 ;

  vl_type dataType ;
  mxClassID classID ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError (vlmxErrInvalidArgument,
               "At least three arguments required.");
  }

  classID = mxGetClassID (IN(DATA)) ;
  switch (classID) {
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
      mxGetClassID (IN(ASSIGN)) != classID ) {
    vlmxError (vlmxErrInvalidArgument,
               "MEANS and ASSIGNMENTS must be of same class as DATA.") ;
  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;
  numClusters = mxGetN (IN(MEANS)) ;


  if (dimension == 0) {
    vlmxError (vlmxErrInvalidArgument, "SIZE(DATA,1) is zero") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;
    switch (opt) {
      case opt_verbose :
        ++ verbosity ;
        break ;
      case opt_normalize :
        normalize = VL_TRUE ;
        break ;
    case opt_multithreading :
      if (!vlmxIsString (optarg, -1))
      {
        vlmxError (vlmxErrInvalidArgument,
                   "MULTITHREADING must be a string.") ;
      }
      if (mxGetString (optarg, buf, sizeof(buf)))
      {
        vlmxError (vlmxErrInvalidArgument,
                   "MULTITHREADING argument too long.") ;
      }

      if (vlmxCompareStringsI("serial", buf) == 0)
      {
        multithreading = VlVLADSerial ;
      }
      else if (vlmxCompareStringsI("parallel", buf) == 0)
      {
        multithreading = VlVLADParallel ;
      }
      else
      {
        vlmxError (vlmxErrInvalidArgument,
                   "Invalid value %s for MULTITHREADING.", buf) ;
      }
      break ;
    default :
      abort() ;
      break ;
    }

  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  data = mxGetPr(data_array);
  means = mxGetPr(means_array);
  assignments = mxGetPr(assign_array);

  if (verbosity) {
    mexPrintf("VLAD encoding ...\n") ;
  }

  /* -------------------------------------------------------------- */
  /*                                                       Encoding */
  /* -------------------------------------------------------------- */

  switch(dataType) {
    case VL_TYPE_FLOAT:
      enc = (void*)vl_malloc(sizeof(float) * dimension * numClusters);
      break;
    case VL_TYPE_DOUBLE:
      enc = (void*)vl_malloc(sizeof(double) * dimension * numClusters);
      break;
    default:
      abort();
      break;
  }

  vl_vlad_encode
  (dataType,
   data,
   means,
   assignments,
   enc,
   dimension,
   numData,
   numClusters,
   normalize,
   multithreading);

  OUT(ENC) = mxCreateNumericMatrix (dimension, numClusters, classID, mxREAL) ;

  /* copy encodings */
  memcpy (mxGetData(OUT(ENC)),
          enc,
          vl_get_type_size (dataType) * dimension * numClusters) ;


}
