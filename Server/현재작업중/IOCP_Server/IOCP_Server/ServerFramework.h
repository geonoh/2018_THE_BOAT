#pragma once
#include "stdafx.h"

class ServerFramework
{
private:
	CtsPacket * cts_packet;
	StcPacket* stc_packet;
	WSADATA wsa;


public:
	ServerFramework();
	~ServerFramework();

	// Windows Socket Create
	int CreateWSA();

	// Connection 
	int Initialize();

	// WorkerThread for networking with client.
	DWORD WINAPI WorkerThread(LPVOID arg);
};

