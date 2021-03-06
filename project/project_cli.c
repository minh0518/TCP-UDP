#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/file.h>
#include <errno.h>
#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit";
char *START_STRING = "Connected to chat_server \n";
char *FIRST="당신이 선공입니다 반드시 먼저 입력하세요.\n";
char *DRAW="비겼습니다\n";
char *WIN="당신이 이겼습니다\n";
char *LOSE="당신이 졌습니다\n";
char *KNOW="상대방의 패를 알려드리겠습니다\n";
char *RULE="1. 가위 2. 바위 3. 보\n";
char *RETRY="x";
char *END="o";
int num_chat = 0;
int clisock_list[MAX_SOCK];
int listen_sock;

void addClient(int s, struct sockaddr_in *newcliaddr);

void removeClient(int s);

int set_nonblock(int sockfd);
int is_nonblock(int sockfd);

int tcp_listen(int host, int port, int backlog);
void errquit(char *mesg) { perror(mesg); exit(1); }

int judgement(char one[],char two[]);


int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr; 
	char buf[MAXLINE];
    char one[MAXLINE];
    char two[MAXLINE];
    char choose[MAXLINE];
	int i, j, nbyte, count;
	int accp_sock, clilen;
    int times=0;

	if (argc != 2) {
		printf("사용법 : %s port\n", argv[0]);
		exit(0);
	}

	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 2);
    

	if (listen_sock == -1)
		errquit("tcp_listen fail");
	if (set_nonblock(listen_sock) == -1)
		errquit("set_nonblock fail");

	for (count = 0; ; count++) {
	
		clilen = sizeof(cliaddr);
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &clilen);
		if (accp_sock == -1 && errno != EWOULDBLOCK)
			errquit("accept fail");
		else if (accp_sock > 0) {
			if (is_nonblock(accp_sock) != 0 && set_nonblock(accp_sock) < 0) {
				errquit("set_nonblock fail");
			}
			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0);
			send(accp_sock, RULE, strlen(RULE), 0);
			if(num_chat==1){
				send(accp_sock, FIRST, strlen(FIRST), 0);
			}
			printf("%d번째 사용자 추가.\n", num_chat);
		}

        for (i = 0; i < num_chat; i++) { 
           
			errno = 0;
			nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
			if (nbyte == 0) {
				removeClient(i);
				continue;
			}
			else if (nbyte == -1 && errno == EWOULDBLOCK)
				continue;
			if (strstr(buf, EXIT_STRING) != NULL) {
				removeClient(i);
                continue;
			}

           	buf[nbyte] = 0;

            if(times==0){
				strcpy(one,buf);
            }

            else if(times==1){
                strcpy(two,buf);
            }

          
            printf("%s\n", buf);
		
            times++; 

            if(times==2){ 
            
                  send(clisock_list[0], KNOW, strlen(KNOW), 0);
				send(clisock_list[0], two, MAXLINE, 0);

                  send(clisock_list[1], KNOW, strlen(KNOW), 0);
				send(clisock_list[1], one, MAXLINE, 0);

                if(judgement(one,two)==0){
                    for (j = 0; j < num_chat; j++) {
                    send(clisock_list[j], DRAW, strlen(DRAW), 0);
                    }
                }       
                else if(judgement(one,two)==1){
                    send(clisock_list[0], WIN, strlen(WIN), 0);
                    send(clisock_list[1], LOSE, strlen(LOSE), 0);
                }

			printf("다시 도전하시겠습니까? (o , x) ");
            if(fgets(choose, MAXLINE, stdin)){

			
			if (strstr(choose,RETRY) != NULL) {
				puts("Good bye.");
				close(accp_sock);
				close(listen_sock);
				exit(0);
           }
         }
			}
		}
   
        }
	return 0;
}

int judgement(char one[],char two[]){
    int first=atoi(one);
    int second=atoi(two);

    if(first==second){
        return 0;
    }

    else if(
        (first==1 && second==3)||
        (first==2 && second==1)||
        (first==3 && second==2)
    ){
        return 1;
    }

    else{
        return 2;
    }
    
}


void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client:%s\n", buf);
	clisock_list[num_chat] = s;
	num_chat++;
}

void removeClient(int s) {
	close(clisock_list[s]);
	if (s != num_chat - 1)
		clisock_list[s] = clisock_list[num_chat - 1];
	num_chat--;
	printf("채팅 참가자1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}


int is_nonblock(int sockfd) {
	int val;

	val = fcntl(sockfd, F_GETFL, 0);
	if (val & O_NONBLOCK)
		return 0;

	return -1;
}


int set_nonblock(int sockfd) {
	int val;
	val = fcntl(sockfd, F_GETFL, 0);
	if (fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
		return -1;
	return 0;
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