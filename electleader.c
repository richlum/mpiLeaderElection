#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "electleader.h"
#include <math.h>
//# ############################
//# richardlum13@gmail.com
//# 20130217
//# cpsc 417
//# assign 2



int msent = 0;
int mrecved = 0;
int participant =0;
int leader=-1;
int inleft=0;
int inright=0;
int inleftandright=0;
int phase =0;
int NUM;

// send msg to next higher rank
void sendmsgNext(message* msg, int* rank, int* size){
	DBGMSG
	if (msg->distance<=0)
	  return;
	int next_neighbour = (*rank)+1;
	if (*rank >= (NUM-1))
	  next_neighbour=0;
	msent++;
#ifdef DEBUG
	printf("%d:%d sending %d %d, sent=%d\n", *rank, next_neighbour, msg->msgtype, msg->UID,msent); 
#endif
	MPI_Send(msg,sizeof(msg),MPI_INT, next_neighbour, msg->msgtype, MPI_COMM_WORLD);
	DBGMSG
}

// send msg to prev (lower rank)
void sendmsgPrev(message* msg, int* rank, int* size){
	DBGMSG
	if (msg->distance<=0)
	  return;
	int next_neighbour = (*rank)-1;
	if (*rank ==0 )
	   next_neighbour = NUM -1 ;
	msent++;
#ifdef DEBUG
	printf("%d:%d sending %d %d, sent=%d\n", *rank, next_neighbour, msg->msgtype, msg->UID,msent); 
#endif
	MPI_Send(msg,sizeof(msg),MPI_INT, next_neighbour, msg->msgtype, MPI_COMM_WORLD);
	DBGMSG
}




