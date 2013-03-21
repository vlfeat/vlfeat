% VL_SVMTRAIN Train a Support Vector Machine
%   [W B INFO] = VL_SVMTRAIN(DATA , LAMBDA [, OPTS]) learns Support Vector Machine with model W
%   and bias B given training struct DATA and the regularization parameter
%   LAMBDA. Structure INFO provides diagnostic information.
%   The algorithm finds a minimizer W of the objective function
%
%     LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W, X(:,i), Y(i))
%
%   where LOSS(W,X,Y) is the used loss function and N is
%   the number of training vectors in X.
%   For more details see the VLFeat documentation for Support Vector Machines.
%
%   
%   By default, VL_SVMTRAIN uses the SGD method with L1 hinge loss function
%   to train the SVM. By setting appropriate parameter, (S)DCA with selected
%   loss function can be used instead.
%
%
%   INFO is a struct containing diagnostic information:
%
%   iterations::
%     Number of iterations performed.
%     
%   maxIterations::
%     The maximum number of iterations to be performed.
%     
%   elapsedTime::
%     Elapsed time since the start of the SVM learning.
%     
%   energy::
%     SVM energy value.
%     
%   regularizerTerm::
%     Value of the SVM regulizer term.
%
%   lossPos::
%     Value of loss function only for positively labeled data points.
%     
%   lossNeg::
%     Value of loss function only for nagatively labeled data points.
%     
%   hardLossPos::
%     Number of mislabeled positive points.
%     
%   hardLossNeg::
%     Number of mislabeled negative points.
%
%   
%
%   VL_SVMCDDUAL() accepts the following options:
%
%   Epsilon:: [0]
%     Tolerance for the stopping criterion.
%
%   MaxIterations:: [1000]
%     Sets the maximum number of iterations.
%
%   BiasMultiplier:: [1]
%     Appends to the data X the specified scalar value B for learning with bias.
%
%   DiagnosticFunction:: [empty]
%     Specifies a function handle to be called every DIAGNOSTICFREQ
%     iterations.
%     The function must be of the form:
%
%       function o = diagnostics(svm) ,
%
%   EnergyFreq:: [100]
%     Specifies how often the energy is computed and the stopping condition checked.
%
%   ValidationData::
%     Specify a validation dataset. The validation dataset must be
%     created using VL_MAKETRAININGSET. If specified, the energy
%     value and all the diagnostic informations are computed on the
%     validation dataset, otherwise the training dataset is used.
%
%
%   DCA:: [0]
%     Set to 1 for the (S)DCA solver, 0 for SGD.
%
%
%
%   For (S)DCA, the following specific options may be set:
%
%   Loss:: [1]
%     Specifies the loss function (L1 hinge loss or L2 hinge loss).
%
%   RandomPermutation:: [1]
%     Use Random Permutation of Sub-problems.
%
%   OnlineSetting:: [0]
%     Use Online Setting.
%
%
%   For SGD, the following specific options may be set:
%
%   StartingModel:: [null vector]
%     Specifies the initial value for the weight vector W.
%
%   StartingBias:: [0]
%     Specifies the inital bias value.
%
%   StartingIteration:: [1]
%     Specify the iteration number to start from. The only effect
%     is to change the step size, as this is inversely proportional
%     to the iteration number.
%
%   Permutation:: [empty]
%     Specify a permutation PERM to be used to sample the data (this
%     disables random sampling). Specifically, at the T-th iteration
%     the algorithm takes a step w.r.t. the PERM[T']-th data point,
%     where T' is T modulo the number of data samples
%     (i.e. MOD(T'-1,NUMSAMPLES)+1). PERM needs not to be
%     bijective. This allows specifying certain data points more or
%     less frequently, implicitly increasing their relative weight in
%     the error term. A common application is to balance an unbalanced
%     dataset.
%
%   BiasPreconditioner:: [1]
%     Specify a preconditioner for the bias. This value is
%     multiplied to the bias subgradient before adding
%     the latter to the current model estimate.
%
%
%
%   REFERENCES::
%   [1] Cho-Jui Hsieh, Kai-Wei Chang, Chih-Jen Lin, S. Sathiya Keerthi, and S. Sundararajan. 2008. A dual coordinate descent method for large-scale linear SVM. In Proceedings of the 25th international conference on Machine learning (ICML '08). ACM, New York, NY, USA, 408-415. 
%   http://ntu.csie.org/~cjlin/papers/cddual.pdf
%
%   See also: VL_MAKETRAININGSET(), VL_HELP().

% AUTHORIGHTS
