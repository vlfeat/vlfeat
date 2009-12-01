function config = vl_override(config,update,varargin)
% VL_OVERRIDE  Override structure subset
%   CONFIG = VL_OVERRIDE(CONFIG, UPDATE) copies recursively the fileds of
%   the structure UPDATE to the corresponding fields of the struture
%   CONFIG.
%
%   While CONFIG and UPDATE can arbitrary structures, usually CONFIG
%   is a list of defaul parameters and UPDATE is a list of parameter
%   updates.
%
%   VL_OVERRIDE(..., 'warn') prints a warning message whenever copying
%   UPDATE adds new fileds to CONFIG, or overrides a non-empty
%   substucture.
%
%   VL_OVERRIDE(..., 'skip') does not add any new parameters but only
%   updates existing ones.
%
%   VL_OVERRIDE(..., 'casei') matches field names up to their case.
%
%   Remark:: Each field path of UPDATE is matched as deeply as
%    possible to a path of CONFIG.  Two fields A(IA).(FA) and B(IA).FB
%    match if, and only if, (i) the struct arrays A and B have the
%    same dimensions, (ii) the indeces IA and IB are equal, and the
%    field names FA and FB are equal.
%
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
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

