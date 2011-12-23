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
%   ALGORITHM. PEGASOS is an implementation of stochastic subgradient
%   descent. At each iteration a data point is selected at random, the
%   subgradient of the cost function relative to that data point is
%   computed, and a step is taken in that direction. The step size is
%   inversely proportional to the iteration number. See [1] for
%   details.
%
%   NumIterations:: [10 / LAMBDA]
%     Sets the maximum number of iterations.
%
%   BiasMultiplier:: [0]
%     Appends to the data X the specified scalar value B. This
%     approximates the training of a linear SVM with bias.  The bias
%     can be recovered from the optimal weight vector W as W(end) *
%     B. It is often useful to precondition the computation of the
%     gradient with respect to this element by a small value,
%     e.g. 0.1/B (see PRECONDITIONER).
%
%   StartingModel:: [null vector]
%     Specify the initial value for the weight vector W.
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
%   Preconditioner:: [empty]
%     Specify a diagonal preconditioner PREC. The elements of this
%     vector are multiplied to the function subgradient before adding
%     the latter to the current model estimate. The dimension of the
%     vector PREC is the same of the model, plus one if the SVM has
%     bias (see BIASMULTIPLIER).
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
%   See also: VL_HOMKERMAP(), VL_HELP().

% AUTHORIGHTS
