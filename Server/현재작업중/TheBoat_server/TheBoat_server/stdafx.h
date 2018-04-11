// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
#include <thread>
#include "protocol.h"

using namespace std;
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

struct OverlappedExtensionSet {
	WSAOVERLAPPED wsa_over;
	bool is_recv;
	WSABUF wsabuf;
	char io_buffer[MAX_BUFFER_SIZE];
};

struct Client {
	SOCKET s;
	bool in_use;

	int x, y, z;			// 1km * 1km
	bool is_ready;
	Team team;
	ARWeapons ar_weapons;
	char ar_mag = 0;		// 탄창
	SubWeapons sub_weapons;
	char sub_mag = 0;
	// Look Vector
	bool is_running;		// true : 달리기, false : 걷기
	bool boat_parts[4];		// 0 : 하판, 1 : 상판, 2 : 엔진, 3 : 조향장치

	OverlappedExtensionSet overlapped_ex;
	int packet_size;
	int prev_packet_size;
	char prev_packet[MAX_PACKET_SIZE];

};

