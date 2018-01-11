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
      t.verifyError(@()vl_argparse(opts, {'field2', 5}),?MException) ;
      t.verifyError(@()vl_argparse(opts, {'field1.field2', 5}),?MException) ;
      vl_argparse(opts, {'field1', 5}) ;
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
    
    function test_substruct(t)
      opts.field1.field1 = 1 ;
      opts.field2 = 2 ;
      
      opts_ = opts ;
      opts_.field1.field1 = 3 ;
      opts_.field2 = 10 ;
      
      % Test the dot-syntax
      opts = vl_argparse(opts, {'field2', 10, 'field1.field1', 3}) ;
      t.verifyEqual(opts, opts_) ;
      
      % Test the struct arguments
      opts_.field1.field1 = 5;
      opts = vl_argparse(opts, {'field1', struct('field1', 5)}) ;
      t.verifyEqual(opts, opts_) ;
      
      % Test the nonrecursive arg - replace a struct
      opts_.field1 = 3;
      opts = vl_argparse(opts, {'field1', 3}, 'nonrecursive') ;
      t.verifyEqual(opts, opts_) ;
    end
    
    function test_merge(t)
      opts.field1 = 1 ;
      opts.field2 = 2 ;
      
      opts_ = opts ;
      opts_.field1 = 3 ;
      opts_.field3 = 10 ;
      
      opts = vl_argparse(opts, {'field3', 10, 'field1', 3}, 'merge') ;
      t.verifyEqual(opts, opts_) ;
    end
  end
end