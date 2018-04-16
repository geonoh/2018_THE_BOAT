#pragma once

#define SERVER_IP	127.0.0.1
#define SERVER_PORT 4000
#define MAX_BUFFER_SIZE 4000
#define MAX_PACKET_SIZE 256
#define MAXIMUM_PLAYER	4
#define	WM_SOCKET				WM_USER + 1
#define CLIENT_BUF_SIZE	1024
 
// Server To Client
#define SC_ENTER_PLAYER		1
#define SC_POS				2
#define SC_REMOVE_PLAYER	3


// Client To Server
#define CS_KEY_PRESS_UP			1
#define CS_KEY_PRESS_DOWN		2
#define CS_KEY_PRESS_LEFT		3
#define CS_KEY_PRESS_RIGHT		4
#define CS_KEY_PRESS_SPACE		5
#define CS_KEY_PRESS_1			6
#define CS_KEY_PRESS_2			7
#define CS_KEY_PRESS_SHIFT		8

#define CS_KEY_RELEASE_UP			9
#define CS_KEY_RELEASE_DOWN			10
#define CS_KEY_RELEASE_LEFT			11
#define CS_KEY_RELEASE_RIGHT		12
#define CS_KEY_RELEASE_SPACE		13
#define CS_KEY_RELEASE_1			14
#define CS_KEY_RELEASE_2			15
#define CS_KEY_RELEASE_SHIFT		16


#define CS_LEFT_BUTTON_DOWN		17
#define CS_RIGHT_BUTTON_DOWN	18

#define CS_PLAYER_READY		100
#define CS_PLAYER_TEAM_SELECT	101



// MODE가 2:2의 경우 TEAM 1, 2
// MODE가 Melee의 경우 모두 사용
enum GameMode {
	TEAM_MODE, MELEE
};
enum Team {
	NON_TEAM = 0, TEAM_1, TEAM_2, TEAM_3, TEAM_4
};
enum ARWeapons {
	NON_AR = 0
};
enum SubWeapons {
	NON_SUB = 0
};

// Server To Client packet
struct SC_PACKET_ENTER_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
	bool player_in[4];
	bool player_ready[4];
};

struct SC_PACKET_POS {
	BYTE size;
	BYTE type;
	WORD id;
	int x, y, z;
};

// 포지션 관련된 패킷 하나 더 필요



// Client To Server 오직 키보드 입력 Ready
struct CS_PACKET_BIGGEST {
	BYTE size;
	BYTE type;
	WORD id;
	bool player_in[4];
};




struct CS_PACKET_KEYUP {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYDOWN {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYLEFT {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYRIGHT {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEY1 {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEY2 {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYSPACE {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYSHIFT {
	BYTE size;
	BYTE type;
};

// 마우스 입력 
struct CS_PACKET_MOUSEMOVE {
	BYTE size;
	BYTE type;
	// 여기에 룩벡터를 받아야함..?
};
struct CS_PACKET_LEFTBUTTON {
	BYTE size;
	BYTE type;
};

//
struct CS_PACKET_READY {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_MODE_SELECT {
	BYTE size;
	BYTE type;
	GameMode game_mode;	// false - Melee
						// true	- Team
};
struct CS_PACKET_TEAM_SELECT {
	BYTE size;
	BYTE type;
	Team team;
};

struct SC_PACKET_REMOVE_PLAYER {
	BYTE size;
	BYTE type;
	WORD client_id;
};