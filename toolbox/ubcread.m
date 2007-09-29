function [f,d] = ubcread(file) 
% SIFTREAD Read Lowe's SIFT implementation data files
%   [F,D] = UBCREAD(FILE) reads the frames F and the descriptors D
%   from FILE in UBC (Lowe's SIFT) or Oxford format.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

verbosity=0 ;

g = fopen(file, 'r');
if g == -1
    error(['Could not open file ''', file, '''.']) ;
end
[header, count] = fscanf(g, '%d', [1 2]) ;
if count ~= 2
    error('Invalid keypoint file header.');
end
K = header(1) ; 
DL = header(2) ;

if(verbosity > 0)
	fprintf('%d keypoints, %d descriptor length.\n', K, DL) ;
end

%creates two output matrices
P = zeros(4,K) ;
L = zeros(DL,K) ;

%parse tmp.key
for k = 1:K

	% Record format: i,j,s,th
	[record, count] = fscanf(g, '%f', [1 4]) ; 
	if count ~= 4
		error(...
			sprintf('Invalid keypoint file (parsing keypoint %d)',k) );
	end
	P(:,k) = record(:) ;
	
	% Record format: descriptor
	[record, count] = fscanf(g, '%d', [1 DL]) ;
	if count ~= DL
		error(...
			sprintf('Invalid keypoint file (parsing keypoint %d)',k) );
	end
	L(:,k) = record(:) ;
	
end
fclose(g) ;

L=double(L) ;
P(1:2,:) = flipud(P(1:2,:)) + 1 ; % i,j -> x,y

f=[ P(1:2,:) ; P(3,:) ; -P(4,:) ] ;
d=uint8(L) ;

p=[1 2 3 4 5 6 7 8] ;
q=[1 8 7 6 5 4 3 2] ;
for j=0:3
  for i=0:3
   d(8*(i+4*j)+p,:) = d(8*(i+4*j)+q,:) ;
  end
end