#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
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


void transpose(int *arr,int size_mat){

  int i,j,pos=0;
    
    while(pos<size_mat){
       #pragma omp parallel shared(pos) private( i, j) num_threads(NUM_THREADS)  
       
	 for (int i = pos; i < size_mat; i++) {
	   #pragma omp for schedule(dynamic,size_mat)  nowait
              for (int j = pos+1; j < size_mat; j++) {
                    swap((arr + pos*size_mat + j),(arr + j*size_mat + pos));
              
                }

		        break;
         }
                #pragma omp critical
                {
    	        pos++;
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
       //PrintMatrix(A, n); 
        gettimeofday(&end, NULL);
        printf("time - %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
    }
        
     exit(0);
}
