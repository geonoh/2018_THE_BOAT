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
	bool player_ready[4] = { 0 };		// Player_Ready 패킷 도착하면 해당 
										// Client_ID에 맞는 배열 true
										// 모두 true가 되면 게임 시작 함수 실행
	HeightMap* height_map;
<<<<<<< HEAD
<<<<<<< HEAD
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

=======
>>>>>>> [�꽌踰� �봽�젅�엫�썙�겕] : HeightMap Class 遺숈씠湲� �꽦怨�. �씠�젣 留듬쭔 留뚮뱾硫� �맖
=======
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

>>>>>>> [�꽌踰� �봽�젅�엫�썙�겕] : �궎 push, down �뿉 �뵲瑜� �씠�룞 諛⑹떇 援ы쁽 [�뜑誘명겢�씪�씠�뼵�듃] : �궎 �엯�젰 以묒꺽 �븞�릺�룄濡� 肄붾뱶 �닔�젙
public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// 패킷 수신후 정리해서 송신
	void DisconnectPlayer(int cl_id);				// 플레이어 접속 해지
	bool IsStartGame();

	// 이 함수는 ElaspsedTime을 측정하는 스레드 함수이다.
	void TimerFunc();
	// ElapsedTime을 받아와서 업데이트 하는 함수이다. 
<<<<<<< HEAD
	void Update(duration<float>& elapsed_time);
=======
	void Update(duration<double>& elapsed_time);
>>>>>>> [�꽌踰� �봽�젅�엫�썙�겕] : �궎 push, down �뿉 �뵲瑜� �씠�룞 諛⑹떇 援ы쁽 [�뜑誘명겢�씪�씠�뼵�듃] : �궎 �엯�젰 以묒꺽 �븞�릺�룄濡� 肄붾뱶 �닔�젙
	ServerFramework();
	~ServerFramework();
};

