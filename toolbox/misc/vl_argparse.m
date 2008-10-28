function conf = vl_argparse(conf, varargin)
% VL_ARGPARSE  Parse option arguments
%   CONF = VL_ARGPARSE(CONF, PAR1, VAL1, ... PARN, VALN) updates the
%   structure CONF based on the specified parameter-value pairs.
%
%   Exampe:: The function can be used to quickly parse a variable list
%     of arguments passed to a MATLAB functions:
%
%      conf = vl_argparse(conf, varargin{:})
%
%   See also:: VL_HELP().
%   Authors:: Andrea Vedaldi

% AUTORIGHTS

if ~isstruct(conf), error('CONF must be a structure') ; end

for ai = 1:2:length(varargin)
  try 
    paramName = varargin{ai} ;
    value     = varargin{ai+1} ;
  catch
    error('Parameter-value pair expected (missing value?)') ;
  end

  confParamName = findFieldI(conf, paramName) ;

  if isempty(confParamName)
    error(sprintf('Unknown parameter ''%s''', ...
                  paramName)) ;
  end  
  conf.(confParamName) = value ;
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
