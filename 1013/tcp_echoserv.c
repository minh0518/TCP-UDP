#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock,addrlen = sizeof(cliaddr),nbyte;
	char buf[MAXLINE + 1];


	if (argc != 2) {
		printf("usage:%s port\n", argv[0]);
		exit(0);
	}

	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}


	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

//���ε带 �ϴµ� ���� �� ������ ��Ʈ��ȣ�� 4025�� �����ߴٸ�
//�����û�� '�޴�' listen_sock���ٰ� 4025��Ʈ��ȣ�� 
//INADDR_ANY�� ���� IP�� ���ļ� ���� 
//Ŭ�������� ���߿� 111.11.... 4025 �̷��� IP�ּҿ� ��Ʈ��ȣ��
//�ְ� �Ǹ� ���� listen_sock ���Ͽ� ������ �Ǵ� ���̴�.
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}

	listen(listen_sock, 5);
	while (1) {
		puts("������ �����û�� ��ٸ�..");
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
		//cliaddr�� Ŭ���� servaddr_in ��� �� ���̺귯������ ������
//���� ������. �׳� cliaddr�� ������ �������� accept�Լ��� �ι�°
//���ڷ� �������ν� ��μ� �̰� Ŭ���� servaddr�� �Ǵ� ���̴�.
		if (accp_sock < 0) {
			perror("accept fail");
			exit(0);
		}

		puts("Ŭ���̾�Ʈ�� �����");
		nbyte = read(accp_sock, buf, MAXLINE);
		write(accp_sock, buf, nbyte);
		close(accp_sock);
	
		}
	close(listen_sock);
	return 0;
	}