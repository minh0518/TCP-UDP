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
//�̰� ������ϵ� get_hostent �κ��̶� ���� �ֶ�
//tcp_echocli1 �̶� �� ���ľ� ��

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf2[20];
	//������� get_hostent�κп� �ִ� ���� ���� �߰�
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE + 1];

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	hp = gethostbyname(argv[1]);// �̺κ� �߰�
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}// �̺κ� �߰�

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	memcpy(&in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr));
	inet_ntop(AF_INET, &in, buf2, sizeof(buf2));
	printf("Server IP : %s\n", buf2);
	//������ ip�ּҰ� �������� ��츦 ����ؼ� for������ �������� 
	//���� �ǽ��ϴ� css�� ip�ּҰ�1���̹Ƿ� hp->h_addr_list[0]���� �ؼ� 
	//�׳� 1���� �޵��� ����

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, buf2, &servaddr.sin_addr);
//����ٰ� buf2�� �����ο��� ip�� �ٲ� �ּҸ� ����
	servaddr.sin_port = htons(atoi(argv[2])); 


	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}



	printf("�Է� :");

	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		exit(0);
	}
	nbyte = strlen(buf);
	if (write(s, buf, nbyte) < 0) {
		printf("write error\n");
		exit(0);
	}

	printf("���� :");
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}


	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);


	return 0;
}