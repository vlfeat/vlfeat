function config = vl_override(config,update,varargin)
% VL_OVERRIDE  Override structure subset
%   CONFIG = VL_OVERRIDE(CONFIG, UPDATE) copies recursively the fileds
%   of the structure UPDATE to the corresponding fields of the
%   struture CONFIG.
%
%   Usually CONFIG is interpreted as a list of paramters with their
%   default values and UPDATE as a list of new paramete values.
%
%   VL_OVERRIDE(..., 'Warn') prints a warning message whenever: (i)
%   UPDATE has a field not found in CONFIG, or (ii) non-leaf values of
%   CONFIG are overwritten.
%
%   VL_OVERRIDE(..., 'Skip') skips fields of UPDATE that are not found
%   in CONFIG instead of copying them.
%
%   VL_OVERRIDE(..., 'CaseI') matches field names in a
%   case-insensitive manner.
%
%   Remark::
%     Fields are copied at the deepest possible level. For instance,
%     if CONFIG has fields A.B.C1=1 and A.B.C2=2, and if UPDATE is the
%     structure A.B.C1=3, then VL_OVERRIDE() returns a strucuture with
%     fields A.B.C1=3, A.B.C2=2. By contrast, if UPDATE is the
%     structure A.B=4, then the field A.B is copied, and VL_OVERRIDE()
%     returns the structure A.B=4 (specifying 'Warn' would warn about
%     the fact that the substructure B.C1, B.C2 is being deleted).
%
%   Remark::
%     Two fields are matched if they correspond exactly. Specifically,
%     two fileds A(IA).(FA) and B(IA).FB of two struct arrays A and B
%     match if, and only if, (i) A and B have the same dimensions,
%     (ii) IA == IB, and (iii) FA == FB.
%
%   See also: VL_ARGPARSE(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

warn  = false ;
skip  = false ;
err   = false ;
casei = false ;

if length(varargin) == 1 & ~ischar(varargin{1})
  % legacy
  warn = 1 ;
end

if ~warn & length(varargin) > 0
  for i=1:length(varargin)
    switch lower(varargin{i})
      case 'warn' 
        warn = true ;
      case 'skip'
        skip = true ;
      case 'err'
        err = true ;
      case 'argparse'
        argparse = true ;
      case 'casei'
        casei = true ;
      otherwise
        error(sprintf('Unknown option ''%s''.',varargin{i})) ;
    end
  end
end

% if CONFIG is not a struct array just copy UPDATE verbatim
if ~isstruct(config)
  config = update ;
  return ;
end

% if CONFIG is a struct array but UPDATE is not, no match can be
% established and we simply copy UPDATE verbatim
if ~isstruct(update)
  config = update ; 
  return ;
end

% if CONFIG and UPDATE are both struct arrays, but have different
% dimensions then nom atch can be established and we simply copy
% UPDATE verbatim
if numel(update) ~= numel(config)
  config = update ;
  return ;
end

% if CONFIG and UPDATE are both struct arrays of the same
% dimension, we override recursively each field

for idx=1:numel(update)
  fields = fieldnames(update) ;

  for i = 1:length(fields)
    updateFieldName = fields{i} ;
    if casei
      configFieldName = findFieldI(config, updateFieldName) ;
    else
      configFieldName = findField(config, updateFieldName) ;
    end

    if ~isempty(configFieldName)
      config(idx).(configFieldName) = ...
          vl_override(config(idx).(configFieldName), ...
                      update(idx).(updateFieldName)) ;
    else
      if warn
        warning(sprintf('copied field ''%s'' which is in UPDATE but not in CONFIG', ...
                        updateFieldName)) ;
      end
      if err
        error(sprintf('The field ''%s'' is in UPDATE but not in CONFIG', ...
                      updateFieldName)) ;
      end
      if skip
        if warn
          warning(sprintf('skipping field ''%s'' which is in UPDATE but not in CONFIG', ...
                          updateFieldName)) ;
        end
        continue ;
      end
      config(idx).(updateFieldName) = update(idx).(updateFieldName) ;
    end
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

% --------------------------------------------------------------------
function field = findField(S, matchField)
% --------------------------------------------------------------------

field =  '' ;
fieldNames = fieldnames(S) ;
for fi=1:length(fieldNames)
  if strcmp(fieldNames{fi}, matchField)
    field = fieldNames{fi} ;
  end
end

