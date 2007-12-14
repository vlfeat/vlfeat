function y=rcos(r,x)
% RCOS  rcos function
%   Y=RCOS(R,X) computes the rcos function with roll-off R.

x = abs(x) ;
if(r > 0)
	y = (x < (1-r) ) + ...
		0.5 * (1 + cos( ((1-r) - x)/r*pi )) .* ...
		(x <= 1) .* (x >= 1-r) ;
else
	y = (abs(x) <= 1) ;
end
