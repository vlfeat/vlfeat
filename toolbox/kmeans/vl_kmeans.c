/** @file   vl_kmeans.c
 ** @brief  vl_kmeans MEX definition.
 ** @author Andrea Vedaldi
 **/

/* AUTORIGHTS */

#include <vl/kmeans.h>
#include <mexutils.h>
#include <string.h>

enum {
  opt_max_num_iterations,
  opt_algorithm,
  opt_distance,
  opt_initialization,
  opt_verbose
} ;

enum {
  INIT_RANDSEL,
  INIT_PLUSPLUS
} ;

vlmxOption  options [] = {
  {"MaxNumIterations",  1,   opt_max_num_iterations  },
  {"Algorithm",         1,   opt_algorithm           },
  {"Distance",          1,   opt_distance            },
  {"Verbose",           0,   opt_verbose             },
  {"Initialization",    1,   opt_initialization      },
  {"Initialisation",    1,   opt_initialization      }, /* UK spelling */
  {0,                   0,   0                       }
} ;

/* driver */
void
mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_DATA = 0, IN_NUMCENTERS, IN_END} ;
  enum {OUT_CENTERS = 0, OUT_ASSIGNMENTS, OUT_ENERGY} ;

  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  vl_size numCenters ;
  vl_size dimension ;
  vl_size numData ;

  void const * data = NULL ;

  VlKMeansAlgorithm algorithm = VlKMeansLLoyd ;
  VlVectorComparisonType distance = VlDistanceL2 ;
  int maxNumIterations = 100 ;
  int verbosity = 0 ;
  int initialization = INIT_PLUSPLUS ;

  int dataType ;
  mxClassID classID ;

  VlKMeans * kmeans ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    vlmxError (vlmxErrInvalidArgument,
              "At least two arguments required.");
  }
  else if (nout > 3) {
    vlmxError (vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  classID = mxGetClassID (IN(DATA)) ;
  switch (classID) {
    case mxSINGLE_CLASS: dataType = VL_TYPE_FLOAT ; break ;
    case mxDOUBLE_CLASS: dataType = VL_TYPE_DOUBLE ; break ;
    default:
      vlmxError (vlmxErrInvalidArgument,
                "DATA must be of class SINGLE or DOUBLE") ;
      assert(0) ;
  }

  dimension = mxGetM (IN(DATA)) ;
  numData = mxGetN (IN(DATA)) ;

  if (!vlmxIsPlainScalar(IN(NUMCENTERS)) ||
      (numCenters = (vl_size) mxGetScalar(IN(NUMCENTERS))) < 1  ||
      numCenters > numData) {
    vlmxError (vlmxErrInvalidArgument,
              "NUMCENTERS must be a positive integer not greater "
              "than the number of data.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;

    switch (opt) {

      case opt_verbose :
        ++ verbosity ;
        break ;

      case opt_max_num_iterations :
        if (!vlmxIsPlainScalar(optarg) ||
            (maxNumIterations = (vl_size) mxGetScalar(optarg)) < 0) {
          vlmxError (vlmxErrInvalidArgument,
                    "MAXNUMITERATIONS must be a non-negative integer scalar") ;
        }
        break ;

      case opt_algorithm :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                    "ALGORITHM must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf))) {
          vlmxError (vlmxErrInvalidArgument,
                    "ALGORITHM argument too long.") ;
        }
        if (uStrICmp("lloyd", buf) == 0) {
          algorithm = VlKMeansLLoyd ;
        } else if (uStrICmp("elkan", buf) == 0) {
          algorithm = VlKMeansElkan ;
        } else if (uStrICmp("ann", buf) == 0) {
          algorithm = VlKMeansANN ;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                    "Invalid value %s for ALGORITHM", buf) ;
        }
        break ;

      case opt_initialization :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                    "INITLAISATION must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf))) {
          vlmxError (vlmxErrInvalidArgument,
                    "INITIALISATION argument too long.") ;
        }
        if (uStrICmp("plusplus", buf) == 0 ||
            uStrICmp("++", buf) == 0) {
          initialization = INIT_PLUSPLUS ;
        } else if (uStrICmp("randsel", buf) == 0) {
          initialization = INIT_RANDSEL ;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                    "Invalid value %s for INITIALISATION", buf) ;
        }
        break ;

      case opt_distance :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                    "DISTANCE must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf))) {
          vlmxError (vlmxErrInvalidArgument,
                    "DISTANCE argument too long.") ;
        }
        if (uStrICmp("l2", buf) == 0) {
          distance = VlDistanceL2 ;
        } else if (uStrICmp("l1", buf) == 0) {
          distance = VlDistanceL1 ;
        } else if (uStrICmp("chi2", buf) == 0) {
          distance = VlDistanceChi2 ;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                    "Invalid value %s for DISTANCE", buf) ;
        }
        break ;

      default :
        assert(0) ;
        break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  if (verbosity) {
    char const * algorithmName = 0 ;
    switch (algorithm) {
      case VlKMeansLLoyd: algorithmName = "Lloyd" ; break ;
      case VlKMeansElkan: algorithmName = "Elkan" ; break ;
      case VlKMeansANN:   algorithmName = "ANN" ; break ;
      default :
        assert (0) ;
    }
    mexPrintf("kmeans: MaxNumIterations = %d\n", maxNumIterations) ;
    mexPrintf("kmeans: Algorithm = %s\n", algorithmName) ;
  }

  data = mxGetPr(IN(DATA)) ;

  kmeans = vl_kmeans_new (algorithm, distance, dataType) ;

  vl_kmeans_set_verbosity (kmeans, verbosity) ;
  vl_kmeans_set_max_num_iterations (kmeans, maxNumIterations) ;
  switch (initialization) {
    case INIT_RANDSEL:
      vl_kmeans_seed_centers_with_rand_data (kmeans,
                                             data, dimension, numData,
                                             numCenters) ;
      break ;
    case INIT_PLUSPLUS:
      vl_kmeans_seed_centers_plus_plus (kmeans,
                                        data, dimension, numData,
                                        numCenters) ;
      break ;
    default:
      assert(0) ;
  }
  if (verbosity) {
    mexPrintf("kmeans: data type = %s\n",
              vl_get_type_name(vl_kmeans_get_data_type(kmeans))) ;
    mexPrintf("kmeans: distance = %s\n",
              vl_get_vector_comparison_type_name(vl_kmeans_get_distance(kmeans))) ;
    mexPrintf("kmeans: data dimension = %d\n", vl_kmeans_get_dimension(kmeans)) ;
    mexPrintf("kmeans: num. data points = %d\n", numData) ;
    mexPrintf("kmeans: num. centers = %d\n", vl_kmeans_get_num_centers(kmeans)) ;
    mexPrintf("kmeans: initialization = ") ;
    switch (initialization) {
      case INIT_PLUSPLUS : mexPrintf("plusplus") ; break ;
      case INIT_RANDSEL : mexPrintf("randsel") ; break ;
    }
    mexPrintf("\n") ;
  }

  vl_tic() ;
  vl_kmeans_train (kmeans, data, numData) ;
  if (verbosity) {
    mexPrintf("kmeans: trained in %.2f s\n", vl_toc()) ;
  }

  /* -----------------------------------------------------------------
   *                                                    Return results
   * -------------------------------------------------------------- */

  {
    switch (vl_kmeans_get_data_type(kmeans)) {
      case VL_TYPE_FLOAT:
        OUT(CENTERS) = mxCreateNumericMatrix (dimension, numCenters,
                                              mxSINGLE_CLASS, mxREAL) ;
        memcpy (mxGetData (OUT(CENTERS)),
                vl_kmeans_get_centers (kmeans),
                sizeof(float) * dimension * vl_kmeans_get_num_centers(kmeans)) ;
        break ;
      case VL_TYPE_DOUBLE:
        OUT(CENTERS) = mxCreateNumericMatrix (dimension, numCenters,
                                              mxDOUBLE_CLASS, mxREAL) ;
        memcpy (mxGetData (OUT(CENTERS)),
                vl_kmeans_get_centers (kmeans),
                sizeof(double) * dimension * vl_kmeans_get_num_centers(kmeans)) ;
        break ;
    }
  }

  if (nout > 1) {
    vl_uindex j ;
    vl_uint32 * assignments  ;
    OUT(ASSIGNMENTS) = mxCreateNumericMatrix (1, numData, mxUINT32_CLASS, mxREAL) ;
    assignments = mxGetData (OUT(ASSIGNMENTS)) ;

    vl_kmeans_push (kmeans, assignments, NULL, data, numData) ;

    /* use MATLAB indexing convention */
    for (j = 0 ; j < numData ; ++j) { assignments[j] += 1 ; }
  }

  if (nout > 2) {
    OUT(ENERGY) = vlmxCreatePlainScalar (vl_kmeans_get_energy (kmeans)) ;
  }

  vl_kmeans_delete (kmeans) ;
}

