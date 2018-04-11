#include "stdafx.h"
#include "ServerMgr.h"

void ServerMgr::ErrorDisplay(const char* msg, int err_no) {
	_wsetlocale(LC_ALL, L"korean");
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s", msg);
	wprintf(L"%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void ServerMgr::Initialize() {
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(sock, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(sock, async_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = CLIENT_BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = CLIENT_BUF_SIZE;
	cout << "server_mgr 초기화" << endl;

}

void ServerMgr::ReadPacket() {
	DWORD io_bytes, io_flag = 0;

	int retval = WSARecv(sock, &recv_wsabuf, 1, &io_bytes, &io_flag, NULL, NULL);
	if (retval == false) {
		int err_code = WSAGetLastError();
		ErrorDisplay("[WSARecv] : 에러 ", err_code);
	}
	char* ptr = reinterpret_cast<char*>(recv_buffer);

	while (io_bytes != 0) {
		if (in_packet_size == 0)
			in_packet_size = ptr[0];
		if (io_bytes + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + in_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_bytes -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_bytes);
			saved_packet_size += io_bytes;
			io_bytes = 0;
		}

	}
}
void ServerMgr::ProcessPacket(char* ptr) {
	static bool first_time = true;
	switch (ptr[1]) {
	case SC_ENTER_PLAYER: {
		SC_PACKET_ENTER_PLAYER * packets = reinterpret_cast<SC_PACKET_ENTER_PLAYER*>(ptr);
		// 최초 수신 했다는 정보를 받게 되면 my_client_id에 
		// 수신한 client_id 정보를 넣는다.
		if (first_set_id)
			my_client_id = packets->id;
		
		cout << "[SC_ENTER_PLAYER] : " << packets->id << "플레이어 입장" << endl;
		break; }
	case SC_POS: {
		SC_PACKET_POS* packets = reinterpret_cast<SC_PACKET_POS*>(ptr);

		cout << "[SC_PACKET_POS] : " << packets->id << "플레이어 이동" << endl;
		break; }
	}
}
void ServerMgr::SendPacket(int type) {
	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(send_buffer);
	packet_buffer->size = sizeof(packet_buffer);
	send_wsabuf.len = sizeof(packet_buffer);
	int retval = 0;
	DWORD iobytes;
	switch (type) {
	case CS_KEY_UP:
		packet_buffer->type = CS_KEY_UP;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_DOWN:
		packet_buffer->type = CS_KEY_DOWN;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_RIGHT:
		packet_buffer->type = CS_KEY_RIGHT;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_LEFT:
		packet_buffer->type = CS_KEY_LEFT;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;

	case CS_KEY_SHIFT:
		packet_buffer->type = CS_KEY_SHIFT;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_SPACE:
		packet_buffer->type = CS_KEY_SPACE;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_1:
		packet_buffer->type = CS_KEY_1;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_KEY_2:
		packet_buffer->type = CS_KEY_2;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;

	case CS_LEFT_BUTTON_DOWN:
		packet_buffer->type = CS_LEFT_BUTTON_DOWN;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	case CS_RIGHT_BUTTON_DOWN:
		packet_buffer->type = CS_RIGHT_BUTTON_DOWN;
		retval = WSASend(sock, &send_wsabuf, 1, &iobytes, 0, NULL, NULL);
		break;
	}
	if (retval == 1) {
		int error_code = WSAGetLastError();
		ErrorDisplay("[WSASend] 에러 : ", error_code);
	}
	
}


void ServerMgr::ClientError() {
	exit(-1);
}