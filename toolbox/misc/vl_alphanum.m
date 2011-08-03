function [strings, perm] = vl_alphanum(strings)
% VL_ALPHANUM  Sort strings using the Alphanum algorithm
%   STRINGS = VL_ALPHANUM(STRINGS) sorts the cell array of strings
%   STRINGS by using the Alphanum algorithm [1]. [STRINGS,PERM] =
%   VL_ALPHANUM(...) returns the corresponding permutation PERM as
%   well.
%
%   Example::
%     Alphanum sorts strings in a way that 'makes sense'. For instance
%
%      strings = {'B1', 'B2', 'B12', 'A12', 'A1', 'A2'} ;
%      sorted = vl_alphanum(strings) ;
%
%     produces the sorted array {'A1', 'A2', 'A12', 'B1', B2',
%     'B12'}. By contrast, SORT() produces the array {'A1', 'A12',
%     'A2', 'B1', B12', 'B2'} (note the position of the elements
%     'A12', 'B12').
%
%   References:
%   [1] Dave Koelle, 'The Alphanum Algorithm',
%   http://www.davekoelle.com/alphanum.html

chunks = regexp(strings, '(\d+|\D+)', 'tokens') ;
for i = 1:length(strings)
  chunks{i} = [chunks{i}{:}] ;
  for j = 1:length(chunks{i})
    if isstrprop(chunks{i}{j},'digit')
      chunks{i}{j} = sprintf('%020.0f',sscanf(chunks{i}{j},'%d')) ;
    end
  end
  chunks{i} = [chunks{i}{:}] ;
end

[dorp,perm] = sort(chunks) ;
strings = strings(perm) ;
