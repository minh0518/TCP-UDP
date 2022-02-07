//udp_echocli.c에서 도메인주소를 받아서 dotted ip로 넘겨주게 수정


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
//헤더파일 추가하는거 필수!

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[MAXLINE + 1];

	//아래의 변수들 추가
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf2[20];

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	hp = gethostbyname(argv[1]);// 이부분 추가
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}// 이부분 추가



	memcpy(&in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr));
	inet_ntop(AF_INET, &in.s_addr, buf2, sizeof(buf2));
	//원래는 ip주소가 여러개인 경우를 대비해서 for문으로 돌리지만 
	//지금 실습하는 css는 ip주소가1개이므로 hp->h_addr_list[0]으로 해서 
	//그냥 1개만 받도록 설정


	bzero((char *)&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(buf2);
//여기다가 buf2로 도메인에서 ip로 바꾼 주소를 넣음

	servaddr.sin_port = htons(atoi(argv[2]));
	printf("입력 :");
	if (fgets(buf, MAXLINE, stdin) == NULL) {
		printf("fgets 실패");
		exit(0);
	}//"입력 : " 다음에 값을 입력해서 buf에 값을 넣고 그 buf를 
// sendto로 보내준다 . sendto를 보내면서 연결이 되는 것인데 
// 보내면서 그냥 보내는 값을 만들기 위해 여기서 입력받는 것 뿐이다.

	if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");
		exit(0);
	}


	if ((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(0);
	}



	buf[nbyte] = 0;
	printf("%s\n", buf);
	close(s);


	return 0;
}