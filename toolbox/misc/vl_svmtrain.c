/** @file vl_svmtrain.c
 ** @brief vl_svmtrain MEX definition
 ** @author Daniele Perrone
 ** @author Milan Sulc
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2013 Milan Sulc
Copyright (C) 2011-13 Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/svm.h>
#include <vl/mathop.h>
#include <vl/homkermap.h>
#include <vl/stringop.h>
#include <assert.h>
#include <string.h>

/* option codes */
enum {
  /* common */
  opt_epsilon,
  opt_max_num_iterations,
  opt_bias_multiplier,
  opt_diagnostic_function,
  opt_diagnostic_frequency,
  opt_validation_subset,
  opt_loss,
  opt_model,
  opt_bias,
  opt_weights,

  /* switching to SDCA */
  opt_verbose,
  opt_solver,

  /* SGD specific */
  opt_starting_iteration,
  opt_bias_learning_rate

  /* DCA specific */
};


/* options */
vlmxOption  options [] = {
  {"Epsilon",             1,   opt_epsilon             },
  {"MaxNumIterations",    1,   opt_max_num_iterations  },
  {"BiasMultiplier",      1,   opt_bias_multiplier     },
  {"DiagnosticFunction",  1,   opt_diagnostic_function },
  {"DiagnosticFrequency", 1,   opt_diagnostic_frequency},
  {"ValidationSubset",    1,   opt_validation_subset   },
  {"Loss",                1,   opt_loss                },
  {"Verbose",             0,   opt_verbose             },
  {"Solver",              1,   opt_solver              },
  {"Model",               1,   opt_model               },
  {"Bias",                1,   opt_bias                },
  {"Weights",             1,   opt_weights             },

  /* SGD specific */
  {"StartingIteration",   1,   opt_starting_iteration  },
  {"BiasLearningRate",    1,   opt_bias_learning_rate  },

  /* DCA specific */
  {0,                     0,   0                       }
} ;

mxArray * createScalarStructArray(void const **fields)
{
  void const **iter ;
  char const **niter ;
  char const **names ;
  vl_size numFields = 0 ;
  mxArray * s ;
  mwSize dims [] = {1, 1} ;

  for (iter = fields ; *iter ; iter += 2) numFields++ ;

  names = vl_calloc(numFields, sizeof(char const*)) ;

  for (iter = fields, niter = names ; *iter ; iter += 2, niter++) {
    *niter = *iter ;
  }

  s = mxCreateStructArray(sizeof(dims)/sizeof(dims[0]),
                          dims,
                          (int)numFields,
                          names) ;
  for (iter = fields, niter = names ; *iter; iter += 2, niter++) {
    mxSetField(s, 0, *niter, (mxArray*)(*(iter+1))) ;
  }
  return s ;
}

/* ---------------------------------------------------------------- */
/*                                                 Parsing datasets */
/* ---------------------------------------------------------------- */

