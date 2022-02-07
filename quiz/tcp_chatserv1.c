// tcp_chatserv.c는 내가 보낸 메세지가 다시 나한테 오곤 했었다. 
// 이걸 loop back이라고 한다. 
// 우선 이걸 제거해서 내가 보낸 메세지는 나한테 다시 오지 않도록 한다.

// 서버의 55행의 getmax()를 매번 부를 필요 없이 
// 꼭 필요할 때에만 부를 수 있도록 코드를 수정.
// 55행에 있는 것이 아니라 여러군데에 있어야 한다. 
// 클라가 나갈때 , 들어올때 변화가 생기니까.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit";
char *START_STRING = "Connected to chat_server \n";

int maxfdp1;
int num_chat = 0;
int clisock_list[MAX_SOCK];
int listen_sock;

void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeClient(int s);
int tcp_listen(int host, int port, int backlog);
void errquit(char *mesg) { perror(mesg); exit(1); }

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	char buf[MAXLINE + 1];
	int i, j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
	fd_set read_fds;

	if (argc != 2) {
		printf("사용법 : %s port\n", argv[0]);
		exit(0);
	}

	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
	maxfdp1 = getmax() + 1;

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		for (i = 0; i < num_chat; i++)
			FD_SET(clisock_list[i], &read_fds);
		//maxfdp1 = getmax() + 1;
		puts("wait for client");
		if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0)
			errquit("select fail");



		if (FD_ISSET(listen_sock, &read_fds)) {
 //listen_sock에서 변화가 생긴지 확인
			accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
			if (accp_sock == -1)
				errquit("accept fail");
			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0);
			printf("%d번째 사용자 추가.\n", num_chat);
		}

		for (i = 0; i < num_chat; i++) { 	//기존에 있던 것에서 변화가 생긴지 확인
			if (FD_ISSET(clisock_list[i], &read_fds)) {
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
				if (nbyte <= 0) {
					removeClient(i);
					continue;
				}
				buf[nbyte] = 0;

				if (strstr(buf, EXIT_STRING) != NULL) {
					removeClient(i);
					continue;
				}

				for (j = 0; j < num_chat; j++)
						if (j != i) { //j를 통해 모든 클라들한테 보내는데, 지금 사용되는 
							    //i번째 클라(변화가 감지된 클라 에게는 보내지 않음
								//그러면 해당 클라는 recv를 하지 않아서 
								//보낸사람은 다시 메세지를 받지 않는다
						send(clisock_list[j], buf, nbyte, 0);
					}
				printf("%s\n", buf);
			}
		}
	}  //end of while
	return 0;
}

void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client:%s\n", buf);
	clisock_list[num_chat] = s;
	num_chat++;
	maxfdp1 = getmax() + 1;  // 새로 추가가 되면 maxfpd1을 새로고침. 
//반드시 num_chat까지 추가가 되고
//난 다음에 해야함. 최댓값 구하는데 num_chat을 사용하기 때문
}

void removeClient(int s) {
	close(clisock_list[s]);
	if (s != num_chat - 1)
		clisock_list[s] = clisock_list[num_chat - 1];
	num_chat--;
	maxfdp1 = getmax() + 1;   // 삭제가 되면 maxfpd1을 새로고침
	printf("채팅 참가자1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}

int getmax() {
	int max = listen_sock;
	int i;

	for (i = 0; i < num_chat; i++)
		if (clisock_list[i] > max)
			max = clisock_list[i];
	return max;
}

int tcp_listen(int host, int port, int backlog) {
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket fail");
		exit(1);
	}

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);
	if (bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail"); exit(1);
	}

	listen(sd, backlog);
	return sd;


}