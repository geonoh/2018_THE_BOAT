#include "NetworkDeclare.h"

#define SERVERPORT 9000
#define BUFSIZE    512

void InitServerData() {

}

enum ErrorMsg {
	quit_fail = 1,
	what,
	dorae,
	kindof
};

int main() 
{
	ErrorMsg whatkindof;
	whatkindof = quit_fail;

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


	CtsPacket test1;
	test1.keyboard_click = 0x00010101;
	test1.PushArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	test1.ReleaseArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	printf("%d\n", sizeof(test1));
}