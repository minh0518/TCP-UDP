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

	if (argc != 2) {
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
//	servaddr.sin_port = htons(7); 
	servaddr.sin_port = htons(4000); 
//교수님이 포트번호를 4000으로 하라고 하셨는데 이것도 안된다는 애들
//있어서 3000으로 
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}

//여기까진 이전 코드와 똑같다 (exit숫자 ,포트번호 바뀐거 빼고)

	printf("입력 :");

	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		exit(0);
	}
	nbyte = strlen(buf); //버퍼의 크기가 아니라 버퍼의 길이만큼.

//서버에 write하는데 이제 connect되었으므로 해당 소켓번호에
//write합니다. buf에 있는 내용을 buf의 크기만큼 write합니다.
	if (write(s, buf, nbyte) < 0) {
		printf("write error\n");
		exit(0);
	}

	printf("수신 :");
//소켓번호 s로부터 다시 읽어옵니다. buf의 내용을 그냥
//죄다 끝까지 싹다 읽어오는 것입니다.
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
//read()는 읽어온 데이터의 크기를 바이트 단위로 리턴
		perror("read fail");
		exit(0);
	}


	buf[nbyte] = 0; //죄다 읽어왔으므로 내용의 끝에 0을 넣어서 EOF.
						//읽어온 데이터의 크기를 바이트 단위로 리턴받았으므로
					//그 크기만큼만 딱 가져오겠다는 의미이다.
	printf("%s", buf); 
	close(s);


	return 0;
}