#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void error_handling(char* message);
void read_routine(int sock,char* buf);
void write_routine(int sock,char* buf,char* username);

int main(int argc,char* argv[]){
	int sock;
	pid_t pid;
	char buf[BUF_SIZE];
	char username[10];
	//username의 최대 글자수는 10개 
	struct sockaddr_in serv_adr;
	if(argc != 4){
		printf("Usage : %s <IP> <port> <username>\n",argv[0]);
		exit(1);
	}
	sock = socket(PF_INET,SOCK_STREAM,0);
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	//소켓 연결 
	strncpy(username,argv[3],strlen(argv[3])+1);
	//username 담기 
	pid = fork();
	//입출력 분할을 위한 프로세스 fork 
	if(pid == 0)
	//한 프로세스는 쓰기 루틴 실행 
		write_routine(sock,buf,username);
	else
	//다른 프로세스는 읽기 루틴 실행 
		read_routine(sock,buf);
	close(sock);
	return 0;
}
void read_routine(int sock,char* buf){
	char readmsg[90];
	char recvusername[10];
	while(1){
		int str_len = read(sock,buf,BUF_SIZE);
		if(str_len == 0)
			return;
		buf[str_len] = 0;
		strncpy(readmsg,buf,90);
		strncpy(recvusername,buf+90,10);
		//buffer에 있는 message와 username을 꺼내옴 
		printf("[%s] %s",recvusername,readmsg);
		//username과 서버에서 받은 메시지를 출력 
	}
}
void write_routine(int sock,char* buf,char *username){
	char writemsg[90];
	while(1){
		fgets(writemsg,90,stdin);
		if(!strcmp(writemsg,"q\n") || !strcmp(writemsg,"Q\n")){
			shutdown(sock,SHUT_WR);
			//half close 기법 사용 
			return;
		}
		strncpy(buf,writemsg,strlen(writemsg)+1);
		strncpy(buf+90,username,strlen(username)+1);
		//buffer에 데이터 쓰기 
		write(sock,buf,BUF_SIZE);
		//서버로 데이터 보내기 
	}
}
void error_handling(char* message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
