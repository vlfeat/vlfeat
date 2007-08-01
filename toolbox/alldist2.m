% ALLDIST2  Pairwise Euclidean distance
%  D = ALLDIST2(S1,S2) returns the pairwise distance matrix D of
%  the columns of S1 and S2, yielding
%
%   D(i,j) = sum( ( S1(:,i) - S2(:,j) ).^2 ) for all i,j.
%
%  ALLDIST2(S) returns the pairwise distance matrix fo the columns
%  of S1, yielding
%
%   D(i,j) = sum( ( S(:,i) - S(:,j) ).^2 ) for all i,j.
%
%  ALLDIST2() supports directly most data types. S1 and S2 must
%  have the same storage class. D has a `promoted' storage class
%  (to reduce the chance of overflow) defined as such:
%
%   S1 & S2 CLASS     D CLASS
%   UINT8             UINT32
%    INT8             UINT32
%   UINT16            UINT32
%    INT16            UINT32
%   UINT32            UINT32
%    INT32            UINT32
%   SINGLE            SINGLE
%   DOUBLE            DOUBLE

