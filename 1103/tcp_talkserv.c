#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>


char *EXIT_STRING = "exit";
int recv_and_print(int sd);
int input_and_send(int sd);

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock, addrlen = sizeof(cliaddr);
	
	pid_t pid;


	if (argc != 2) {
		printf("usage:%s port\n", argv[0]);
		exit(0);
	}

	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}

	puts("서버가 연결요청을 기다림..");

	listen(listen_sock, 1);

	
	accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
	
	if (accp_sock < 0) {
		perror("accept fail");
		exit(0);
	}

	puts("클라이언트가 연결됨");
	

//accept가 되어서 클라랑 연결이 되면 101페이지에 필기한 대로 
//자식 프로세스를 새로 생성

	if ((pid = fork()) > 0) { // 부모프로세스일때
		input_and_send(accp_sock); //클라는 그냥 소켓번호를 넣어주고
			//서버측은 accp_sock을 넣어준다
//read/write에서 순서가 자유로운 것이지 , TCP기반 서버는
//accp_sock을 통해서 통신하는 것은 변함이 없다.
	}
	else if (pid == 0) { // 자식프로세스일때
		recv_and_print(accp_sock);
	}

	close(accp_sock);
	close(listen_sock);
	return 0;
}


int input_and_send(int sd) {
	char buf[MAXLINE + 1];
	int nbyte;
	while (fgets(buf,sizeof(buf),stdin)!=NULL) {
		nbyte = strlen(buf);
		write(sd, buf, strlen(buf));

		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye");
			close(sd);
			exit(0);
		}
	}
	return 0;
}

int recv_and_print(int sd) {
	char buf[MAXLINE + 1];
	int nbyte;

	while (1) {
		if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;
		if (strstr(buf, EXIT_STRING) != NULL) {
						break;
//여기서 break를 썼으므로 상대방이 exit를 입력해도
//상대방 자신만 종료가 되고, 그걸 받는 쪽은 그냥 넘긴다.
//직접 자기가 exit를 입력해야 자기것이 꺼지는 것을 의도한 것이다.
		}
		printf("%s", buf);
	}
	return 0;
}