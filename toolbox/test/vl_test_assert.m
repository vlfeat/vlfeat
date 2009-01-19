function condition = vl_test_assert(condition, message)
%
%
%
  
stack = dbstack('-completenames') ;
testName = sprintf('%s:%d:%s', stack(2).name, stack(2).line, message) ;

if condition
  fprintf('%s ... pass\n', testName) ;
else
  fprintf('%s ... FAIL\n', testName) ;
end
  