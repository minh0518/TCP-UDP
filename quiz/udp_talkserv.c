#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 511

char *EXIT_STRING = "exit";
int recv_and_print(int sd,struct sockaddr_in cliaddr);
int input_and_send(int sd,struct sockaddr_in cliaddr);
//여기도 마찬가지로 서버가 소통을 하기 위한 클라의 소켓구조체를
//들고가야 한다.

int main(int argc, char *argv[]) {
    
	struct sockaddr_in servaddr, cliaddr;
	int s,addrlen= sizeof(struct sockaddr);

    //이전 updecho 코드를 보면, 클라에선 addrlen을 그냥 sizeof(servaddr)처럼 소켓구조체 
		//의 변수형으로 사용했었다.
    //여기에서 addrlen은 우선 구조체를 초기화하기 위해 여기서 사용되는데 
    //(물론 보내고 받는 메소드에도 쓰임)
    //udp는 자기 서버와 클라측 것 2개를 초기화 해줘야 한다. 그래서 각각 
    //sizeof(servaddr) , sizeof(cliaddr) 이렇게 해주지 않고 
    //변수형인 sizeof (struct sockaddr) 로 통일해서 사용한다. 그냥  귀찮아서 이렇게 편하게
    //사용한 것일뿐 추가적인 것은 없다.
    
    int nbyte;
	char buf[MAXLINE + 1];
//upd talk클라에선 이걸 지웠다. 하지만 서버에서는 먼저 fork하기전에 여기서
//recvfrom을 해야 하므로(이유는 노션에 적어둠) nbyte하고 buf를 남겨준다
	pid_t pid;


	if (argc != 2) {
		printf("usage:%s port\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	bzero((char *)&servaddr, addrlen);
	bzero((char *)&cliaddr, addrlen);

//클라가 연결할 서버정보 입력(자기자신)
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

//bind설명은 71페이지
	if (bind(s, (struct sockaddr *)&servaddr,addrlen) < 0) {
		perror("bind fail");
		exit(0);
	}

    //먼저 recvfrom을 받고 이걸 통해 cliaddr를 받음. 이제 이걸 가지고 fork를 해야 함
    puts("Server : waiting request");
    nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &addrlen);

		if (nbyte < 0) {
			perror("recvfrom fail");
			exit(0);
		}
	buf[nbyte] = 0;
    printf("%d byte recv: %s\n", nbyte, buf);
//이거 반드시 해줘야 함. 이거 안하면 연결은 되는데 첫번째 메세지를 못 출력하니까
    
    //클라 정보를 가지고 fork시작
    if ((pid = fork()) > 0) {
		input_and_send(s,cliaddr);  
	}
	else if (pid == 0) {
		recv_and_print(s,cliaddr);
	}


	return 0;
}



int input_and_send(int sd,struct sockaddr_in cliaddr) { 

    //main에 있던 buf와 nbyte는 어쩔 수 없이 클라의 정보를 한번 미리 받아놔야 하기 때문에
    //buf와 nbyte를 써야 해서 쓴거고
    //이제 여기서 다시 주고받아야하기 때문에 또 다시 선언
//근데 메인은 부모여서 여기에 또 적어야 하나?
	char buf[MAXLINE + 1];
	int nbyte,addrlen= sizeof(struct sockaddr);
//여기도 클라와 마찬가지로 addrlen를 어쩔수 없이 
//sizeof(servaddr)로 못 쓰고 변수형태로 작성(사실 recvfrom때문임)

	printf("입력 :");
	while (fgets(buf,sizeof(buf),stdin)!=NULL) {
		nbyte = strlen(buf);

		if (sendto(sd, buf, nbyte, 0, (struct sockaddr *)&cliaddr, addrlen) < 0) {
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



int recv_and_print(int sd,struct sockaddr_in cliaddr) {
	char buf[MAXLINE + 1];
	int nbyte,addrlen= sizeof(struct sockaddr);

	while (1) {
					
		if ((nbyte = recvfrom(sd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
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