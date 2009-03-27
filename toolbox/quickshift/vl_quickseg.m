function [Iseg labels map gaps E] = vl_quickseg(I, ratio, sigma, tau)
% QUICKSEG Produce a quickshift segmentation of a grayscale or color image
%   [ISEG LABELS MAPS GAPS E] = QUICKSEG(I, RATIO, SIGMA, TAU)
% 
% Produces a Quickshift segmentation of an image. See QUICKSHIFT for more
% details.
%
% Inputs:
%   I      Input image, may be RGB or Grayscale. RGB images are first converted
%          to LAB.
%   RATIO  Tradeoff between spatial consistency and color consistency.
%          Small ratio gives more importance to the spatial component.
%          Note that distance calculations happen in unnormalized image
%          coordinates, so RATIO should be adjusted to compensate for larger
%          images.  
%   SIGMA  The standard deviation of the parzen window density estimator.
%   TAU    The maximum distance between nodes in the quickshift tree. Used to
%          cut links in the tree to form the segmentation.
%
% Outputs:
%   ISEG   A color image where each pixel is labeled by the mean color in its
%          region.
%   LABELS A labeled image where the number corresponds to the cluster identity
%   MAP    MAP as returned by QUICKSHIFT: For each pixel, the pointer to the
%          nearest pixel which increases the estimate of the density
%   GAPS   GAPS as returned by QUICKSHIFT: For each pixel, the distance to
%          the nearest pixel which increases the estimate of the density
%   E      E as returned by QUICKSHIFT: The estimate of the density 

I = im2double(I);
% Add less than one pixel noise to break ties caused by constant regions in an
% arbitrary fashon
I = I + rand(size(I))/2550;
if size(I,3) == 1
  Ix = ratio * I;
else
  Ix = ratio * vl_xyz2lab(vl_rgb2xyz(I));
  %Ix = Ix(:,:,2:3); % Throw away L
end

% Perform quickshift to obtain the segmentation tree, which is already cut by
% tau. If a pixel has no nearest neighbor which increases the density, its
% parent in the tree is itself, and gaps is inf.
[map,gaps,E] = vl_quickshift(Ix, sigma, tau) ;

% Follow the parents of the tree until we have reached the root nodes
% mapped: a labeled segmentation where the labels are the indicies of the modes
% in the original image.
% labels: mapped after having been renumbered 1:nclusters and reshaped into a
% vector
[mapped labels] = vl_flatmap(map) ;
labels = reshape(labels, size(map));

% imseg builds an average description of the region by color
Iseg = vl_imseg(I, labels);
