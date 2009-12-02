% VL_DEMO_MSER_CMD Demo: MSER: test command line utility

pfx = fullfile(vl_root,'results') ;
if ~ exist(pfx, 'dir')
  mkdir(pfx) ;
end

p_img = fullfile(vl_root,'data','spots.jpg') ;
p_pgm = fullfile(pfx,'spots.pgm') ;
p_sed = fullfile(pfx,'spots.mser') ;
p_frm = fullfile(pfx,'spots.frame') ;
p_tmp = fullfile(pfx,'spots.tmp') ;

I = imread(p_img) ;
I = uint8(rgb2gray(I)) ;
imwrite(I,p_pgm,'pgm') ;

% --------------------------------------------------------------------
%                                                             Test cmd
% --------------------------------------------------------------------

[err,msg] = vl_demo_cmd('mser',[p_pgm ...
                    ' --seeds=ascii://' p_sed ...
                    ' --frames=ascii://' p_frm ...
                    ' --max-variation=0.2 ' ...
                    ' --min-diversity=0.7 ' ...
                    ' --delta=10 ' ...
                    ' --verbose '] ) ;

r_ = load(p_sed,'-ASCII')';
% note: bright-on-dark region seeds are negative
r_ = r_ + sign(r_); % add one for matlab indexing
f_ = load(p_frm,'-ASCII')' ; f_(1:2,:) = f_(1:2,:) + 1 ;

[r,f] = vl_mser(I','MinDiversity',0.7,'MaxVariation',0.2,'Delta',10) ;

if(any(r ~= r_) | any(any(abs(f - f_)>1e-5)))
  warning('demo_mser_cmd: failed consistency test of commmand line and MATLAB drivers.') ;
end
