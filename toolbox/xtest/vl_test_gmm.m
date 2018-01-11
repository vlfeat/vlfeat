classdef vl_test_gmm < matlab.unittest.TestCase
  properties
    X
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      t.X = randn(128, 1000) ;
    end
  end
  
  methods (Test)
    function test_multithreading(t)
      dataTypes = {'single','double'} ;      
      for dataType = dataTypes
        conversion = str2func(char(dataType)) ;
        X = conversion(t.X) ;
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
        
        t.verifyEqual(means, means_, 'AbsTo', 1e-2) ;
        t.verifyEqual(covariances, covariances_, 'AbsTo', 1e-2) ;
        t.verifyEqual(priors, priors_, 'AbsTo', 1e-2) ;
        t.verifyEqual(ll, ll_, 'AbsTo', 1e-2 * abs(ll)) ;
        t.verifyEqual(posteriors, posteriors_, 'AbsTo', 1e-2) ;
      end
    end
  end
end
