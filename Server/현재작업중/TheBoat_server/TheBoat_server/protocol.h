#pragma once

#define SERVER_PORT 9000
#define MAX_BUFFER_SIZE 4000
#define MAX_PACKET_SIZE 256
#define MAXIMUM_PLAYER	4

// Server To Client
#define SC_POS				1
#define SC_ENTER_PLAYER		2


// Client To Server
#define SC_KEY_UP			1
#define SC_KEY_DOWN			2
#define SC_KEY_LEFT			3
#define SC_KEY_RIGHT		4
#define SC_KEY_1			5
#define SC_KEY_2			6
#define SC_KEY_SHIFT		7
#define SC_KEY_SPACE		8



// MODE�� 2:2�� ��� TEAM 1, 2
// MODE�� Melee�� ��� ��� ���
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

struct SC_PACKET_ENTER_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
	bool player_in[4];
};

// Client To Server ���� Ű���� �Է� Ready
//
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

// ���콺 �Է� 
struct CS_PACKET_MOUSEMOVE {
	BYTE size;
	BYTE type;
	// ���⿡ �躤�͸� �޾ƾ���..?
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
