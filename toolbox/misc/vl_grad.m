function [Fx,Fy] = vl_grad(F,varargin) ;
% VL_GRAD  Gradient
%   [Fx,Fy] = VL_GRAD(F) returns the finite differences gradient of the
%   function F. The function uses central differences and for all
%   but the boundaries pixels, for which it uses forward/backward
%   differences as appropriate.
%
%   VL_GRAD(F,'forward') uses only forward differences (except on the
%   lower and right boundaries, where backward difference is used).
%   Similarly, VL_GRAD(F,'backward') uses only backward differences.
%  
%   See also:: GRADIENT(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

type = 'central' ; % forward, backward

for k=1:2:length(varargin)
  switch varargin{k}
    case 'Type'
      type = varargin{k+1} ;
    otherwise
      error(['Parameter ''', varargin{k}, ''' unknown']) ;      
  end
end


[M,N]=size( F ) ;

switch type
  case 'central'
    Fx = [ F(:,2)-F(:,1) , ...
           0.5*(F(:,3:end)-F(:,1:end-2)) , F(:,end)-F(:,end-1) ] ;
    Fy = [ F(2,:)-F(1,:) ; ...
           0.5*(F(3:end,:)-F(1:end-2,:)) ; F(end,:)-F(end-1,:) ] ;
    
  case 'forward'
    Fx = [ F(:,2:end)-F(:,1:end-1) , F(:,end)-F(:,end-1) ] ;
    Fy = [ F(2:end,:)-F(1:end-1,:) ; F(end,:)-F(end-1,:) ] ;
    
  case 'backward'
    Fx = [ F(:,2)-F(:,1) , F(:,2:end)-F(:,1:end-1)] ;
    Fy = [ F(2,:)-F(1,:) ; F(2:end,:)-F(1:end-1,:)] ;
    
  otherwise
    error(['Difference type ''', type, ''' unknown']) ;      
end

