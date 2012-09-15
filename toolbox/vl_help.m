function vl_help(mode)
% VL_HELP VLFeat Toolbox
%   The VLFeat MATLAB toolbox provides implementations of common
%   computer vision algorithms (HOG, SIFT, dense SIFT, MSER, AIB,
%   KMEANS, learning large scale SVMs...) and many utility functions
%   (e.g. precision-recall, ROC, DET curve calculation, plotting
%   functions). To get started:
%
%   * Type vl_help doc
%   * Type vl_help online
%   * Use the help command to get information about any VLFeat function
%   * Directly check the documentation online: http://www.vlfeat.org

% Author: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if nargin == 0
  help vl_help ;
else
  switch lower(mode)
    case 'doc', web(fullfile(vl_root, 'doc', 'index.html'), '-helpbrowser') ;
    case 'online', web('http://www.vlfeat.org/') ;
    otherwise
      error('Unkown help mode %s', mode);
  end
end
