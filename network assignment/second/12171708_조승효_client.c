#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE];
char msg[BUF_SIZE];
char name_to_server[NAME_SIZE+BUF_SIZE];

int main(int argc,char* argv[]){
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread,rcv_thread;
	void * thread_return;
	
	if(argc != 4){
		printf("Usage : %s <IP> <port> <name>\n",argv[0]);
		exit(1);
	}
	sprintf(name,"%s",argv[3]);
	sock = socket(PF_INET,SOCK_STREAM,0);
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("connect() error");
		
	sprintf(name_to_server,"!%s",argv[3]);
	write(sock,name_to_server,strlen(name_to_server));
	//send username to server
	pthread_create(&snd_thread,NULL,send_msg,(void*)&sock);
	//make send thread 
	pthread_create(&rcv_thread,NULL,recv_msg,(void*)&sock);
	//make receive thread
	pthread_join(snd_thread,&thread_return);
	pthread_join(rcv_thread,&thread_return);
	close(sock);
	return 0;
}

void * send_msg(void * arg){
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1){
		memset(msg,0,sizeof(msg));
		fgets(msg,BUF_SIZE,stdin);
		//get user input
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")){
			close(sock);
			exit(0);
		}
		memset(name_msg,0,sizeof(name_msg));
		sprintf(name_msg,"%s %s",name,msg);
		write(sock,name_msg,strlen(name_msg));
		//send name and message to server
	}
	return NULL;
}

void * recv_msg(void* arg){
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	memset(name_msg,0,sizeof(name_msg));
	int str_len;
	while(1){
		str_len = read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);
		//receive message from server
		if(str_len == -1)
			return (void*)-1;
		name_msg[str_len] = 0;
		fputs(name_msg,stdout);
		//display message
	}
	return NULL;
}

void error_handling(char * msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}
