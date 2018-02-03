#include "NetworkDeclare.h"

#define SERVERPORT 9000
#define BUFSIZE    512

void InitServerData() {

}

int main() 
{
	int retval;


	// ���� �ʱ�ȭ
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


	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	char recv_buf[sizeof(CtsPacket)] = { 0 };
	CtsPacket packet_buffer;
	// ���� �Ǻ���
	while (true) {
		addrlen = sizeof(clientaddr);
		cout << "Accept �����" << endl;
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		cout << "Accept �Ϸ�" << endl;
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�(���� ����)
			cout << "������ �ޱ� ���" << endl;
			retval = recvn(client_sock, (char *)&recv_buf, sizeof(CtsPacket), 0);

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			memcpy(&packet_buffer, recv_buf, sizeof(recv_buf));

			printf("%x \n", packet_buffer.keyboard_click);
			//// ������ �ޱ�(���� ����)
			//cout << "������ �ޱ� ���" << endl;
			//retval = recvn(client_sock, (char *)&len, sizeof(int), 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("recv()");
			//	break;
			//}
			//else if (retval == 0)
			//	break;

			//// ������ �ޱ�(���� ����)
			//retval = recvn(client_sock, buf, len, 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("recv()");
			//	break;
			//}
			//else if (retval == 0)
			//	break;

			//// ���� ������ ���
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