VlSvmDataset * parseDataset(const mxArray * dataset_array)
{
  VlSvmDataset * dataset ;
  {
    mxArray * data_array ;
    mxClassID dataClass ;
    vl_size dimension ;
    vl_size numData ;
    vl_type dataType ;
    if (! mxIsStruct(dataset_array)) {
      vlmxError(vlmxErrInvalidArgument, "DATASET is not a structure.") ;
    }
    if (mxGetNumberOfElements(dataset_array) != 1) {
      vlmxError(vlmxErrInvalidArgument, "DATASET is not a singleton.") ;
    }
    data_array = mxGetField(dataset_array, 0, "data") ;
    if (data_array == NULL) {
      vlmxError(vlmxErrInvalidArgument, "DATASET is missing the DATA field.") ;
    }
    if (!vlmxIsMatrix(data_array,-1,-1)) {
      vlmxError(vlmxErrInvalidArgument,"DATASET.DATA is not a matrix.") ;
    }
    dimension = mxGetM (data_array) ;
    numData = mxGetN (data_array) ;
    dataClass = mxGetClassID (data_array) ;

    if (dimension == 0 || numData == 0) {
      vlmxError(vlmxErrInvalidArgument, "DATASET.DATA is empty.") ;
    }

    switch (dataClass) {
      case mxSINGLE_CLASS : dataType = VL_TYPE_FLOAT ; break ;
      case mxDOUBLE_CLASS : dataType = VL_TYPE_DOUBLE ; break ;
      default:
        vlmxError(vlmxErrInvalidArgument, "DATASET.DATA is neither either SINGLE or DOUBLE.") ;
    }
    dataset = vl_svmdataset_new(dataType, mxGetData(data_array), dimension, numData) ;
  }

  /* homogeneous kernel map support */
  {
    VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
    VlHomogeneousKernelMapWindowType windowType = VlHomogeneousKernelMapWindowRectangular ;
    double gamma = 1.0 ;
    double period = -1 ;
    int n = 1 ;
    VlHomogeneousKernelMap * hom = NULL ;
    mxArray * hom_array ;
    mxArray * field ;

    hom_array = mxGetField(dataset_array, 0, "homkermap") ;
    if (hom_array != NULL)
    {
      if (!mxIsStruct(hom_array)) {
        vlmxError(vlmxErrInvalidArgument, "DATASET.HOMKERMAP is not a structure") ;
      }

      field = mxGetField(hom_array, 0, "order") ;
      if (field != NULL) {
        if (! vlmxIsPlainScalar(field)) {
          vlmxError(vlmxErrInvalidArgument, "DATASET.HOMKERMAP.ORDER is not a scalar.") ;
        }
        n = *mxGetPr(field) ;
        if (n < 0) {
          vlmxError(vlmxErrInvalidArgument, "DATASET.HOMKERMAP.ORDER is negative.") ;
        }
      }

      field = mxGetField(hom_array, 0, "kernel") ;
      if (field != NULL) {
        char buffer [1024] ;
        mxGetString(field, buffer, sizeof(buffer) / sizeof(char)) ;
        if (vl_string_casei_cmp("kl1", buffer) == 0) {
          kernelType = VlHomogeneousKernelIntersection ;
        } else if (vl_string_casei_cmp("kchi2", buffer) == 0) {
          kernelType = VlHomogeneousKernelChi2 ;
        } else if (vl_string_casei_cmp("kjs", buffer) == 0) {
          kernelType = VlHomogeneousKernelJS ;
        } else if (vl_string_casei_cmp("kinters", buffer) == 0) {
          kernelType = VlHomogeneousKernelIntersection ;
        } else {
          vlmxError(vlmxErrInvalidArgument, "DATASET.HOMKERMAP.KERNEL is not a recognized kernel type.") ;
        }
      }

      field = mxGetField(hom_array, 0, "window") ;
      if (field != NULL) {
        char buffer [1024] ;
        mxGetString(field, buffer, sizeof(buffer) / sizeof(char)) ;
        if (vl_string_casei_cmp("uniform", buffer) == 0) {
          windowType = VlHomogeneousKernelMapWindowUniform ;
        } else if (vl_string_casei_cmp("rectangular", buffer) == 0) {
          windowType = VlHomogeneousKernelMapWindowRectangular;
        } else {
          vlmxError(vlmxErrInvalidArgument, "DATASET.HOMKERMAP.WINDOW is not a recognized window type.") ;
        }
      }

      field = mxGetField(hom_array, 0, "gamma") ;
      if (field != NULL) {
        if (! vlmxIsPlainScalar(field)) {
          vlmxError(vlmxErrInvalidArgument, "GAMMA is not a scalar.") ;
        }
        gamma = *mxGetPr(field) ;
        if (gamma <= 0) {
          vlmxError(vlmxErrInvalidArgument, "GAMMA is not positive.") ;
        }
      }

      field = mxGetField(hom_array, 0, "period") ;
      if (field != NULL) {
        if (! vlmxIsPlainScalar(field)) {
          vlmxError(vlmxErrInvalidArgument, "PERIOD is not a scalar.") ;
        }
        period = *mxGetPr(field) ;
        if (period <= 0) {
          vlmxError(vlmxErrInvalidArgument, "PERIOD is not positive.") ;
        }
      }

      hom = vl_homogeneouskernelmap_new (kernelType, gamma, n, period, windowType) ;
      vl_svmdataset_set_homogeneous_kernel_map (dataset, hom) ;
    }
  }
  return dataset ;
}

