function results = vl_test_aib(varargin)
% VL_TEST_AIB
vl_test_init ;

function s = setup()
s = [] ;

function test_basic(s)
Pcx = [.3 .3 0   0
       0   0   .2 .2] ;

% This results in the AIB tree
%
%  1 - \
%       5 - \
%  2 - /     \
%             - 7
%  3 - \     /
%       6 - /
%  4 - /
%
% coded by the map [5 5 6 6 7 1] (1 denotes the root).

[parents,cost] = vl_aib(Pcx) ;
vl_assert_equal(parents, [5 5 6 6 7 7 1]) ;
vl_assert_almost_equal(mi(Pcx)*[1 1 1], cost(1:3), 1e-3) ;

[cut,map,short] = vl_aibcut(parents,2) ;
vl_assert_equal(cut, [5 6]) ;
vl_assert_equal(map, [1 1 2 2 1 2 0]) ;
vl_assert_equal(short, [5 5 6 6 5 6 7]) ;

function test_cluster_null(s)
Pcx = [.5 .5   0   0
       0   0   0   0] ;

% This results in the AIB tree
%
%  1 - \
%       5
%  2 - /
%
%  3 x
%
%  4 x
%
% If ClusterNull is specified, the values 3 and 4
% which have zero probability are merged first
%
%  1 ----------\
%               7
%  2 ----- \   /
%           6-/
%  3 -\    /
%      5 -/
%  4 -/

parents1 = vl_aib(Pcx) ;
parents2 = vl_aib(Pcx,'ClusterNull') ;
vl_assert_equal(parents1, [5 5 0 0 1 0 0]) ;
vl_assert_equal(parents2(3), parents2(4)) ;

function x = mi(P)
% mutual information
P1 = sum(P,1) ;
P2 = sum(P,2) ;
x = sum(sum(P .* log(max(P,1e-10) ./ (P2*P1)))) ;
