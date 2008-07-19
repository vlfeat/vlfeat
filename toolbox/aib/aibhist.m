% AIBHIST  Compute histogram over AIB tree
%  H = AIBHIST(PARENTS, DATA) computes the histogram of the data
%  points DATA on the AIB tree defined by PARENTS. Each element of
%  DATA indexes one of the leaves of the AIB tree.
%
%  H = AIBHIST(PARENTS, DATA, 'HIST') treats DATA as an histograms.
%  In this case each compoment of DATA is the number of occurences of
%  the AIB leaves corresponding to that component.
%
%  H has the same dimension of parents and counts how many data points
%  are descendent of the corresponding node of the AIB tree.
%
%  See also:: HELP_VLFEAT(), AIB(), AIBCUTPUSH().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
