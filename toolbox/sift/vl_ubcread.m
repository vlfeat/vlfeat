function [f,d] = vl_ubcread(file, varargin) 
% SIFTREAD Read Lowe's SIFT implementation data files
%   [F,D] = VL_UBCREAD(FILE) reads the frames F and the descriptors D
%   from FILE in UBC (Lowe's original implementation of SIFT) format
%   and returns F and D as defined by VL_SIFT().
%
%   VL_UBCREAD(FILE, 'FORMAT', 'OXFORD') assumes the format used by
%   Oxford VGG implementations .
%
%   See also: VL_SIFT(), VL_HELP().

% Authors: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

opts.verbosity = 0 ;
opts.format = 'ubc' ;
opts = vl_argparse(opts, varargin) ;

g = fopen(file, 'r');
if g == -1
    error(['Could not open file ''', file, '''.']) ;
end
[header, count] = fscanf(g, '%d', [1 2]) ;
if count ~= 2
    error('Invalid keypoint file header.');
end
switch opts.format
  case 'ubc'
    numKeypoints  = header(1) ; 
    descrLen      = header(2) ;
    
  case 'oxford'
    numKeypoints  = header(2) ; 
    descrLen      = header(1) ;
    
  otherwise
    error('Unknown format ''%s''.', opts.format) ;
end

if(opts.verbosity > 0)
	fprintf('%d keypoints, %d descriptor length.\n', numKeypoints, descrLen) ;
end

%creates two output matrices
switch opts.format
  case 'ubc'
    P = zeros(4,numKeypoints) ;
    
  case 'oxford'
    P = zeros(5,numKeypoints) ;
end

L = zeros(descrLen, numKeypoints) ;

%parse tmp.key
for k = 1:numKeypoints

  switch opts.format
    case 'ubc'
      % Record format: i,j,s,th
      [record, count] = fscanf(g, '%f', [1 4]) ; 
      if count ~= 4
        error(...
          sprintf('Invalid keypoint file (parsing keypoint %d, frame part)',k) );
      end
      P(:,k) = record(:) ;
      
    case 'oxford'
      % Record format: x, y, a, b, c such that x' [a b ; b c] x = 1
      [record, count] = fscanf(g, '%f', [1 5]) ; 
      if count ~= 5
        error(...
          sprintf('Invalid keypoint file (parsing keypoint %d, frame part)',k) );
      end
      P(:,k) = record(:) ;
  end
  
	
	% Record format: descriptor
	[record, count] = fscanf(g, '%d', [1 descrLen]) ;
	if count ~= descrLen
		error(...
			sprintf('Invalid keypoint file (parsing keypoint %d, descriptor part)',k) );
	end
	L(:,k) = record(:) ;
	
end
fclose(g) ;

switch opts.format
  case 'ubc'
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
    
  case 'oxford'
    P(1:2,:) = P(1:2,:) + 1 ; % matlab origin
    f = P  ;
    f(3:5,:) = inv2x2(f(3:5,:)) ;
    d = uint8(L) ;
end


% --------------------------------------------------------------------
function S = inv2x2(C)
% --------------------------------------------------------------------

den = C(1,:) .* C(3,:) - C(2,:) .* C(2,:) ;
S = [C(3,:) ; -C(2,:) ; C(1,:)] ./ den([1 1 1], :) ;
