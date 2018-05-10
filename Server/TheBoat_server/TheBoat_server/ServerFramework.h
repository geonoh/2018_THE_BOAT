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
	bool player_ready[4] = { 0 };		// Player_Ready 패킷 도착하면 해당 
										// Client_ID에 맞는 배열 true
										// 모두 true가 되면 게임 시작 함수 실행
	CHeightMapImage* height_map2;
	time_point<system_clock> prev_time = system_clock::now();
	float sender_time = 0;
	mutex send_locker;

	// Timer전용 OverlappedExtensionSetd
	OverlappedExtensionSet ol_ex[4];


public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// 패킷 수신후 정리해서 송신
	void DisconnectPlayer(int cl_id);				// 플레이어 접속 해지
	bool IsStartGame();

	// 이 함수는 ElaspsedTime을 측정하는 스레드 함수이다.
	//void TimerFunc();
	void TimerSend(duration<float>& elapsed_time);
	// ElapsedTime을 받아와서 업데이트 하는 함수이다. 
	void Update(duration<float>& elapsed_time);
	ServerFramework();
	~ServerFramework();
};

