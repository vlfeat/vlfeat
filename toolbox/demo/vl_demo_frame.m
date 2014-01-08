% VL_DEMO_FRAME  Demonstrates vl_plotframe and vl_frame2oell

%% A standard frame is a unit circle centered at the origin with a
% radius pointing in the positive Y direction. Since images use a
% left-handed coordinate system (X axis pointing right, Y axis
% pointing downwards), the radius will point downwards.

A = eye(2) ;
T = [0;0] ;

figure(1) ; clf ;
vl_plotframe([T ; A(:)], 'b', 'linewidth', 5) ; hold on ;

% The same frame can be specified as 4 numbers: translation, scale
% (unit), and rotation (zero).
vl_plotframe([T ; 1 ; 0]) ;

axis equal ; set(gca,'ydir','reverse') ; grid on ;
vl_figaspect(1) ;
vl_demo_print(1, 'frame_standard') ;

%% A positive rotation is appears clockwise in the standard image
% reference frame.

angle = pi/4 ;
R = [cos(angle), -sin(angle) ; sin(angle) cos(angle)] ;
A = R ;

figure(2) ; clf ;
vl_plotframe([T ; A(:)], 'b', 'linewidth', 5) ; hold on ;

% The same frame can be specified as 4 numbers: translation, scale and
% rotation.
vl_plotframe([T ; 1 ; angle]) ;

axis equal ; set(gca,'ydir','reverse') ; grid on ;
vl_figaspect(1) ;
vl_demo_print(2, 'frame_rotated') ;

%% A more complex example, involving a generic affine
% frame. The frame is the result of applying a given affine
% transformation to the standard frame.

angle = pi/4 ;
R = [cos(angle), -sin(angle) ; sin(angle) cos(angle)] ;
S = [1 0 ; 0 .5] ;
A = R*S*R ;

figure(3) ; clf ;
vl_plotframe([T ; A(:)], 'b', 'linewidth', 3) ; hold on ;

axis equal ; set(gca,'ydir','reverse') ; grid on ;
vl_figaspect(1) ;
vl_demo_print(3, 'frame_affine') ;

%% An oriented ellipse can be easily transformed in an unoriented
% one by removing the orientation. The opposite transformation is
% possible as well, provided that a conventional orientation is
% assigned to it.

clear frames1 frames2 ;
randn('state',0) ;
rand('state',0) ;
for i = 1 : 5
  % sample a random ellipse
  angle1 = rand * 2*pi;
  angle2 = rand * 2*pi;
  R1 = [cos(angle1), -sin(angle1) ; sin(angle1) cos(angle1)] ;
  R2 = [cos(angle2), -sin(angle2) ; sin(angle2) cos(angle2)] ;
  A = 0.3*R1*diag(rand(2,1)+0.5)*R2 ;
  frames1(:,i) = [i ; 0 ; A(:)] ;

  % get the unoriented variant of the same ellipse
  S = A*A' ;
  frames2(:,i) = [i ; 0 ; S(1,1) ; S(1,2) ; S(2,2)] ;
end

% Recover oriented ellipses from the unoriented ones; note that
% vl_frame2oell assigns all frames an upright orientation.

frames3 = vl_frame2oell(frames2) ;

frames1(2,:) = 0 ;
frames2(2,:) = 1 ;
frames3(2,:) = 2 ;

figure(4) ; clf ;
vl_plotframe(frames1,'r') ;
vl_plotframe(frames2,'g') ;
vl_plotframe(frames3,'b') ;

axis equal tight ; set(gca,'ydir','reverse') ; grid on ;
vl_figaspect(5/3) ;
vl_demo_print(4, 'frame_frame2oell') ;
