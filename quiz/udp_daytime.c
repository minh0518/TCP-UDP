//������ �ǽ��ߴ� mydaytime�� tcp���¿��� udp���·� �ٲٴ� ��
//���� ������ ip�ּҸ� �޴� ���� �ƴ϶�
//������ �ּҸ� �޾Ƽ� ��ȯ�ؼ� ����ؾ� �Ѵ�

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
//������� �߰��ϴ°� �ʼ�!


#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[MAXLINE + 1];

	//������ �ּҸ� �޾Ƽ� dotted�� �ٲٱ� ����
//�Ʒ��� ������ �߰�
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf2[20];

	if (argc != 3) {
		printf("Usage:%s ip_address\n", argv[0]);
		exit(0);
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	hp = gethostbyname(argv[1]);// �̺κ� �߰�
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}// �̺κ� �߰�

 //��Ʈ �ϳ��� ����
	memcpy(&in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr));
	inet_ntop(AF_INET, &in.s_addr, buf2, sizeof(buf2));
	//������ ip�ּҰ� �������� ��츦 ����ؼ� for������ �������� 
	//���� �ǽ��ϴ� css�� ip�ּҰ�1���̹Ƿ� hp->h_addr_list[0]���� �ؼ� 
	//�׳� 1���� �޵��� ����



	bzero((char *)&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(buf2);
//����ٰ� buf2�� �����ο��� ip�� �ٲ� �ּҸ� ����
	servaddr.sin_port = htons(atoi(argv[2]));

//	printf("�Է� :");
//	if (fgets(buf, MAXLINE, stdin) == NULL) {
//		printf("fgets ����");
//		exit(0);
//	}  �Է� �κ��� ����� �ϹǷ� ���ʿ� 
//�Է����� �ʾƵ� �ٷ� sendto�� �����Բ� ��.
//�׷��� �Է¾��� dummy? buf�� �����鼭 �ش� ��Ʈ��ȣ�� ���� 


	if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");
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