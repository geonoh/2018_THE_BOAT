#include "NetworkDeclare.h"

#define SERVERPORT 9000
#define BUFSIZE    512

void InitServerData() {

}

int main() 
{
	int retval;


	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	char recv_buf[sizeof(CtsPacket)] = { 0 };
	CtsPacket packet_buffer;
	// 연결 되보림
	while (true) {
		addrlen = sizeof(clientaddr);
		cout << "Accept 대기중" << endl;
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		cout << "Accept 완료" << endl;
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기(고정 길이)
			cout << "데이터 받기 대기" << endl;
			retval = recvn(client_sock, (char *)&recv_buf, sizeof(CtsPacket), 0);

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			memcpy(&packet_buffer, recv_buf, sizeof(recv_buf));

			printf("%x \n", packet_buffer.keyboard_click);
			//// 데이터 받기(고정 길이)
			//cout << "데이터 받기 대기" << endl;
			//retval = recvn(client_sock, (char *)&len, sizeof(int), 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("recv()");
			//	break;
			//}
			//else if (retval == 0)
			//	break;

			//// 데이터 받기(가변 길이)
			//retval = recvn(client_sock, buf, len, 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("recv()");
			//	break;
			//}
			//else if (retval == 0)
			//	break;

			//// 받은 데이터 출력
			//buf[retval] = '\0';
			//printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			//	ntohs(clientaddr.sin_port), buf);
		}


	}


	CtsPacket test1;
	test1.keyboard_click = 0x00010101;
	test1.PushArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	test1.ReleaseArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	printf("%d\n", sizeof(test1));
}