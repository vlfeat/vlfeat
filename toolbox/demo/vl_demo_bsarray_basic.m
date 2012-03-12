% VL_DEMO_BSARRAY_BASIC Demo: BSARRAY: basic functionality

% --------------------------------------------------------------------
%                                                     Create a BSARRAY
% --------------------------------------------------------------------

%size of the blocks
M = 30;
N = 1;

% Creates a bsarray from a sequence of blocks. If each block is a MxN
% matrix, and L is the number of blocks, the result will be a
% bsarray of L*MxN. The blocks can have different number of rows,
% but must have the same number of columns.
SP = single(zeros(M,N));
SP(10,1) = 1;
SP(20,1) = 10;
R = single(randn(M,N));
A1 = vl_bsarray_new(R);


A1 = vl_bsarray_add_block(A1,single(ones(M,N)),SP);


A1T = [R; single(ones(M,N)); single(full(SP))];


A2 = vl_bsarray_new(1,R,M+1,SP,2*M+1,single(ones(M,N)));

A2T = [R; single(full(SP)); single(ones(M,N));];

A1F = vl_bsarray_full(A1);
if ~all(single(A1F(:)) == A1T(:))
    error('A1F not correct!');
end

A2F = vl_bsarray_full(A2);
if ~all(single(A2F(:)) == A2T(:))
    error('A2F not correct!');
end

A1Q = vl_bsarray_mtimes(A1,A2F) ;

if (A1F'*A2F ~= A1Q)
    error('mtimes not working.') ;
end


A3 = vl_bsarray_new(single(A2F(:)),'Auto') ;

A3F = vl_bsarray_full(A3);

if ~all(single(A3F(:)) == single(A2F(:)))
    error('A2F not correct!');
end
