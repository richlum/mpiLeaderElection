#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "electleader.h"

// using Hirschberg-Sinclair algorithm
// ref https://docs.google.com/viewer?a=v&q=cache:qqtoGluBypYJ:www.di.univaq.it/~proietti/slide_algdist2010/2-LE.ppt+&hl=en&gl=ca&pid=bl&srcid=ADGEESguv4jdZjjV4ypRKj3niGI9xY-yzajprhOzWggCKICUK63i3y4uHABByju8VasWsWanhXMFpcSBgRf3t2zv0InLcJP_yhLeiYzeehPt3un0W-el4qbNdVMczN8y0vJ_4KcgFloz&sig=AHIEtbSkLeUHbb8UPfZdUrzhimjbWpcOvQ
//     https://docs.google.com/viewer?pid=bl&srcid=ADGEEShymSElfUGsYjYXWcQbIr2s9sBjpPINbaZpQ3IbBwkaxoG5TAZ7Ppe_x5MeJU2foimQ-zsc_dFjGJAXWwaG63HfwjeUMW367e6y3OE_uToIFfMZRa7zfntHb756k8GnpO_5IQtK&q=cache%3A3k7P69nWwGYJ%3Awww.cs.utexas.edu%2Fusers%2Florenzo%2Fcorsi%2Fcs380d%2Fpast%2F01F%2Fnotes%2FLeader.ppt%20&docid=245c8f8f6d8d44eab6dd1b0248cd2e03&a=bi&pagenumber=28&w=703
// send out messages left and right 
// receivers only allow messages to travel 2^phase  away from originator (decr cntr as msg travels)
// each phase elects local leaders based on node receiving OK from both left and right side
// stop when (2^phase) is greater than n/2 - message travels at least half way round so
//   elected node must receive confirmation from all nodes in the circle.



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
	printf("%d:%d sending %d %d, sent=%d\n", *rank, next_neighbour, msg->msgtype, msg->UID,msent); 
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
	printf("%d:%d sending %d %d, sent=%d\n", *rank, next_neighbour, msg->msgtype, msg->UID,msent); 
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
	printf("%d:source=%d, uid=%d tag=%d  lngth = %d\n", *rank, mstat.MPI_SOURCE, msg.UID, mstat.MPI_TAG, count );
	
	DBGMSG
	
	if (msg.msgtype==OUT){
	  DBGMSG
	    if(fromleft(source,*rank,*size)){
	      DBGMSG
		if ((msg.UID > *id)&&(msg.distance>1)){
		  //decr distance and forward message
		  msg.distance--;
		  DBGMSG
		  sendmsgNext(&msg, rank, size);
		}else if ((msg.UID > *id)&&(msg.distance==1)){
		  //respond to message
		  printf("%d: id=%d, type=%d, uid=%d, dist=%d \n", *rank,*id, msg.msgtype, msg.UID, msg.distance); 
		  msg.msgtype=IN;
		  msg.distance=1;
		  DBGMSG
		  sendmsgPrev(&msg, rank, size);
		}else if (msg.UID == *id){
		  DBGMSG
		  leader=*id;
		  announce(*rank,*id,NUM);
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
		if (msg.UID==*id)
		  inleft=1;
		else{
		  DBGMSG
		  sendmsgNext(&msg, rank, size);
		}
		if (inright)
		  inleftandright=1;
	    }else{
		if (msg.UID==*id)
		  inright=1;
		else{
		  DBGMSG
		  sendmsgPrev(&msg, rank, size);
		}
		if (inleft)
		  inleftandright=1;
	    }
	    //received responses from both sides, we're locally elected 
	    if (inleftandright){
		phase++;
      		message msg;
		msg.msgtype=OUT;  
		msg.distance=2^phase;  // increase distance of voting  based on phase
		msg.UID=*id; 
		printf("phase = %d\n",phase);
		participant=1;
		  DBGMSG
		  sendmsgNext(&msg,rank,size);
		  sendmsgPrev(&msg,rank,size);
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

