/** @file   vl_gmm.c
 ** @brief  vl_gmm MEX definition.
 ** @author David Novotny
 **/

/*
Copyright (C) 2013 David Novotny.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/gmm.h>
#include <mexutils.h>
#include <string.h>
#include <stdio.h>

enum
{
  opt_max_num_iterations,
  opt_distance,
  opt_initialization,
  opt_num_repetitions,
  opt_verbose,
  opt_means,
  opt_covariances,
  opt_priors,
  opt_covariance_bound
} ;

vlmxOption  options [] =
{
  {"MaxNumIterations",  1,   opt_max_num_iterations  },
  {"Verbose",           0,   opt_verbose             },
  {"NumRepetitions",    1,   opt_num_repetitions,    },
  {"Initialization",    1,   opt_initialization      },
  {"Initialisation",    1,   opt_initialization      }, /* UK spelling */
  {"InitMeans",         1,   opt_means               },
  {"InitCovariances",   1,   opt_covariances         },
  {"InitPriors",        1,   opt_priors              },
  {"CovarianceBound",   1,   opt_covariance_bound    },
  {0,                   0,   0                       }
} ;

/* driver */
void
mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_NUMCLUSTERS, IN_END} ;
  enum {OUT_MEANS, OUT_COVARIANCES, OUT_PRIORS, OUT_LL, OUT_POSTERIORS} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  vl_size i;

  vl_size numClusters = 10;
  vl_size dimension ;
  vl_size numData ;

  void * initCovariances = 0 ;
  void * initMeans = 0 ;
  void * initPriors = 0 ;

  double covarianceScalarBound = VL_NAN_D ;
  double const * covarianceBound = NULL ;
  void const * data = NULL ;

  vl_size maxNumIterations = 100 ;
  vl_size numRepetitions = 1 ;
  double LL ;
  int verbosity = 0 ;
  VlGMMInitialization initialization = VlGMMRand ;
  vl_bool initializationSet = VL_FALSE ;

  vl_type dataType ;
  mxClassID classID ;

  VlGMM * gmm ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2)
  {
    vlmxError (vlmxErrInvalidArgument,
               "At least two arguments required.");
  }
  else if (nout > 5)
  {
    vlmxError (vlmxErrInvalidArgument,
               "Too many output arguments.");
  }

  classID = mxGetClassID (IN(DATA)) ;
  switch (classID) {
    case mxSINGLE_CLASS: dataType = VL_TYPE_FLOAT ; break ;
    case mxDOUBLE_CLASS: dataType = VL_TYPE_DOUBLE ; break ;
    default:
      vlmxError (vlmxErrInvalidArgument,
                 "DATA is neither of class SINGLE or DOUBLE.") ;
      abort() ;
  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;

  if (dimension == 0)
  {
    vlmxError (vlmxErrInvalidArgument, "SIZE(DATA,1) is zero.") ;
  }

  if (!vlmxIsPlainScalar(IN(NUMCLUSTERS)) ||
      (numClusters = (vl_size) mxGetScalar(IN(NUMCLUSTERS))) < 1  ||
      numClusters > numData)
  {
    vlmxError (vlmxErrInvalidArgument,
               "NUMCLUSTERS must be a positive integer not greater "
               "than the number of data.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0)
  {
    char buf [1024] ;

    switch (opt)
    {
      case opt_verbose : ++ verbosity ; break ;

      case opt_max_num_iterations :
        if (!vlmxIsPlainScalar(optarg) || mxGetScalar(optarg) < 0) {
          vlmxError (vlmxErrInvalidArgument,
                     "MAXNUMITERATIONS must be a non-negative integer scalar") ;
        }
        maxNumIterations = (vl_size) mxGetScalar(optarg) ;
        break ;

      case opt_covariance_bound :
        if (vlmxIsPlainScalar(optarg)) {
          covarianceScalarBound = mxGetScalar(optarg) ;
          continue ;
        }
        if (!vlmxIsPlainVector(optarg,dimension)) {
          vlmxError (vlmxErrInvalidArgument,
                     "COVARIANCEBOUND must be a DOUBLE vector of size "
                     "equal to the dimension of the data X.") ;
        }
        covarianceBound = mxGetPr(optarg) ;
        break ;

      case opt_priors : {
        if (mxGetClassID (optarg) != mxGetClassID(IN(DATA))) {
          vlmxError (vlmxErrInvalidArgument, "INITPRIORS is not of the same class as the data X.") ;
        }
        if (! vlmxIsVector (optarg, numClusters) || ! vlmxIsReal (optarg)) {
          vlmxError(vlmxErrInvalidArgument, "INITPRIORS is not a real vector or does not have the correct size.") ;
        }
        initPriors = mxGetPr(optarg) ;
        break ;
      }

      case opt_means : {
        if (mxGetClassID (optarg) != mxGetClassID(IN(DATA))) {
          vlmxError (vlmxErrInvalidArgument, "INITMEANS is not of the same class as the data X.") ;
        }
        if (! vlmxIsMatrix (optarg, dimension, numClusters) || ! vlmxIsReal (optarg)) {
          vlmxError(vlmxErrInvalidArgument, "INITMEANS is not a real matrix or does not have the correct size.") ;
        }
        initMeans = mxGetPr(optarg) ;
        break;
      }

      case opt_covariances : {
        if (mxGetClassID (optarg) != mxGetClassID(IN(DATA))) {
          vlmxError (vlmxErrInvalidArgument, "INITCOVARIANCES is not of the same class as the data X.") ;
        }
        if (! vlmxIsMatrix (optarg, dimension, numClusters) || ! vlmxIsReal (optarg)) {
          vlmxError(vlmxErrInvalidArgument, "INITCOVARIANCES is not a real matrix or does not have the correct size.") ;
        }
        initCovariances = mxGetPr(optarg) ;
        break;
      }

      case opt_initialization :
        if (!vlmxIsString (optarg, -1))
        {
          vlmxError (vlmxErrInvalidArgument,
                     "INITLAIZATION must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf)))
        {
          vlmxError (vlmxErrInvalidArgument,
                     "INITIALIZATION argument too long.") ;
        }
        if (vlmxCompareStringsI("rand", buf) == 0) {
          initialization = VlGMMRand ;
        }
        else if (vlmxCompareStringsI("custom", buf) == 0) {
          initialization = VlGMMCustom ;
        }
        else if (vlmxCompareStringsI("kmeans", buf) == 0) {
          initialization = VlGMMKMeans ;
        }
        else {
          vlmxError (vlmxErrInvalidArgument,
                     "Invalid value '%s' for INITIALIZATION.", buf) ;
        }
        initializationSet = VL_TRUE ;
        break ;

      case opt_num_repetitions :
        if (!vlmxIsPlainScalar (optarg)) {
          vlmxError (vlmxErrInvalidArgument,
                     "NUMREPETITIONS is not a scalar.") ;
        }
        if (mxGetScalar (optarg) < 1) {
          vlmxError (vlmxErrInvalidArgument,
                     "NUMREPETITIONS is not larger than or equal to 1.") ;
        }
        numRepetitions = (vl_size) mxGetScalar (optarg) ;
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

  switch(dataType){
    case VL_TYPE_DOUBLE:
      for(i = 0; i < numData*dimension; i++) {
        double datum = *((double*)data + i);
        if(!(datum < VL_INFINITY_D && datum > -VL_INFINITY_D)){
          vlmxError (vlmxErrInvalidArgument,
                     "DATA contains NaNs or Infs.") ;
        }
      }
      break;
    case VL_TYPE_FLOAT:
      for(i = 0; i < numData*dimension; i++) {
        float datum = *((float*)data + i);
        if(!(datum < VL_INFINITY_F && datum > -VL_INFINITY_F)){
          vlmxError (vlmxErrInvalidArgument,
                     "DATA contains NaNs or Infs.") ;
        }
      }
      break;
    default:
      abort();
      break;
  }

  if (initPriors || initMeans || initCovariances) {
    if (!initPriors || !initMeans || !initCovariances) {
      vlmxError (vlmxErrInvalidArgument,
                 "All or none of INITPRIORS, INITMEANS, "
                 "INITCOVARIANCES must be set.") ;
    }
    if (initializationSet && initialization != VlGMMCustom) {
      vlmxError (vlmxErrInvalidArgument,
                 "INITPRIORS, INITMEANS, and "
                 "INITCOVARIANCES requires 'custom' INITALIZATION.") ;
    }
    initialization = VlGMMCustom ;
  }

  gmm = vl_gmm_new (dataType, dimension, numClusters) ;
  vl_gmm_set_verbosity (gmm, verbosity) ;
  vl_gmm_set_num_repetitions (gmm, numRepetitions) ;
  vl_gmm_set_max_num_iterations (gmm, maxNumIterations) ;
  vl_gmm_set_initialization (gmm, initialization) ;

  if (!vl_is_nan_d(covarianceScalarBound)) {
    vl_gmm_set_covariance_lower_bound (gmm, covarianceScalarBound) ;
  }
  if (covarianceBound) {
    vl_gmm_set_covariance_lower_bounds (gmm, covarianceBound) ;
  }
  if (initPriors) {
    vl_gmm_set_priors(gmm, initPriors) ;
  }
  if (initMeans) {
    vl_gmm_set_means(gmm, initMeans) ;
  }
  if (initCovariances) {
    vl_gmm_set_covariances(gmm, initCovariances) ;
  }

  if (verbosity) {
    char const * initializationName = 0 ;

    switch (vl_gmm_get_initialization(gmm)) {
      case VlGMMRand : initializationName = "rand" ; break ;
      case VlGMMKMeans : initializationName = "kmeans" ; break ;
      case VlGMMCustom : initializationName = "custom" ; break ;
      default: abort() ;
    }

    mexPrintf("vl_gmm: initialization = %s\n", initializationName) ;
    mexPrintf("vl_gmm: maxNumIterations = %d\n", vl_gmm_get_max_num_iterations(gmm)) ;
    mexPrintf("vl_gmm: numRepetitions = %d\n", vl_gmm_get_num_repetitions(gmm)) ;
    mexPrintf("vl_gmm: data type = %s\n", vl_get_type_name(vl_gmm_get_data_type(gmm))) ;
    mexPrintf("vl_gmm: data dimension = %d\n", dimension) ;
    mexPrintf("vl_gmm: num. data points = %d\n", numData) ;
    mexPrintf("vl_gmm: num. Gaussian modes = %d\n", numClusters) ;
    mexPrintf("vl_gmm: lower bound on covariance = [") ;
    if (dimension < 3) {
      for (i = 0 ; i < dimension ; ++i) {
        mexPrintf(" %f", vl_gmm_get_covariance_lower_bounds(gmm)[i]) ;
      }
    } else {
      mexPrintf(" %f %f ... %f",
                vl_gmm_get_covariance_lower_bounds(gmm)[0],
                vl_gmm_get_covariance_lower_bounds(gmm)[1],
                vl_gmm_get_covariance_lower_bounds(gmm)[dimension-1]) ;
    }
    mexPrintf("]\n") ;
  }

  /* -------------------------------------------------------------- */
  /*                                                     Clustering */
  /* -------------------------------------------------------------- */

  LL = vl_gmm_cluster(gmm, data, numData) ;

  /* copy centers */
  OUT(MEANS) = mxCreateNumericMatrix (dimension, numClusters, classID, mxREAL) ;
  OUT(COVARIANCES) = mxCreateNumericMatrix (dimension, numClusters, classID, mxREAL) ;
  OUT(PRIORS) = mxCreateNumericMatrix (numClusters, 1, classID, mxREAL) ;
  OUT(POSTERIORS) = mxCreateNumericMatrix (numClusters, numData, classID, mxREAL) ;

  memcpy (mxGetData(OUT(MEANS)),
          vl_gmm_get_means (gmm),
          vl_get_type_size (dataType) * dimension * vl_gmm_get_num_clusters(gmm)) ;

  memcpy (mxGetData(OUT(COVARIANCES)),
          vl_gmm_get_covariances (gmm),
          vl_get_type_size (dataType) * dimension * vl_gmm_get_num_clusters(gmm)) ;

  memcpy (mxGetData(OUT(PRIORS)),
          vl_gmm_get_priors (gmm),
          vl_get_type_size (dataType) * vl_gmm_get_num_clusters(gmm)) ;

  /* optionally return loglikelihood */
  if (nout > 3) {
    OUT(LL) = vlmxCreatePlainScalar (LL) ;
  }

  /* optionally return posterior probabilities */
  if (nout > 4) {
    memcpy (mxGetData(OUT(POSTERIORS)),
            vl_gmm_get_posteriors (gmm),
            vl_get_type_size (dataType) * numData * vl_gmm_get_num_clusters(gmm)) ;
  }

  vl_gmm_delete (gmm) ;
}
