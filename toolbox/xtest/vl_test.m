function results = vl_test(suite, test)
% VL_TEST  Run test suite
%  RESULTS = VL_TEST() runs all VLFeat test suites. The tests
%  verify that VLFeat is working correctly.
%
%  RESULTS is a structure listing the result of each test.
%  It has fileds:
%
%  TESTNAME:   name of the test
%  SUCCEDED:   a boolean flag indicating whether the test succeded
%  EXCEPTION:  the exception generated if the test failed
%
%  VL_TEST(SUITE) runs only the specified SUITE.
%
%  VL_TEST(SUITE, TEST) runs the specified SUITE/TEST without catching
%  the potential exception. Useful to DBSTOP to debug.

% Author: Andrea Vedaldi

% Copyright (C) 2013-14 Andrea Vedaldi.
% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

clear functions ;

testRoot = fileparts(mfilename('fullpath')) ;

if nargin == 0
  files = dir(fullfile(testRoot, 'vl_test_*.m')) ;
elseif nargin == 1
  files.name = fullfile(['vl_test_' suite '.m']) ;
end

if nargin < 2
  results = {} ;
  for i = 1:length(files)
    testCommand = files(i).name(1:end-2) ;
    if strcmp(testCommand, 'vl_test_init'), continue ; end
    fprintf('vl_test: running %s\n', testCommand) ;
    results{i} = feval(str2func(testCommand)) ;
  end

  results = cat(2, results{:}) ;
  for i = 1:length(results)
    if results(i).succeded
      fprintf('vl_test: %-35s ... passed\n', results(i).testName) ;
    else
      fprintf('vl_test: %-35s ... failed\n', results(i).testName) ;
      fprintf('%s:%d: %s\n', ...
              results(i).exception.stack(1).name, ...
              results(i).exception.stack(1).line, ...
              results(i).exception.message) ;
    end
  end

  fprintf('vl_test: %d tested, %d succeded, %d failed\n', ...
          numel(results), sum([results.succeded]), sum(~[results.succeded])) ;
else
  feval(str2func(sprintf('vl_test_%s', suite)), test) ;
end
