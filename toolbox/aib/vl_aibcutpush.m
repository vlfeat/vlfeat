function y = vl_aibcutpush(map, x)
% VL_AIBCUTPUSH  Quantize based on VL_AIB cut
%  Y = VL_AIBCUTPUSH(MAP, X) maps the data X to elements of the AIB
%  cut specified by MAP.
%
%  The function is equivalent to Y = MAP(X).
%
%  See also: VL_HELP(), VL_AIB().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

y = map(x) ;
