//bankingClient
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
int quitCheck = -1;

char* commandCreator(int start, int length, char* tempStr){
	int i;
	char* testStr = (char*)malloc((length+1)*sizeof(char));
	for(i=start;i<length;i++){
		testStr[i]=tempStr[i];
	}
	return testStr;
}

void sendChatter(int sockfd){
	char buff[300]; 
	char* testStr = (char*)malloc(8*sizeof(char));
    		int n; 
    		for(;;) {
        	bzero(buff, sizeof(buff));
		if(quitCheck == 0){
			
			break;	
		} 
        	n = 0;
		
		//printf("Enter Command:\n");
        	while ((buff[n++] = getchar()) != '\n')
		;
		if ((strncmp(buff, "quit\n", 5)) == 0) { 
			quitCheck = 0; 
		} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(buff[0] == 'c'){		
			testStr = commandCreator(0, 7, buff);
			if(strcmp(testStr,"create ")==0 && strlen(buff) >= 9){
		        	write(sockfd, buff, sizeof(buff));
				sleep(2);
			}
			else{
				printf("Improper Input Format.\n");
			}
		}
		else if(buff[0] == 's'){
			testStr = commandCreator(0, 6, buff);
			if(strcmp(testStr, "serve ")==0 && strlen(buff) >= 8){
        			write(sockfd, buff, sizeof(buff));
				sleep(2);
			
			}
			else{
				printf("Improper Input Format.\n");				
			}
		}
		else if(buff[0] == 'd'){
			testStr = commandCreator(0, 8, buff);
			if(strcmp(testStr,"deposit ")==0 && strlen(buff) >= 10){
				write(sockfd, buff, sizeof(buff));
				sleep(2);
			}
			else{
				printf("Improper Input Format.\n");
			}
		}
		else if(buff[0] == 'w'){
			testStr = commandCreator(0, 9, buff);
			if(strcmp(testStr,"withdraw ")==0 && strlen(buff) >= 11){
        			write(sockfd, buff, sizeof(buff));
				sleep(2);							
			}
			else{
				printf("Improper Input Format.\n");
			}
		}
		else if(buff[0] == 'e'){
			testStr = commandCreator(0, 3, buff);
			if(strcmp(testStr, "end")==0 && strlen(buff) <= 4){
        			write(sockfd, buff, sizeof(buff));
				sleep(2);		
			}
			else{
				printf("Improper Input Format.\n");
			}
		}
		else if(buff[0] == 'q'){
			if(strcmp(buff, "query\n")==0 && strlen(buff) <= 6){
        			write(sockfd, buff, sizeof(buff));
				sleep(2);
			}
			else if(strcmp(buff, "quit\n")==0 && strlen(buff) <= 5){
        			write(sockfd, buff, sizeof(buff));
				sleep(2);
			}
			else{
				printf("Improper Input Format.\n");
			}

		}
		else{
			printf("Improper Input Format.\n");
		}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
		
		}
		return ;

}

void recvChatter(int sockfd){
	char buff[300]; 
    	//int n; 
	for (;;) { 
        	bzero(buff, sizeof(buff)); 	
		read(sockfd, buff, sizeof(buff));
		printf("Response From Server: %s", buff); 
		if ((strncmp(buff, "quit\n", 5)) == 0) {
			printf("You have been disconnected from the server.\n");
			quitCheck = 0;
			close(sockfd);
        		kill(0,SIGKILL); 
		}

		if ((strncmp(buff, "Server Shutdown\n", 16)) == 0) { 
        		printf("You have been disconnected from the server.\n");
			quitCheck = 0; 
			//write(sockfd, buff, sizeof(buff));
			close(sockfd);
        		kill(0,SIGKILL); 
		} 
	}
return;
}

void* send_thread_func(void* threadClinf){
	//pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	clinf* newClinf = (clinf*) threadClinf;
	//pthread_mutex_lock( &mutex1 );
	sendChatter(newClinf->sockfd);
	//pthread_mutex_unlock( &mutex1 );
	return 0;
}

void* recv_thread_func(void* threadClinf){
	//pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	clinf* newClinf = (clinf*) threadClinf;
	//pthread_mutex_lock( &mutex1 );
	recvChatter(newClinf->sockfd);
	//pthread_mutex_unlock( &mutex1 );
	return 0;
}


int main(int argc, char** argv){

	int sendStatus;
	int recvStatus;
	if(argc != 3){
		printf("Invalid # of arguments\n");
	}
	char* machineName = argv[1];
	machineName = machineName;
	char ip_address[200];
	int portNum = atoi(argv[2]);

	struct hostent *he;
	struct in_addr ** serv_addr_list;
	
	if((he = gethostbyname(machineName)) == NULL){
		perror("gethostbyname error\n");
		return -1;	
	}
	serv_addr_list = (struct in_addr **) he->h_addr_list;
	
	strcpy(ip_address, inet_ntoa(*serv_addr_list[0]));

	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip_address);
	serv_addr.sin_port = htons(portNum);

	while(connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0){
		
		printf("Connection Failed, Retrying...\n");
		sleep(3);
	
			
	}
	//ANNOUCEMENT OF CONNECTION SUCCESS
	printf("Connection Successful.\n");


	clinf* newClinf = (clinf*)malloc(sizeof(clinf));
	newClinf->sockfd = client_sock;	

	pthread_t send_thread_id;
	sendStatus = pthread_create(&send_thread_id, NULL, (*send_thread_func) , (void*) newClinf);
	sendStatus = sendStatus;

	pthread_t recv_thread_id;
	recvStatus = pthread_create(&recv_thread_id, NULL, (*recv_thread_func) , (void*) newClinf);
	recvStatus = recvStatus;

	pthread_join(recv_thread_id,0);
	pthread_join(send_thread_id,0);



	close(client_sock);
		
	return 0;
	
	
}
