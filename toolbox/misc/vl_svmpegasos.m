% VL_SVMPEGASOS PEGASOS linear SVM solver
%   W = VL_SVMPEGASOS(DATA, LAMBDA) learns a linear SVM W given training
%   struct DATA, and the regularization parameter LAMBDA using the
%   PEGASOS [1] solver. The algorithm finds a minimizer W of the
%   objective function
%
%     LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W, X(:,i), Y(i))
%
%   where LOSS(W,X,Y) = MAX(0, 1 - Y W'X) is the hinge loss and N is
%   the number of training vectors in X.
%
%   The training struct DATA is created using the function
%   VL_MAKETRAININGSET.
% 
%   [W B INFO] = VL_SVMPEGASOS(DATA, LAMBDA) learns a linear SVM W
%   and a bias B given training struct DATA, and the regularization
%   parameter LAMBDA using the PEGASOS [1] solver. INFO is a struct
%   containing the input parameters plus diagnostic informations:
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
%   BiasLearningRate:: [1]
%     Specify the frequency of the bias learning. The default
%     setting updates the bias at each iteration.
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
%     Specify a function handle to be called every ENERGYFREQ
%     iterations.
%     The function must be of the form:
%       
%       function o = diagnostics(svm,x)
%     
%     Where in the first iteration x is the variable passed as 
%     "DiagnosticCallRef", and the consecutive ones x is the
%     variable o returned in the previous iteration.
% 
%   DiagnosticCallRef:: [empty]
%     Specify a paramater to be passed to the DIAGNOSTICFUNCTION handle.
% 
%   EnergyFreq:: [100]
%     Specify how often the SVM energy is computed.
% 
%   ValidationData::
%     Specify a validation dataset. The validation dataset must be
%     created using VL_MAKETRAININGSET. If specified, the energy
%     value and all the diagnostic informations are computed on the
%     validation dataset, otherwise the training dataset is used.
%
%   Verbose::
%     Be verbose.
%
%   Example::
%     The options StartingModel and StartingIteration can be used
%     to continue training. I.e., the command
%
%       vl_twister('state',0) ;
%       dataset = vl_maketrainingset(x,y) ;
%       w = vl_svmpegasos(dataset,lambda,'NumIterations',1000) ;
%
%     produces the same result as the sequence
%
%       vl_twister('state',0) ;
%       dataset = vl_maketrainingset(x,y) ;
%       w = vl_svmpegasos(dataset,lambda,'NumIterations',500) ;
%       w = vl_svmpegasos(dataset,lambda,'NumIterations',500, ...
%                      'StartingIteration', 501, ...
%                      'StartingModel', w) ;
%
%   REFERENCES::
%     [1] S. Shalev-Shwartz, Y. Singer, N. Srebro, and
%     A. Cotter. Pegasos: Primal Estimated sub-GrAdient SOlver for
%     SVM. MBP, 2010.
%
%   See also: VL_MAKETRAININGSET(), VL_HOMKERMAP(), VL_HELP().

% AUTHORIGHTS