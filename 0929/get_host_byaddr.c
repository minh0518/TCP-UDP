#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


int main(int argc,char *argv[]){

	struct hostent *myhost;
	struct in_addr in;
	
	if(argc<2){
		printf("���� : %s ip_address \n", argv[0]);
		exit(0);
	}

	inet_pton(AF_INET,argv[1],&in.s_addr);
	
	myhost=gethostbyaddr((char *)&(in.s_addr),sizeof(in.s_addr),AF_INET);
//	gethostbyaddr�� ù��° �μ��� �����̹Ƿ� in.s_addr��
// ����ȯ ����


	if(myhost==NULL){
		printf("Error at gethoustbyaddr() \n");
		exit(0);
	}

	printf("ȣ��Ʈ �̸� : %s\n",myhost->h_name);
	return 0;
}