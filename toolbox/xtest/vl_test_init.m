% VL_TEST_INIT  Intialize unit test file
%   This is a VLFeat internal function used to initialize an .m file
%   containing a unit test.
%
%   See also: VL_TEST(), VL_HELP().

% Author:: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% Copyright (C) 2013 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% Obtain the path to the m-file that called VL_TEST_INIT()
stack = dbstack(1) ;
mfilePath = stack(1).file ;
mfileName = stack(1).name ;
fid = fopen(mfilePath, 'r') ;

% Parse the source code of the caller to get the names of its
% subfunctions. One could use which('-subfun', stack(1).name) here,
% but this feature is undocumented.

setupFunc = [] ;
teardownFunc = [] ;

tests = {} ;
l = 0 ;
t = 0 ;
while 1
  tline = fgetl(fid) ;
  l = l + 1 ;
  if ~ischar(tline), break ; end
  mo = regexp(tline, '^\s*function\s+(\w\s*=\s*)?test_(?<name>[\w_]+)\s*\(.*\).*$', ...
              'names') ;
  if length(mo) > 0
    t = t + 1 ;
    tests{t}.name = mo(1).name ;
    tests{t}.func = str2func(['test_' mo(1).name]) ;
    tests{t}.lineNumber = l ;
  end

  mo = regexp(tline, '^\s*function\s+(\w+\s*=\s*)?(?<name>setup|teardown)\s*\(.*\).*$', ...
              'names') ;
  if length(mo) > 0
    switch mo(1).name
      case 'setup'
        if isempty(setupFunc)
          setupFunc = @setup ;
        else
          warning('%s:%d: ignoring redundant definition of the setup function', ...
                  mfilePath) ;
        end
      case 'teardown'
        if isempty(teardownFunc)
          teardownFunc = @teardown ;
        else
          warning('%s:%d: ignoring redundant definition of the setup function', ...
                  mfilePath) ;
        end
      otherwise
        assert(0) ;
    end
  end
end

if ~isempty(setupFunc), state = setupFunc() ; end

if length(varargin) > 0
  testFunc = str2func(sprintf('test_%s', varargin{1})) ;
  if exist('state','var')
    testFunc(state) ;
  else
    testFunc() ;
  end
  if ~isempty(teardownFunc), teardownFunc(state) ; end
  return ;
end

% Run all tests collecting the results.
if length(tests) == 0
  error('No tests found in ''%s''', mfileName)  ;
end

for t = 1:length(tests)
  subTestName = sprintf('test_%s', tests{t}.name) ;
  results{t}.testName = sprintf('%s:%s', mfileName(9:end), tests{t}.name) ;
  results{t}.succeded = true ;
  results{t}.exception = [] ;
  try
    if exist('state', 'var')
      tests{t}.func(state) ;
    else
      tests{t}.func() ;
    end
  catch exception
    results{t}.succeded = false ;
    results{t}.exception = exception ;
  end
end

if ~isempty(teardownFunc), teardownFunc(state) ; end

results = cat(2, results{:}) ;
