classdef vl_test_imarray < matlab.unittest.TestCase
  methods (Test)    
    function test_movie_rgb(t)
      A = rand(23,15,3,4) ;
      B = vl_imarray(A,'movie',true) ;
      t.verifyTrue(true) ;
    end   
    function test_movie_indexed(t)
      cmap = get(0,'DefaultFigureColormap') ;
      A = uint8(size(cmap,1)*rand(23,15,4)) ;
      A = min(A,size(cmap,1)-1) ;
      B = vl_imarray(A,'movie',true) ;
      t.verifyTrue(true) ;
    end
    function test_movie_gray_indexed(t)
      A = uint8(255*rand(23,15,4)) ;
      B = vl_imarray(A,'movie',true,'cmap',gray(256)) ;      
      for k=1:size(A,3)
        t.verifyEqual(squeeze(A(:,:,k)), ...
          frame2im(B(k))) ;
      end
    end
    function test_basic(t)
      M = 3 ;
      N = 4 ;
      width = 32 ;
      height = 15 ;
      for i=1:M
        for j=1:N
          A{i,j} = rand(width,height) ;
        end
      end
      A1 = A';
      A1 = cat(3,A1{:}) ;
      A2 = cell2mat(A) ;
      B = vl_imarray(A1, 'layout', [M N]) ;
      t.verifyEqual(A2,B) ;
    end
  end
end