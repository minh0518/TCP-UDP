#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock,addrlen = sizeof(cliaddr),nbyte;
	char buf[MAXLINE + 1];


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

//바인드를 하는데 내가 이 서버의 포트번호를 4025로 지정했다면
//연결요청을 '받는' listen_sock에다가 4025포트번호와 
//INADDR_ANY로 받은 IP를 합쳐서 이제 
//클라측에서 나중에 111.11.... 4025 이렇게 IP주소와 포트번호를
//주게 되면 여기 listen_sock 소켓에 연결이 되는 것이다.
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}

	listen(listen_sock, 5);
	while (1) {
		puts("서버가 연결요청을 기다림..");
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
		//cliaddr가 클라의 servaddr_in 라고 뭐 라이브러리마냥 정해진
//것은 없었다. 그냥 cliaddr는 변수에 그쳤지만 accept함수의 두번째
//인자로 사용됨으로써 비로소 이게 클라의 servaddr이 되는 것이다.
		if (accp_sock < 0) {
			perror("accept fail");
			exit(0);
		}

		puts("클라이언트가 연결됨");
		nbyte = read(accp_sock, buf, MAXLINE);
		write(accp_sock, buf, nbyte);
		close(accp_sock);
	
		}
	close(listen_sock);
	return 0;
	}