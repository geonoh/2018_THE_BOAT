#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdafx.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    50

using namespace std;

CtsPacket test_packet = { 0 };

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void InputKey(int input_number) {
	switch (input_number) {
	case 1:
		test_packet.PushArrowKeyUp();
		break;
	case 2:
		test_packet.PushArrowKeyDown();
		break;
	case 3:
		test_packet.PushArrowKeyRight();
		break;
	case 4:
		test_packet.PushArrowKeyLeft();
		break;
	case 5:
		test_packet.PushArrowKeyShift();
		break;
	case 6:
		test_packet.PushArrowKeySpaceBar();
		break;
	case 7:
		test_packet.PushArrowKeyOne();
		break;
	case 8:
		test_packet.PushArrowKeyTwo();
		break;
	case 9:
		test_packet.ReleaseArrowKeyUp();
		break;
	case 10:
		test_packet.ReleaseArrowKeyDown();
		break;
	case 11:
		test_packet.ReleaseArrowKeyRight();
		break;
	case 12:
		test_packet.ReleaseArrowKeyLeft();
		break;
	case 13:
		test_packet.ReleaseArrowKeyShift();
		break;
	case 14:
		test_packet.ReleaseArrowKeySpaceBar();
		break;
	case 15:
		test_packet.ReleaseArrowKeyOne();
		break;
	case 16:
		test_packet.ReleaseArrowKeyTwo();
		break;
	}

}

void PrintMenu() {
	printf("1. UP \n");
	printf("2. Down \n");
	printf("3. RIGHT \n");
	printf("4. Left \n");
	printf("5. Shift \n");
	printf("6. SpaceBar\n");
	printf("7. One \n");
	printf("8. Two \n\n");
	printf("9. 놓기_Up \n");
	printf("10. 놓기_Down \n");
	printf("11. 놓기_Up \n");
	printf("12. 놓기_Right \n");
	printf("13. 놓기_Left \n");
	printf("14. 놓기_Shift \n");
	printf("15. 놓기_SpaceBar \n");
	printf("16. 놓기_One \n");
	printf("17. 놓기_Two \n");


}
int main() {
	int retval;
	int input_number = 0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	while (true) {
		scanf("%d", &input_number);
		InputKey(input_number);
		
		printf("%x \n", test_packet.keyboard_click);

		// test_packet 전송

		retval = send(sock, (char*)&test_packet, sizeof(test_packet), 0);
		if (retval == SOCKET_ERROR) {
			return 0;
		}

	}
	return 0;
}