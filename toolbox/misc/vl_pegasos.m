% VL_PEGASOS PEGASOS binary SVM solver
%   W = VL_PEGASOS(X, Y, LAMBDA) learns a binary SVM W given
%   training vectors X, their labels Y, and the regularization
%   parameter LAMBDA. The solver solves the problem
%
%     MIN_W LAMBDA/2 |W|^2 + 1/N SUM_i LOSS(W, X(:,i), Y(i))
%
%   where LOSS(W,X,Y) = MAX(0, 1 - Y W'X) is the hinge loss and N is
%   the number of training vectors in X.
%
%   NumIterations:: 10 / LAMBDA
%     Sets the maximum number of iterations.
%
%   BiasMultiplier:: 0
%     Appends to X this scalar value in order to estimate an SVM
%     with bias.
%
%   StartingModel:: null vector
%     Specify the initial value for the model W.
%
%   StartingIteration:: 1
%     Specify the first iteration number.
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
%   See also: VL_HELP(), VL_HOMKERMAP().

% AUTHORIGHTS