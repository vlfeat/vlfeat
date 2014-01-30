function results = vl_test_kmeans(varargin)
% VL_TEST_KMEANS

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

vl_test_init ;

function s = setup()
randn('state',0) ;
s.X = randn(128, 100) ;

function test_basic(s)
[centers, assignments, en] = vl_kmeans(s.X, 10, 'NumRepetitions', 10) ;
[centers_, assignments_, en_] = simpleKMeans(s.X, 10) ;
assert(en_ <= 1.1 * en, 'vl_kmeans did not optimize enough') ;

function test_algorithms(s)
distances = {'l1', 'l2'} ;
dataTypes = {'single','double'} ;

for dataType = dataTypes
  for distance = distances
    distance = char(distance) ;
    conversion = str2func(char(dataType)) ;
    X = conversion(s.X) ;
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

    vl_assert_almost_equal(centers, centers_, 1e-5) ;
    vl_assert_almost_equal(assignments, assignments_, 1e-5) ;
    vl_assert_almost_equal(en, en_, 1e-4) ;

    vl_assert_almost_equal(centers, centers__, 1e-5) ;
    vl_assert_almost_equal(assignments, assignments__, 1e-5) ;
    vl_assert_almost_equal(en, en__, 1e-4) ;

    vl_assert_almost_equal(centers_, centers__, 1e-5) ;
    vl_assert_almost_equal(assignments_, assignments__, 1e-5) ;
    vl_assert_almost_equal(en_, en__, 1e-4) ;
  end
end

function test_patterns(s)
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
    assert(isempty(setdiff(data', centers', 'rows'))) ;
  end
end

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
