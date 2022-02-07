#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[MAXLINE + 1];

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));

//�����ϰ��� �ϴ� ������ �����ּ� ����ü ����
	servaddr.sin_family = AF_INET;
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));
	printf("�Է� :");//�Ʒ� stdin������ �׳� �Է��ص� fgets�� ��
	if (fgets(buf, MAXLINE, stdin) == NULL) {
		printf("fgets ����");
		exit(0);
	}//�̰� ���� ���ϰ��� �ϴ� �޼����� buf�� ��Ƶ�

																				//����ȯ�� p64�� ����
	if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");                 //������ �����ּұ���ü
		exit(0);
	}


	if ((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(0);
	}



	buf[nbyte] = 0;
	printf("%s\n", buf);
	close(s);


	return 0;
}