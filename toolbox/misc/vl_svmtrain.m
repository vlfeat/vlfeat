% VL_SVMTRAIN Train a Support Vector Machine
%   [W B] = VL_SVMTRAIN(X, Y, LAMBDA) trains a linear Support Vector
%   Machine (SVM) from the data vectors X and the labels Y. X is a D
%   by N matrix, with one column for example and D feature dimensions
%   (SINGLE or DOUBLE). Y is a DOUBLE vector with N elements with a
%   binary (-1 or +1) label for each training point. To a first order
%   approximation, the function computes a weight vector W and offset
%   B such that the score W'*X(:,i)+B has the same sign of LABELS(i)
%   for all i.
%
%   VL_SVMTRAIN(DATASET, LABELS, LAMBDA) takes as input a DATASET
%   structure, which allows more sophisticated input formats to be
%   supported (see VL_SVMMAKEDATASET()).
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
%   VL_SVMTRAIN() accepts the following options:
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
%   Solver:: SGD
%     One of SGD (stochastic gradient descent [1]) or SDCA (stochastic
%     dual coordinate ascent [2,3]).
%
%   StartingModel:: null vector
%     Specifies the initial value for the weight vector W (SGD only).
%
%   StartingBias:: 0
%     Specifies the initial value of the bias term (SGD only).
%
%   FORMULATION
%
%   VL_SVMTRAIN() minimizes the objective function of the form:
%
%     LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W' X(:,i), Y(i))
%
%   where LOSS(W,X,Y) is the loss (hinge by default). The bias is
%   incorporated by extending each data point X with a feature of
%   constant value B0, such that the objective becomes
%
%    LAMBDA/2 (|W|^2 + WB^2) 1/N SUM_i LOSS(W' X(:,i) + WB B0, Y(i))
%
%   Note that this causes the learned bias B = WB B0 to shrinks
%   towards the origin.
%
%   REFERENCES::
%
%   [1] S. Shalev-Shwartz, Y. Singer, and N. Srebro. Pegasos: Primal
%       estimated sub-GrAdient SOlver for SVM. In Proc. ICML, 2007.
%
%   [2] Cho-Jui Hsieh, Kai-Wei Chang, Chih-Jen Lin, S. Sathiya
%       Keerthi, and S. Sundararajan. 2008. A dual coordinate descent
%       method for large-scale linear SVM. Proc. ICML, 2008.
%
%   [3] S. Shalev-Shwartz and T. Zhang. Stochastic Dual Coordinate
%       Ascent Methods for Regularized Loss Minimization. In
%       Proc. NIPS, 2012.
%
%   See also: VL_MAKETRAININGSET(), VL_HELP().

% AUTHORIGHTS
