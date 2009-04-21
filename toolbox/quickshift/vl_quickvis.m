function [Iedge dists map gaps] = vl_quickvis(I, ratio, kernelsize, maxdist, maxcuts)
% VL_QUICKVIS Create an edge image from a Quickshift segmentation.
%   IEDGE = VL_QUICKVIS(I, RATIO, KERNELSIZE, MAXDIST, MAXCUTS) creates an edge
%   stability image from a Quickshift segmentation. RATIO controls the tradeoff
%   between color consistency and spatial consistency (See VL_QUICKSEG) and
%   KERNELSIZE controls the bandwidth of the density estimator (See VL_QUICKSEG,
%   VL_QUICKSHIFT). MAXDIST is the maximum distance between neighbors which
%   increase the density. 
%
%   VL_QUICKVIS takes at most MAXCUTS thresholds less than MAXDIST, forming at
%   most MAXCUTS segmentations. The edges between regions in each of these
%   segmentations are labeled in IEDGE, where the label corresponds to the
%   largest DIST which preserves the edge. 
%
%   [IEDGE,DISTS] = VL_QUICKVIS(I, RATIO, KERNELSIZE, MAXDIST, MAXCUTS) also
%   returns the DIST thresholds that were chosen.
%   
%   IEDGE = VL_QUICKVIS(I, RATIO, KERNELSIZE, DISTS) will use the DISTS
%   specified
%
%   [IEDGE,DISTS,MAP,GAPS] = VL_QUICKVIS(I, RATIO, KERNELSIZE, MAXDIST, MAXCUTS)
%   also returns the MAP and GAPS from VL_QUICKSHIFT.
%
% See Also: VL_QUICKSHIFT, VL_QUICKSEG

if nargin == 4
  dists = maxdist;
  maxdist = max(dists);
  [Iseg labels map gaps E] = vl_quickseg(I, ratio, kernelsize, maxdist);
else
  [Iseg labels map gaps E] = vl_quickseg(I, ratio, kernelsize, maxdist);
  dists = unique(floor(gaps(:)));
  dists = dists(2:end-1); % remove the inf thresh and the lowest level thresh
  if length(dists) > maxcuts
    ind = round(linspace(1,length(dists), maxcuts));
    dists = dists(ind);
  end
end

[Iedge dists] = mapvis(map, gaps, dists);

function [Iedge dists] = mapvis(map, gaps, maxdist, maxcuts)
% MAPVIS Create an edge image from a Quickshift segmentation.
%   IEDGE = MAPVIS(MAP, GAPS, MAXDIST, MAXCUTS) creates an edge
%   stability image from a Quickshift segmentation. MAXDIST is the maximum
%   distance between neighbors which increase the density. 
%
%   MAPVIS takes at most MAXCUTS thresholds less than MAXDIST, forming at most
%   MAXCUTS segmentations. The edges between regions in each of these
%   segmentations are labeled in IEDGE, where the label corresponds to the
%   largest DIST which preserves the edge. 
%
%   [IEDGE,DISTS] = MAPVIS(MAP, GAPS, MAXDIST, MAXCUTS) also returns the DIST
%   thresholds that were chosen.
%   
%   IEDGE = MAPVIS(MAP, GAPS, DISTS) will use the DISTS specified
%
% See Also: VL_QUICKVIS, VL_QUICKSHIFT, VL_QUICKSEG

if nargin == 3
  dists = maxdist;
  maxdist = max(dists);
else
  dists = unique(floor(gaps(:)));
  dists = dists(2:end-1); % remove the inf thresh and the lowest level thresh
  % throw away min region size instead of maxdist?
  ind = find(dists < maxdist);
  dists = dists(ind);
  if length(dists) > maxcuts
    ind = round(linspace(1,length(dists), maxcuts));
    dists = dists(ind);
  end
end


Iedge = zeros(size(map));

for i = 1:length(dists)
  s = find(gaps >= dists(i));
  mapdist = map;  
  mapdist(s) = s;
  [mapped labels] = vl_flatmap(mapdist);
  fprintf('%d/%d %d regions\n', i, length(dists), length(unique(mapped)))

  borders = getborders(mapped);
  
  Iedge(borders) = dists(i);
  %Iedge(borders) = Iedge(borders) + 1;
  %Iedge(borders) = i;
end

%%%%%%%%% GETBORDERS
function borders = getborders(map)

dx = conv2(map, [-1 1], 'same');
dy = conv2(map, [-1 1]', 'same');
borders = find(dx ~= 0 | dy ~= 0);
