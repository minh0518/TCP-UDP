#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
//이거 헤더파일도 get_hostent 부분이랑 원래 있떤
//tcp_echocli1 이랑 다 합쳐야 함

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf2[20];
	//여기까지 get_hostent부분에 있는 변수 새로 추가
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE + 1];

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	hp = gethostbyname(argv[1]);// 이부분 추가
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}// 이부분 추가

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	memcpy(&in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr));
	inet_ntop(AF_INET, &in, buf2, sizeof(buf2));
	printf("Server IP : %s\n", buf2);
	//원래는 ip주소가 여러개인 경우를 대비해서 for문으로 돌리지만 
	//지금 실습하는 css는 ip주소가1개이므로 hp->h_addr_list[0]으로 해서 
	//그냥 1개만 받도록 설정

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, buf2, &servaddr.sin_addr);
//여기다가 buf2로 도메인에서 ip로 바꾼 주소를 넣음
	servaddr.sin_port = htons(atoi(argv[2])); 


	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}



	printf("입력 :");

	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		exit(0);
	}
	nbyte = strlen(buf);
	if (write(s, buf, nbyte) < 0) {
		printf("write error\n");
		exit(0);
	}

	printf("수신 :");
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}


	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);


	return 0;
}