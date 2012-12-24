% VL_PEGASOS PEGASOS linear SVM solver
%   W = VL_PEGASOS(X, Y, LAMBDA) learns a linear SVM W given training
%   vectors X, their labels Y, and the regularization parameter LAMBDA
%   using the PEGASOS [1] solver. The algorithm finds a minimizer W of
%   the objective function
%
%     LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W, X(:,i), Y(i))
%
%   where LOSS(W,X,Y) = MAX(0, 1 - Y W'X) is the hinge loss and N is
%   the number of training vectors in X.
%
%   [W B INFO] = VL_SVMPEGASOS(X, Y, LAMBDA) learns a linear SVM W
%   and a bias B given training vectors X, their labels Y, and the
%   regularization parameter LAMBDA using the PEGASOS [1]
%   solver. INFO is a struct containing the input parameters plus
%   diagnostic informations:
%
%   energy::
%     SVM energy value.
%
%   iterations::
%     Number of iterations performed.
%
%   elapseTime::
%     Elapsed time since the start of the SVM learning.
%
%   regulizerTerm::
%     Value of the SVM regulizer term.
%
%   lossPos::
%     Value of loss function only for data points labeled positives.
%
%   lossNeg::
%     Value of loss function onlt for data points labeled negatives.
%
%   hardLossPos::
%     Number of mislabeled positive points.
%
%   hardLossNeg::
%     Number of mislabeled negative points.
%
%   ALGORITHM. PEGASOS is an implementation of stochastic subgradient
%   descent. At each iteration a data point is selected at random, the
%   subgradient of the cost function relative to that data point is
%   computed, and a step is taken in that direction. The step size is
%   inversely proportional to the iteration number. See [1] for
%   details.
%
%   VL_SVMPEGASOS() accepts the following options:
%
%   Epsilon:: [empty]
%     Specify the SVM stopping criterion threshold. If not
%     specified VL_SVMPEGASOS will finish when the maximum number
%     of iterations is reached. The stopping criterion is tested
%     after each ENERGYFREQ iteration.
%
%   MaxIterations:: [10 / LAMBDA]
%     Sets the maximum number of iterations.
%
%   BiasMultiplier:: [0]
%     Appends to the data X the specified scalar value B. This
%     approximates the training of a linear SVM with bias.
%
%   StartingModel:: [null vector]
%     Specify the initial value for the weight vector W.
%
%   StartingIteration:: [1]
%     Specify the iteration number to start from. The only effect
%     is to change the step size, as this is inversely proportional
%     to the iteration number.
%
%   StartingBias:: [0]
%     Specify the inital bias value.
% 
%   BiasPreconditioner:: [1]
%     Specify a preconditioner for the bias. This value is
%     multiplied to the bias subgradient before adding
%     the latter to the current model estimate.
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
%   DiagnosticFunction:: [empty]
%     Specify a function handle to be called every ENERGYFREQ iterations.
%
%   DiagnosticCallRef:: [empty]
%     Specify a paramater to be passed to the DIAGNOSTICFUNCTION
%     handle. The handle is returned as fourth output of the function.
%
%   EnergyFreq:: [100]
%     Specify how often the SVM energy is computed.
%
%   HOMKERMAP:: [empty]
%     Specify the use of an Homogeneus Kernel map for the training
%     data (See [2],[3]). The passed value N is such that a 2*N+1
%     dimensional approximated kernel map is computed. Each
%     training data point is expanded online into a vector of
%     dimension 2*N+1.
%
%   KChi2::
%     Compute the map for the Chi2 kernel.
%
%   KINTERS::
%     Compute the map for the intersection kernel.
%
%   KL1::
%     Same as KINTERS, but deprecated as the name is not fully
%     accurate.
%
%   KJS::
%     Compute the map for the JS (Jensen-Shannon) kernel.
%
%   Period:: [automatically tuned]
%     Set the period of the kernel specturm. The approximation is
%     based on periodicizing the kernel specturm. If not specified,
%     the period is automatically set based on the heuristic described
%     in [2].
%
%   Window:: [RECTANGULAR]
%     Set the window used to truncate the spectrum before The window
%     can be either RECTANGULAR or UNIFORM window. See [2] and the API
%     documentation for details.
%
%   Gamma:: [1]
%     Set the homogeneity degree of the kernel. The standard kernels
%     are 1-homogeneous, but sometimes smaller values perform better
%     in applications. See [2] for details.
%
%   Verbose::
%     Be verbose.
%
%   Example::
%     The options StartingModel and StartingIteration can be used
%     to continue training. I.e., the command
%
%       vl_twister('state',0) ;
%       w = vl_pegasos(x,y,lambda,'NumIterations',1000) ;
%
%     produces the same result as the sequence
%
%       vl_twister('state',0) ;
%       w = vl_pegasos(x,y,lambda,'NumIterations',500) ;
%       w = vl_pegasos(x,y,lambda,'NumIterations',500, ...
%                      'StartingIteration', 501, ...
%                      'StartingModel', w) ;
%
%   REFERENCES::
%     [1] S. Shalev-Shwartz, Y. Singer, N. Srebro, and
%     A. Cotter. Pegasos: Primal Estimated sub-GrAdient SOlver for
%     SVM. MBP, 2010.
%
%     [2] A. Vedaldi and A. Zisserman
%     `Efficient Additive Kernels via Explicit Feature Maps',
%     Proc. CVPR, 2010.
%
%     [3] A. Vedaldi and A. Zisserman
%     `Efficient Additive Kernels via Explicit Feature Maps',
%     PAMI, 2011 (submitted).
%
%   See also: VL_HOMKERMAP(), VL_HELP().

% AUTHORIGHTS
