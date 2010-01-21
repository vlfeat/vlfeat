function vl_test_binsum
% VL_TEST_BINSUM  Test VL_BINSUM function

testh({[0 0],   1, 2},                  [0 1]   ) ;
testh({[1 7],  -1, 1},                  [0 7]   ) ;
testh({[1 7],  -1, [1 2 2 2 2 2 2 2]},  [0 0]   ) ;
testh({eye(3), [1 1 1],  [1 2 3],  1 }, 2*eye(3)) ;
testh({eye(3), [1 1 1]', [1 2 3]', 2 }, 2*eye(3)) ;
testh({eye(3), 1, [1 2 3],  1 },        2*eye(3)) ;
testh({eye(3), 1, [1 2 3]', 2 },        2*eye(3)) ;

Z = zeros(3,3,3) ;
B = 3*ones(3,1,3) ;
R = Z ; R(:,3,:) = 17 ;

testh({Z, 17, B, 2}, R) ;

Z = zeros(3,3,3) ;
B = 3*ones(3,3,1) ;
X = zeros(3,3,1) ; X(:,:,1) = 17 ;
R = Z ; R(:,:,3) = 17 ;

testh({Z, X, B, 3}, R) ;

function testh(args, H_)
H__ = vl_binsum(args{:}) ;
if any(any(any(H_ ~= H__)))
  fprintf('H:\n') ; disp(args{1});
  fprintf('X:\n') ; disp(args{2});
  fprintf('B:\n') ; disp(args{3});
  if length(args) > 3, 
    fprintf('d:\n') ; disp(args{4}) ;
  end
  fprintf('R computed:\n') ; disp(H__) ;
  fprintf('R correct:\n') ; disp(H_) ;
  error('vl_binsum regression test failed') ;
end
