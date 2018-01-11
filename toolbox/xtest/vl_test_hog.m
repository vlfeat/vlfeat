classdef vl_test_hog < matlab.unittest.TestCase
  properties
    round
    im
    imSmall
    imSmallFlipped
  end
  
  methods (TestClassSetup)
    function setup(t)
      t.im = im2single(vl_impattern('roofs1')) ;
      [x,y]= meshgrid(linspace(-1,1,128)) ;
      t.round = single(x.^2+y.^2);
      t.imSmall = t.im(1:128,1:128,:) ;
      t.imSmall = t.im ;
      t.imSmallFlipped = t.imSmall(:,end:-1:1,:) ;
    end
  end
    
  methods (Test)
    function test_basic_call(t)
      cellSize = 8 ;
      hog = vl_hog(t.im, cellSize) ;
      t.verifyTrue(true) ;
    end
    function test_bilinear_orientations(t)
      cellSize = 8 ;
      vl_hog(t.im, cellSize, 'bilinearOrientations') ;
      t.verifyTrue(true) ;
    end
    function test_variants_and_flipping(t)
      variants = {'uoctti', 'dalaltriggs'} ;
      numOrientationsRange = 3:9 ;
      cellSize = 8 ;      
      for cellSize = [4 8 16]
        for i=1:numel(variants)
          for j=1:numel(numOrientationsRange)
            args = {'bilinearOrientations', ...
              'variant', variants{i}, ...
              'numOrientations', numOrientationsRange(j)} ;
            hog = vl_hog(t.imSmall, cellSize, args{:}) ;
            perm = vl_hog('permutation', args{:}) ;
            hog1 = vl_hog(t.imSmallFlipped, cellSize, args{:}) ;
            hog2 = hog(:,end:-1:1,perm) ;
            %norm(hog1(:)-hog2(:))
            t.verifyEqual(hog1,hog2,'AbsTol',single(1e-3)) ;
          end
        end
      end
    end
    function test_polar(t)
      cellSize = 8 ;
      im = t.round ;
      for b = [0 1]
        if b
          args = {'bilinearOrientations'} ;
        else
          args = {} ;
        end
        hog1 = vl_hog(im, cellSize, args{:}) ;
        [ix,iy] = vl_grad(im) ;
        m = sqrt(ix.^2 + iy.^2) ;
        a = atan2(iy,ix) ;
        m(:,[1 end]) = 0 ;
        m([1 end],:) = 0 ;
        hog2 = vl_hog(cat(3,m,a), cellSize, 'DirectedPolarField', args{:}) ;
        tol = norm(hog1(:))/1000 ;
        if b == 1, tol = tol * 100 ; end
        t.assertEqual(hog1,hog2,'AbsTol',single(tol)) ;
      end
    end
  end
end
