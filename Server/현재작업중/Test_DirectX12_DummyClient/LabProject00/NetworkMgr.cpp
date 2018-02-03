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
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
}

void NetworkMgr::ConnectClient() {
	int retval = 0;


	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//if (retval == SOCKET_ERROR) err_quit("connect()");


	int addrlen;
	int len;
}


NetworkMgr::~NetworkMgr()
{
	delete client_to_server_packet;
}

void NetworkMgr::ReleaseUPandSend() {
	int retval = 0;
	client_to_server_packet->ReleaseArrowKeyUp();

	retval = send(sock, (char*)client_to_server_packet, sizeof(*client_to_server_packet), 0);
	if (retval == SOCKET_ERROR) {
		std::cout << "�����°� ��������" << std::endl;
	}
}
void NetworkMgr::ReleaseDownandSend() {

}
void NetworkMgr::ReleaseRightandSend() {

}
void NetworkMgr::ReleaseLeftandSend() {

}
void NetworkMgr::PushUPandSend() {
	int retval = 0;
	client_to_server_packet->PushArrowKeyUp();

	retval = send(sock, (char*)client_to_server_packet, sizeof(*client_to_server_packet), 0);
	if (retval == SOCKET_ERROR) {
		std::cout << "�����°� ��������" << std::endl;
	}

}
void NetworkMgr::PushDownandSend() {

}
void NetworkMgr::PushRightandSend() {

}
void NetworkMgr::PushLeftandSend() {

}

