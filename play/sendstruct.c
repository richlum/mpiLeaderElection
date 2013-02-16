#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


typedef struct message{
    int msgtype;
    int UID;
}message;


int main(int argc,char** argv){

    int rank, NUM;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&NUM);
    printf("%d:size of world = %d\n",rank, NUM);
    
    message msg;
    msg.msgtype = 1;
    msg.UID = rank;

    int next_neighbour = (rank+1)%(NUM);
    MPI_Send(&msg,sizeof(msg),MPI_INT, next_neighbour, msg.msgtype, MPI_COMM_WORLD);
    printf ("%d,send msg to %d (size=%d)/n",rank,next_neighbour,sizeof(msg));
    next_neighbour = (rank-1)%(NUM);
    MPI_Status mstat;
    MPI_Recv(&msg,2,MPI_INT, next_neighbour, MPI_ANY_TAG, MPI_COMM_WORLD,
        &mstat);
    printf("%d: received message\n", rank);
    printf("msg = (%d , %d) \n", msg.msgtype, msg.UID);

    MPI_Finalize();
    return 0;
}
