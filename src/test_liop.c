#include <vl/liop.h>
#include <vl/generic.h>
#include <stdio.h>
#include <vl/mathop.h>
#include <vl/imopv.h>
#include <math.h>

void printFloatArray(float * array, vl_size length);
void printFloatArray(float * array, vl_size length);
void printVlIntArray(vl_int *array, vl_size length);
void printFloatMatrix(float * array, vl_size length);

// tests
void testCircle();
void testSorting();
void testPermutation();

int main(){
    //testSorting();
    //testCircle();
    //testPermutation();

    vl_int i;
    VlLiopDesc * liop ;
    vl_int * table ;
    float * desc ;
    vl_size size = 11*11;
    float mat[] = {6,6,6,6,6,6,6,6,6,6,6,
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

    printf("cos(pi/2) %e\n",cosf(VL_PI/2));
    printf("sin(0) %e\n",sinf(0));


    for(i = 0; i < (signed)size; i++){
        patch[i] = mat[i];
    }
    //float * patch2 = vl_malloc(sizeof(float)*size);//
    //vl_imsmooth_f(patch2,size,patch,size,size,size,0.11,0.11);//

    liop = vl_liopdesc_new(4,6,2,5,11);
    //printFloatMatrix(patch2,size);//

    table = compute_circle_look_up_table(liop->patchRadius,liop->patchRadius*2 + 1);
    printVlIntArray(table,liop->patchRadius*2 + 1);
    printf("%d\n",liop->innerPatchIndexesSize);

    printf("first element: %f\n",patch[liop->innerPatchIndexes[0]]);

    //test new circle
    for(i = 0; i < liop->innerPatchIndexesSize; i++){
        patch[liop->innerPatchIndexes[i]] = 0;
        printf("%d\n",liop->innerPatchIndexes[i]);
    }
    printf("first element: %f\n",patch[liop->innerPatchIndexes[0]]);
    printFloatMatrix(patch,size);

    desc = vl_malloc(sizeof(vl_uint)*liop->liopArraySize);
    printf("allocated\n");
    compute_liop_descriptor(liop, patch, desc);
    printf("computed\n");

    for(i = 0; i < liop->numberOfBins; i++){
        VlLiopBin * bin = liop->liopBins[i];
        printf("Bin %d\n", i);
        printFloatArray(bin->binLiop, bin->binLiopSize);
    }

    // BIN DEVISION TEST
    /*
    for(i = 0; i < liop->B; i++){
        printf("bin %d\n", i);
        VlLiopBin * bin = liop->liopBins[i];
        printVlIntArray(bin->binIndexes,bin->binSize);
    }
    */

    {
      float x,y;
      vl_uindex ind ;
      float inter  ;

      // COORDINATE + POSITION TEST
      get_coordinates(liop->patchSideLength, 15 , &x, &y);
      printf("x: %f\n",x);
      printf("y: %f\n",y);

      ind =  get_vector_position(liop->patchSideLength, x, y);
      printf("pos %d\n", ind);

      // INTERPOLATION TEST
      inter = interpolate(patch, 11, -1.75, -4.75);
      printf("interpolation: %f\n",inter);
      printf("xM %f\n", (float)vl_ceil_f(x));
      printf("yM %f\n", (float)vl_ceil_f(y));
      //printVlIntArray(liop->circleLookUpTable, liop->patchSideLength);

      //printFloatMatrix(patch,size);
    }
}

//----------------------FUNCTION TESTS------------------------------------


/*
 *permutation test
 */
void testPermutation(){
    vl_size size = 4;
    vl_uint P[] = {3,2,0,1};
    vl_uint * permutation = vl_malloc(sizeof(vl_uint)*size);
    vl_uindex i; 
    vl_int index ;
    for(i = 0; i < size; i++){
        permutation[i] = P[i];
        //printf("%d\n",permutation[i]);
    }
    index = get_permutation_index(permutation,size);
    printf("%d\n",index);
}

/*
 *circle table test
 */
/*
void testCircle(){
    // create circle look up table
    vl_int radius = 20;
    vl_int sideLength = 41;
    vl_int * table = compute_circle_look_up_table(radius,sideLength);
    printVlIntArray(table,sideLength);

    // create patch
    float * patch;
    vl_size patchSize = sideLength*sideLength;
    patch = vl_malloc(sizeof(float)*patchSize);
    vl_index i;
    for(i = 0; i < (signed)patchSize; ++i){
        patch[i] = 1;
    }

    // make a circle form patch
    reshape_patch_to_circle(patch, table, sideLength);
    printFloatMatrix(patch,patchSize);
}
*/
/*
 *sorting test
 */
void testSorting(){
    vl_size N = 9;
    float array_h[] = {8,3,1,2,6,4,7,-1,3};
    //float array_h[] = {5,3,5,3};
    float * array = vl_malloc(sizeof(float)*N);
    vl_uindex * indexes = vl_malloc(sizeof(vl_uindex)*N);
    vl_uindex i,j;
    printf("unsorted\n");
    for(i = 0; i < N;i++){
        array[i] = array_h[i];
        indexes[i] = i;
        printf("%f\n",array[i]);
    }
    stable_indexes_qsort(array,indexes,0,N-1);
    printf("sorted\n");
    for(i = 0; i < N; i++){
        printf("%f\n", array[indexes[i]]);
    }
    printf("indexes\n");
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            if(indexes[j] == i){
                printf("%d\n",j);
            }
        }
    }

}

//-------------------END FUNCTION TESTS------------------------------------

//-----------------------PRINTING----------------------------------------

void printFloatMatrix(float * array, vl_size length){
    vl_index i;
    float count = 0;
    float a = sqrtf((float)length);
    for(i = 0; i < (signed)length; i++){
        printf("%f ", array[i]);
        if(count < a - 1){
           count++;
        }else{
           printf("\n");
           count = 0;
        }
    }
}

void printFloatArray(float * array, vl_size length){
        vl_index i;
        for(i = 0; i < (signed)length; i++){
            printf("%f\n", array[i]);
        }
}

void printVlIntArray(vl_int * array, vl_size length){
    vl_index i;

    for(i = 0; i < (signed)length; i++){
       printf("%f\n", array[i]);
    }
}

//---------------------------END PRINTING------------------------------------




