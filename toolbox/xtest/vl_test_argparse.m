classdef vl_test_argparse < matlab.unittest.TestCase  
  methods (Test)
    function test_basic(t)
      opts.field1 = 1 ;
      opts.field2 = 2 ;
      opts.field3 = 3 ;
      
      opts_ = opts ;
      opts_.field1 = 3 ;
      opts_.field2 = 10 ;
      
      opts = vl_argparse(opts, {'field2', 10, 'field1', 3}) ;
      t.verifyEqual(opts, opts_) ;
      
      opts_.field1 = 9 ;
      opts = vl_argparse(opts, {'field1', 4, 'field1', 9}) ;
      t.verifyEqual(opts, opts_) ;
    end
    
    function test_error(t)
      opts.field1 = 1 ;
      try
        opts = vl_argparse(opts, {'field2', 5}) ;
      catch e
        return ;
      end
      t.verifyTrue(false) ;
    end
    
    function test_leftovers(t)
      opts1.field1 = 1 ;
      opts2.field2 = 1 ;
      opts1_.field1 = 2 ;
      opts2_.field2 = 2 ;
      
      [opts1,args] = vl_argparse(opts1, {'field1', 2, 'field2', 2}) ;
      opts2 = vl_argparse(opts2, args) ;
      
      t.verifyEqual(isequal(opts1,opts1_), isequal(opts2,opts2_)) ;
    end
  end
end
