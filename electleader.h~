#ifndef __ELECTLEADER_H__ 
#define __ELECTLEADER_H__

#define ELECTION 1
#define ELECTED 2
// to classify traffic as initiating election or responding to  election msg
#define OUT 10
#define IN 11

typedef struct message{
	int msgtype;
	int distance;
	int UID;
}message;

//#define DEBUG

#ifdef DEBUG
#define DBGMSG printf("\t%d:%s:%d\n",*rank,__FILE__, __LINE__);
#else
#define DBGMSG
#endif



#endif
