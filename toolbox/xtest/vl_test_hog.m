function results = vl_test_hog(varargin)
% VL_TEST_HOG
vl_test_init ;

function s = setup()
s.im = im2single(vl_impattern('roofs1')) ;
[x,y]= meshgrid(linspace(-1,1,128)) ;
s.round = single(x.^2+y.^2);
s.imSmall = s.im(1:128,1:128,:) ;
s.imSmall = s.im ;
s.imSmallFlipped = s.imSmall(:,end:-1:1,:) ;

function test_basic_call(s)
cellSize = 8 ;
hog = vl_hog(s.im, cellSize) ;

function test_bilinear_orientations(s)
cellSize = 8 ;
vl_hog(s.im, cellSize, 'bilinearOrientations') ;

function test_variants_and_flipping(s)
variants = {'uoctti', 'dalaltriggs'} ;
numOrientationsRange = 3:9 ;
cellSize = 8 ;

for cellSize = [4 8 16]
  for i=1:numel(variants)
    for j=1:numel(numOrientationsRange)
      args = {'bilinearOrientations', ...
              'variant', variants{i}, ...
              'numOrientations', numOrientationsRange(j)} ;
      hog = vl_hog(s.imSmall, cellSize, args{:}) ;
      perm = vl_hog('permutation', args{:}) ;
      hog1 = vl_hog(s.imSmallFlipped, cellSize, args{:}) ;
      hog2 = hog(:,end:-1:1,perm) ;
      %norm(hog1(:)-hog2(:))
      vl_assert_almost_equal(hog1,hog2,1e-3) ;
    end
  end
end

function test_polar(s)
cellSize = 8 ;
im = s.round ;
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
  vl_assert_almost_equal(hog1,hog2,norm(hog1(:))/1000) ;
end
