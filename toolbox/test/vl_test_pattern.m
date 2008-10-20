function I = vl_test_pattern(n)
% VL_TEST_PATTERN  Generate test pattern
%   I=VL_TEST_PATTERN(N) returns the N-th test pattern.

ur    = linspace(-1,1,128) ;
vr    = linspace(-1,1,128) ;
[u,v] = meshgrid(ur,vr);

switch n
  case 1
    %I     = u.^2 + v.^2 > (1/4).^2 ;
    I     = abs(u) + abs(v) > (1/4) ;
    I     = 255 * I ;
    I(1:64,:) = 0 ;
  
  case 2
    I = zeros(100,100) ;
    I(20:100-20+1,20:100-20+1) = 128 ;
    I(30:100-30+1,30:100-30+1) = 200 ;
    I(50,50)                   = 255 ;
    I(50,55)                   = 250 ;
    I(50,45)                   = 245 ;
    I = 255 - I ;
        
  case 3 
    I = 255 * vl_imsmooth(checkerboard(10,10),1) ;
    
  case 4
    I = 255 * rand(32,32) ;
    
  case 101
    I = 255 * vl_imreadbw(fullfile(vlfeat_root,'data','a.jpg')) ;
    
  case 102
	 I = 255 * vl_imreadbw(fullfile(vlfeat_root,'data','box.pgm')) ;   
		
 case 'cone'
	I = sqrt(u.^2+v.^2) ;
	
end
