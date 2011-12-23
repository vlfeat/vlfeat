function prop=vl_linespec2prop(spec)
% VL_LINESPEC2PROP  Convert PLOT style line specs to properties
%  PROPR = VL_LINESPEC2PROP(SPEC) converts the string SPEC to a cell
%  array of properties PROPR. SPEC is in the format of PLOT().
%
%  If SPEC is not in a recognized format, the string SPEC is returned
%  unaltered as the only element of PROPR.
%
%  See also: VL_PLOTFRAME(), PLOT(), VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

prop = {} ;

if ~ ischar(spec)
  error('SPEC must be a string') ;
end

spec_ = spec ;

switch spec(1:min(numel(spec),1))
  case {'b' 'g' 'r' 'c' 'm' 'y' 'k' 'w'}
    prop = {prop{:}, 'Color', spec(1)} ;
    spec(1) = [] ;
end

switch spec(1:min(numel(spec),1))
  case {'.' 'o' 'x' '+' '*' 's' 'd' 'v' '^' '<' '>' 'p' 'h'}
    prop = {prop{:}, 'Marker', spec(1)} ;
    spec(1) = [] ;
end

if isempty(spec)
  return ;
end

switch spec
  case {'-' ':' '-.' '--'}
    prop = {prop{:}, 'LineStyle', spec} ;
  otherwise
    prop = {spec_} ;
end
