function y = gaussian(x)
% GAUSSIAN  Standard Gaussian density
%   Y=GAUSSIAN(X) computes the standard (zero mean, unit variance)
%   Gaussian density.  
%
%   To obtain the Gaussian density of standard deviation S do
%
%     Y = 1/S * GAUSSIAN(X/S).
%
%   See also DGAUSSIAN(), DDGAUSSIAN().

% AUTORIGHTS

y = 1/sqrt(2*pi)*exp(-0.5*x.^2) ;

