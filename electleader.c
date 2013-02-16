#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "electleader.h"

int startelection(message* msg, int* participant,int* rank,int* size){
	//mark self as a participant
	*participant=1;
	int next_neighbour = (rank+1)%(*size);
	//  *data, qty, type, dest, tag, mpi_comm
	MPI_Send(msg,2,MPI_INT, next_neighbour, msg->type,MPI_COMM_WORLD); 
} 

void sendmsg(message* msg, int* participant, *int rank, *int size){
	//mark self as a participant
	*participant=1;
	int next_neigbour = (rank+1)%(*size);
	MPI_Send(msg,sizeof(msg),MPI_INT, next_neighbor, msg->type, MPI_COMM_WORLD);
}


int checkmsgs(int* uid,int* participant,int* rank, int* size,int* id){
	message msg;
	int prev_neighour = (rank-1)%(*size);
	MPI_Status mstat;
	MPI_Rev(&msg,2,MPI_INT, prev_neighbour, MPI_ANY_TAG, MPI_COMM_WORLD,
		&mstat); 
	*uid = msg.uid;
	if (msg.msgtype==ELECTION){
		if (*uid>*id){	
			sendmsg(&message,partipant,rank, size);	
		}else if (*uid<*id){
			if (*participant==0){
				msg.UID=id;
				sendmsg(&message,partipant,rank, size);	
			}else{
			//discard msg, process already sent outlarger uid
			}
		}else if (*uid == *id){
			//this process is a leader
			startleading(participant,rank,size);
		}
	}else if (buf.msgtype==ELECTED){
		*participant=0;
		
	
	}else{
		fprintf(stderr,"msgtype (%d) unknown",buf.msgtype);
	}	
}

int main(int argc,char** argv){

	int rank, NUM;
	int leader=-1;
	int mrecved=0,msent=0;	
	int PNUM = 11;
	if (argc>1){
		PNUM=atoi(argv[1]);
	}
	printf("PNUM = %d\n",PNUM);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&NUM);
	printf("size of world = %d\n", NUM);

	int id= (rank+1)*PNUM % NUM;
	//mark initially as non participant
	int particpant = 0;
	if (leader==-1){
		//no leader, start election
		message msg;
		msg.type=ELECTION;
		msg.UID=id;
		sendmsg(&msg,&participant,&rank,&NUM);
		int uid;
		while(leader==-1){
			uid = checkmsgs(&uid,&participant,&rank,&NUM,&id);
		}
	
	}



	
	printf("rank=%d, id=%d, leader=%d, mrecved=%d, msent=%d\n", 
		rank, id, leader, mrecved, msent);
	MPI_Finalize();
	return 0;
}

