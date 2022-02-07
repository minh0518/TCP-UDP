#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE + 1];

	if (argc != 3) {//이것도 포트번호까지 받았으므로 길이가 늘어났으므로 2에서 3으로
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	servaddr.sin_port = htons(atoi(argv[2]));
	//포트번호를 ip 다음으로 입력받은 값으로 사용
	//바로 위의 pton함수는 함수 원형이 ip값이라도 문자열로 받는 것이었지만
	//이건 정수로 된 포트번호를 넣어줘야 하기 때문에 형변환 필수

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