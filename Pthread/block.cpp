#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

const int block_len=2;    // length of inner blocks
const int size_mat =4096;  // length of main matrix {128,1024,2048}
long int _MAT[size_mat][size_mat];
const int NUM_THREADS=8;
int next_pos=NUM_THREADS;
pthread_mutex_t lock;

void CreateMatrix(){
    long int val=1;

      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
               _MAT [i][j] = val;
                val++;
         }      
       }
}

void PrintMatrix(){
      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                cout << _MAT[i][j]<<"  ";
             }cout <<endl;
          }
}

void getSubMatrix(int A,int B, long int (&arr)[block_len][block_len]){   // getting 2x2 blocks based on co-ordinates A and B
   for (int i=0;i<block_len;i++){
      for (int j=0;j<block_len;j++){
            arr[i][j] = _MAT[A+i][B+j];
      }
   }
   
}

void setSubMatrix(int A,int B, const long int (&block)[block_len][block_len]){   // replacing 2x2 blocks based on co-ordinates A and B
   for (int i=0;i<block_len;i++){
      for (int j=0;j<block_len;j++){
             _MAT[A+i][B+j]= block[i][j];
      }
   }


}
void swap(long int& a,long int& b){
    long int tmp;
    tmp=a;
    a=b;
    b=tmp;
}

void transpose(long int (&arr)[block_len][block_len]){        // normal transpose algorithm
    for (int i=0;i<block_len; i++) {
            for (int j=i+1;j<block_len; j++) {
                   swap(arr[i][j],arr[j][i]);
            }
    }
}

void* transpose_blocks(void* ind_){        // transposing inside of all blocks in matrix (part 1)
    
    long local;
    local = (long) ind_;

    int pos;
    int posY=0;
    int tmp = size_mat/block_len;
    int x;
    long int block[block_len] [block_len];

    while (1){
        pos = (block_len*local)%size_mat;
        x = local/tmp;
        posY = x*block_len;
        
        if (posY>=size_mat){ break;}

        getSubMatrix(posY,pos,block);
        transpose(block);
        setSubMatrix(posY,pos,block);

        pthread_mutex_lock(&lock);
            local= next_pos;
            next_pos++; 
       	pthread_mutex_unlock(&lock);
    
    }

}

void* shuffle(){             // transposing actual blocks (part 2)
    
      long int block[block_len][block_len];
      long int block2[block_len][block_len];
      for (int i=0;i<size_mat;i+=block_len){
        for (int j=i;j<size_mat;j+=block_len){
            getSubMatrix(i,j,block);
            getSubMatrix(j,i,block2);
            setSubMatrix(i,j,block2);
            setSubMatrix(j,i,block);
        }
      }
}

int main(){
    
    CreateMatrix();
    //PrintMatrix();
    pthread_t threads[NUM_THREADS];
    int index[NUM_THREADS];
    struct timeval start, end;

    gettimeofday(&start, NULL);
    for (int i=0; i<NUM_THREADS;i++){
        index[i]=i;
        pthread_create(&threads[i],NULL,transpose_blocks,(void*)index[i]);
    }
    
    for (int i=0; i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);      // waiting for all blocks to be transposed
    }

    shuffle();
    gettimeofday(&end, NULL);
    cout << "time - "<< ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))<<endl;
   // PrintMatrix();
    
    return 0;
}
