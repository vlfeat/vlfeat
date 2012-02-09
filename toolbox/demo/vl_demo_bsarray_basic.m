% VL_DEMO_BSARRAY_BASIC Demo: BSARRAY: basic functionality

% --------------------------------------------------------------------
%                                                     Create a BSARRAY
% --------------------------------------------------------------------

%size of the blocks
M = 10;
N = 1;

% Creates a bsarray from a sequence of blocks. If each block is a MxN
% matrix, and L is the number of blocks, the result will be a
% bsarray of L*MxN. The blocks can have different number of rows,
% but must have the same number of columns.
SP = sparse(zeros(M,N));
SP(5,1) = 1;
SP(10,1) = 10;
R = single(randn(M,N));
A1 = vl_bsarray(0,1,vl_bsarray.FLOAT,R,M+1,vl_bsarray.FLOAT, ...
               single(ones(M,N)),2*M+1,vl_bsarray.FLOAT,single(full(SP)));

A1T = [R; single(ones(M,N)); single(full(SP))];


A2 = vl_bsarray(0,1,vl_bsarray.FLOAT,R,M+1,vl_bsarray.FLOAT,single(full(SP)), 2*M+1,vl_bsarray.FLOAT, ...
               single(ones(M,N)));

A2T = [R; single(full(SP)); single(ones(M,N));];

A1F = full(A1);
if ~all(A1F(:) == A1T(:))
    error('A1F not correct!');
end

A2F = full(A2);
if ~all(A2F(:) == A2T(:))
    error('A2F not correct!');
end


if (single(A1F)'*single(A2F) ~= A1'*single(A2F))
    error('mtimes not working.') ;
end
