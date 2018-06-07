classdef vl_test_plotbox < matlab.unittest.TestCase
  methods (Test)
    function test_basic(s)
      figure(1) ; clf ;
      vl_plotbox([-1 -1 1 1]') ;
      xlim([-2 2]) ;
      ylim([-2 2]) ;
      close(1) ;
    end
    function test_multiple(s)
      figure(1) ; clf ;
      randn('state', 0) ;
      vl_plotbox(randn(4,10)) ;
      close(1) ;
    end
    function test_style(s)
      figure(1) ; clf ;
      randn('state', 0) ;
      vl_plotbox(randn(4,10), 'r-.', 'LineWidth', 3) ;
      close(1) ;
    end
  end
end



