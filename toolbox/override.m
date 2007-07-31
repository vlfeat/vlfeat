function config = override(config,update,warn)
% OVERRIDE  Override structure subset
%   CONFIG = OVERRIDE(CONFIG, UPDATE) copies recursively the fileds of
%   the structure UPDATE to the corresponding fields of the struture
%   CONFIG.
%
%   While CONFIG and UPDATE can be any structure, typicall CONFIG
%   is used to store the default configration of some object, and
%   UPDATE is used to update only a specific subset of it.
%
%   OVERRIDE(...,1) will print a warning message whenever 

% AUTORIGHTS
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

if ~isstruct(config)
  config = update ;
  return ;
end

fields = fieldnames(update) ;
for i=1:length(fields)
  f=fields{i} ;
  if(isfield(config, f))
    config.(f) = override(config.(f), update.(f)) ;
  else
    if nargin > 2
      warning(sprintf('field ''%s'' is in UPDATE but not in CONFIG',f)) ;
    end
    config.(f) = update.(f) ;
  end
end
