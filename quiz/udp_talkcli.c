//1110에 했었던 talk 프로그램을 tcp가 아니라 udp로 바꾸는 것

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 511
char *EXIT_STRING = "exit";
int recv_and_print(int sd,struct sockaddr_in servaddr);
int input_and_send(int sd,struct sockaddr_in servaddr);
//tcp의 경우, 미리 연결을 소켓에다가 했으니까 소켓만 매개변수로
//받지만 udp는 연결도 미리 하지 않으므로 소켓구조체도 같이
//들고간다.연결안하고 그냥 소켓구조체에다 바로 소통하니까


int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s; 
//addrlen = sizeof(servaddr); 이것도 sendto/recvfrom에 쓰이는데 main에 이게 없으므로 지운다.
	pid_t pid;

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));

	//클라가 연결할 서버의 정보 입력
	servaddr.sin_family = AF_INET;

	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));


//여기서 fork추가. 어차피 클라는 서버를 알고 있기 때문에 여기서 fork생성
//바로 위에 보면 클라가 연결할 서버 정보를 argv[1],argv[2]를 통해 적어줬으니까!

//그래서 이렇게 각각 서버에 대한 정보(servaddr)를 가지고 들어간다.
	if ((pid = fork()) > 0) {
		input_and_send(s,servaddr); 
	}
	else if (pid == 0) {
		recv_and_print(s,servaddr);
	}

	
	close(s);


	return 0;
}


int input_and_send(int sd,struct sockaddr_in servaddr) { //매개변수도 소켓번호만 받지 않고 
					//연결하고자 하는 서버의 소켓구조체,구조체 길이도 같이 받는다
					//왜? tcp는 미리 연결하고 read/write를 여기서 하는데
					// udp는 미리 연결이 없고 sendto/recvfrom을 여기서 하면서 동시에
					//서버의 소켓구조체,구조체 길이를 사용하기 때문
					


	//nbyte와 buf모두 다 sendto같은 것들이 main에 있어서 main에다가 선언했었는데 
	//지금은 sendto같은 것들이 여기 있으므로 여기다 선언
	char buf[MAXLINE + 1];
	int nbyte,addrlen = sizeof(servaddr);
	//addrlen지우려 했는데recvfrom에서 &는 반드시 변수행태로넣어야 해서
				//작성. addrlen또한 main에있었지만 이것 또한 어차피 
				//보내고 받는 메소드에 쓰이는 것이므로 여기다 작성

	printf("입력 :");
	while (fgets(buf,sizeof(buf),stdin)!=NULL) {
		nbyte = strlen(buf);

		if (sendto(sd, buf, nbyte, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("sendto fail");
		exit(0);
	}
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye");
			close(sd);
			exit(0);
		}
	}
	return 0;
}


//그리고 buf와 nbyte 는 이전에는 하나만 두고 사용했는데 왜 이렇게 따로 두냐는 의문이 생길텐데
//당연히 이전에는 talk서비스가 아니라 일반 echo서비스여서 하나 주고 하나 받고 하는 것이었다.
//하나의 프로세스로 서로 한번씩 번갈아가며 주고받는 것이었는데 이건
//프로세스를 2개를 두고 하는 것이므로 각 프로세스마다 데이터를 전달하는데 쓰이는 buf와 nbyte가
//필요한 것이다. >> tcp talk서비스도 buf와 nbyte를 각각 두고 있음


int recv_and_print(int sd,struct sockaddr_in servaddr) {
	char buf[MAXLINE + 1];
	int nbyte,addrlen = sizeof(servaddr);

	while (1) {
						//여기도 마찬가지로 recvfrom을 여기에서 사용
if ((nbyte = recvfrom(sd, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;
		if (strstr(buf, EXIT_STRING) != NULL) {
			break;
		}
		printf("%s", buf);
	}
	return 0;
}