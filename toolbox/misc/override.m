function config = override(config,update,warn)
% OVERRIDE  Override structure subset
%   CONFIG = OVERRIDE(CONFIG, UPDATE) copies recursively the fileds of
%   the structure UPDATE to the corresponding fields of the struture
%   CONFIG.
%
%   While CONFIG and UPDATE can be any structure, typicall CONFIG is
%   used to store the default configration of some object, and UPDATE
%   is used to update only a specific subset of it.
%
%   OVERRIDE(...,1) will print a warning message whenever 
%
%   See also:: HELP_VLFEAT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

if ~isstruct(config)
  config = update ;
  return ;
end

if ~isstruct(update)
  error('Attempted to update a structure with a non-structure') ;
end

% make sure that we can put UPDATE in correspondence with CONFIG.
% otherwise assume a copy-over operation
if numel(update) ~= numel(config)
  config = update ;
  return ;
end

for idx=1:numel(update)
  fields = fieldnames(update) ;
  for i=1:length(fields)
    f=fields{i} ;
    if(isfield(config, f))
      config(idx).(f) = override(config(idx).(f), update(idx).(f)) ;
    else
      if nargin > 2
        warning(sprintf('field ''%s'' is in UPDATE but not in CONFIG',f)) ;
      end
      config(idx).(f) = update(idx).(f) ;
    end
  end
end
