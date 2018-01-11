classdef vl_test_imdisttf < matlab.unittest.TestCase
  methods (Test)
    function test_basic(t)
      for conv = {@single, @double}
        conv = conv{1} ;        
        I = conv([0 0 0 ; 0 -2 0 ; 0 0 0]) ;
        D = vl_imdisttf(I);
        t.verifyEqual(D, conv(- [0 1 0 ; 1 2 1 ; 0 1 0])) ;        
        I(2,2) = -3 ;
        [D,map] = vl_imdisttf(I) ;
        t.verifyEqual(D, conv(-1 - [0 1 0 ; 1 2 1 ; 0 1 0])) ;
        t.verifyEqual(map, 5 * ones(3)) ;
      end
    end    
    function test_1x1(t)
      t.verifyEqual(1, vl_imdisttf(1)) ;
    end    
    function test_rand(t)
      I = rand(13,31) ;
      for q=1:4
        param = [rand randn rand randn] ;
        [D0,map0] = t.imdisttf_equiv(I,param) ;
        [D,map] = vl_imdisttf(I,param) ;
        t.verifyEqual(D,D0,'AbsTol',1e-10)
        t.verifyEqual(map,map0) ;
      end
    end
    function test_param(t)
      I = zeros(3,4) ;
      I(1,1) = -1 ;
      
      [D,map] = vl_imdisttf(I,[1 0 1 0]);
      t.verifyEqual(-[1 0 0 0 ;
        0 0 0 0 ;
        0 0 0 0 ;], D) ;
      
      D0 = -[1 .9 .6 .1 ;
        0 0 0 0 ;
        0 0 0 0 ;] ;
      [D,map] = vl_imdisttf(I,[.1 0 1 0]);
      t.verifyEqual(D,D0,'AbsTol',1e-10);
      
      D0 = -[1  .9 .6 .1 ;
        .9 .8 .5  0 ;
        .6 .5 .2  0 ;]  ;
      [D,map] = vl_imdisttf(I,[.1 0 .1 0]);
      t.verifyEqual(D,D0,'AbsTol',1e-10);
      
      D0 = -[.9  1  .9  .6 ;
        .8 .9  .8  .5 ;
        .5 .6  .5  .2 ; ] ;
      [D,map] = vl_imdisttf(I,[.1 1 .1 0]);
      t.verifyEqual(D,D0,'AbsTol',1e-10);
    end
    function test_special(t)
      I = rand(13,31) -.5 ;
      D = vl_imdisttf(I, [0 0 1e5 0]) ;
      t.verifyEqual(D(:,1),min(I,[],2),'AbsTol',1e-10);
      D = vl_imdisttf(I, [1e5 0 0 0]) ;
      t.verifyEqual(D(1,:),min(I,[],1),'AbsTol',1e-10);
    end
  end
  methods (Static)
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
    end
  end
end
