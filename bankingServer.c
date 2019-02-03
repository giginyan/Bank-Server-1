/*#include <stdio.h>
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
*/
#include "bankingServer.h"
account* head = NULL;
connection* connectionHead = NULL;
int server_socket_original;
/*
pthread_mutex_t twoSecMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t twoSec_cond = PTHREAD_COND_INITIALIZER;
*/
sem_t mutex;

void printer(void* arg){
		account* ptr = head;
		printf("\n");
		while(ptr!=NULL){
			char* temp = (char*)malloc((strlen(ptr->name)-1)*sizeof(char));
			strncpy(temp,ptr->name,strlen(ptr->name)-1);
			printf("ACCOUNT NAME: %s\t",temp);
			printf("BALANCE: %f\t",ptr->balance);
			if(ptr->inService == 0){
				printf("IN SERVICE\n");
			}
			else{
				printf("\n");
			}
			
			ptr = ptr->next;
		}
	return;
}
void* printer_thread_func(void* arg){
	sem_wait(&mutex);
	printer(NULL);
	sem_post(&mutex);
	return 0;
}


void signal_catcher(int sig, siginfo_t *siginfo, void *context){
	char str [300];
	connection* ptr = connectionHead;
	printf("\n");
	while(ptr!=NULL){
		strncpy(str, "Server Shutdown\n", 300);
		printf("Disconnecting Client: %d\n",ptr->sockfd);
		write(ptr->sockfd, str, sizeof(str));
		//close(ptr->sockfd);
		ptr=ptr->next;	
	}
	close(server_socket_original);
	kill(0, SIGKILL);
	return;
	
}

void print_handler(int signum){
	//printf("INSIDE PRINT HANDLER\n");
	sem_init(&mutex,0,1);
	pthread_t printer_thread_id;
	int printerStatus = pthread_create(&printer_thread_id,NULL,(*printer_thread_func), NULL);
	printerStatus = printerStatus;
	
	pthread_join(printer_thread_id,0);
	sem_destroy(&mutex);
	return;
}

char* commandCreator(int start, int length, char* tempStr){
	int i;
	char* testStr = (char*)malloc((length+1)*sizeof(char));
	for(i=start;i<length;i++){
		testStr[i]=tempStr[i];
	}
	return testStr;
}

