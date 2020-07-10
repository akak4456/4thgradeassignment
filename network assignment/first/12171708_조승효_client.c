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
	//username�� �ִ� ���ڼ��� 10�� 
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
	//���� ���� 
	strncpy(username,argv[3],strlen(argv[3])+1);
	//username ��� 
	pid = fork();
	//����� ������ ���� ���μ��� fork 
	if(pid == 0)
	//�� ���μ����� ���� ��ƾ ���� 
		write_routine(sock,buf,username);
	else
	//�ٸ� ���μ����� �б� ��ƾ ���� 
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
		//buffer�� �ִ� message�� username�� ������ 
		printf("[%s] %s",recvusername,readmsg);
		//username�� �������� ���� �޽����� ��� 
	}
}
void write_routine(int sock,char* buf,char *username){
	char writemsg[90];
	while(1){
		fgets(writemsg,90,stdin);
		if(!strcmp(writemsg,"q\n") || !strcmp(writemsg,"Q\n")){
			shutdown(sock,SHUT_WR);
			//half close ��� ��� 
			return;
		}
		strncpy(buf,writemsg,strlen(writemsg)+1);
		strncpy(buf+90,username,strlen(username)+1);
		//buffer�� ������ ���� 
		write(sock,buf,BUF_SIZE);
		//������ ������ ������ 
	}
}
void error_handling(char* message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
