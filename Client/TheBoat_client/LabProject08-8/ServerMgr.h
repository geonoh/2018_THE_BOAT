#pragma once
class ServerMgr
{
	WSADATA wsa;
	SOCKET sock;
	SOCKADDR_IN server_addr;
	HWND async_handle;
	WSABUF send_wsabuf;
	WSABUF recv_wsabuf;
	int clients_id = 0;

	Bullet bullets[MAX_BULLET_SIZE] = { 0 };
	int recvd_bullet_id = 0;

	bool first_set_id = true;

	char send_buffer[CLIENT_BUF_SIZE] = { 0 };
	//char send_buffer_vector[CLIENT_BUF_SIZE] = { 0 };
	char recv_buffer[CLIENT_BUF_SIZE] = { 0 };

	char packet_buffer[CLIENT_BUF_SIZE] = { 0 };
	DWORD in_packet_size = 0;
	DWORD saved_packet_size = 0;

	XMFLOAT3 sc_vec_buff[4];
	XMFLOAT3 sc_look_vec;

	XMFLOAT3 collision_pos;
	float client_hp[4] = { 0 };
	int camera_id = 0;
	string server_ip;

	// 아이템 생성 부분
	XMFLOAT3 item_pos;
	bool is_item_gen;

	XMFLOAT3 building_pos[OBJECT_BUILDING];

public:
	void IPInput();
	void Initialize(HWND& hwnd);
	void ClientError();
	void ReadPacket();
	void SendPacket(int type);
	void SendPacket(int type, XMFLOAT3& xmvector);
	void ProcessPacket(char* ptr);
	void ErrorDisplay(const char* msg, int err_no);
	int GetClientID();
	int ReturnCameraID();
	Bullet GetBullet();
	XMFLOAT3 ReturnXMFLOAT3(int client_id);
	XMFLOAT3 ReturnLookVector();
	XMFLOAT3 ReturnCollsionPosition();
	// 아이템 생성 후 위치 Return
	bool IsItemGen();
	XMFLOAT3 ReturnItemPosition();

	// 플레이어 체력
	float GetPlayerHP(int p_n);

	// 
	void ReturnBuildingPosition(XMFLOAT3* building_pos);
};