void chatting(int sockfd){
	char str [300];
	int i;
	account* ptr = head;
	connection* cPtr = NULL;
	connection* prev = NULL;
	char* current_inSession = NULL;
	double amount = 0.0;
	//struct timespec timeToWait;
    	//struct timeval nowTime;
    	//int returnWait;
	//gettimeofday(&nowTime,NULL);

	while(1){
		//bzero(str, 300);
		/*
		struct timespec timeToWait;
    		struct timeval nowTime;
		int returnWait;

		gettimeofday(&nowTime,NULL);
	
		timeToWait.tv_sec = nowTime.tv_sec+2;
		timeToWait.tv_nsec = ((nowTime.tv_usec+2000000)*1000);

		pthread_mutex_lock(&twoSecMutex);
	
		 returnWait = pthread_cond_timedwait(&twoSec_cond, &twoSecMutex, &timeToWait);
		printf("returnWait: %d\n", returnWait);
		pthread_mutex_unlock(&twoSecMutex);
		printf("\nDone\n");	
	*/
		read(sockfd, str, sizeof(str));
		char* testStr = (char*)malloc(8*sizeof(char));
		char* tempName= (char*)malloc(257*sizeof(char));
		char* name= (char*)malloc(256*sizeof(char));
		

		//check for type of command received
			//check if command can be performed ex. check if withdraw attempt <= balance
			//perform command
			//write response to client

		if(str[0] == 'c'){		
			testStr = commandCreator(0, 7, str);
			if(strcmp(testStr,"create ")==0 && strlen(str) >= 9){
				
				if(current_inSession != NULL){
					strncpy(str, "Client already in session, unable to create new account.\n", 300);		
				}
				else{
					//printf("Create command Received, %s.\n",testStr);
					tempName = strchr(str,' ');
					for(i=0; i<strlen(tempName);i++){
						name[i]=tempName[i+1];
					}
					if(head==NULL){
						account* newAccount = (account*)malloc(sizeof(account));
						newAccount->name = name;
						newAccount->balance = 0;
						newAccount->inService = -1;
						head=newAccount;
						head->next = NULL;
						strncpy(str, "Account Created Sucessfully.\n", 300);
					}
					else{
						ptr = head;
						while(ptr!=NULL){
							if(strcmp(ptr->name, name)==0){
								strncpy(str, "Account name already exists, account not created\n", 300);
								break;
							}
							ptr=ptr->next;				
						}
						if(ptr == NULL){
							account* newAccount = (account*)malloc(sizeof(account));
							newAccount->name = name;
							newAccount->balance = 0;
							newAccount->inService = -1;
							newAccount->next = head;
							head = newAccount;
							strncpy(str, "Account Created Sucessfully.\n", 300);
							}	
						}
					}
				}
			else{//THIS LINE UNCESSECARY?
				strncpy(str, "'create' Attempt Failed.\n", 300);			
			}//end 44 conditional
		}// end first char c
		else if(str[0] == 's'){
			testStr = commandCreator(0, 6, str);
			if(strcmp(testStr, "serve ")==0 && strlen(str) >= 8){
				if(current_inSession != NULL){
					strncpy(str, "Client already in session, unable to serve another account\n", 300);		
				}
				else{
					//printf("Serve command Received, %s.\n",testStr);
					tempName = strchr(str,' ');
					for(i=0; i<strlen(tempName);i++){
						name[i]=tempName[i+1];
					}
					ptr = head;
					while(ptr!=NULL){
						if(strcmp(ptr->name, name)==0){
							if(ptr->inService == -1){
								current_inSession = ptr->name;
								ptr->inService = 0;
								strncpy(str, "Serving request successful\n", 300);
								break;
							}
							else{
								strncpy(str,"Account aready in Service by another Client\n",300);
								break;
							}
						}
						ptr=ptr->next;
					}
					if(ptr == NULL){
						strncpy(str,"Account name given does not exist in Server\n",300);
					}			
				}						
			}
			else{
				strncpy(str, "Bad input serve\n", 300);			
			}	
		}
/////////////////////////////////////////////////////////////////
		else if(str[0] == 'd'){
			int periodFlag = 1;
			testStr = commandCreator(0, 8, str);
			if(strcmp(testStr,"deposit ")==0 && strlen(str) >= 10){
				if(current_inSession == NULL){
					strncpy(str, "Client is not currently serving an account\n", 300);		
				}
				else{
				//	printf("Deposit command Received, %s.\n",testStr);
					tempName = strchr(str,' ');
					for(i=0; i<strlen(tempName);i++){
						name[i]=tempName[i+1];
					}
					if(strcmp(name,".\n")==0){
						strncpy(str, "Invalid 'deposit' value: only period given\n", 300);
					}
					else{
						for(i = 0; i<strlen(name)-1; i++){
							if(!(isdigit(name[i]))){
								if(name[i] == '.'){
									if(periodFlag == 1){
										periodFlag = 0;
									}
									else{
										strncpy(str, "Invalid 'deposit' value: too many periods.\n", 300);
										break;		
									}							
								}
								else{
									strncpy(str, "Invalid 'deposit' value: non-numeric character present.  \n", 300);
									break;	
								}						
							}						
						}//end for
						if(i == strlen(name)-1){
							amount = atof(name);
							ptr = head;
							while(ptr!=NULL){
								if(ptr->inService == 0){
									if(strcmp(ptr->name, current_inSession)==0){
										ptr->balance += amount;	
										strncpy(str, "Deposit Request Successful\n", 300);
										break;
									}						
								}
								ptr = ptr->next;					
							}//end while					
						}
					}
			
				}
			}
			else{
				strncpy(str, "Bad input deposit\n", 300);
			}			
		}

		else if(str[0] == 'w'){
			int periodFlag = 1;
			testStr = commandCreator(0, 9, str);
			if(strcmp(testStr,"withdraw ")==0 && strlen(str) >= 11){
				if(current_inSession == NULL){
					strncpy(str, "Client is not currently serving an account\n", 300);		
				}
				else{
				//	printf("Withdraw command Received, %s.\n",testStr);
					tempName = strchr(str,' ');
					for(i=0; i<strlen(tempName);i++){
						name[i]=tempName[i+1];
					}
					if(strcmp(name,".\n")==0){
						strncpy(str, "Invalid 'withdraw' value: only period given\n", 300);
					}
					else{
						for(i = 0; i<strlen(name)-1; i++){
							if(!(isdigit(name[i]))){
								if(name[i] == '.'){
									if(periodFlag == 1){
										periodFlag = 0;
									}
									else{
										strncpy(str, "Invalid 'withdraw' value: too many periods\n", 300);
										break;		
									}							
								}
								else{
									strncpy(str, "Invalid 'withdraw' value: non-numeric character present.\n", 300);
									break;	
								}						
							}						
						}//end for
						if(i == strlen(name)-1){
							amount = atof(name);
							ptr = head;
							while(ptr!=NULL){
								if(ptr->inService == 0){
									if(strcmp(ptr->name, current_inSession)==0){
										if(amount > ptr->balance){
											strncpy(str,"'withdraw' error: Amount exceeds current balance\n", 300);
											break;
										}
										else{
											ptr->balance -= amount;	
											strncpy(str, "'withdraw' Request Successful.\n", 300);
											break;
										}
									}						
								}
								ptr = ptr->next;					
							}//end while					
						}
					}
			
				}
			}
			else{
				strncpy(str, "Bad input withdraw\n", 300);
			}			
		}
		if(str[0] == 'e'){
			testStr = commandCreator(0, 3, str);
			if(strcmp(testStr, "end")==0 && strlen(str) <= 4){
				if(current_inSession == NULL){
					strncpy(str, "'end' error: No account session to end\n", 300);		
				}
				else{
					ptr = head;
					while(ptr!=NULL){
						if(strcmp(ptr->name, current_inSession)==0){						
							current_inSession = NULL;
							ptr->inService = -1;
							strncpy(str, "Session ended Sucessfully.\n", 300);
							break;
						}
						ptr=ptr->next;
					}				
				}
			}
			else{
				strncpy(str, "Bad input end\n", 300);
			}
		}
		if(str[0] == 'q'){
			if(strcmp(str, "query\n")==0 && strlen(str) <= 6){
				if(current_inSession == NULL){
					strncpy(str, "Client is not currently serving an account\n", 300);		
				}
				else{
					ptr = head;
					while(ptr!=NULL){
						if(strcmp(ptr->name, current_inSession)==0){
							char buffer [300];
							sprintf(buffer, "Current balance: $%f\n", ptr->balance);
							strncpy(str, buffer, 300);
							break;
						}						
						ptr = ptr->next;					
					}//end while				
				}
			}
			else if(strcmp(str, "quit\n")==0 && strlen(str) <= 5){
				ptr = head;
				while(ptr!=NULL){
					if(ptr->name == current_inSession){
						ptr->inService = -1;
						break;
					}						
					ptr = ptr->next;
				}
				current_inSession = NULL;
				cPtr = connectionHead;
				prev = NULL;
				while(cPtr!=NULL){
					if(cPtr->sockfd == sockfd){
						if(prev != NULL){
							printf("Disconnecting Client: %d\n",cPtr->sockfd);
							prev->next = cPtr->next;
							break;
						}
						else{
							printf("Disconnecting Client: %d\n",cPtr->sockfd);
							connectionHead = cPtr->next;
							break;						
						}
					}						
					prev = cPtr;
					cPtr = cPtr->next;
				}				
				
			}
			else{
				strncpy(str, "Bad input Q\n", 300);			
			}
		}		
		
		write(sockfd, str, sizeof(str));

		if(strcmp("quit\n", str) == 0){
			//printf("Server exit\n");
			break;
			//return;
		}

		/*
		ptr = head;
		while(ptr!=NULL){
		printf("Account Name: %s", ptr->name);
		printf("Balance: %f", ptr->balance);
		printf("In service: %d\n", ptr->inService);
		printf("\n");
		ptr=ptr->next;
		}
		cPtr = connectionHead;
		while(cPtr!=NULL){
			printf("Connection LinkedList update%d\n",cPtr->sockfd);
			cPtr=cPtr->next;
		}	
*/

	}//end while 1
	return;
}

