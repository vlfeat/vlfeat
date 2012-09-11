function results = vl_test_hog(varargin)
% VL_TEST_HOG
vl_test_init ;

function s = setup()
s.im = im2single(imread(fullfile(vl_root,'data','a.jpg'))) ;
[x,y]= meshgrid(linspace(-1,1,128)) ;
s.im = single(x.^2+y.^2);
%s.imSmall = s.im(1:128,1:128,:) ;
s.imSmall = s.im ;
s.imSmallFlipped = s.imSmall(:,end:-1:1,:) ;

function test_basic_call(s)
cellSize = 8 ;
%hog = vl_hog(s.im, cellSize) ;

function test_variants_and_flipping(s)
variants = {'uoctti', 'dalaltriggs'} ;
numOrientationsRange = 3:9 ;
cellSize = 8 ;

for cellSize = [4 8 16]
  for i=1:numel(variants)
    for j=1:numel(numOrientationsRange)
      args = {'variant', variants{i}, ...
              'numOrientations', numOrientationsRange(j)} ;
      hog = vl_hog(s.imSmall, cellSize, args{:}) ;
      perm = vl_hog('permutation', args{:}) ;
      hog1 = vl_hog(s.imSmallFlipped, cellSize, args{:}) ;
      hog2 = hog(:,end:-1:1,perm) ;
      vl_assert_almost_equal(hog1,hog2,1e-3) ;
    end
  end
end
