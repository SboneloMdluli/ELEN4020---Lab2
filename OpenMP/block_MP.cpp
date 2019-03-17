#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <vector>
#include <time.h>
#include <sys/time.h>

using namespace std;

using tensor = vector <vector<long int>>;

const int block_len=2;  // length of inner blocks
tensor _MAT;
const int NUM_THREADS=8;
int next_pos=NUM_THREADS;

void CreateMatrix(long int size_mat){
    long int val=1;
    vector<long int> num_;

      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                num_.push_back(val);
                val++;
         }_MAT.push_back(num_);
           num_.clear();
                
       }
}

void PrintMatrix(long int size_mat){
      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                cout << _MAT[i][j]<<"  ";
             }cout <<endl;
          }
}

tensor getSubMatrix(int A,int B){   // getting 2x2 blocks based on co-ordinates A and B
    tensor sub(block_len,vector<long int>(block_len));
   for (int i=0;i<block_len;i++){
      for (int j=0;j<block_len;j++){
            sub[i][j] = _MAT[A+i][B+j];
      }
   }
   
    return sub;
}

void setSubMatrix(int A,int B, tensor block){   // replacing 2x2 blocks based on co-ordinates A and B
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

void transpose(tensor& arr){        // normal transpose algorithm
    int size;    
    size = arr.size();
    for (int i=0;i<size; i++) {
            for (int j=i+1;j<size; j++) {
                   swap(arr[i][j],arr[j][i]);
            }
    }
}

void transpose_blocks(long int size_mat){        // transposing inside of all blocks in matrix (part 1)
    int thread_counter =0;
  #pragma omp parallel shared (thread_counter) num_threads(NUM_THREADS)
    { 
    long local=thread_counter;
    thread_counter++;
    int pos;
    int posY=0;
    int tmp = size_mat/block_len;
    int x;
    tensor block;
   
    while (1){
        pos = (block_len*local)%size_mat;
        x = local/tmp;
        posY = x*block_len;
        if (posY>=size_mat){ break;}
        block = getSubMatrix(posY,pos);
        transpose(block);
        setSubMatrix(posY,pos,block);

        #pragma omp critical
        {
            local= next_pos;
            next_pos++; }
	  }
    }
    

}

void shuffle(long int size_mat){        // transposing actual blocks (part 2)

    #pragma omp parallel num_threads(NUM_THREADS)
    {
      for (int i=0;i<size_mat;i+=block_len){
         #pragma omp for schedule(static,size_mat) nowait
            for (int j=i;j<size_mat;j+=block_len){
                tensor block = getSubMatrix(i,j);
                tensor block2 = getSubMatrix(j,i);
                setSubMatrix(i,j,block2);
                setSubMatrix(j,i,block);
            }         
      }
    }
}

int main(){
    long int size[5] = {8,128,1024,2048,4096}; // test sizes
    struct timeval start, end;
    
    int i=0;        // Have to run 1 at a time
   // for (int i=0; i<4; i++){
    CreateMatrix(size[i]);
    gettimeofday(&start, NULL);
    transpose_blocks(size[i]);
    shuffle(size[i]);
    gettimeofday(&end, NULL);
    cout << "time - "<< ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))<<endl;
    PrintMatrix(size[i]);
   // }
    
    return 0;
}
