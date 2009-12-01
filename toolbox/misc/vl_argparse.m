function [conf, args] = vl_argparse(conf, varargin)
% VL_ARGPARSE  Parse option arguments
%   CONF = VL_ARGPARSE(CONF, PAR1, VAL1, ... PARN, VALN) updates the
%   structure CONF based on the specified parameter-value pairs. The
%   function produces an error if an uknown parameter is passed in.
%
%   [CONF, ARGS] = VL_ARGPARSE(...) copies unkown parameters to ARGS
%   instead of producing an error.
%
%   Exampe:: The function can be used to quickly parse a variable list
%     of arguments passed to a MATLAB functions:
%
%      conf = vl_argparse(conf, varargin{:})
%
%   See also:: VL_HELP().
%   Authors:: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if ~isstruct(conf), error('CONF must be a structure') ; end

args = {} ;

for ai = 1:2:length(varargin)
  try 
    paramName = varargin{ai} ;
    value     = varargin{ai+1} ;
  catch
    error('Parameter-value pair expected (missing value?)') ;
  end

  confParamName = findFieldI(conf, paramName) ;

  if isempty(confParamName)
    if nargout < 2
      error(sprintf('Unknown parameter ''%s''', ...
                    paramName)) ;
    else
      args(end+1:end+2) = varargin(ai:ai+1) ;
    end
  else
    conf.(confParamName) = value ;
  end
end

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
