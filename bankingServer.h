#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>

#ifndef BANKINGSERVER_H
#define BANKINGSERVER_H

/*typedef struct bankAccount{
	char* accountName;
	double balance;
	struct bankAccount *next;
	int in_session;
}account;*/

typedef struct clientInfo{
	int sockfd;
}clinf;

typedef struct serverInfo{
	//int sockfd;
	int serv_socket;
}srinf;

typedef struct sessionInfo{
	int client_socket;
}seshinf;

typedef struct accountDatabase{
	char* name;
	double balance;
	int inService; // -1 means not in session, 1 is in session
	struct accountDatabase* next;
}account;

typedef struct connectionNode{
	int sockfd;
	struct connectionNode* next;
}connection;


#endif
