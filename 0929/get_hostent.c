#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

int main(int argc,char *argv[]){
	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf[20];

	if(argc<2){
		printf("Usage : %s hostname \n",argv[0]);
		exit(1);
	}

	hp=gethostbyname(argv[1]);
	if(hp==NULL){
		printf("gethostbyname fail\n");
		exit(0);
	}

	printf("ȣ��Ʈ �̸� : %s\n",hp->h_name);
	printf("ȣ��Ʈ �ּ�Ÿ�� ��ȣ : %d\n",hp->h_addrtype);
	printf("ȣ��Ʈ �ּ��� ���� : %d ����Ʈ \n",hp->h_length);
	

//�Ƹ� �߰��� ������ hp->h_addr_list[i]�� 
//hp->h_addr_list[i]�� ���� ���� ������ �� �ǹ��ϴ� ��
	for(i=0; hp->h_addr_list[i]; i++){
		memcpy(&in.s_addr,hp->h_addr_list[i],sizeof(in.s_addr));
		inet_ntop(AF_INET,&in.s_addr,buf,sizeof(buf));
		printf("IP�ּ�(%d��°) : %s\n",i+1,buf);
	}

	for(i=0;hp->h_aliases[i]; i++){
		printf("ȣ��Ʈ ����(%d����) : %s",i+1,hp->h_aliases[i]);
	}
	puts("");
	return 0;
}