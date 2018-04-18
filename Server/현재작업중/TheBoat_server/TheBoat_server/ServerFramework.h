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
<<<<<<< HEAD
<<<<<<< HEAD
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

=======
>>>>>>> [서버 프레임워크] : HeightMap Class 붙이기 성공. 이제 맵만 만들면 됨
=======
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

>>>>>>> [서버 프레임워크] : 키 push, down 에 따른 이동 방식 구현 [더미클라이언트] : 키 입력 중첩 안되도록 코드 수정
public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ��Ŷ ������ �����ؼ� �۽�
	void DisconnectPlayer(int cl_id);				// �÷��̾� ���� ����
	bool IsStartGame();

	// �� �Լ��� ElaspsedTime�� �����ϴ� ������ �Լ��̴�.
	void TimerFunc();
	// ElapsedTime�� �޾ƿͼ� ������Ʈ �ϴ� �Լ��̴�. 
<<<<<<< HEAD
	void Update(duration<float>& elapsed_time);
=======
	void Update(duration<double>& elapsed_time);
>>>>>>> [서버 프레임워크] : 키 push, down 에 따른 이동 방식 구현 [더미클라이언트] : 키 입력 중첩 안되도록 코드 수정
	ServerFramework();
	~ServerFramework();
};

