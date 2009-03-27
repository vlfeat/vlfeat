% QUICKSHIFT Quickshift
%   Quickshift is a mode seeking algorithm which links each pixel to its nearest
%   neighbor which has an increase in the estimate of the density. These links
%   form a tree, where the root of the tree is the pixel which is the highest
%   mode in the image. 
%
%   [MAP,GAPS] = QUICKSHIFT(I, SIGMA, TAU) computes Quickshift on the image
%   I. SIGMA represents the bandwidth of the parzen window estimator of the
%   density. Since searching over all pixels for the nearest neighbor which
%   increases the density would be prohibitively expensive, TAU controls the
%   maximum L2 distance between neighbors that should be linked. MAP represents
%   the resulting forest of trees. Each pixel contains the linear index of its
%   parent. GAPS provides the distance to this parent. Pixels which are at the
%   root of their respective tree have MAP(x) = x and GAPS(x) = inf.
%
%   [MAP,GAPS,E] = QUICKSHIFT(I, SIGMA, TAU) also returns the estimate of
%   the density E.
%   
%   [MAP,GAPS] = QUICKSHIFT(I, SIGMA) uses a default TAU of 3*SIGMA.
%
%   Notes:
%     The distance between pixels is always measured in image coordinates (not
%     normalized), so the importance of the color component should be weighted
%     accordingly before calling this function.
%
%   Options:
% 
%   Verbose::
%     Toggles verbose output.
%
%   Medoid::
%     Perform Medoid shift instead of Quickshift.
