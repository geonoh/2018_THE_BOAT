#pragma once
class HeightMap;
class ServerFramework
{
	WSADATA wsa;
	HANDLE iocp_handle;
	SOCKET listen_socket;
	SOCKADDR_IN server_addr;

	BOOL mode_selector;	// 

	Client clients[MAXIMUM_PLAYER];
	bool player_entered[4] = { 0 };
	bool player_ready[4] = { 0 };		// Player_Ready ��Ŷ �����ϸ� �ش� 
										// Client_ID�� �´� �迭 true
										// ��� true�� �Ǹ� ���� ���� �Լ� ����
	HeightMap* height_map;
public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ��Ŷ ������ �����ؼ� �۽�
	void DisconnectPlayer(int cl_id);				// �÷��̾� ���� ����
	bool IsStartGame();
	ServerFramework();
	~ServerFramework();
};

