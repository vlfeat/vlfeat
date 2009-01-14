% VL_TEST_ALLDIST2

X = [4 ; 8] ;
Y = [2 ; 4] ;

fprintf('l0 norm') ;
disp(sum((X~=Y))) ;
disp(vl_alldist2(double(X),  double(Y), 'l0'  )) ;
disp(vl_alldist2(sparse(double(X)),  sparse(double(Y)), 'l0'  )) ;
disp(vl_alldist2(single(X),  single(Y), 'l0'  )) ;
disp(vl_alldist2( int8(X),    int8(Y) , 'l0'  )) ;
disp(vl_alldist2( int16(X),   int16(Y), 'l0'  )) ;
disp(vl_alldist2( int32(X),   int32(Y), 'l0'  )) ;
disp(vl_alldist2(uint8(X),   uint8(Y) , 'l0'  )) ;
disp(vl_alldist2(uint16(X),  uint16(Y), 'l0'  )) ;
disp(vl_alldist2(uint32(X),  uint32(Y), 'l0'  )) ;


fprintf('l1 norm') ;
disp(sum(abs(X-Y))) ;
disp(vl_alldist2(double(X),  double(Y), 'l1'  )) ;
disp(vl_alldist2(single(X),  single(Y), 'l1'  )) ;
disp(vl_alldist2( int8(X),    int8(Y) , 'l1'  )) ;
disp(vl_alldist2( int16(X),   int16(Y), 'l1'  )) ;
disp(vl_alldist2( int32(X),   int32(Y), 'l1'  )) ;
disp(vl_alldist2(uint8(X),   uint8(Y) , 'l1'  )) ;
disp(vl_alldist2(uint16(X),  uint16(Y), 'l1'  )) ;
disp(vl_alldist2(uint32(X),  uint32(Y), 'l1'  )) ;


fprintf('l2 norm') ;
disp(sum((X-Y).^2)) ;
disp(vl_alldist2(double(X),  double(Y), 'l2'  )) ;
disp(vl_alldist2(single(X),  single(Y), 'l2'  )) ;
disp(vl_alldist2( int8(X),    int8(Y) , 'l2'  )) ;
disp(vl_alldist2( int16(X),   int16(Y), 'l2'  )) ;
disp(vl_alldist2( int32(X),   int32(Y), 'l2'  )) ;
disp(vl_alldist2(uint8(X),   uint8(Y) , 'l2'  )) ;
disp(vl_alldist2(uint16(X),  uint16(Y), 'l2'  )) ;
disp(vl_alldist2(uint32(X),  uint32(Y), 'l2'  )) ;


fprintf('linf norm') ;
disp(max((X-Y))) ;
disp(vl_alldist2(double(X),  double(Y), 'linf'  )) ;
disp(vl_alldist2(single(X),  single(Y), 'linf'  )) ;
disp(vl_alldist2( int8(X),    int8(Y) , 'linf'  )) ;
disp(vl_alldist2( int16(X),   int16(Y), 'linf'  )) ;
disp(vl_alldist2( int32(X),   int32(Y), 'linf'  )) ;
disp(vl_alldist2(uint8(X),   uint8(Y) , 'linf'  )) ;
disp(vl_alldist2(uint16(X),  uint16(Y), 'linf'  )) ;
disp(vl_alldist2(uint32(X),  uint32(Y), 'linf'  )) ;


fprintf('chi2 norm') ;
disp(sum((X - Y).^2 ./ (X + Y) * 2 )) ;
disp(vl_alldist2(double(X),  double(Y), 'chi2'  )) ;
disp(vl_alldist2(single(X),  single(Y), 'chi2'  )) ;
disp(vl_alldist2( int8(X),    int8(Y) , 'chi2'  )) ;
disp(vl_alldist2( int16(X),   int16(Y), 'chi2'  )) ;
disp(vl_alldist2( int32(X),   int32(Y), 'chi2'  )) ;
disp(vl_alldist2(uint8(X),   uint8(Y) , 'chi2'  )) ;
disp(vl_alldist2(uint16(X),  uint16(Y), 'chi2'  )) ;
disp(vl_alldist2(uint32(X),  uint32(Y), 'chi2'  )) ;

