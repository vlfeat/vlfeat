function results = vl_test_imdisttf(varargin)
% VL_TEST_DISTTF
vl_test_init ;

function test_basic()
for conv = {@single, @double}
  conv = conv{1} ;

  I = conv([0 0 0 ; 0 -2 0 ; 0 0 0]) ;
  D = vl_imdisttf(I);
  assert(isequal(D, conv(- [0 1 0 ; 1 2 1 ; 0 1 0]))) ;

  I(2,2) = -3 ;
  [D,map] = vl_imdisttf(I) ;
  assert(isequal(D, conv(-1 - [0 1 0 ; 1 2 1 ; 0 1 0]))) ;
  assert(isequal(map, 5 * ones(3))) ;
end

function test_1x1()
assert(isequal(1, vl_imdisttf(1))) ;

function test_rand()
I = rand(13,31) ;
for t=1:4
  param = [rand randn rand randn] ;
  [D0,map0] = imdisttf_equiv(I,param) ;
  [D,map] = vl_imdisttf(I,param) ;
  vl_assert_almost_equal(D,D0,1e-10)
  assert(isequal(map,map0)) ;
end

function test_param()
I = zeros(3,4) ;
I(1,1) = -1 ;

[D,map] = vl_imdisttf(I,[1 0 1 0]);
assert(isequal(-[1 0 0 0 ;
                 0 0 0 0 ;
                 0 0 0 0 ;], D)) ;

D0 = -[1 .9 .6 .1 ;
       0 0 0 0 ;
       0 0 0 0 ;] ;
[D,map] = vl_imdisttf(I,[.1 0 1 0]);
vl_assert_almost_equal(D,D0,1e-10);

D0 = -[1  .9 .6 .1 ;
       .9 .8 .5  0 ;
       .6 .5 .2  0 ;]  ;
[D,map] = vl_imdisttf(I,[.1 0 .1 0]);
vl_assert_almost_equal(D,D0,1e-10);

D0 = -[.9  1  .9  .6 ;
       .8 .9  .8  .5 ;
       .5 .6  .5  .2 ; ] ;
[D,map] = vl_imdisttf(I,[.1 1 .1 0]);
vl_assert_almost_equal(D,D0,1e-10);

function test_special()
I = rand(13,31) -.5 ;
D = vl_imdisttf(I, [0 0 1e5 0]) ;
vl_assert_almost_equal(D(:,1),min(I,[],2),1e-10);
D = vl_imdisttf(I, [1e5 0 0 0]) ;
vl_assert_almost_equal(D(1,:),min(I,[],1),1e-10);

function [D,map]=imdisttf_equiv(I,param)
D = inf + zeros(size(I)) ;
map = zeros(size(I)) ;
ur = 1:size(D,2) ;
vr = 1:size(D,1) ;
[u,v] = meshgrid(ur,vr) ;
for v_=vr
  for u_=ur
    E = I(v_,u_) + ...
        param(1) * (u - u_ - param(2)).^2 + ...
        param(3) * (v - v_ - param(4)).^2 ;
    map(E < D) = sub2ind(size(I),v_,u_) ;
    D = min(D,E) ;
  end
end
