function [Iedge taus map gaps] = vl_quickvis(I, ratio, sigma, tau_max, maxcuts)
% QUICKVIS Create an edge image from a Quickshift segmentation.
%   IEDGE = QUICKVIS(I, RATIO, SIGMA, TAU_MAX, MAXCUTS) creates an edge
%   stability image from a Quickshift segmentation. RATIO controls the tradeoff
%   between color consistency and spatial consistency (See QUICKSEG) and SIGMA
%   controls the bandwidth of the density estimator (See QUICKSEG,
%   QUICKSHIFT). TAU_MAX is the maximum distance between neighbors which
%   increase the density. 
%
%   QUICKVIS takes at most MAXCUTS thresholds less than TAU_MAX, forming at most
%   MAXCUTS segmentations. The edges between regions in each of these
%   segmentations are labeled in IEDGE, where the label corresponds to the
%   largest TAU which preserves the edge. 
%
%   [IEDGE,TAUS] = QUICKVIS(I, RATIO, SIGMA, TAU_MAX, MAXCUTS) also returns the
%   TAU thresholds that were chosen.
%   
%   IEDGE = QUICKVIS(I, RATIO, SIGMA, TAUS) will use the TAUS specified
%
%   [IEDGE,TAUS,MAP,GAPS] = QUICKVIS(I, RATIO, SIGMA, TAU_MAX, MAXCUTS) also
%   returns the MAP and GAPS from QUICKSHIFT.
%
% See Also: QUICKSHIFT, QUICKSEG

if nargin == 4
  taus = tau_max;
  tau_max = max(taus);
  [Iseg labels map gaps E] = vl_quickseg(I, ratio, sigma, tau_max);
else
  [Iseg labels map gaps E] = vl_quickseg(I, ratio, sigma, tau_max);
  taus = unique(floor(gaps(:)));
  taus = taus(2:end-1); % remove the inf thresh and the lowest level thresh
  if length(taus) > maxcuts
    ind = round(linspace(1,length(taus), maxcuts));
    taus = taus(ind);
  end
end

[Iedge taus] = mapvis(map, gaps, taus);

function [Iedge taus] = mapvis(map, gaps, tau_max, maxcuts)
% MAPVIS Create an edge image from a Quickshift segmentation.
%   IEDGE = MAPVIS(MAP, GAPS, TAU_MAX, MAXCUTS) creates an edge
%   stability image from a Quickshift segmentation. TAU_MAX is the maximum
%   distance between neighbors which increase the density. 
%
%   MAPVIS takes at most MAXCUTS thresholds less than TAU_MAX, forming at most
%   MAXCUTS segmentations. The edges between regions in each of these
%   segmentations are labeled in IEDGE, where the label corresponds to the
%   largest TAU which preserves the edge. 
%
%   [IEDGE,TAUS] = MAPVIS(MAP, GAPS, TAU_MAX, MAXCUTS) also returns the
%   TAU thresholds that were chosen.
%   
%   IEDGE = MAPVIS(MAP, GAPS, TAUS) will use the TAUS specified
%
% See Also: QUICKVIS, QUICKSHIFT, QUICKSEG

if nargin == 3
  taus = tau_max;
  tau_max = max(taus);
else
  taus = unique(floor(gaps(:)));
  taus = taus(2:end-1); % remove the inf thresh and the lowest level thresh
  % throw away min region size instead of tau_max?
  ind = find(taus < tau_max);
  taus = taus(ind);
  if length(taus) > maxcuts
    ind = round(linspace(1,length(taus), maxcuts));
    taus = taus(ind);
  end
end


Iedge = zeros(size(map));

for i = 1:length(taus)
  s = find(gaps >= taus(i));
  maptau = map;  
  maptau(s) = s;
  [mapped labels] = flatmap(maptau);
  fprintf('%d/%d %d regions\n', i, length(taus), length(unique(mapped)))

  borders = getborders(mapped);
  
  Iedge(borders) = taus(i);
  %Iedge(borders) = Iedge(borders) + 1;
  %Iedge(borders) = i;
end

%%%%%%%%% GETBORDERS
function borders = getborders(map)

dx = conv2(map, [-1 1], 'same');
dy = conv2(map, [-1 1]', 'same');
borders = find(dx ~= 0 | dy ~= 0);
