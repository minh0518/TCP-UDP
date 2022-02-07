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
//���⵵ ���������� ������ ������ �ϱ� ���� Ŭ���� ���ϱ���ü��
//����� �Ѵ�.

int main(int argc, char *argv[]) {
    
	struct sockaddr_in servaddr, cliaddr;
	int s,addrlen= sizeof(struct sockaddr);

    //���� updecho �ڵ带 ����, Ŭ�󿡼� addrlen�� �׳� sizeof(servaddr)ó�� ���ϱ���ü 
		//�� ���������� ����߾���.
    //���⿡�� addrlen�� �켱 ����ü�� �ʱ�ȭ�ϱ� ���� ���⼭ ���Ǵµ� 
    //(���� ������ �޴� �޼ҵ忡�� ����)
    //udp�� �ڱ� ������ Ŭ���� �� 2���� �ʱ�ȭ ����� �Ѵ�. �׷��� ���� 
    //sizeof(servaddr) , sizeof(cliaddr) �̷��� ������ �ʰ� 
    //�������� sizeof (struct sockaddr) �� �����ؼ� ����Ѵ�. �׳�  �����Ƽ� �̷��� ���ϰ�
    //����� ���ϻ� �߰����� ���� ����.
    
    int nbyte;
	char buf[MAXLINE + 1];
//upd talkŬ�󿡼� �̰� ������. ������ ���������� ���� fork�ϱ����� ���⼭
//recvfrom�� �ؾ� �ϹǷ�(������ ��ǿ� �����) nbyte�ϰ� buf�� �����ش�
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

//Ŭ�� ������ �������� �Է�(�ڱ��ڽ�)
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

//bind������ 71������
	if (bind(s, (struct sockaddr *)&servaddr,addrlen) < 0) {
		perror("bind fail");
		exit(0);
	}

    //���� recvfrom�� �ް� �̰� ���� cliaddr�� ����. ���� �̰� ������ fork�� �ؾ� ��
    puts("Server : waiting request");
    nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &addrlen);

		if (nbyte < 0) {
			perror("recvfrom fail");
			exit(0);
		}
	buf[nbyte] = 0;
    printf("%d byte recv: %s\n", nbyte, buf);
//�̰� �ݵ�� ����� ��. �̰� ���ϸ� ������ �Ǵµ� ù��° �޼����� �� ����ϴϱ�
    
    //Ŭ�� ������ ������ fork����
    if ((pid = fork()) > 0) {
		input_and_send(s,cliaddr);  
	}
	else if (pid == 0) {
		recv_and_print(s,cliaddr);
	}


	return 0;
}



int input_and_send(int sd,struct sockaddr_in cliaddr) { 

    //main�� �ִ� buf�� nbyte�� ��¿ �� ���� Ŭ���� ������ �ѹ� �̸� �޾Ƴ��� �ϱ� ������
    //buf�� nbyte�� ��� �ؼ� ���Ű�
    //���� ���⼭ �ٽ� �ְ�޾ƾ��ϱ� ������ �� �ٽ� ����
//�ٵ� ������ �θ𿩼� ���⿡ �� ����� �ϳ�?
	char buf[MAXLINE + 1];
	int nbyte,addrlen= sizeof(struct sockaddr);
//���⵵ Ŭ��� ���������� addrlen�� ��¿�� ���� 
//sizeof(servaddr)�� �� ���� �������·� �ۼ�(��� recvfrom������)

	printf("�Է� :");
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