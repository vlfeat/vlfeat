function config = override(config,update,varargin)
% OVERRIDE  Override structure subset
%   CONFIG = OVERRIDE(CONFIG, UPDATE) copies recursively the fileds of
%   the structure UPDATE to the corresponding fields of the struture
%   CONFIG.
%
%   While CONFIG and UPDATE can arbitrary structures, usually CONFIG
%   is a list of defaul parameters and UPDATE is a list of parameter
%   updates.
%
%   OVERRIDE(..., 'warn') prints a warning message whenever copying
%   UPDATE adds new fileds to CONFIG, or overrides a non-empty
%   substucture.
%
%   OVERRIDE(..., 'skip') does not add any new parameters but only
%   updates existing ones.
%
%   Remark:: The function matches as deeply as possible any path of
%    the structure UPDATE to a path of the structure CONFIG.  Two
%    fields A(indA).(fieldA) and B(indA).fieldB match if, and only if,
%    the struct arrays A and B have the same dimensions, indA == indB
%    and fieldA == fieldB.
%
%   See also:: HELP_VLFEAT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

warn = 0 ;
skip = 0 ;

if length(varargin) == 1 & ~ischar(varargin{1})
  % legacy
  warn = 1 ;
end

if ~warn & length(varargin) > 0
  for i=1:length(varargin)
    switch lower(varargin{i})
      case 'warn' 
        warn = 1 ;
      case 'skip'
        skip = 1 ;
      otherwise
        error('Uknown option.') ;
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
    f = fields{i} ;    
    if(isfield(config, f))
      config(idx).(f) = override(config(idx).(f), update(idx).(f)) ;
    else
      if warn
        warning(sprintf('copyed field ''%s'' which is in UPDATE but not in CONFIG',f)) ;
      end
      if skip
        if warn
          warning(sprintf('skipping field ''%s'' which is in UPDATE but not in CONFIG',f)) ;
        end

        continue ;
      end
      config(idx).(f) = update(idx).(f) ;
    end
  end
end