/* ---------------------------------------------------------------- */
/*                                               Diagnostic helpers */
/* ---------------------------------------------------------------- */

mxArray * makeInfoStruct (VlSvm* svm)
{
  VlSvmStatistics const * s = vl_svm_get_statistics(svm) ;
  mxArray * info = 0 ;

  switch (vl_svm_get_solver(svm)) {
    case VlSvmSolverSdca:
    {
      void const * fields [] = {
        "solver", mxCreateString("sdca"),
        "lambda", vlmxCreatePlainScalar(vl_svm_get_lambda(svm)),
        "biasMultiplier", vlmxCreatePlainScalar(vl_svm_get_bias_multiplier(svm)),
        "bias", vlmxCreatePlainScalar(vl_svm_get_bias(svm)),
        "objective", vlmxCreatePlainScalar(s->objective),
        "regularizer", vlmxCreatePlainScalar(s->regularizer),
        "loss", vlmxCreatePlainScalar(s->loss),
        "dualObjective", vlmxCreatePlainScalar(s->dualObjective),
        "dualLoss", vlmxCreatePlainScalar(s->dualLoss),
        "dualityGap", vlmxCreatePlainScalar(s->dualityGap),
        "iteration", vlmxCreatePlainScalar(s->iteration),
        "epoch", vlmxCreatePlainScalar(s->epoch),
        "elapsedTime", vlmxCreatePlainScalar(s->elapsedTime),
        0, 0
      } ;
      info = createScalarStructArray(fields) ;
      break ;
    }

    case VlSvmSolverSgd:
    {
      void const * fields [] = {
        "solver", mxCreateString("sgd"),
        "lambda", vlmxCreatePlainScalar(vl_svm_get_lambda(svm)),
        "biasMultiplier", vlmxCreatePlainScalar(vl_svm_get_bias_multiplier(svm)),
        "bias", vlmxCreatePlainScalar(vl_svm_get_bias(svm)),
        "objective", vlmxCreatePlainScalar(s->objective),
        "regularizer", vlmxCreatePlainScalar(s->regularizer),
        "loss", vlmxCreatePlainScalar(s->loss),
        "scoreVariation", vlmxCreatePlainScalar(s->scoresVariation),
        "iteration", vlmxCreatePlainScalar(s->iteration),
        "epoch", vlmxCreatePlainScalar(s->epoch),
        "elapsedTime", vlmxCreatePlainScalar(s->elapsedTime),
        0, 0
      } ;
      info = createScalarStructArray(fields) ;
      break ;
    }

    case VlSvmSolverNone :
    {
      void const * fields [] = {
        "solver", mxCreateString("none"),
        "lambda", vlmxCreatePlainScalar(vl_svm_get_lambda(svm)),
        "biasMultiplier", vlmxCreatePlainScalar(vl_svm_get_bias_multiplier(svm)),
        "bias", vlmxCreatePlainScalar(vl_svm_get_bias(svm)),
        "objective", vlmxCreatePlainScalar(s->objective),
        "regularizer", vlmxCreatePlainScalar(s->regularizer),
        "loss", vlmxCreatePlainScalar(s->loss),
        "elapsedTime", vlmxCreatePlainScalar(s->elapsedTime),
        0, 0
      } ;
      info = createScalarStructArray(fields) ;
      break ;
    }

  default:
    assert(0) ;
  }
  return info ;
}

