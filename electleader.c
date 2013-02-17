#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "electleader.h"

int msent = 0;
int mrecved = 0;
// using msgtype(ELECTION,ELECTED)  as tag send msgs to next highest rank
// recv msg from next lower rank unless root, receive msg from lowest rank
void sendmsg(message* msg, int* rank, int* size){
	DBGMSG
	int next_neighbour = (*rank+1)%(*size);
	msent++;
	printf("%d:%d sending %d %d\n", *rank, next_neighbour, msg->msgtype, msg->UID); 
	MPI_Send(msg,sizeof(msg),MPI_INT, next_neighbour, msg->msgtype, MPI_COMM_WORLD);
	DBGMSG
}

// void startelection(message* msg, int* participant,int* rank,int* size){
// 	DBGMSG
// 	//mark self as a participant
// 	*participant=1;
// 	int next_neighbour = (*rank+1)%(*size);
// 	msent++;
// 	//  *data, qty, type, dest, tag, mpi_comm
// 	MPI_Send(msg,2,MPI_INT, next_neighbour, msg->msgtype,MPI_COMM_WORLD); 
// 	DBGMSG
// } 

void startleading( int* id, int* rank, int* size){
    DBGMSG
    int next_neighbour = (*rank+1)%(*size);
    message msg;
    msg.msgtype = ELECTED;
    msg.UID = *id;
    printf("%d:%d sending %d %d\n", *rank, next_neighbour, msg.msgtype, msg.UID); 
    msent++;
    MPI_Send(&msg,sizeof(msg),MPI_INT, next_neighbour, msg.msgtype, MPI_COMM_WORLD);
    DBGMSG
}

int checkmsgs(int* uid,int* participant,int* rank, int* size,int* id){
	DBGMSG
	message msg;
	int next_neighbour;
	if (*rank!=0)
	  next_neighbour= (*rank)-1;
	else
	  next_neighbour =(*size) -1;
	printf("%d:receive from %d\n",*rank, next_neighbour);
	MPI_Status mstat;
	DBGMSG
	MPI_Recv(&msg,sizeof(msg),MPI_INT, next_neighbour, MPI_ANY_TAG, MPI_COMM_WORLD,
		&mstat);
	printf("%d:%d recv %d %d\n", *rank, next_neighbour, msg.msgtype, msg.UID); 
	mrecved++;
	int count;
	MPI_Get_count(&mstat, MPI_INT, &count);
	printf("%d:source=%d, uid=%d tag=%d  lngth = %d\n", *rank, mstat.MPI_SOURCE, msg.UID, mstat.MPI_TAG, count );
	DBGMSG
	*uid = msg.UID;
	if (msg.msgtype==ELECTION){
		if (*uid>*id){
			//foward msg
			*participant=1;
			printf("%d: forwarding msg\n", *rank);
			sendmsg(&msg,rank, size);	
		}else if (*uid<*id){
			if (*participant==0){
				//rewrite msg with own uid and send
				printf("%d: substutition msg uid %d to %d\n",*rank,msg.UID,*id);
				msg.UID=*id;
				*participant=1;
				sendmsg(&msg,rank, size);
				DBGMSG
			}else{
			      DBGMSG
			      return -1;
			      //discard msg, process already sent larger uid
			}
		}else if (*uid == *id){
			//we got back our own id, this process is a leader
			startleading(id,rank,size);
			DBGMSG
			return *id;
		}
	}else if (msg.msgtype==ELECTED){
		// mark self non participant, forward message, save leader
		*participant=0;
		printf("%d:forwarding elected message\n",*rank);
		sendmsg(&msg,rank, size);
		DBGMSG
		return msg.UID;
	}else{
		fprintf(stderr,"msgtype (%d) unknown",msg.msgtype);
	}
	//no leader found yet
	DBGMSG
	return -1;
	
}

int main(int argc,char** argv){

	int rank, NUM;
	int leader=-1;
	//mark initially as non participant
	int participant =0;
	int PNUM = 11;
	if (argc>1){
		PNUM=atoi(argv[1]);
	}
	printf("PNUM = %d\n",PNUM);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&NUM);
	printf("%d:size of world = %d\n",rank, NUM);

	int id= (rank+1)*PNUM % NUM;
	printf("%d:started rank=%d, id=%d\n",rank, rank, id);
	if (NUM==1){
	  leader=id;
	  //defacto leader
	}else if (leader==-1){
		//no leader, start election, propose self as leader
		message msg;
		msg.msgtype=ELECTION;
		msg.UID=id;
		participant=1;
		sendmsg(&msg,&rank,&NUM);
		int uid;
		while(leader==-1){
			leader = checkmsgs(&uid,&participant,&rank,&NUM,&id);
		}
	
	}

	
	printf("rank=%d, id=%d, leader=%d, mrecved=%d, msent=%d\n", 
		rank, id, leader, mrecved, msent);
	MPI_Finalize();
	return 0;
}

