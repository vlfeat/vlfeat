function J=vl_imwhiten(I,alpha,cutoff)
% VL_IMWHITEN  Whiten image
%   J = VL_IMWHITEN(I,ALPHA) approximatively whitens the power spectrum
%   of the natural image I. The algorithm assumes that the modulus of
%   the spectrum decays as 1/f^ALPHA (f is the frequency).
%
%   VL_IMWHITEN(I) uses ALPHA=1 (a typical value for natural images).
%
%   VL_IMWHITEN(I,ALPHA,CUTOFF) also applies a low-pass filter with
%   cutoff frequency equal to CUTOFF x FN, where FN is the Nyquist
%   frequency (half of the sampling frequency).
%
%   See also:: VL_HELP()

if ~exist('alpha'),  alpha = 1 ; end
if ~exist('cutoff'), cutoff = [] ; end

[M,N]=size(I) ;

% Frequency domain
fn = 0.5 ; % Nyquist freq (=1/2T, T=1)
fx_range=linspace(-fn, fn, N) ;
fy_range=linspace(-fn, fn, M) ;
[fx fy]=meshgrid(fx_range, fy_range) ;

% Whitening filter
rho=sqrt(fx.*fx+fy.*fy);
filt=rho.^alpha ;

% Low-pass filter
if ~isempty(cutoff)
  fcut = cutoff * fn ;
  filt = filt .* exp(-(rho/fcut).^4);
  %filt = filt .* exp( - 0.5 * (rho / fcut) .^ 2);
end

% Apply filter
J = real(ifft2(fft2(I).*fftshift(filt))) ;
