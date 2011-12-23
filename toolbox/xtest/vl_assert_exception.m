function vl_assert_exception(func, errorId, errorMessage)
% VL_ASSERT_EXCEPTION

try
  func() ;
catch
  e = lasterror ;
  if nargin >= 2
    assert(isequal(e.identifier, errorId), ...
           'Exception ID ''%s'' is not ''%s''.', ...
           e.identifier, errorId) ;
  end
  if nargin >= 3
    assert(isequal(e.message, errorMessage), ...
           'Exception message ''%s'' is not ''%s''.', ...
           e.message, errorMessage) ;
  end
  return ;
end
assert(0, 'The function did not generate an exception.') ;
end
