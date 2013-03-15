#include <mexutils.h>
#include <vl/liop.h>

#include <assert.h>


/* option codes */
enum {
  opt_neighbours,
  opt_bins,
  opt_radius,
  opt_weightThreshold,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
{"Neighbours",       1,   opt_neighbours       },
{"Bins",             1,   opt_bins             },
{"Radius",           1,   opt_radius           },
{"WeightThreshold",  1,   opt_weightThreshold  },
{"Verbose",          0,   opt_verbose          },
{0,                  0,   0                    }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_END} ;
  enum {OUT_DESCRIPTOR = 0} ;

  int                verbose = 0 ;
  int                opt ;
  int                next = IN_END ;
  mxArray const     *optarg ;

  float              *data ;
  int                M, N ;

  int                neighbours = 4 ;
  int                bins = 6 ;
  float              radius = 6.0 ;
  float              weightThreshold = NO_VALUE;
  int                numOfPatches = 1;


  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 1) {
    vlmxError(vlmxErrNotEnoughInputArguments, NULL) ;
  } else if (nout > 2) {
    vlmxError(vlmxErrTooManyOutputArguments, NULL) ;
  }

  if (mxGetClassID(in[IN_I]) != mxSINGLE_CLASS ) {
    vlmxError(vlmxErrInvalidArgument,
              "I must be a matrix of class SINGLE.") ;
  }

  if(mxGetNumberOfDimensions (in[IN_I]) == 2){
      numOfPatches = 1;
  }else{
      vlmxError(vlmxErrInvalidArgument,
                "I must be a matrix of dimension 2.") ;
  }


  data = (float*) mxGetData (in[IN_I]) ;
  M    = mxGetM (in[IN_I]) ;
  N    = mxGetN (in[IN_I]) ;

  if((N != M) || (N%2 == 0)){
      vlmxError(vlmxErrInvalidArgument,"Data must be a square matrix of odd side length.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

      case opt_verbose :
        ++ verbose ;
        break ;

      case opt_neighbours :
        if (!vlmxIsPlainScalar(optarg) || (neighbours = (int) *mxGetPr(optarg)) <= 0) {
            vlmxError(vlmxErrInvalidArgument,"number of NEIGHBOURS is not a scalar or it is negative.") ;
        }else if(neighbours%2 != 0){
            vlmxError(vlmxErrInvalidArgument,"Number of neighbours must be even.") ;
        }
        break ;

      case opt_bins :
        if (!vlmxIsPlainScalar(optarg) || (bins = (int) *mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument,"number of BINS must be a positive scalar.") ;
        }
        break ;

      case opt_radius :
        if ((radius = (int) *mxGetPr(optarg)) <= 0) {
          vlmxError(vlmxErrInvalidArgument,"RADIUS must be a positive number.") ;
        }
        break ;

      case opt_weightThreshold :
        if ((weightThreshold = (float) *mxGetPr(optarg)) < 0) {
          vlmxError(vlmxErrInvalidArgument,"WEIGHT THRESHOLD must be a positive number.") ;
        }
        break ;

      default :
        abort() ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  {

    VlLiopDesc *liop ;
    vl_uint *desc ;

    vl_int i;

    liop = vl_liopdesc_new (neighbours, bins, radius, weightThreshold, M) ;

    out[OUT_DESCRIPTOR] = mxCreateNumericMatrix(liop->liopArraySize, 1, mxSINGLE_CLASS, mxREAL);
    desc = mxGetData(out[OUT_DESCRIPTOR]) ;
    compute_liop_descriptor(liop, data, desc);

    if (verbose) {

      mexPrintf("vl_liop: patach size         [W, H] = [%d, %d]\n", N, M) ;
      mexPrintf("vl_liop: neighbours          %d\n", neighbours) ;
      mexPrintf("vl_liop: number of bins      %d\n", bins) ;
      mexPrintf("vl_liop: radius              %f\n", radius) ;
      mexPrintf("vl_liop: weighting threshold %f\n", liop->weightThreshold) ;
      mexPrintf("vl_liop: descriptor size:    %d\n", liop->liopArraySize) ;
      mexPrintf("\n") ;

      mexPrintf("vl_liop: number of points assigned to each bin:\n") ;

      for(i = 0; i < bins; i++){
          mexPrintf("vl_liop: bins %d             %d\n",i, liop->liopBins[i]->binSize) ;
      }

      mexPrintf("\n") ;

      mexPrintf("vl_liop: number of points used in each bin:\n") ;
      for(i = 0; i < bins; i++){
          mexPrintf("vl_liop: bins %d             %d\n",i, liop->liopBins[i]->numOfUsedPoints) ;
      }


    }




  }
}
