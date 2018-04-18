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
	bool player_ready[4] = { 0 };		// Player_Ready ÆĞÅ¶ µµÂøÇÏ¸é ÇØ´ç 
										// Client_ID¿¡ ¸Â´Â ¹è¿­ true
										// ¸ğµÎ true°¡ µÇ¸é °ÔÀÓ ½ÃÀÛ ÇÔ¼ö ½ÇÇà
	HeightMap* height_map;
<<<<<<< HEAD
<<<<<<< HEAD
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

=======
>>>>>>> [ì„œë²„ í”„ë ˆì„ì›Œí¬] : HeightMap Class ë¶™ì´ê¸° ì„±ê³µ. ì´ì œ ë§µë§Œ ë§Œë“¤ë©´ ë¨
=======
	time_point<system_clock> start_time, end_time;
	time_point<system_clock> prev_time = system_clock::now();

>>>>>>> [ì„œë²„ í”„ë ˆì„ì›Œí¬] : í‚¤ push, down ì— ë”°ë¥¸ ì´ë™ ë°©ì‹ êµ¬í˜„ [ë”ë¯¸í´ë¼ì´ì–¸íŠ¸] : í‚¤ ì…ë ¥ ì¤‘ì²© ì•ˆë˜ë„ë¡ ì½”ë“œ ìˆ˜ì •
public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ÆĞÅ¶ ¼ö½ÅÈÄ Á¤¸®ÇØ¼­ ¼Û½Å
	void DisconnectPlayer(int cl_id);				// ÇÃ·¹ÀÌ¾î Á¢¼Ó ÇØÁö
	bool IsStartGame();

	// ÀÌ ÇÔ¼ö´Â ElaspsedTimeÀ» ÃøÁ¤ÇÏ´Â ½º·¹µå ÇÔ¼öÀÌ´Ù.
	void TimerFunc();
	// ElapsedTimeÀ» ¹Ş¾Æ¿Í¼­ ¾÷µ¥ÀÌÆ® ÇÏ´Â ÇÔ¼öÀÌ´Ù. 
<<<<<<< HEAD
	void Update(duration<float>& elapsed_time);
=======
	void Update(duration<double>& elapsed_time);
>>>>>>> [ì„œë²„ í”„ë ˆì„ì›Œí¬] : í‚¤ push, down ì— ë”°ë¥¸ ì´ë™ ë°©ì‹ êµ¬í˜„ [ë”ë¯¸í´ë¼ì´ì–¸íŠ¸] : í‚¤ ì…ë ¥ ì¤‘ì²© ì•ˆë˜ë„ë¡ ì½”ë“œ ìˆ˜ì •
	ServerFramework();
	~ServerFramework();
};

