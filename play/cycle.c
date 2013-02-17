#include <stdio.h>
#include <stdlib.h>





int fromleft(int source, int self, int size){
    if (self == 0){
      // rank 0, node to left is max rank
      if(source==(size-1)){
        return 1;
      }
    }else{
      // general case for any rank other than 0
      if (source==self-1){
        return 1;
      }
    }
    return 0;
}

int main (int argc, char** argv){

int source;
int self = 2;
int size = 3;
for (source = 0 ;source < size; source++){
    int rc = fromleft(source,self,size);
    printf("rc = %d for src = %d, self = %d, size = %d\n",
        rc, source, self, size);
}



}
