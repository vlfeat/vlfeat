% VL_AIBHIST  Compute histogram over VL_AIB tree
%  H = VL_AIBHIST(PARENTS, DATA) computes the histogram of the data
%  points DATA on the VL_AIB tree defined by PARENTS. Each element of
%  DATA indexes one of the leaves of the VL_AIB tree.
%
%  H = VL_AIBHIST(PARENTS, DATA, 'HIST') treats DATA as an histograms.
%  In this case each compoment of DATA is the number of occurences of
%  the VL_AIB leaves corresponding to that component.
%
%  H has the same dimension of parents and counts how many data points
%  are descendent of the corresponding node of the VL_AIB tree.
%
%  See also: VL_HELP(), VL_AIB(), VL_AIBCUTPUSH().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
