#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

const int block_len=2;  // length of inner blocks
const int size_mat = 4096; // length of main matrix {128,1024,2048}
long int _MAT[size_mat][size_mat];
const int NUM_THREADS=8;
int next_pos=NUM_THREADS;

void CreateMatrix(){
    long int val=1;

      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                _MAT[i][j]=val;
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

void transpose( long int (&arr)[block_len][block_len]){        // normal transpose algorithm
    for (int i=0;i<block_len; i++) {
            for (int j=i+1;j<block_len; j++) {
                   swap(arr[i][j],arr[j][i]);
            }
    }
}

void transpose_blocks(){        // transposing inside of all blocks in matrix (part 1)
    int thread_counter =0;
  #pragma omp parallel shared (thread_counter) num_threads(NUM_THREADS)
    { 
    long local=thread_counter;
    thread_counter++;
    int pos;
    int posY=0;
    int tmp = size_mat/block_len;
    int x;
    long int block[block_len][block_len];
   
    while (1){
        pos = (block_len*local)%size_mat;
        x = local/tmp;
        posY = x*block_len;
        if (posY>=size_mat){ break;}
        getSubMatrix(posY,pos,block);
        transpose(block);
        setSubMatrix(posY,pos,block);

        #pragma omp critical
        {
            local= next_pos;
            next_pos++; }
	  }
    }
    

}

void shuffle(){        // transposing actual blocks (part 2)

    #pragma omp parallel num_threads(NUM_THREADS)
    {
     long int block[block_len][block_len];
     long int block2[block_len][block_len];
      for (int i=0;i<size_mat;i+=block_len){
         #pragma omp for schedule(static,size_mat) nowait
            for (int j=i;j<size_mat;j+=block_len){
                getSubMatrix(i,j,block);
                getSubMatrix(j,i,block2);
                setSubMatrix(i,j,block2);
                setSubMatrix(j,i,block);
            }         
      }
    }
}

int main(){
   
    struct timeval start, end;

    CreateMatrix();
    gettimeofday(&start, NULL);
  //  PrintMatrix();
    transpose_blocks();
    shuffle();
    gettimeofday(&end, NULL);
    cout << "time - "<< ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))<<endl;
  //  PrintMatrix();
    
    return 0;
}