void* clientSesh_thread_func(void* threadSesh){
	//printf("entered client thread function \n");
	seshinf* newseshinf = (seshinf*) threadSesh;
	chatting(newseshinf->client_socket);
	
return 0;
}

void acceptor(int sockfd){
    	
	
	pthread_t clientSesh_thread_id;

	while(1){
		int client_socket;
		struct sockaddr_in client_addr;
		int clientSesh_status;
		memset(&client_addr,0,sizeof(client_addr));
		socklen_t client_addr_size = sizeof(client_addr);
		client_addr = client_addr ;
		client_socket = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_size);
		client_socket=client_socket;
		
		if(client_socket > 0){

			connection* newConnection = (connection*)malloc(sizeof(connection));
			newConnection->sockfd = client_socket;
			
			if(connectionHead==NULL){
				connectionHead=newConnection;
				connectionHead->next = NULL;
			}
			else{
				newConnection->next = connectionHead;
				connectionHead = newConnection;	
			}
			connection* ptr = connectionHead;
			//while(ptr!=NULL){
				printf("Connection Accepted from Client: %d\n",ptr->sockfd);
				//ptr=ptr->next;
			//}
			
			seshinf* newseshinf = (seshinf*)malloc(sizeof(seshinf));
			newseshinf->client_socket = client_socket;

			pthread_t clientSesh_thread_id;
			clientSesh_status= pthread_create(&clientSesh_thread_id, NULL, (*clientSesh_thread_func) , (void*) newseshinf);
			clientSesh_status = clientSesh_status;	
		}
	}
	pthread_join(clientSesh_thread_id,0);
	return ;

}
void* accept_thread_func(void* threadsrinf){
	srinf* newsrinf = (srinf*) threadsrinf;
	acceptor(newsrinf->serv_socket);
	return 0;
}