/* ---------------------------------------------------------------- */
/*                                          SVM diagnostic callback */
/* ---------------------------------------------------------------- */

typedef struct DiagnsoticOpts_
{
  vl_bool verbose ;
  mxArray const * matlabDiagonsticFunctionHandle ;
} DiagnosticOpts ;

void diagnostic (VlSvm * svm, DiagnosticOpts * opts)
{
  VlSvmStatistics const * s = vl_svm_get_statistics(svm) ;
  if ((opts->verbose && s->status != VlSvmStatusTraining) || (opts->verbose > 1)) {
    const char * statusName = 0 ;
    switch (s->status) {
      case VlSvmStatusTraining: statusName = "training" ; break ;
      case VlSvmStatusConverged: statusName = "converged" ; break ;
      case VlSvmStatusMaxNumIterationsReached: statusName = "max num iterations reached" ; break ;
    }
    mexPrintf("vl_svmtrain: iteration: %d (epoch: %d)\n", s->iteration+1, s->epoch+1) ;
    mexPrintf("\ttime elapsed: %f\n", s->elapsedTime) ;
    mexPrintf("\tobjective: %g (regul: %g, loss: %g)\n", s->objective, s->regularizer, s->loss) ;
    switch (vl_svm_get_solver(svm)) {
      case VlSvmSolverSgd:
        mexPrintf("\tscore variation: %f\n", s->scoresVariation) ;
        break;

      case VlSvmSolverSdca:
        mexPrintf("\tdual objective: %g (dual loss: %g)\n", s->dualObjective, s->dualLoss) ;
        mexPrintf("\tduality gap: %g\n", s->dualityGap) ;
        break;

      default:
        break;
    }
    mexPrintf("\tstatus: %s\n", statusName) ;
  }
  if (opts->matlabDiagonsticFunctionHandle) {
    mxArray *rhs[2] ;
    rhs[0] = (mxArray*) opts->matlabDiagonsticFunctionHandle ;
    rhs[1] = makeInfoStruct(svm) ;
    if (mxIsClass(rhs[0] , "function_handle")) {
      mexCallMATLAB(0,NULL,sizeof(rhs)/sizeof(rhs[0]),rhs,"feval") ;
    }
    mxDestroyArray(rhs[1]) ;
  }
}

/* ---------------------------------------------------------------- */
/*                                                  MEX entry point */
/* ---------------------------------------------------------------- */

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATASET = 0, IN_LABELS, IN_LAMBDA, IN_END} ;
  enum {OUT_MODEL = 0, OUT_BIAS, OUT_INFO, OUT_SCORES, OUT_END} ;

  vl_int opt, next;
  mxArray const *optarg ;

  VlSvmSolverType solver = VlSvmSolverSdca ;
  VlSvmLossType loss = VlSvmLossHinge ;
  int verbose = 0 ;
  VlSvmDataset * dataset ;
  double * labels ;
  double * weights = NULL ;
  double lambda ;

  double epsilon = -1 ;
  double biasMultipler = -1 ;
  vl_index maxNumIterations = -1 ;
  vl_index diagnosticFrequency = -1 ;
  mxArray const * matlabDiagnosticFunctionHandle = NULL ;

  mxArray const * initialModel_array = NULL ;
  double initialBias = VL_NAN_D ;
  vl_index startingIteration = -1 ;

  /* SGD */
  double sgdBiasLearningRate = -1 ;

  VL_USE_MATLAB_ENV ;

  if (nin < 3) {
    vlmxError(vlmxErrInvalidArgument, "At least three arguments are required.") ;
  }
  if (nout > OUT_END) {
    vlmxError(vlmxErrInvalidArgument, "Too many output arguments.");
  }

#define GET_SCALAR(NAME, variable) \
if (!vlmxIsPlainScalar(optarg)) { \
vlmxError(vlmxErrInvalidArgument, VL_STRINGIFY(NAME) " is not a plain scalar.") ; \
} \
variable = (double) *mxGetPr(optarg);

