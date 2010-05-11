% VL_HOMKERMAP Homogeneous kernel map
%   V = VL_HOMKERMAP(X, N, L) computes a finite dimensional
%   approximated kernel map for the Chi2 kernel. N is the
%   approximation order and L is the sampling step (see [1] for
%   details). X is an array of data points. Each point is expanded
%   into a vector of dimension 2*N+1 and saved to V. Vectors are
%   stacked along the first dimension of X, so that V has the same
%   dimensions of X except for the first, which is multiplied by
%   2*N+1.
%
%   The function accepts the following options:
%
%   KChi2::
%     Compute the map for the Chi2 kernel.
%
%   KL1::
%     Compute the map for the L1 (intersection) kernel.
%
%   KJS::
%     Compute the map for the JS (Jensen-Shannon) kernel.
%
%   Example::
%     The following code results in approximatively the same
%     similarities matrices between points X and Y:
%
%       x = rand(10,1) ;
%       y = rand(10,100) ;
%       psix = vl_homkermap(x, 3, .4) ;
%       psiy = vl_homkermap(y, 3, .4) ;
%       figure(1) ; clf ;
%       ker = vl_alldist(x, y, 'kchi2') ;
%       ker_ = psix' * psiy ;
%       plot([ker ; ker_]') ;
%
%   REFERENCES
%   [1] A. Vedaldi and A. Zisserman
%       `Efficient Additive Kernels via Explicit Feature Maps',
%       Proc. CVPR, 2010.
%
%   See also: VL_HELP().

% Authors: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
