classdef vl_test_kmeans < matlab.unittest.TestCase
  properties
    X
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      t.X = randn(128, 100) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      [centers, assignments, en] = vl_kmeans(t.X, 10, 'NumRepetitions', 10) ;
      [centers_, assignments_, en_] = t.simpleKMeans(t.X, 10) ;
      assert(en_ <= 1.1 * en, 'vl_kmeans did not optimize enough') ;
    end
    function test_algorithms(t)
      distances = {'l1', 'l2'} ;
      dataTypes = {'single','double'} ;
      
      for dataType = dataTypes
        for distance = distances
          distance = char(distance) ;
          conversion = str2func(char(dataType)) ;
          X = conversion(t.X) ;
          vl_twister('state',0) ;
          [centers, assignments, en] = vl_kmeans(X, 10, ...
            'NumRepetitions', 1, ...
            'MaxNumIterations', 10, ...
            'Algorithm', 'Lloyd', ...
            'Distance', distance) ;
          vl_twister('state',0) ;
          [centers_, assignments_, en_] = vl_kmeans(X, 10, ...
            'NumRepetitions', 1, ...
            'MaxNumIterations', 10, ...
            'Algorithm', 'Elkan', ...
            'Distance', distance) ;
          
          vl_twister('state',0) ;
          [centers__, assignments__, en__] = vl_kmeans(X, 10, ...
            'NumRepetitions', 1, ...
            'MaxNumIterations', 10, ...
            'Algorithm', 'ANN', ...
            'Distance', distance, ...
            'NumTrees', 3, ...
            'MaxNumComparisons',0) ;
          
          t.verifyEqual(centers, centers_, 'AbsTol', 1e-5) ;
          t.verifyEqual(assignments, assignments_, 'AbsTol', 1e-5) ;
          t.verifyEqual(en, en_, 'AbsTol', 1e-4) ;
          
          t.verifyEqual(centers, centers__, 'AbsTol', 1e-5) ;
          t.verifyEqual(assignments, assignments__, 'AbsTol', 1e-5) ;
          t.verifyEqual(en, en__, 'AbsTol', 1e-4) ;
          
          t.verifyEqual(centers_, centers__, 'AbsTol', 1e-5) ;
          t.verifyEqual(assignments_, assignments__, 'AbsTol', 1e-5) ;
          t.verifyEqual(en_, en__, 'AbsTol', 1e-4) ;
        end
      end
    end
    function test_patterns(t)
      distances = {'l1', 'l2'} ;
      dataTypes = {'single','double'} ;
      for dataType = dataTypes
        for distance = distances
          distance = char(distance) ;
          conversion = str2func(char(dataType)) ;
          data = [1 1 0 0 ;
            1 0 1 0] ;
          data = conversion(data) ;
          [centers, assignments, en] = vl_kmeans(data, 4, ...
            'NumRepetitions', 100, ...
            'Distance', distance) ;
          t.verifyTrue(isempty(setdiff(data', centers', 'rows'))) ;
        end
      end
    end
  end
  methods (Static)
    function [centers, assignments, en] = simpleKMeans(X, numCenters)
      [dimension, numData] = size(X) ;
      centers = randn(dimension, numCenters) ;
      
      for iter = 1:10
        [dists, assignments] = min(vl_alldist(centers, X)) ;
        en = sum(dists) ;
        centers = [zeros(dimension, numCenters) ; ones(1, numCenters)] ;
        centers = vl_binsum(centers, ...
          [X ; ones(1,numData)], ...
          repmat(assignments, dimension+1, 1), 2) ;
        centers = centers(1:end-1, :) ./ repmat(centers(end,:), dimension, 1) ;
      end
    end    
  end
end