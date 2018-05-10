#pragma once
class CHeightMapImage;
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
	CHeightMapImage* height_map2;
	time_point<system_clock> prev_time = system_clock::now();
	float sender_time = 0;
	mutex send_locker;

	// Timer���� OverlappedExtensionSetd
	OverlappedExtensionSet ol_ex[4];


public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ��Ŷ ������ �����ؼ� �۽�
	void DisconnectPlayer(int cl_id);				// �÷��̾� ���� ����
	bool IsStartGame();

	// �� �Լ��� ElaspsedTime�� �����ϴ� ������ �Լ��̴�.
	//void TimerFunc();
	void TimerSend(duration<float>& elapsed_time);
	// ElapsedTime�� �޾ƿͼ� ������Ʈ �ϴ� �Լ��̴�. 
	void Update(duration<float>& elapsed_time);
	ServerFramework();
	~ServerFramework();
};