#define GET_NN_SCALAR(NAME, variable) GET_SCALAR(NAME, variable) \
if (variable < 0) { \
vlmxError(vlmxErrInvalidArgument, VL_STRINGIFY(NAME) " is negative.") ; \
}

  /* Mode 1: pass data, labels, lambda, and options */
  if (mxIsNumeric(in[IN_DATASET]))
  {
    mxArray const* samples_array = in[IN_DATASET] ;
    vl_size dimension ;
    vl_size numSamples ;
    void * data ;
    vl_type dataType ;

    if (!vlmxIsMatrix(samples_array, -1, -1)) {
      vlmxError (vlmxErrInvalidArgument,
                 "X is not a matrix.") ;
    }
    if (mxGetClassID(samples_array) == mxDOUBLE_CLASS) {
      dataType = VL_TYPE_DOUBLE ;
    } else if (mxGetClassID(samples_array) == mxSINGLE_CLASS) {
      dataType = VL_TYPE_FLOAT ;
    } else {
      vlmxError (vlmxErrInvalidArgument, "X is not of class SINGLE or DOUBLE.") ;
    }
    if (mxGetNumberOfElements(samples_array) == 0) {
      vlmxError (vlmxErrInvalidArgument, "X is empty.") ;
    }
    data = mxGetData(samples_array) ;
    dimension = mxGetM(samples_array) ;
    numSamples = mxGetN(samples_array) ;
    dataset = vl_svmdataset_new(dataType, data, dimension, numSamples) ;
  }
  /* Mode 2: pass dataset structure */
  else {
    dataset = parseDataset(in[IN_DATASET]) ;
  }

  {
    mxArray const* labels_array = in[IN_LABELS] ;
    if (!vlmxIsPlainMatrix(labels_array, -1, -1)) {
      vlmxError (vlmxErrInvalidArgument, "Y is not a plain matrix.") ;
    }
    labels = mxGetPr(labels_array) ;
    if (mxGetNumberOfElements(labels_array) != vl_svmdataset_get_num_data(dataset)) {
      vlmxError  (vlmxErrInvalidArgument,
                  "The number of labels Y is not the same as the number of data samples X.") ;
    }
    optarg = in[IN_LAMBDA] ;
    GET_NN_SCALAR(LAMBDA, lambda) ;
  }

  /* Parse optional arguments */
  next = 3 ;
  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;
    switch (opt) {
      case opt_verbose: verbose ++ ; break ;
      case opt_epsilon: GET_NN_SCALAR(EPSLON, epsilon) ; break ;
      case opt_bias_multiplier: GET_NN_SCALAR(BIASMULTIPLIER, biasMultipler) ; break ;
      case opt_max_num_iterations: GET_NN_SCALAR(MAXNUMITERATIONS, maxNumIterations) ; break ;
      case opt_diagnostic_frequency: GET_NN_SCALAR(DIAGNOSTICFREQUENCY, diagnosticFrequency) ; break ;
      case opt_diagnostic_function:
        if (!mxIsClass(optarg ,"function_handle")) {
          mexErrMsgTxt("DIAGNOSTICSFUNCTION is not a function handle.");
        }
        matlabDiagnosticFunctionHandle = optarg ;
        break ;

      case opt_solver :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                     "SOLVER must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf))) {
          vlmxError (vlmxErrInvalidArgument,
                     "SOLVER argument too long.") ;
        }
        if (vlmxCompareStringsI("sgd", buf) == 0) {
          solver = VlSvmSolverSgd ;
        } else if (vlmxCompareStringsI("sdca", buf) == 0) {
          solver = VlSvmSolverSdca ;
        } else if (vlmxCompareStringsI("none", buf) == 0) {
          solver = VlSvmSolverNone;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                     "Invalid value %s for SOLVER", buf) ;
        }
        break ;

      case opt_loss :
        if (!vlmxIsString (optarg, -1)) {
          vlmxError (vlmxErrInvalidArgument,
                     "LOSS must be a string.") ;
        }
        if (mxGetString (optarg, buf, sizeof(buf))) {
          vlmxError (vlmxErrInvalidArgument,
                     "LOSS argument too long.") ;
        }
        if (vlmxCompareStringsI("hinge", buf) == 0) {
          loss = VlSvmLossHinge ;
        } else if (vlmxCompareStringsI("hinge2", buf) == 0) {
          loss = VlSvmLossHinge2 ;
        } else if (vlmxCompareStringsI("l1", buf) == 0) {
          loss = VlSvmLossL1 ;
        } else if (vlmxCompareStringsI("l2", buf) == 0) {
          loss = VlSvmLossL2 ;
        } else if (vlmxCompareStringsI("logistic", buf) == 0) {
          loss = VlSvmLossLogistic ;
        } else {
          vlmxError (vlmxErrInvalidArgument,
                     "Invalid value %s for LOSS", buf) ;
        }
        break ;

      case opt_model :
        if (!vlmxIsPlainVector(optarg, vl_svmdataset_get_dimension(dataset))) {
          vlmxError(vlmxErrInvalidArgument, "MODEL is not a plain vector of size equal to the data dimension.") ;
        }
        initialModel_array = optarg ;
        break ;

      case opt_bias: GET_SCALAR(BIAS, initialBias) ; break ;

      case opt_weights:
        if (!vlmxIsPlainVector(optarg, vl_svmdataset_get_num_data(dataset))) {
          vlmxError(vlmxErrInvalidArgument, "WEIGHTS is not a plain vector of size equal to the number of training samples.") ;
        }
        weights = mxGetPr(optarg) ;
        break ;

      /* SGD specific */
      case opt_starting_iteration: GET_NN_SCALAR(STARTINGITERATION, startingIteration) ; break ;
      case opt_bias_learning_rate: GET_NN_SCALAR(BIASLEARNINGRATE, sgdBiasLearningRate) ; break ;

      /* DCA specific */
    } /* choose option */
  } /* next option */

  {
    VlSvm * svm = vl_svm_new_with_dataset(solver, dataset, labels, lambda) ;
    DiagnosticOpts dopts ;

    if (initialModel_array) {
      if (solver != VlSvmSolverNone && solver != VlSvmSolverSgd) {
        vlmxError(vlmxErrInvalidArgument, "MODEL cannot be specified with this type of solver.") ;
      }
      if (mxGetNumberOfElements(initialModel_array) != vl_svm_get_dimension(svm)) {
        vlmxError(vlmxErrInvalidArgument, "MODEL has not the same dimension as the data.") ;
      }
      vl_svm_set_model(svm, mxGetPr(initialModel_array)) ;
    }

    if (! vl_is_nan_d(initialBias)) {
      if (solver != VlSvmSolverNone && solver != VlSvmSolverSgd) {
        vlmxError(vlmxErrInvalidArgument, "BIAS cannot be specified with this type of solver.") ;
      }
      vl_svm_set_bias(svm, initialBias) ;
    }

    if (epsilon >= 0) vl_svm_set_epsilon(svm, epsilon) ;
    if (maxNumIterations >= 0) vl_svm_set_max_num_iterations(svm, maxNumIterations) ;
    if (biasMultipler >= 0) vl_svm_set_bias_multiplier(svm, biasMultipler) ;
    if (sgdBiasLearningRate >= 0) vl_svm_set_bias_learning_rate(svm, sgdBiasLearningRate) ;
    if (diagnosticFrequency >= 0) vl_svm_set_diagnostic_frequency(svm, diagnosticFrequency) ;
    if (startingIteration >= 0) vl_svm_set_iteration_number(svm, (unsigned)startingIteration) ;
    if (weights) vl_svm_set_weights(svm, weights) ;
    vl_svm_set_loss (svm, loss) ;

    dopts.verbose = verbose ;
    dopts.matlabDiagonsticFunctionHandle = matlabDiagnosticFunctionHandle ;
    vl_svm_set_diagnostic_function (svm, (VlSvmDiagnosticFunction)diagnostic, &dopts) ;

    if (verbose) {
      double C = 1.0 / (vl_svm_get_lambda(svm) * vl_svm_get_num_data (svm)) ;
      char const * lossName = 0 ;
      switch (loss) {
        case VlSvmLossHinge: lossName = "hinge" ; break ;
        case VlSvmLossHinge2: lossName = "hinge2" ; break ;
        case VlSvmLossL1: lossName = "l1" ; break ;
        case VlSvmLossL2: lossName = "l2" ; break ;
        case VlSvmLossLogistic: lossName = "logistic" ; break ;
      }
      mexPrintf("vl_svmtrain: parameters (verbosity: %d)\n", verbose) ;
      mexPrintf("\tdata dimension: %d\n",vl_svmdataset_get_dimension(dataset)) ;
      mexPrintf("\tnum samples: %d\n", vl_svmdataset_get_num_data(dataset)) ;
      mexPrintf("\tlambda: %g (C equivalent: %g)\n", vl_svm_get_lambda(svm), C) ;
      mexPrintf("\tloss function: %s\n", lossName) ;
      mexPrintf("\tmax num iterations: %d\n", vl_svm_get_max_num_iterations(svm)) ;
      mexPrintf("\tepsilon: %g\n", vl_svm_get_epsilon(svm)) ;
      mexPrintf("\tdiagnostic frequency: %d\n", vl_svm_get_diagnostic_frequency(svm)) ;
      mexPrintf("\tusing custom weights: %s\n", VL_YESNO(weights)) ;
      mexPrintf("\tbias multiplier: %g\n", vl_svm_get_bias_multiplier(svm)) ;
      switch (vl_svm_get_solver(svm)) {
        case VlSvmSolverNone:
          mexPrintf("\tsolver: none (evaluation mode)\n") ;
          break ;
        case VlSvmSolverSgd:
          mexPrintf("\tsolver: sgd\n") ;
          mexPrintf("\tbias learning rate: %g\n", vl_svm_get_bias_learning_rate(svm)) ;
          break ;
        case VlSvmSolverSdca:
          mexPrintf("\tsolver: sdca\n") ;
          break ;
      }
    }

    vl_svm_train(svm) ;

    {
      mwSize dims[2] ;
      dims[0] = vl_svmdataset_get_dimension(dataset) ;
      dims[1] = 1 ;
      out[OUT_MODEL] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL) ;
      memcpy(mxGetPr(out[OUT_MODEL]),
             vl_svm_get_model(svm),
             vl_svm_get_dimension(svm) * sizeof(double)) ;
    }
    out[OUT_BIAS] = vlmxCreatePlainScalar(vl_svm_get_bias(svm)) ;
    if (nout >= 3) {
      out[OUT_INFO] = makeInfoStruct(svm) ;
    }
    if (nout >= 4) {
      mwSize dims[2] ;
      dims[0] = 1 ;
      dims[1] = vl_svmdataset_get_num_data(dataset) ;
      out[OUT_SCORES] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL) ;
      memcpy(mxGetPr(out[OUT_SCORES]),
             vl_svm_get_scores(svm),
             vl_svm_get_num_data(svm) * sizeof(double)) ;
    }


    vl_svm_delete(svm) ;
    if (vl_svmdataset_get_homogeneous_kernel_map(dataset)) {
      VlHomogeneousKernelMap * hom = vl_svmdataset_get_homogeneous_kernel_map(dataset) ;
      vl_svmdataset_set_homogeneous_kernel_map(dataset,0) ;
      vl_homogeneouskernelmap_delete(hom) ;
    }
    vl_svmdataset_delete(dataset) ;
  }
}
