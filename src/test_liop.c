#include <vl/liop.h>
#include <vl/generic.h>
#include <stdio.h>
#include <vl/mathop.h>
#include <vl/imopv.h>
#include <math.h>

int main(){
  vl_int i ;
  VlLiopDesc * liop ;
  vl_size size = 11*11 ;
  float mat[] = {
    6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,5,4,4,4,5,6,6,6,
    6,6,5,4,3,3,3,4,5,6,6,
    6,5,4,3,2,2,2,3,4,5,6,
    6,4,3,2,2,1,2,2,3,4,6,
    6,4,3,2,1,1,1,2,3,4,6,
    6,4,3,2,2,1,2,2,3,4,6,
    6,5,4,3,2,2,2,3,4,5,6,
    6,6,5,4,3,3,3,4,5,6,6,
    6,6,6,5,4,4,4,5,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6};
  float * patch = vl_malloc(sizeof(float)*size);

  for(i = 0; i < (signed)size; i++){
    patch[i] = mat[i];
  }

  liop = vl_liopdesc_new(4,6,2,11);

  vl_liopdesc_delete(liop) ;
  vl_free(patch) ;
}




