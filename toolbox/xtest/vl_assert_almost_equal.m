function vl_assert_almost_equal(x, y, varargin)
  epsilon = 1e-6 ;
  if length(varargin) > 0
    if isnumeric(varargin{1})
      epsilon = varargin{1} ;
      varargin(1) = [] ;
    end
  end
  assert(isequal(class(x), class(y)), varargin{:}) ;
  assert(isequal(size(x),size(y)), varargin{:}) ;
  i = isnan(x) ;
  assert(all(isnan(y(i))), varargin{:}) ;
  assert(max(abs(x(~i) - y(~i))) < epsilon, varargin{:}) ;
end
