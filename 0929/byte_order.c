#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(){
        struct servent *servent;
					// �ڷ���      �����ͺ����̸�(servent���� �ִ� �ּҰ��� �� �� ����)
        servent = getservbyname("echo","udp");
        if(servent == NULL){
                printf("���� ������ ���� �� ����.\n\n");
                exit(0);
        }
        printf("UDP ���� ��Ʈ��ȣ(��ũ��ũ ����) : %d\n",servent->s_port);
        printf("UDP ���� ��Ʈ��ȣ(ȣ��Ʈ ����) : %d\n",ntohs(servent->s_port));

        return 0;
}