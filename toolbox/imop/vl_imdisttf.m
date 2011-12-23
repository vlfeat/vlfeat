% VL_IMDISTTF  Image distance transform
%   DT = VL_IMDISTTF(I) computes the distance transform of image I.
%   The distance transform is defined as
%
%       DT(x,y) = min_{u,v} I(u,v) + (u - x)^2 + (v - y)^2.
%
%   [D, INDEXES] = VL_IMDISTTF(...) returns a matrix INDEXES that
%   contains for each pixel (x,y) the index of the pixel (u,v) which
%   is the minimizer of the distance transform objective.
%
%   VL_IMDSITTF(I, PARAM) uses differenet parameters for the
%   transform:
%
%      DT(u,v) = min_{u,v} I(u,v) + PARAM(1) (u - x - PARAM(2))^2
%                                 + PARAM(3) (v - y - PARAM(4))^2
%
%   The function uses the fast algorithm from [1].
%
%   REFERENCES::
%     [1] P. F. Felzenszwalb and D. P. Huttenlocher. Distance transforms
%     of sampled functions. Technical report, Cornell University, 2004.
%
%   See also: VL_HELP().

% Author:: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
