#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
const int NUM_THREADS = 8;

int *CreateMatrix(int size){
        
    int *arr = (int *)calloc(size * size, sizeof(int)); 
    
    int i, j, val = 1; 
    #pragma omp for schedule (static, size) 
    for (i = 0; i <  size; i++){ 
      for (j = 0; j < size; j++){ 
         *(arr + i*size + j) = val++;
         } 
    }
    
    return arr;
}


int PrintMatrix(int *arr, int size){
    
    int i,j;
    
    for (i = 0; i <  size; i++){
         for (j = 0; j < size; j++){
         printf("%d ", *(arr + i*size + j)); 
        }
         printf("\n");
     } 
}


void swap(int* num1, int* num2)
{
    long int temp;
    temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}


void transpose(int *arr,int size){

    int i,j;
  
    #pragma omp parallel shared(arr) private( i, j) num_threads(NUM_THREADS) 
    for (i = 0; i < size; i++) {
        #pragma omp for schedule(static,size)  nowait
            for (j = i+1; j < size; j++) {
                   swap((arr + i*size + j),(arr + j*size + i));
            }
    }
}

int main()
{
     
     int size[4] = {128, 1024, 2048 ,4096}; 
     int n;
     
     struct timeval start, end;
     gettimeofday(&start, NULL);
     
     
     for(int i=0;i < 4;i++){
        //printf("\n");
        n = *(size +i);
        int *A = CreateMatrix(n);
        //PrintMatrix(A, n);
        //printf("\n");
        transpose(A,n);
      //  PrintMatrix(A, n); 
        gettimeofday(&end, NULL);
        printf("time - %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
     }
        
     exit(0);
}
