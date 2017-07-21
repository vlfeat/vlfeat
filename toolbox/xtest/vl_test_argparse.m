function results = vl_test_argparse(varargin)
% VL_TEST_ARGPARSE
vl_test_init ;

function test_basic()
opts.field1 = 1 ;
opts.field2 = 2 ;
opts.field3 = 3 ;

opts_ = opts ;
opts_.field1 = 3 ;
opts_.field2 = 10 ;

opts = vl_argparse(opts, {'field2', 10, 'field1', 3}) ;
assert(isequal(opts, opts_)) ;

opts_.field1 = 9 ;
opts = vl_argparse(opts, {'field1', 4, 'field1', 9}) ;
assert(isequal(opts, opts_)) ;

function test_error()
opts.field1 = 1 ;
try
  opts = vl_argparse(opts, {'field2', 5}) ;
catch e
  return ;
end
% Test the recursive case
opts.field1.field1 = 1 ;
try
  opts = vl_argparse(opts, {'field1.field2', 5}) ;
catch e
  return ;
end
% Test assigning non-struct to a struct
try
  opts = vl_argparse(opts, {'field1', 5}) ;
catch e
  return ;
end
assert(false) ;

function test_leftovers()
opts1.field1 = 1 ;
opts2.field2 = 1 ;
opts1_.field1 = 2 ;
opts2_.field2 = 2 ;

[opts1,args] = vl_argparse(opts1, {'field1', 2, 'field2', 2}) ;
opts2 = vl_argparse(opts2, args) ;

assert(isequal(opts1,opts1_), isequal(opts2,opts2_)) ;


function test_substruct()
opts.field1.field1 = 1 ;
opts.field2 = 2 ;

opts_ = opts ;
opts_.field1.field1 = 3 ;
opts_.field2 = 10 ;

% Test the dot-syntax
opts = vl_argparse(opts, {'field2', 10, 'field1.field1', 3}) ;
assert(isequal(opts, opts_)) ;

% Test the struct arguments
opts_.field1.field1 = 5;
opts = vl_argparse(opts, {'field1', struct('field1', 5)}) ;
assert(isequal(opts, opts_)) ;

% Test the nonrecursive arg - replace a struct
opts_.field1 = 3;
opts = vl_argparse(opts, {'field1', 3}, 'nonrecursive') ;
assert(isequal(opts, opts_)) ;


function test_merge()
opts.field1 = 1 ;
opts.field2 = 2 ;

opts_ = opts ;
opts_.field1 = 3 ;
opts_.field3 = 10 ;

opts = vl_argparse(opts, {'field3', 10, 'field1', 3}, 'merge') ;
assert(isequal(opts, opts_)) ;
