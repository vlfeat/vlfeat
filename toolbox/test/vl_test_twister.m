function vl_test_twister
% VL_TEST_TWISTER

% test seed by scalar
rand('twister',1) ; a = rand ;
vl_twister('state',1) ; b = vl_twister ;
check(a,b,'twister: seed by scalar + VL_TWISTER()') ;

% read state
rand('twister') ; a = rand('twister') ;
vl_twister('state') ; b = vl_twister('state') ;
check(a,b,'twister: read state') ;

% set state
a(1) = a(1)+1 ;
rand('twister',a) ; b = rand('twister') ;
check(a,b,'twister: set state') ;

% VL_TWISTER([M N P ...])
rand('twister',b) ;
vl_twister('state',b) ;
a=rand([1 2 3 4 5]) ;
b=vl_twister([1 2 3 4 5]) ;
check(a,b,'twister: VL_TWISTER([M N P ...])') ;

% VL_TWISTER(M, N, P ...)
a=rand(1, 2, 3, 4, 5) ;
b=vl_twister(1, 2, 3, 4, 5) ;
check(a,b,'twister: VL_TWISTER(M, N, P, ...)') ;

% VL_TWISTER(M, N, P ...)
a=rand(1, 2, 3, 4, 5) ;
b=vl_twister(1, 2, 3, 4, 5) ;
check(a,b,'twister: VL_TWISTER(M, N, P, ...)') ;

% VL_TWISTER(N)
a=rand(10) ;
b=vl_twister(10) ;
check(a,b,'twister: VL_TWISTER(N)') ;

% ---------------------------------------------------------------
function check(a,b,msg)

fprintf('test: %-40s ... ', msg) ;
if isequal(a,b)
  fprintf('ok.\n') ;
else
  fprintf('!!!! FAIL !!!!\n') ;
  keyboard
end
