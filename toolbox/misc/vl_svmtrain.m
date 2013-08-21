% VL_SVMTRAIN   Train a Support Vector Machine
%   [W B] = VL_SVMTRAIN(X, Y, LAMBDA) trains a linear Support Vector
%   Machine (SVM) from the data vectors X and the labels Y. X is a D
%   by N matrix, with one column per example and D feature dimensions
%   (SINGLE or DOUBLE). Y is a DOUBLE vector with N elements with a
%   binary (-1 or +1) label for each training point. To a first order
%   approximation, the function computes a weight vector W and offset
%   B such that the score W'*X(:,i)+B has the same sign of LABELS(i)
%   for all i.
%
%   VL_SVMTRAIN(DATASET, LABELS, LAMBDA) takes as input a DATASET
%   structure, which allows more sophisticated input formats to be
%   supported (see VL_SVMDATASET()).
%
%   [W, B, INFO] = VL_SVMTRAIN(...) additionally returns a structure
%   INFO with the following fields:
%
%   iteration::
%     Number of iterations performed.
%
%   epoch::
%     Number of iterations over number of training data points.
%
%   elapsedTime::
%     Time elapsed since the start of training.
%
%   objective::
%     SVM objective value.
%
%   regularizer::
%     Regularizer value.
%
%   loss::
%     Loss value.
%
%   scoreVariation:: [SGD only]
%     Mean square root of the difference between the last two
%     values of the SVM scores for each point.
%
%   dualObjective:: [SDCA only]
%     Dual objective value.
%
%   dualLoss:: [SDCA only]
%     Dual loss value::
%
%   dualityGap:: [SDCA only]
%     Difference between the objective and the dual objective.
%
%   [W, B, INFO, SCORES] = VL_SVMTRAIN(X, Y, LABMDA) returns a row
%   vector of the SVM score for each training point. This can be used
%   in combination with the options SOLVER, MODEL, and BIAS to
%   evaluate an existing SVM on new data points. Furthermore INFO will
%   contain the corresponding SVM loss, regularizer, and objective
%   function value. If this information is not of interest, it is
%   possible to pass a null vector Y instead of the actual labels as
%   well as a null regularizer.
%
%   VL_SVMTRAIN() accepts the following options:
%
%   Verbose::
%     Specify one or multiple times to increase the verbosity level.
%     Given only once, produces messages at the beginning and end of
%     the learning. Verbosity of at least 2 prints information at
%     every diagnostic step.
%
%   Epsilon:: 1e-3
%     Tolerance for the stopping criterion.
%
%   MaxNumIterations:: 10/LAMBDA
%     Maximum number of iterations.
%
%   BiasMultiplier:: 1
%     Value of the constant B0 used as bias term (see below).
%
%   BiasLearningRate:: 0.5
%     Learning rate for the bias (SGD solver only).
%
%   DiagnosticFunction:: []
%     Diagnostic function callback. The callback takes the INFO
%     structure as only argument. To trace energies and plot graphs,
%     the callback can update a global variable or, preferably, be
%     defined as a nested function and update a local variable in the
%     parent function.
%
%   DiagnosticFrequency:: Number of data points
%     After how many iteration the diagnostic is run. This step check
%     for convergence, and is done rarely, typically after each epoch
%     (pass over the data). It also calls the DiangosticFunction,
%     if any is specified.
%
%   Loss:: HINGE
%     Loss function. One of HINGE, HINGE2, L1, L2, LOGISTIC.
%
%   Solver:: SDCA
%     One of SGD (stochastic gradient descent [1]), SDCA (stochastic
%     dual coordinate ascent [2,3]), or NONE (no training). The
%     last option can be used in combination with the options MODEL
%     and BIAS to evaluate an existing SVM.
%
%   Model:: null vector
%     Specifies the initial value for the weight vector W (SGD only).
%
%   Bias:: 0
%     Specifies the initial value of the bias term (SGD only).
%
%   Weights:: []
%     Specifies a weight vector to assign a different non-negative
%     weight to each data point. An application is to rebalance
%     unbalanced datasets.
%
%   FORMULATION
%
%   VL_SVMTRAIN() minimizes the objective function of the form:
%
%     LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W' X(:,i), Y(i))
%
%   where LOSS(W' Xi,Yi) is the loss (hinge by default) for i-th
%   data point. The bias is incorporated by extending each data
%   point X with a feature of constant value B0, such that the
%   objective becomes
%
%    LAMBDA/2 (|W|^2 + WB^2) 1/N SUM_i LOSS(W' X(:,i) + WB B0, Y(i))
%
%   Note that this causes the learned bias B = WB B0 to shrink
%   towards the origin.
%
%   Example::
%     Learn a linear SVM from data X and labels Y using 0.1
%     as regularization coefficient:
%
%       [w, b] = vl_svmtrain(x, y, 0.1) ;
%
%     The SVM can be evaluated on new data XTEST with:
%
%       scores = w'*xtest + b ;
%
%     Alternatively, VL_SVMTRAIN() can be used for evaluation too:
%
%       [~,~,~, scores] = vl_svmtrain(xtest, y, 0, 'model', w, 'bias', b, 'solver', 'none') ;
%
%     The latter form is particularly useful when X is a DATASET structure.
%
%   See also: <a href="matlab:vl_help('svm')">SVM fundamentals</a>,
%   VL_SVMDATASET(), VL_HELP().

% AUTHORIGHTS
