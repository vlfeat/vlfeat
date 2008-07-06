% DEMO_MSER_CMD Demo: MSER: test command line utility

p_img = fullfile(vlfeat_root,'data','spots.jpg') ;
p_pgm = fullfile(vlfeat_root,'results','spots.pgm') ;
p_sed = fullfile(vlfeat_root,'results','spots.mser') ;
p_frm = fullfile(vlfeat_root,'results','spots.frame') ;
p_tmp = fullfile(vlfeat_root,'results','spots.tmp') ;

I = imread(p_img) ;
I = uint8(rgb2gray(I)) ;
imwrite(I,p_pgm,'pgm') ;

% --------------------------------------------------------------------
%                                                             Test cmd
% --------------------------------------------------------------------

[err,msg] = demo_cmd('mser',[p_pgm ...
                    ' --seeds=ascii://' p_sed ...
                    ' --frames=ascii://' p_frm ...
                    ' --max-variation=0.2 ' ...
                    ' --min-diversity=0.7 ' ...
                    ' --delta=10 ' ...
                    ' --verbose '] ) ;

r_ = load(p_sed,'-ASCII')' + 1 ;
f_ = load(p_frm,'-ASCII')' ; f_(1:2,:) = f_(1:2,:) + 1 ;

[r,f] = mser(I','MinDiversity',0.7,'MaxVariation',0.2,'Delta',10) ;

if(any(r ~= r_) | any(any(abs(f - f_)>1e-5)))
  warning('demo_mser_cmd: failed consistency test of commmand line and MATLAB drivers.') ;
end
