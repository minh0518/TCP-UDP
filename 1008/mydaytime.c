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
		exit(1);
	}


//이전까진 sockaddr_in을 제대로 사용하지도 않았고
//그리고 여기서부턴 연결을 위해 연결하고자 하는 sockaddr_in을 
//제대로 설정해줘야 하므로 sockaddr_in에 값을 직접 넣어준다.
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
//내가 연결하고자 하는(=입력한) ip주소를 servaddr에 저장
	servaddr.sin_port = htons(13);
//daytime의 포트번호를 서버의 포트번호에 넣어 줌으로써 해당
//servaddr로 connect하면 소켓과 포트번호가 서로 연결(맞나?)이 되어서
//해당 응용플그램 기능을 수행하는 것

	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);
	}

	if ((nbyte = read(s, buf, MAXLINE))< 0){
		perror("read fail");
		exit(1);
	}

	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);


	return 0;
}

//원래 포트번호는 해당 소켓을 찾아가기 위한 것이라 배웠는데
//그건 연결이 된 상태에서 해당되는 말이고
//이처럼 처음 연결할 때는 servaddr와 소켓을 이렇게 연결하고
//연결이 되고 나서야 해당 포트번호에 연결된 해당 소켓이 이렇게
//포트번호에게 주어진 응용프로그램을 수행하는 것 같다.

//몰라 그냥 포트번호가 응용프로그램을 구분한다고 알아도 될거 같다.
//책에는 중복의 우려가 있다고 하는데 모르겠다.(p39)
//소켓은 그 와중에 응용프로그램을 tcp가 찾아가기 위해서 꼭 필요