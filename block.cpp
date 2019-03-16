#include <iostream>
#include <cstdlib>
//#include <pthreads.h>
#include <vector>

using namespace std;

using tensor = vector <vector<long int>>;


int block_len=2;
long int size_mat =8;
tensor _MAT;

void CreateMatrix(){
    long int val=1;
    vector<long int> num_;

    num_.clear();_MAT.clear();

      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                num_.push_back(val);
                val++;
         }_MAT.push_back(num_);
           num_.clear();
                
       }
}

void PrintMatrix(){
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

void transpose_blocks(){        // transposing inside of all blocks in matrix
      for (int i=0;i<size_mat;i+=block_len){
        for (int j=0;j<size_mat;j+=block_len){
            tensor block = getSubMatrix(i,j);
            transpose(block);
            setSubMatrix(i,j,block);
        }
      }    
}

void shuffle(){             // transposing actual blocks
      for (int i=0;i<size_mat;i+=block_len){
        for (int j=i;j<size_mat;j+=block_len){
            tensor block = getSubMatrix(i,j);
            tensor block2 = getSubMatrix(j,i);
            setSubMatrix(i,j,block2);
            setSubMatrix(j,i,block);
        }
      }
}






int main(){
    
    CreateMatrix();
    PrintMatrix();
    cout <<endl;
    transpose_blocks();
    shuffle();
    PrintMatrix();
    
    return 0;
}
