function [Fx,Fy] = vl_imgrad(F,varargin) ;
% VL_IMGRAD  Image gradient
%   [Fx,Fy] = VL_GRAD(F) returns the finite differencies gradient of the
%   image F. The function uses central differencies and for all but
%   the boundaries pixels, for which it uses forward/backward
%   differencies as appropriate.
%
%   VL_GRAD(F,'forward') uses only forward differencies (except on the
%   lower and right boundaries, where backward difference is used).
%   Similarly, VL_GRAD(F,'backward') uses only backward differencies.
%
%   See also: GRADIENT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
