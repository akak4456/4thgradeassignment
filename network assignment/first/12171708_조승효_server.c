#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char* buf);

int main(int argc,char* argv[]){
	int serv_sock,clnt_sock;
	struct sockaddr_in serv_adr,clnt_adr;
	struct timeval timeout;
	fd_set reads,cpy_reads;
	
	socklen_t adr_sz;
	
	int fd_max,str_len,fd_num,i,j;
	char buf[BUF_SIZE];
	if(argc != 2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock,5) == -1)
		error_handling("listen() error");
	//클라이언트를 받아들이기 위한 처리	
	FD_ZERO(&reads);
	//혹시 모를 오류 방지 
	FD_SET(serv_sock,&reads);
	//server socket에서 발생하는 이벤트 감시 
	fd_max = serv_sock;
	while(1){
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec=5000;
		
		if((fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout)) == -1)
			break;
		if(fd_num == 0)
			//timeout이 되면 
			continue;
		for(i=0;i<fd_max+1;i++){
			if(FD_ISSET(i,&cpy_reads)){
				if(i == serv_sock){
					//server socket에서 이벤트가 발생하면
					//client socket을 만든다 
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
					FD_SET(clnt_sock,&reads);
					//client socket 이벤트 감시 
					if(fd_max < clnt_sock)
						fd_max=clnt_sock;
					printf("connected client : %d\n",clnt_sock);
				}else{
					//client socket에서 이벤트가 발생하면 
					str_len = read(i,buf,BUF_SIZE);
					//client에서 보낸 메시지를 받는다 
					if(str_len == 0){
						//client socket이 닫혔다면 
						FD_CLR(i,&reads);
						close(i);
						printf("closed client: %d\n",i);
					}else{
						for(j=3;j<fd_max+1;j++){
							//클라이언트끼리 서로의 채팅을 볼 수 있어야함
							//그래서 모든 클라이언트에게 서버가 받은 메시지를 보냄
							//이때 0~2는 입출력, 에러와 관련된 파일 디스크립터이므로
							//3부터 순회하면서 확인
							if(j != serv_sock&&FD_ISSET(j,&reads)){
								//원본을 확인하는 이유는
								//여기에 client list가 있기 때문임
								//그러면 j는 client socket을 가리키게 됨 
								write(j,buf,str_len);
							} 
						}
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}
void error_handling(char* buf){
	fputs(buf,stderr);
	fputc('\n',stderr);
	exit(1);
}
