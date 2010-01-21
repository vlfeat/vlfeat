function vl_assert_equal(x, y, varargin)
  assert(isequalwithequalnans(x,y),varargin{:}) ;
end
