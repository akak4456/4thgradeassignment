#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define MAX_CLNT 256
struct User{
	int sock;
	char name[NAME_SIZE];
};
void * handle_clnt(void * arg);
void send_msg_to_all(char* senderName,char* msg,int len);
void send_msg_to_someone(char* senderName,char* msg,int len, int tosock);
void error_handling(char* msg);
int compareName(char* msg);
int user_cnt = 0;
struct User users[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char* argv[]){
	int serv_sock,clnt_sock;
	struct sockaddr_in serv_adr,clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc != 2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx,NULL);
	serv_sock=socket(PF_INET,SOCK_STREAM,0);
	
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock,5)==-1)
		error_handling("listen() error");
		
	while(1){
		clnt_adr_sz = sizeof(clnt_adr);
		
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_create(&t_id,NULL,handle_clnt,(void*)&clnt_sock);
		pthread_detach(t_id);
	}
	close(serv_sock);
	return 0;
}
void * handle_clnt(void * arg){
	int clnt_sock = *((int*)arg);
	int str_len =0,i,j;
	char msg[BUF_SIZE+NAME_SIZE];
	char registerName[NAME_SIZE];
	memset(msg,0,sizeof(msg));
	memset(registerName,0,sizeof(registerName));
	while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0){
		if(msg[0] == '!'){
			//the message is for registering user to server.
			strncpy(registerName,msg+1,str_len-1);
			printf("User @%s has entered the chat\n",registerName);	
			pthread_mutex_lock(&mutx);
			users[user_cnt].sock = clnt_sock;
			strncpy(users[user_cnt].name,registerName,strlen(registerName));
			user_cnt++;
			pthread_mutex_unlock(&mutx);
		}else{
			//this is chat message
			int startIdx = 0;
			char senderName[NAME_SIZE];
			char extractMsg[BUF_SIZE+NAME_SIZE];
			char extractName[NAME_SIZE];
			memset(senderName,0,sizeof(senderName));
			memset(extractMsg,0,sizeof(extractMsg));
			memset(extractName,0,sizeof(extractName));
			for(i=0;i<strlen(msg);i++){
				if(msg[i] == ' '){
					if(startIdx == 0){
						//it mean senderName
						strncpy(senderName,msg,i);
						startIdx = i+1;
						continue;
					}
					strncpy(extractName,msg+startIdx+1,i-startIdx-1);
					strncpy(extractMsg,msg+i+1,strlen(msg)-i-1);
					if(!strcmp(extractName,"all")){
						//send to all
						send_msg_to_all(senderName,extractMsg,strlen(extractMsg));
					}else{
						//send to someone
						int retSock = compareName(extractName);
						if(retSock != -1){
							//if there is user for extractName 
							strncpy(extractMsg,msg+i+1,strlen(msg)-i-1);
							send_msg_to_someone(senderName,extractMsg,strlen(extractMsg),retSock);
							//send message to the user
						}else{
							//if there is no user for extractName
							char errMsg[BUF_SIZE+NAME_SIZE] = "Target user not found!\n";
							write(compareName(senderName),errMsg,strlen(errMsg));
							//send error message to sender
						}
						break;
					}
				}
			}			
		}
	}
		
	pthread_mutex_lock(&mutx);
	for(i=0;i<user_cnt;i++){
		if(clnt_sock == users[i].sock){
			printf("User @%s has left the chat\n",users[i].name);
			while(i < user_cnt-1){
				users[i] = users[i+1];
				i++;
			}
			//socket delete
			break;
		}
	}
	user_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
int compareName(char* name){
	//return sock num when name matches user.name
	//return -1 when no user match arg name
	int i;
	for(i=0;i<user_cnt;i++){
		if(!strcmp(users[i].name,name))
			return users[i].sock;
	}
	return -1;
}
void send_msg_to_all(char* senderName,char* msg,int len){
	int i;
	char sendMsg[BUF_SIZE+NAME_SIZE];
	memset(sendMsg,0,sizeof(sendMsg));
	sprintf(sendMsg,"[%s] %s",senderName,msg);
	for(i=0;i<user_cnt;i++){
		write(users[i].sock,sendMsg,strlen(sendMsg));
	}
}
void send_msg_to_someone(char* senderName,char* msg,int len, int tosock){
	char sendMsg[BUF_SIZE+NAME_SIZE];
	memset(sendMsg,0,sizeof(sendMsg));
	sprintf(sendMsg,"[%s] %s",senderName,msg);
	write(tosock,sendMsg,strlen(sendMsg));
} 
void error_handling(char* msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}
