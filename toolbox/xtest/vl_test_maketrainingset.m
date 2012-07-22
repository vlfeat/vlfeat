function results = vl_test_maketrainingset(varargin)
% VL_TEST_KDTREE
vl_test_init ;

function s = setup()
randn('state',0) ;

s.biasMultiplier = 10 ;
s.lambda = 0.01 ;

Np = 10 ;
Nn = 10 ;
Xp = diag([1 3])*randn(2, Np) ;
Xn = diag([1 3])*randn(2, Nn) ;
Xp(1,:) = Xp(1,:) + 2 + 1 ;
Xn(1,:) = Xn(1,:) - 2 + 1 ;

s.X = [Xp Xn] ;
s.y = [ones(1,Np) -ones(1,Nn)] ;


function test_plain_trainingset(s)
for conv = {@single,@double}
  vl_twister('state',0) ;
  conv = conv{1} ;
  tset = vl_maketrainingset(conv(s.X),int8(s.y)) ;
  vl_assert_almost_equal(tset.data, conv(s.X), 0.1) ;
  vl_assert_almost_equal(tset.labels, int8(s.y), 0.1) ;
end


function test_homkermap(s)
for conv = {@single,@double}
  vl_twister('state',0) ;
  conv = conv{1} ;
  tset = vl_maketrainingset(conv(s.X),int8(s.y),'homkermap',1, ...
                            'kchi2', 'gamma', .5) ;
  
  vl_assert_almost_equal(tset.data, conv(s.X), 0.1) ;
  vl_assert_almost_equal(tset.labels, int8(s.y), 0.1) ;
  vl_assert_equal(tset.map.order, 1) ;
end
