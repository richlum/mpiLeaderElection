#ifndef __ELECTLEADER_H__ 
#define __ELECTLEADER_H__

#define ELECTION 1
#define ELECTED 2
typedef struct message{
	int msgtype;
	int UID;
}message;

#define DEBUG

#ifdef DEBUG
#define DBGMSG printf("\t%d:%s:%d\n",*rank,__FILE__, __LINE__);
#endif



#endif
