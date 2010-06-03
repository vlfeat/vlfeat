function [conf, args] = vl_argparse(conf, args)
% VL_ARGPARSE  Parse option arguments
%   CONF = VL_ARGPARSE(CONF, ARGS) updates the structure CONF based on
%   the specified parameter-value pairs ARGS={PAR1, VAL1, ... PARN,
%   VALN}. The function produces an error if an unknown parameter name
%   is passed in.
%
%   [CONF, ARGS] = VL_ARGPARSE(...) copies any unkown parameters to
%   ARGS instead of producing an error.
%
%   Example::
%     The function can be used to quickly parse a variable list of
%     arguments passed to a MATLAB functions:
%
%       conf = vl_argparse(conf, varargin)
%
%   See also: VL_OVERRIDE(), VL_HELP().

% Author: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if ~isstruct(conf), error('CONF must be a structure') ; end

remainingArgs = {} ;

for ai = 1:2:length(args)
  try 
    paramName = args{ai} ;
    value = args{ai+1} ;
  catch
    error('Parameter-value pair expected (missing value?).') ;
  end

  confParamName = findFieldI(conf, paramName) ;

  if isempty(confParamName)
    if nargout < 2
      error('Unknown parameter ''%s''.', paramName) ;
    else
      remainingArgs(end+1:end+2) = args(ai:ai+1) ;
    end
  else
    conf.(confParamName) = value ;
  end
end

args = remainingArgs ;

% --------------------------------------------------------------------
function field = findFieldI(S, matchField)
% --------------------------------------------------------------------
field =  ''  ;
fieldNames = fieldnames(S) ;
for fi=1:length(fieldNames)
  if strcmpi(fieldNames{fi}, matchField)
    field = fieldNames{fi} ;
  end
end
