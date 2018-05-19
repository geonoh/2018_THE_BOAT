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
	CHeightMapImage* height_map;
	mutex client_lock;

	// Timer���� OverlappedExtensionSetd
	// 4�� �÷��̾� ��ġ ������Ʈ ����
	// 5�� �浹üũ����
	// 6�� �÷��̾� �Ѿ� ����
	// 7�� �Ѿ� ������Ʈ
	OverlappedExtensionSet ol_ex[8];

	Bullet bullets[4][MAX_BULLET_SIZE] = { 0 };
	mutex bullet_lock;
	// �÷��̾ �� ��° �Ѿ˱��� �߻��ߴ��� �����ϴ� ����
	int bullet_counter[4] = { 0 };


	// �÷��̾�� bullet �ð��� ������ �ִ�. 
	float bullet_times[4];

	class Comparison {
		bool reverse;
	public:
		Comparison() {

		}
		bool operator() (const ServerEvent lhs, const ServerEvent rhs) {
			return (lhs.start_time > rhs.start_time);
		}
	};

	// �ð��� ���õ� ����	
	int sender_time = 0;
	int previous_time = 0;
	int send_prev_time = 0;
	priority_queue < ServerEvent, vector<ServerEvent>,
		Comparison> timer_queue;
	// Ÿ�̸� �Լ� ������
	void AddTimer(int id, int type, unsigned int start_time);
	void AddTimer(int id, int type, unsigned int start_time, XMFLOAT3& look_vec);



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
	void TimerSend(int elapsed_time);
	// ElapsedTime�� �޾ƿͼ� ������Ʈ �ϴ� �Լ��̴�. 
	void Update(int elapsed_time);

	void CallPQCS(ServerEvent& process_event, OverlappedExtensionSet* process_ovex);
	void TimerThread();
	ServerFramework();
	~ServerFramework();
};

