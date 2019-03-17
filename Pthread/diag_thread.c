#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

pthread_mutex_t lock; 
int Sizes[1] = {8};//, 1024, 2048 ,4096}; 
int size_mat;
int NUM_THREADS = 10;
int next_pos = 1;
int *arr;

struct details{
	int index;
	int t_id;
};


int *CreateMatrix(){
        
    int *C = (int *)calloc(size_mat * size_mat, sizeof(int)); 
    
    int i, j, val = 1; 
    
    for (i = 0; i <  size_mat; i++){ 
      for (j = 0; j < size_mat; j++){ 
         *(C + i*size_mat + j) = val++;
         } 
    }
    
    return C;
}


void swap(int* a,int* b){
	long int tmp;
	tmp = *a;
	*a=*b;
	*b=tmp;
}

void *transpose(void *args){

      struct details* local = args; 
 
      int pos;
        
      while(pos<size_mat){
        pos = local->index;
	for (int i = pos; i < size_mat; i++) {
            for (int j = pos+1; j < size_mat; j++) {
                   swap((arr + pos*size_mat + j),(arr + j*size_mat + pos));
      
            }

		break;
    	}
    	
        // lock the variable for updating index
	pthread_mutex_lock(&lock);
	 
	        int n = next_pos++; 
	        local->index = n;
	        
	pthread_mutex_unlock(&lock); 
	
       }
}

int PrintMatrix(int *pMat){
    
    int i,j;
    
    for (i = 0; i <  size_mat; i++){
         for (j = 0; j < size_mat; j++) {
         printf("%d ", *(pMat + i*size_mat + j)); 
        }
         printf("\n");
     } 
}


int main(){
	
	for(int i=0;i < 1;i++){
	
	        size_mat = Sizes[i];

	        pthread_t threads[size_mat];
	        struct details args[NUM_THREADS];
	        struct timeval start, end;
	        
                arr = CreateMatrix(size_mat);
        
                gettimeofday(&start, NULL);


	                for(int i=0;i<NUM_THREADS;i++){
	                        args[i].index = i;
		                pthread_create(&threads[i],NULL, transpose,(void*) &args); // create 8 threads 

	                }
                
                gettimeofday(&end, NULL);
                printf("time - %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	        PrintMatrix(arr);
	}
	exit(0);

}
