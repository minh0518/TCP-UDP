//이전에 실습했던 mydaytime을 tcp형태에서 udp형태로 바꾸는 것
//또한 기존의 ip주소를 받는 것이 아니라
//도메인 주소를 받아서 변환해서 사용해야 한다

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

	//도메인 주소를 받아서 dotted로 바꾸기 위해
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

 //비트 하나씩 복사
	memcpy(&in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr));
	inet_ntop(AF_INET, &in.s_addr, buf2, sizeof(buf2));
	//원래는 ip주소가 여러개인 경우를 대비해서 for문으로 돌리지만 
	//지금 실습하는 css는 ip주소가1개이므로 hp->h_addr_list[0]으로 해서 
	//그냥 1개만 받도록 설정



	bzero((char *)&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(buf2);
//여기다가 buf2로 도메인에서 ip로 바꾼 주소를 넣음
	servaddr.sin_port = htons(atoi(argv[2]));

//	printf("입력 :");
//	if (fgets(buf, MAXLINE, stdin) == NULL) {
//		printf("fgets 실패");
//		exit(0);
//	}  입력 부분이 없어야 하므로 애초에 
//입력하지 않아도 바로 sendto를 보내게끔 함.
//그러면 입력없이 dummy? buf만 보내면서 해당 포트번호에 연결 


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