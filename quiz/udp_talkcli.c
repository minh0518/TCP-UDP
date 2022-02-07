//1110�� �߾��� talk ���α׷��� tcp�� �ƴ϶� udp�� �ٲٴ� ��

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
//tcp�� ���, �̸� ������ ���Ͽ��ٰ� �����ϱ� ���ϸ� �Ű�������
//������ udp�� ���ᵵ �̸� ���� �����Ƿ� ���ϱ���ü�� ����
//�����.������ϰ� �׳� ���ϱ���ü���� �ٷ� �����ϴϱ�


int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s; 
//addrlen = sizeof(servaddr); �̰͵� sendto/recvfrom�� ���̴µ� main�� �̰� �����Ƿ� �����.
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

	//Ŭ�� ������ ������ ���� �Է�
	servaddr.sin_family = AF_INET;

	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));


//���⼭ fork�߰�. ������ Ŭ��� ������ �˰� �ֱ� ������ ���⼭ fork����
//�ٷ� ���� ���� Ŭ�� ������ ���� ������ argv[1],argv[2]�� ���� ���������ϱ�!

//�׷��� �̷��� ���� ������ ���� ����(servaddr)�� ������ ����.
	if ((pid = fork()) > 0) {
		input_and_send(s,servaddr); 
	}
	else if (pid == 0) {
		recv_and_print(s,servaddr);
	}

	
	close(s);


	return 0;
}


int input_and_send(int sd,struct sockaddr_in servaddr) { //�Ű������� ���Ϲ�ȣ�� ���� �ʰ� 
					//�����ϰ��� �ϴ� ������ ���ϱ���ü,����ü ���̵� ���� �޴´�
					//��? tcp�� �̸� �����ϰ� read/write�� ���⼭ �ϴµ�
					// udp�� �̸� ������ ���� sendto/recvfrom�� ���⼭ �ϸ鼭 ���ÿ�
					//������ ���ϱ���ü,����ü ���̸� ����ϱ� ����
					


	//nbyte�� buf��� �� sendto���� �͵��� main�� �־ main���ٰ� �����߾��µ� 
	//������ sendto���� �͵��� ���� �����Ƿ� ����� ����
	char buf[MAXLINE + 1];
	int nbyte,addrlen = sizeof(servaddr);
	//addrlen����� �ߴµ�recvfrom���� &�� �ݵ�� �������·γ־�� �ؼ�
				//�ۼ�. addrlen���� main���־����� �̰� ���� ������ 
				//������ �޴� �޼ҵ忡 ���̴� ���̹Ƿ� ����� �ۼ�

	printf("�Է� :");
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


//�׸��� buf�� nbyte �� �������� �ϳ��� �ΰ� ����ߴµ� �� �̷��� ���� �γĴ� �ǹ��� �����ٵ�
//�翬�� �������� talk���񽺰� �ƴ϶� �Ϲ� echo���񽺿��� �ϳ� �ְ� �ϳ� �ް� �ϴ� ���̾���.
//�ϳ��� ���μ����� ���� �ѹ��� �����ư��� �ְ�޴� ���̾��µ� �̰�
//���μ����� 2���� �ΰ� �ϴ� ���̹Ƿ� �� ���μ������� �����͸� �����ϴµ� ���̴� buf�� nbyte��
//�ʿ��� ���̴�. >> tcp talk���񽺵� buf�� nbyte�� ���� �ΰ� ����


int recv_and_print(int sd,struct sockaddr_in servaddr) {
	char buf[MAXLINE + 1];
	int nbyte,addrlen = sizeof(servaddr);

	while (1) {
						//���⵵ ���������� recvfrom�� ���⿡�� ���
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