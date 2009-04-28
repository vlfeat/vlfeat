function Q = vl_imseg(I,labels)
% IMSEG Color an image based on the segmentation
%   ISEG = IMSEG(I,LABELS) Labels ISEG with the average color from I of 
%   each cluster indicated by LABELS

[M,N,K] = size(I) ;
Q = 0*I ;
for k=1:size(I,3)
  acc = zeros(M,N) ;
  nrm = zeros(M,N) ;
  acc = vl_binsum(acc,  I(:,:,k), labels) ;
  nrm = vl_binsum(nrm, ones(M,N), labels) ;
  acc = acc ./ (nrm+eps) ;
  Q(:,:,k) = acc(labels) ;
end

Q = min(1,Q);