int fromleft(int source, int self, int size){
    if (self == 0){
      // rank 0, node to left is max rank
      if(source==(NUM-1)){
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

void announce(int rank, int id, int size){
    message msg;
    msg.msgtype=ELECTED;
    msg.distance=size;
    msg.UID=id;
    sendmsgNext(&msg, &rank, &size);
  
}


void checkmsgs(int* rank, int* size,int* id){
	message msg;
	MPI_Status mstat;
	DBGMSG
	MPI_Recv(&msg,sizeof(msg),MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
		&mstat);
	mrecved++;
	//printf("%d:%d recv %d %d, recvd = %d\n", *rank, next_neighbour, msg.msgtype, msg.UID, mrecved); 
	int count;
	MPI_Get_count(&mstat, MPI_INT, &count);
	int source = mstat.MPI_SOURCE;
#ifdef DEBUG
	printf("%d:source=%d, uid=%d tag=%d  lngth = %d\n", *rank, mstat.MPI_SOURCE, msg.UID, mstat.MPI_TAG, count );
#endif
	
	DBGMSG
	
	if (msg.msgtype==OUT){
	  DBGMSG
	    if(fromleft(source,*rank,*size)){
	      DBGMSG
		if ((msg.UID > *id)&&(msg.distance>1)){
		  //decr distance and forward message
		  msg.distance--;
		  DBGMSG
#ifdef DEBUG
		  printf("%d,%d:propagating %d, distance set to %d",*rank,*id, msg.UID, msg.distance);
#endif
		  sendmsgNext(&msg, rank, size);
		}else if ((msg.UID > *id)&&(msg.distance==1)){
		  //respond to message
#ifdef DEBUG
		  printf("%d, respoding to %d, distance = %d\n",*rank, msg.UID,msg.distance);
		  printf("%d: id=%d, type=%d, uid=%d, dist=%d \n", *rank,*id, msg.msgtype, msg.UID, msg.distance); 
#endif
		  msg.msgtype=IN;
		  msg.distance=1;
		  DBGMSG
		  sendmsgPrev(&msg, rank, size);
		}else if (msg.UID == *id){
		  DBGMSG
		  leader=*id;
		  announce(*rank,*id,NUM);
		}else if ((msg.distance <=1)||(msg.UID<*id)){
#ifdef DEBUG
		  printf("%d,%d: Stopping msg\n",*rank,*id);
		  printf("%d: msg.UID = %d, msg.distance = %d\n", *rank, msg.UID, msg.distance);
#endif
		}else {
		  printf("%d,%d: unexpected case - stopped\n",*rank,*id);
		  printf("%d: msg.UID = %d, msg.distance = %d\n", *rank, msg.UID, msg.distance);
		}

		DBGMSG
	    }else{
		DBGMSG
		//from right
		if ((msg.UID > *id)&&(msg.distance>1)){
		  //decr distance and forward message
		  msg.distance--;
		  DBGMSG
		  sendmsgPrev(&msg, rank, size);
		}else if ((msg.UID > *id)&&(msg.distance==1)){
		  //respond to message
		  msg.msgtype=IN;
		  msg.distance=1;
		  DBGMSG
		  sendmsgNext(&msg, rank, size);
		}else if (msg.UID == *id){
		  DBGMSG
		  leader=*id;
		  announce(*rank,*id,NUM);
		}     
		DBGMSG
	    }
	}else if ((msg.msgtype==IN)){
	  DBGMSG
	    if(fromleft(source,*rank,*size)){
		if (msg.UID==*id){
		  inleft=1;
		}else{
		  DBGMSG
		  inleft=1;
		  sendmsgNext(&msg, rank, size);
		}
		if (inright)
		  inleftandright=1;
	    }else{
		if (msg.UID==*id){
		  inright=1;
		  DBGMSG
		  sendmsgPrev(&msg, rank, size);
		}
		if (inleft){
		  inleftandright=1;
		}
	    }
	   
	    //received responses from both sides, we're locally elected 
	    if (inleftandright){
		phase++;
      		message msg;
		msg.msgtype=OUT;
		msg.distance=pow(2,phase);  // increase distance of voting  based on phase
		int newsize=msg.distance;
		msg.UID=*id; 
		printf("%d:Phase = %d, newdistance = %d\n",*rank,phase, newsize);
		participant=1;
		DBGMSG
		sendmsgNext(&msg,rank,&newsize);
		sendmsgPrev(&msg,rank,&newsize);
	    }
	    DBGMSG
	}else if (msg.msgtype==ELECTED){
	   if (msg.UID>*id){
	     leader=msg.UID;
	     if (msg.distance>=0){
	       announce(*rank,leader,msg.distance -1);
	     }
	   }else if (msg.UID<*id){
	     //claimed leader is not maximum, restart election process 
	     	message emsg;
		emsg.msgtype=OUT;  // to handle initiation vs response seperately
		emsg.distance=1;  // to limit distance of 'local' elections based on phase
		emsg.UID=*id;  
		participant=1;
		sendmsgNext(&emsg,rank,size);
		sendmsgPrev(&emsg,rank,size);
	   }else{
	     //we received a message confirming we are the leader
	     leader=*id;
	   }
	     
	}else{
	  fprintf (stderr, "Error unknown msgtype (%d)\n",msg.msgtype);
	}
	DBGMSG
	
}

int main(int argc,char** argv){
	int rank;
	int PNUM = 113;
	if (argc>1){
		PNUM=atoi(argv[1]);
	}
#ifdef DEBUG
	printf("PNUM = %d\n",PNUM);
#endif
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&NUM);
#ifdef DEBUG
	printf("%d:size of world = %d\n",rank, NUM);
#endif
	
	int id= (rank+1)*PNUM % NUM;
	printf("%d:started rank=%d, id=%d, world = %d\n",rank, rank, id, NUM);
	if (NUM==1){
	  leader=id;
	  //defacto leader
	}else if (leader==-1){
		//no leader, start election, propose self as leader
		message msg;
		msg.msgtype=OUT;  // to handle initiation vs response seperately
		msg.distance=1;  // to limit distance of 'local' elections based on phase
		msg.UID=id;  
		participant=1;
		sendmsgNext(&msg,&rank,&NUM);
		sendmsgPrev(&msg,&rank,&NUM);

		while(leader==-1){
		  //global value leader is updated inside checkmsgs
		  checkmsgs(&rank,&NUM,&id);
		}
	}

	
	printf("rank=%d, id=%d, leader=%d, mrecved=%d, msent=%d\n", 
		rank, id, leader, mrecved, msent);
	participant=0;
	
	MPI_Finalize();
	return 0;
}