int main(int argc, char** argv){
	
	int err;
	int serv_socket;
	int status_code;
	int acceptStatus;
	//int printerStatus;
	//int client_socket;
	//sem_init(&mutex,0,1);
	struct sigaction newAct;
	memset (&newAct, '\0', sizeof(newAct));
	newAct.sa_sigaction = &signal_catcher;
	newAct.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &newAct, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	int portNum = atoi(argv[1]);

	if(argc != 2){
		printf("Invalid # of arguments\n");
	}

	err = (serv_socket = socket(AF_INET, SOCK_STREAM, 0));
	if(err == -1){
		perror("socket");
		printf("Failed to Create Socket Endpoint\n");
		return err;
	}
		
	server_socket_original = serv_socket;

		
	struct linger so_linger;
	so_linger.l_onoff = 0;
	so_linger.l_linger = 0;

	status_code = setsockopt(serv_socket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));

	if(status_code == -1){
		perror("setsocketopt\n");	
	}

	struct sockaddr_in serv_addr;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portNum);
	
	if(bind(serv_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0){
		perror("bind failure\n");
		return -1;	
	}

	listen(serv_socket, 300);
	

	srinf* newsrinf = (srinf*)malloc(sizeof(srinf));
	newsrinf->serv_socket = serv_socket;	

	pthread_t accept_thread_id;
	acceptStatus= pthread_create(&accept_thread_id, NULL, (*accept_thread_func) , (void*) newsrinf);
	acceptStatus = acceptStatus;
//timer
	struct sigaction siggy;
	struct itimerval timer;
	memset(&siggy, 0, sizeof(siggy));
	siggy.sa_handler = &print_handler;
	sigaction(SIGALRM, &siggy, NULL);

	timer.it_value.tv_sec = 15;
	timer.it_interval.tv_sec = 15;
	setitimer(ITIMER_REAL, &timer, NULL);

	
//timer end	
	//sem_init(&mutex,0,1);
	//pthread_t printer_thread_id;
	//printerStatus = pthread_create(&printer_thread_id,NULL,(*printer_thread_func), NULL);
	//printerStatus = printerStatus;

	pthread_join(accept_thread_id,0);
	//pthread_join(printer_thread_id,0);
	sem_destroy(&mutex);
	//client_socket = accept(serv_socket, (struct sockaddr*) &client_addr, &client_addr_size);
	
	//chatting(client_socket);

	printf("close it\n");
	close(serv_socket);
	
	return 0;

}
