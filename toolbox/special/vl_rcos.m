function y=vl_rcos(r,x)
% VL_RCOS  vl_rcos function
%   Y=VL_RCOS(R,X) computes the vl_rcos function with roll-off R.
%
%   See also:: VL_HELP().

x = abs(x) ;
if(r > 0)
	y = (x < (1-r) ) + ...
		0.5 * (1 + cos( ((1-r) - x)/r*pi )) .* ...
		(x <= 1) .* (x >= 1-r) ;
else
	y = (abs(x) <= 1) ;
end
