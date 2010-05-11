% VL_TEST_INIT  Intialize test file
%   This is a VLFeat internal function used to initialize
%   a file containing tests.
%
%   Author:: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

% obtain path to caller m-file
stack = dbstack(1) ;
mfilePath = stack(1).file ;
mfileName = stack(1).name ;
fid = fopen(mfilePath, 'r') ;

% Parse source code to get subfunction names. It could use
% which('-subfun', stack(1).name) instead, but it is undocumented.

setupFunc = [] ;
teardownFunc = [] ;

tests = {} ;
l = 0 ;
t = 0 ;
while 1
  tline = fgetl(fid) ;
  l = l + 1 ;
  if ~ischar(tline), break ; end
  mo = regexp(tline, '^\s*function\s+(\w\s*=\s*)?test_(?<name>[\w_]+)\s*\(.*\)\s*$', ...
              'names') ;
  if length(mo) > 0
    t = t + 1 ;
    tests{t}.name = mo(1).name ;
    tests{t}.func = str2func(['test_' mo(1).name]) ;
    tests{t}.lineNumber = l ;
  end

  mo = regexp(tline, '^\s*function\s+(\w+\s*=\s*)?(?<name>setup|teardown)\s*\(.*\)\s*$', ...
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
