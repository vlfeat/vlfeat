% MSER  Maximally Stable Extremal Regions
%   R=MSER(I,DELTA) computes the Maximally Stable Extremal Regions
%   (MSER) [1] of image I with stability threshold DELTA. I is any
%   array of class UINT8, while DELTA is a scalar of the same class.
%   R is an index set (of class UINT32) which enumerates the
%   representative pixels of the detected regions.
%
%   A region R can be recovered from a representative pixel X as the
%   connected component of the level set {Y:I(Y) <= I(X)} which
%   contains X.
%
%   [R,ELL]=MSER(...) also returns ellipses fitted to the regions
%   R. Each column of the matrix ELL describes an ellipse; ELL(1:D,i)
%   is the ellipse center and ELL(D:end,i) the independent elments of
%   the co-variance of the ellipse and D is the image domain
%   dimension. The coordinates are in the same order of the indexes of
%   I as an array; so the first coordinate is the row index, which in
%   MATLAB convention is usally corresponds to the Y image
%   coorinate. If the other convetion is needed, the elements of ELL
%   must be swapped according.
%
%   REFERENCES
%   [1] J. Matas, O. Chum, M. Urban, and T. Pajdla, "Robust wide
%       baseline stereo from maximally stable extremal regions," in
%       Proc. BMVC, 2002.
