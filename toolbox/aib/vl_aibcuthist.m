function hist = vl_aibcuthist(map, x, varargin)
% VL_AIBCUTHIST Compute a histogram by using an AIB compressed alphabet
%  HIST = VL_AIBCUTHIST(MAP, X) computes the histogram of the data X
%  using the compressed alphabet specified by a cut MAP of the AIB
%  tree (as returned by VL_AIBCUT()). Each element of HIST counts how
%  many elements of X are projected to the corresponding cut node
%  (encoding the compressed alphabet).
%
%  Data are mapped to bins as specified by VL_AIBCUTPUSH(). Data mapped
%  to the null node are dropped.
%
%  VL_AIBCUTHIST() accepts the following options:
%
%  Nulls:: [drop]
%    What to do of null nodes: drop ('drop'), accumulate to an
%    extra bin at the end of HIST ('append'), or accumulate to
%    the first bin ('first')
%
%  See also: VL_AIB(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

mode = 'drop' ;

for k=1:2:length(varargin)
  opt=varargin{k} ;
  arg=varargin{k+1} ;
  switch lower(opt)
    case 'nulls'
      switch lower(arg)
        case 'drop'
          mode = 'drop' ;
        case 'append'
          mode = 'append' ;
        case 'first'
          mode = 'first' ;
        otherwise
          error(sprintf('Illegal argument ''%s'' for ''Nulls''', arg)) ;
      end
    otherwise
      error(sprintf('Unknown option ''%''', opt)) ;
  end
end

% determine cut size
cut_size = max(map) ;

% relabel data
y = vl_aibcutpush(map, x) ;

% null?
if any(y == 0)
  switch mode
    case 'drop'
      y = y(y ~= 0) ;
    case 'append'
      cut_size = cut_size + 1 ;
      y(y == 0) = cut_size ;
    case 'first'
      y(y == 0) = 1 ;
  end
end

% Now we have the nodes of the cut. Accumulate.
hist = zeros(1, cut_size) ;
hist = vl_binsum(hist, ones(size(y)), y) ;
