function y = vl_aibcutpush(map, x)
% VL_AIBCUTPUSH  Quantize based on VL_AIB cut
%  Y = VL_AIBCUTPUSH(MAP, X) maps the data X to elements of the VL_AIB cut
%  specified by MAP.
%
%  The function is equivalent to Y = MAP(X).
%
%  See also VL_HELP(), VL_AIB().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

y = map(x) ;
