function results = vl_test_imarray(varargin)
% VL_TEST_IMARRAY
vl_test_init ;

function test_movie_rgb(s)
A = rand(23,15,3,4) ;
B = vl_imarray(A,'movie',true) ;

function test_movie_indexed(s)
cmap = get(0,'DefaultFigureColormap') ;
A = uint8(size(cmap,1)*rand(23,15,4)) ;
A = min(A,size(cmap,1)-1) ;
B = vl_imarray(A,'movie',true) ;

function test_movie_gray_indexed(s)
A = uint8(255*rand(23,15,4)) ;
B = vl_imarray(A,'movie',true,'cmap',gray(256)) ;

for k=1:size(A,3)
  vl_assert_equal(squeeze(A(:,:,k)), ...
                  frame2im(B(k))) ;
end

function test_basic(s)
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
vl_assert_equal(A2,B) ;
