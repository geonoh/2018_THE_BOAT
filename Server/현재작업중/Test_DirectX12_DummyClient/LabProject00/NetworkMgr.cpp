#include "stdafx.h"
#include "NetworkMgr.h"


NetworkMgr::NetworkMgr()
{
	int retval = 0;

	client_to_server_packet = new CtsPacket;

	// WSA�� listen���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "WSA �ʱ�ȭ ����" << std::endl;
	}
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	// 
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	
	/* ������ Ŭ���̾�Ʈ �� ���� ����� �����̹Ƿ� listen ���� �� ���� ����Ѵ�. */

	int addrlen;
	int len;


	
}

void NetworkMgr::ConnectClient() {

}


NetworkMgr::~NetworkMgr()
{
}

void NetworkMgr::ReleaseUPandSend() {

}
void NetworkMgr::ReleaseDownandSend() {

}
void NetworkMgr::ReleaseRightandSend() {

}
void NetworkMgr::ReleaseLeftandSend() {

}
void NetworkMgr::PushUPandSend() {

}
void NetworkMgr::PushDownandSend() {

}
void NetworkMgr::PushRightandSend() {

}
void NetworkMgr::PushLeftandSend() {

}

