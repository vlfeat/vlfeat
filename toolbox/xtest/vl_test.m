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
%
%  Author:: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

clear functions ;

testRoot = fileparts(mfilename('fullpath')) ;

if nargin == 0
  files = dir(fullfile(testRoot, 'vl_test_*m')) ;
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

else
  feval(str2func(sprintf('vl_test_%s', suite)), test) ;
end