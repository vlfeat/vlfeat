function test_twister
% TEST_TWISTER

% test seed by scalar
rand('twister',1) ; a = rand ;
twister('state',1) ; b = twister ;
check(a,b,'twister: seed by scalar + TWISTER()') ;

% read state
rand('twister') ; a = rand('twister') ;
twister('state') ; b = twister('state') ;
check(a,b,'twister: read state') ;

% set state
a(1) = a(1)+1 ;
rand('twister',a) ; b = rand('twister') ;
check(a,b,'twister: set state') ;

% TWISTER([M N P ...])  
rand('twister',b) ; 
twister('state',b) ;
a=rand([1 2 3 4 5]) ;
b=twister([1 2 3 4 5]) ;
check(a,b,'twister: TWISTER([M N P ...])') ;

% TWISTER(M, N, P ...)  
a=rand(1, 2, 3, 4, 5) ;
b=twister(1, 2, 3, 4, 5) ;
check(a,b,'twister: TWISTER(M, N, P, ...)') ;

% TWISTER(M, N, P ...)  
a=rand(1, 2, 3, 4, 5) ;
b=twister(1, 2, 3, 4, 5) ;
check(a,b,'twister: TWISTER(M, N, P, ...)') ;

% TWISTER(N)
a=rand(10) ;
b=twister(10) ;
check(a,b,'twister: TWISTER(N)') ;

% ---------------------------------------------------------------
function check(a,b,msg)

fprintf('test: %-40s ... ', msg) ;
if isequal(a,b)
  fprintf('ok.\n') ;
else
  fprintf('!!!! FAIL !!!!\n') ;
  keyboard
end
