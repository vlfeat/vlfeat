K = 2 ;
N = 5 ; 


next = zeros(1,K,'uint32') ;
h    = zeros(1,K,'uint32') ;

id   = zeros(2,K,'uint8') ;
x    = zeros(2,N,'uint8') ;

[h,id,next] = vl_ihashsum(h,id,next,K, uint8([0;1])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([0;2])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([0;3])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([1;3])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([0;2])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([1;3])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([7;1])) ;
disp(h)
[h,id,next] = vl_ihashsum(h,id,next,K, uint8([1;3])) ;
disp(h)

for i=1:size(id,2)
  disp(vl_ihashfind(id,next,K, id(:,i)));
end


