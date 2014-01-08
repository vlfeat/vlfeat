function vl_help(topic, varargin)
% VL_HELP   VLFeat toolbox builtin help
%   VL_HELP('doc') displays the HTML documentation in MATLAB
%   web-browser. VL_HELP('vl_function') displays the HTML
%   documentation relative to the specified funtion. VL_HELP('topic')
%   displays the HTML documentation relative to a particular 'topic'.
%
%   VL_HELP(..., 'ONLINE', true) uses the online documentation instead of
%   the local copy.
%
%   Example::
%     To get information about the library, a topic such as Fisher vectors,
%     or a function such as VL_FISHER(), use:
%        vl_help doc
%        vl_help fisher
%        vl_help vl_fisher
%
%   See also the <a href="matlab:vl_help('index')">core VLFeat
%   documentation</a>.

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if nargin == 0
  help vl_help ;
  return ;
end

opts.online = false ;
opts = vl_argparse(opts, varargin) ;

local = fullfile(vl_root, 'doc') ;
if opts.online
  prefix = 'http://www.vlfeat.org/doc' ;
else
  prefix = local ;
end

if strcmp(topic, 'doc')
  topic = 'home' ;
end

[ids,urls] = textread(fullfile(local, 'index.txt'),'%s%s','delimiter','|') ;
k = find(strcmp(topic,ids)) ;
if numel(k) > 0
  web(fullfile(prefix, urls{k(1)}))
else
  error('''%s'' is not a valid function or topic.', topic) ;
end
