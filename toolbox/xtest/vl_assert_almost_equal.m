function vl_assert_almost_equal(x, y, varargin)
  epsilon = 1e-6 ;
  if length(varargin) > 0
    if isnumeric(varargin{1})
      epsilon = varargin{1} ;
      varargin(1) = [] ;
    end
  end
  assert(isequal(class(x), class(y)), varargin{:}) ;
  assert(isequal(size(x), size(y)), varargin{:}) ;
  if isstruct(x)
    fx = fieldnames(x) ;
    fy = fieldnames(y) ;
    assert(isequal(fx,fy), varargin{:}) ;
    for i=1:numel(fx)
      vl_assert_almost_equal(x.(fx{i}), y.(fy{i}), epsilon, varargin{:}) ;
    end
  else
    i = isnan(x) ;
    j = isnan(y) ;
    assert(isequal(i,j), varargin{:}) ;
    assert(max(abs(x(~i) - y(~i))) < epsilon, varargin{:}) ;
  end
end
