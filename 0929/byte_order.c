#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(){
        struct servent *servent;
					// 자료형      포인터변수이름(servent형이 있는 주소값만 들어갈 수 있음)
        servent = getservbyname("echo","udp");
        if(servent == NULL){
                printf("서비스 정보를 얻을 수 없음.\n\n");
                exit(0);
        }
        printf("UDP 에코 포트번호(네크워크 순서) : %d\n",servent->s_port);
        printf("UDP 에코 포트번호(호스트 순서) : %d\n",ntohs(servent->s_port));

        return 0;
}