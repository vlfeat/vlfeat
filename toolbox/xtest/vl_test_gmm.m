function results = vl_test_gmm(varargin)
% VL_TEST_GMM

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

vl_test_init ;

end

function s = setup()
  randn('state',0) ;
  s.X = randn(128, 1000) ;
end

function test_multithreading(s)
  dataTypes = {'single','double'} ;

  for dataType = dataTypes
    conversion = str2func(char(dataType)) ;
    X = conversion(s.X) ;
    vl_twister('state',0) ;
    vl_threads(0) ;
    [means, covariances, priors, ll, posteriors] = ...
        vl_gmm(X, 10, ...
               'NumRepetitions', 1, ...
               'MaxNumIterations', 10, ...
               'Initialization', 'rand') ;
    vl_twister('state',0) ;
    vl_threads(1) ;
    [means_, covariances_, priors_, ll_, posteriors_] = ...
        vl_gmm(X, 10, ...
               'NumRepetitions', 1, ...
               'MaxNumIterations', 10, ...
               'Initialization', 'rand') ;

    vl_assert_almost_equal(means, means_, 1e-2) ;
    vl_assert_almost_equal(covariances, covariances_, 1e-2) ;
    vl_assert_almost_equal(priors, priors_, 1e-2) ;
    vl_assert_almost_equal(ll, ll_, 1e-2 * abs(ll)) ;
    vl_assert_almost_equal(posteriors, posteriors_, 1e-2) ;
  end
end
