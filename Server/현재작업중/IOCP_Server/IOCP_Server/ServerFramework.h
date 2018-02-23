#pragma once
#include "stdafx.h"

class ServerFramework
{
private:
	CtsPacket * cts_packet;
	StcPacket* stc_packet;
	WSADATA wsa;

	SOCKET listen_socket;
	SOCKADDR_IN server_address;
	HANDLE hcp;		// Handle for IOCP

public:
	ServerFramework();
	~ServerFramework();

	// Windows Socket Create
	int CreateWSA();

	// Connection 
	int Initialize();

	// WorkerThread for networking with client.
	// Warning! should be declared by static.
	static DWORD WINAPI WorkerThread(LPVOID arg);

	// Connect IOCP from client_socket_buffer (client number added)
	int AcceptClient(UINT client_number);
	
	// collide check and generate boat module
	int Update();
};

