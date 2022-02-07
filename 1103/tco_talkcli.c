#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>


char *EXIT_STRING = "exit";
int recv_and_print(int sd);
int input_and_send(int sd);

#define MAXLINE 511

int main(int argc, char *argv[]) {
	
    static struct sockaddr_in servaddr;
		pid_t pid;
    static int s;

	if (argc != 3) {
		printf("usage:%s port\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Client : Can't open stream socket.\n");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
  servaddr.sin_port=htons(atoi(argv[2]));

  if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}
   
   
	if ((pid = fork()) > 0) {
		input_and_send(s); 
	}
	else if (pid == 0) {
		recv_and_print(s);
	}

	close(s);
	return 0;
}


int input_and_send(int sd) {
	char buf[MAXLINE + 1];
	int nbyte;
	while (fgets(buf,sizeof(buf),stdin)!=NULL) {
		nbyte = strlen(buf);
		write(sd, buf, strlen(buf));

		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye");
			close(sd);
			exit(0);
		}
	}
	return 0;
}

int recv_and_print(int sd) {
	char buf[MAXLINE + 1];
	int nbyte;

	while (1) {
		if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
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