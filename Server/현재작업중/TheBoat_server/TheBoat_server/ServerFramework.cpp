#include "stdafx.h"
#include "HeightMap.h"
#include "ServerFramework.h"


void ErrorDisplay(const char* msg, int err_no) {
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s", msg);
	wprintf(L"¿¡·¯%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);

}


ServerFramework::ServerFramework()
{
}


ServerFramework::~ServerFramework()
{
	delete height_map;
}

void ServerFramework::InitServer() {
	int retval = 0;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		printf("WSAStartup() ¿¡·¯\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (iocp_handle == NULL)
		printf("ÃÖÃÊ: CreateIoCompletionPort() ¿¡·¯\n");

	// ºñµ¿±â ¹æ½ÄÀÇ Listen ¼ÒÄÏ »ı¼º
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_socket == INVALID_SOCKET)
		printf("listen_socket »ı¼º ¿À·ù\n");

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);			// 9000¹ø Æ÷Æ®
	retval = ::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
		printf("bind ¿¡·¯\n");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		printf("listen ¿¡·¯\n");


	// HeightMap ºÒ·¯¿À±â
	XMFLOAT3 xmf_3_scale(1.f, 0.2f, 1.f);
	LPCTSTR file_name = _T("MapResource/HeightMap.raw");
	height_map = new HeightMap(file_name, 257, 257, xmf_3_scale);

<<<<<<< HEAD
	//printf("%f\n", height_map->GetHeight(20.f, 30.1f));
=======
	printf("%f\n", height_map->GetHeight(20.f, 30.1f));

	// 
>>>>>>> [ì„œë²„ í”„ë ˆì„ì›Œí¬] : HeightMap Class ë¶™ì´ê¸° ì„±ê³µ. ì´ì œ ë§µë§Œ ë§Œë“¤ë©´ ë¨
}

void ServerFramework::AcceptPlayer() {
	SOCKADDR_IN c_addr;
	ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(SERVER_PORT);
	c_addr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(SOCKADDR_IN);

	// ¸®½¼¼ÒÄÏ¿¡¼­ ¹ŞÀº Á¤º¸ Å¬¶óÀÌ¾ğÆ® ¼ÒÄÏ¿¡ ¿¬°á
	int new_key = -1;
	auto client_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&c_addr), &addr_len, NULL, NULL);

	printf("[TCP ¼­¹ö] Å¬¶óÀÌ¾ğÆ® Á¢¼Ó: IP ÁÖ¼Ò=%s, Æ÷Æ® ¹øÈ£=%d\n",
		inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

	int client_id = -1;
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use == false) {
			client_id = i;
			break;
		}
	}
	if (client_id == -1) {
		printf("ÃÖ´ë À¯Àú ÃÊ°ú\n");
	}
	// Accept ÇÏ°í ³ª¼­ ¾Æ·¡ ÄÚµå ½ÇÇà
	printf("[%d] ÇÃ·¹ÀÌ¾î ÀÔÀå\n",client_id);
	clients[client_id].s = client_socket;
	clients[client_id].ar_mag = 0;
	clients[client_id].sub_mag = 0;
	clients[client_id].ar_weapons = ARWeapons::NON_AR;
	clients[client_id].sub_weapons = SubWeapons::NON_SUB;
	for (int i = 0; i < 4; ++i) {
		clients[client_id].boat_parts[i] = false;
	}
	clients[client_id].is_ready = false;
	clients[client_id].is_running = false;
	ZeroMemory(&clients[client_id].overlapped_ex.wsa_over, sizeof(WSAOVERLAPPED));
	// ÀÏ´Ü ¹Ş±â·Î ¸ÕÀú ¼³Á¤
	clients[client_id].overlapped_ex.is_recv = true;
	clients[client_id].overlapped_ex.wsabuf.buf = clients[client_id].overlapped_ex.io_buffer;
	clients[client_id].overlapped_ex.wsabuf.len = sizeof(clients[client_id].overlapped_ex.io_buffer);
	clients[client_id].packet_size = 0;
	clients[client_id].prev_packet_size = 0;
	clients[client_id].team = Team::NON_TEAM;

	// client_id == 0 -> ¹æÀå

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
		iocp_handle, client_id, 0);
	// ÇÃ·¹ÀÌ¾î ÀÔÀå Ç¥½Ã
	player_entered[client_id] = true;
	//
	clients[client_id].in_use = true;
	unsigned long flag = 0;
	WSARecv(client_socket, &clients[client_id].overlapped_ex.wsabuf, 1, NULL,
		&flag, &clients[client_id].overlapped_ex.wsa_over, NULL);

	// ÇÃ·¹ÀÌ¾î ÀÔÀåÇß´Ù°í ÆĞÅ¶ º¸³»Áà¾ßÇÔ.
	SC_PACKET_ENTER_PLAYER packet;
	packet.id = client_id;
	packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
	packet.type = SC_ENTER_PLAYER;
	for (int i = 0; i < 4; ++i) {
		packet.player_in[i] = player_entered[i];
		// ÀÌ°Å »Ó ¾Æ´Ï¶ó ÇÃ·¹ÀÌ¾îÀÇ ·¹µğ »óÅÂµµ ´ç»çÀÚ¿¡°Ô º¸³»¾ßÇÔ.
		packet.player_ready[i] = player_ready[i]; 
	}

	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {

		if(clients[i].in_use)
			SendPacket(i, &packet);	// ¸ğµç ÇÃ·¹ÀÌ¾î¿¡°Ô ÀÔÀåÁ¤º¸ º¸³»¾ßÇÔ
	}
	
}

void ServerFramework::ProcessPacket(int cl_id, char* packet) {
	// ³»»ı°¢À¸·Î´Â ÀÌ ¸ğµç ÆĞÅ¶À» ¾Æ¿ì¸¦¼ö ÀÖ´Â°É·Î
	// ÀÏ´Ü reinterpret ÇØÁÖ°í, type¸¸ È®ÀÎÇÑ´ÙÀ½¿¡ ´Ù½Ã 
	// reinterpretÇØÁÖ¸é µÇÁö¾ÊÀ»±î? ÀÌ·±½ÄÀ¸·Î ÆĞÅ¶ ±¸ºĞÇÏ¸é ÁÁÀ»µí?
	// Çüº¯È¯ ¿À¹öÇìµå Å­?
	// NO! ÀÛÀº°Å¿¡¼­ Å«°É·Î reinterpretÇØÁàµµ °¡´ÉÇÏ´Ù

	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(packet);
	switch (packet_buffer->type) {

		// <´Ü¼ø>ÇÃ·¹ÀÌ¾î Å° ÀÔ·Â ºÎºĞ
	case CS_KEY_PRESS_UP:
		clients[cl_id].is_move_foward = true;
		break;
	case CS_KEY_PRESS_DOWN:
		clients[cl_id].is_move_backward = true;
		break;
	case CS_KEY_PRESS_LEFT:
		clients[cl_id].is_move_left = true;
		break;
	case CS_KEY_PRESS_RIGHT:
		clients[cl_id].is_move_right = true;
		break;

		// ¹«±â º¯°æÇÏ±â
	case CS_KEY_PRESS_1:
		printf("[ProcessPacket] :: AR ¹«±â ¼±ÅÃ\n");
		break;
	case CS_KEY_PRESS_2:
		printf("[ProcessPacket] :: ±ÇÃÑ ¹«±â ¼±ÅÃ\n");

		break;

	case CS_KEY_PRESS_SHIFT:
		clients[cl_id].is_running = true;
		break;
	case CS_KEY_PRESS_SPACE:
		break;

	case CS_KEY_RELEASE_UP:
		clients[cl_id].is_move_foward = false;
		break;
	case CS_KEY_RELEASE_DOWN:
		clients[cl_id].is_move_backward = false;
		break;
	case CS_KEY_RELEASE_LEFT:
		clients[cl_id].is_move_left = false;
		break;
	case CS_KEY_RELEASE_RIGHT:
		clients[cl_id].is_move_right = false;
		break;
	case CS_KEY_RELEASE_1:
		break;
	case CS_KEY_RELEASE_2:
		break;
	case CS_KEY_RELEASE_SHIFT:
		clients[cl_id].is_running = false;
		break;
	case CS_KEY_RELEASE_SPACE:
		break;


		// ¸¶¿ì½º ÀÔ·Â Ã³¸® ÇØÁÖ±â
	case CS_RIGHT_BUTTON_DOWN:
		printf("¿ìÃø Å¬¸¯ÇßÀ½\n");
		clients[cl_id].is_right_click = true;
		break;
	case CS_RIGHT_BUTTON_UP:
		printf("¿ìÃø Å¬¸¯ ¶«\n");
		clients[cl_id].is_right_click = false;
		break;


	case CS_LEFT_BUTTON_DOWN:
		clients[cl_id].is_left_click = true;
		break;
	case CS_LEFT_BUTTON_UP:
		clients[cl_id].is_left_click = false;
		break;

		// ¸¶¿ì½º ¿òÁ÷ÀÓ¿¡µµ PlayerÀÇ Look º¤ÅÍ¸¦ º¸³»Áà¾ßÇÑ´Ù. 
	case CS_MOUSE_MOVE:
		//printf("¸¶¿ì½º ¿òÁ÷ÀÓ ¼­¹ö¿¡¼­ °¨Áö\n");
		break;


		// ·¹µğ, ÆÀ¼±ÅÃ
	case CS_PLAYER_READY:
		player_ready[cl_id] = true;
		break;
	case CS_PLAYER_TEAM_SELECT:
		break;
	}
	// ÀÌ ¾Æ·¡ ¸ğµç ÆĞÅ¶ ´Ù º¸³»Áà¾ßÇÑ´Ù.
	// ¸ğµç ÇÃ·¹ÀÌ¾î¿¡°Ô ÇØ´ç ÇÃ·¹ÀÌ¾î°¡ ÀÌµ¿ÇÑ°Í¸¸ º¸³»ÁÖ¸é µÈ´Ù.

	// ÀÌµ¿ °ü·ÃµÈ ÆĞÅ¶ Ã³¸® (Á¡ÇÁ Æ÷ÇÔ)
	if (CS_KEY_PRESS_UP <= packet_buffer->type && packet_buffer->type <= CS_KEY_PRESS_SPACE) {
		SC_PACKET_POS packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_POS);
		packets.type = SC_POS;
		packets.x = clients[cl_id].x;
		packets.y = clients[cl_id].y;
		packets.z = clients[cl_id].z;
		// Æ÷Áö¼Ç ÆĞÅ¶ 
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}

	}
	// ÀÌµ¿À» ÇÏ´Ù°¡ key¸¦ ¶§¼­ ÀÌµ¿À» Á¤ÁöÇÏ´Â°Íµµ ÇÃ·¹ÀÌ¾î¿¡°Ô ¾Ë·ÁÁà¾ßÇÑ´Ù. 
	else if (CS_KEY_RELEASE_UP <= packet_buffer->type && packet_buffer->type <= CS_KEY_RELEASE_SPACE) {
		SC_PACKET_POS packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_POS);
		packets.type = SC_POS;		// Å°¸¦ ‹šµµ Æ÷Áö¼Ç °ü·Ã ÆĞÅ¶ÀÌ º¸³»Áø´Ù.
		packets.x = clients[cl_id].x;
		packets.y = clients[cl_id].y;
		packets.z = clients[cl_id].z;
		// Æ÷Áö¼Ç ÆĞÅ¶ 
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}
	}

	// ÇöÀç Å¬¶óÀÌ¾ğÆ®ÀÇ Look º¤ÅÍ °°ÀÌ ÇØ¼­ º¸³»¾ßÇÔ.
	else if (CS_MOUSE_MOVE == packet_buffer->type) {
		printf("¿©±â¼­ ¸¶¿ì½º ¿òÁ÷ÀÓ Ã³¸®ÇÑ ÆĞÅ¶ º¸³»Áà¾ßÁhµğ¤¿\n"); 


	}

	// ÇÃ·¹ÀÌ¾î ÃÑ¾Ë ¹ß»ç °ü·Ã ¿¹¿Ü Ã³¸® ÇØ¾ßÇÔ


	// ÇÃ·¹ÀÌ¾î »óÅÂ º¯È­ _ Ready, Team º¯°æ, Mode º¯°æµî
	else if (100 <= packet_buffer->type) {
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {

			}
		}

	}

}

void ServerFramework::WorkerThread() {
	unsigned long data_size = 0;
	unsigned long long client_id = 0;

	WSAOVERLAPPED* overlapped;

	while (true) {
		bool retval = GetQueuedCompletionStatus(iocp_handle, &data_size,
			&client_id, &overlapped, INFINITE);

		// ¼Û¼ö½Å È®ÀÎ ¿ë 
		OverlappedExtensionSet* is_recv_or_send = reinterpret_cast<OverlappedExtensionSet*>(overlapped);
		if(is_recv_or_send->is_recv==true)
			printf("[WorkerThread::GQCS] ¼ö½Å ClientID : %d, Size : %d\n", client_id, data_size);
		else
			printf("[WorkerThread::GQCS] ¼Û½Å ClientID : %d, Size : %d\n", client_id, data_size);

		if (retval == FALSE) {
			printf("[WorkerThread::GQCS] Á¢¼Ó ÇØÁ¦ ClientID : %d\n", client_id);
			DisconnectPlayer(client_id);
			continue;
		}
		// ÇÃ·¹ÀÌ¾î°¡ Á¢¼ÓÀ» ÇØÁöÇßÀ» ¶§ 
		if (data_size == 0) {
			DisconnectPlayer(client_id);
			continue;
		}
		OverlappedExtensionSet* overlapped_buffer = reinterpret_cast<OverlappedExtensionSet*>(overlapped);

		// Data ¼ö½Å ½ÃÀÛ
		if (overlapped_buffer->is_recv == true) {
			int recved_size = data_size;
			char* ptr = overlapped_buffer->io_buffer;
			while (recved_size > 0) {
				if (clients[client_id].packet_size == 0) {
					clients[client_id].packet_size = ptr[0];
				}
				int remain = clients[client_id].packet_size - clients[client_id].prev_packet_size;
				// (¹Ş¾Æ¾ß ÇÏ´Â µ¥ÀÌÅÍ ñé) ¹ŞÀºµ¥ÀÌÅÍ°¡ ³²Àºµ¥ÀÌÅÍº¸´Ù ´õ ¸¹À» ¶§
				if (remain <= recved_size) {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						remain);
					// ÆĞÅ¶ ÀçÁ¶¸³ ÈÄ ¸ğµç Å¬¶óÀÌ¾ğÆ®¿¡ ÆĞÅ¶ ³»¿ë Ã³¸® ÈÄ Àü¼ÛÇØÁÖ±â
					ProcessPacket(static_cast<int>(client_id), clients[client_id].prev_packet);
					recved_size -= remain;
					ptr += remain;
					clients[client_id].packet_size = 0;
					clients[client_id].prev_packet_size = 0;
				}
				// (¹Ş¾Æ¾ß ÇÏ´Â µ¥ÀÌÅÍ ñé) ³²Àº µ¥ÀÌÅÍ°¡ ´õ Å¬ °æ¿ì
				else {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						recved_size);
					recved_size -= recved_size;
					ptr += recved_size;	// ÀÌ°Íµµ ÀÇ¹Ì°¡ ¾ø´Â ÄÚµå
				}
			}

			unsigned long rflag = 0;
			ZeroMemory(&overlapped_buffer->wsa_over, sizeof(WSAOVERLAPPED));
			int retval = WSARecv(clients[client_id].s, &overlapped_buffer->wsabuf, 1, NULL, &rflag, &overlapped_buffer->wsa_over, NULL);
			if (retval != 0) {
				int err_no = WSAGetLastError();
				// ÀÌ ÇÔ¼ö¿¡ ´ëÇÑ ¼³¸íÀº Send ºÎºĞ ÂüÁ¶
				if (err_no != WSA_IO_PENDING) {
					ErrorDisplay("Error in WorkerThread(Recv) : ", err_no);
				}
			}

		}
		// Send·Î ÀÎÇØ ÇÒ´çµÈ ¿µ¿ª ¹İ³³
		else {
			delete overlapped_buffer;
		}
	}
}

void ServerFramework::SendPacket(int cl_id, void* packet) {
	OverlappedExtensionSet* overlapped = new OverlappedExtensionSet;
	char* send_buffer = reinterpret_cast<char*>(packet);

	memcpy(&overlapped->io_buffer, packet, send_buffer[0]);
	overlapped->is_recv = false;
	overlapped->wsabuf.buf = overlapped->io_buffer;
	overlapped->wsabuf.len = send_buffer[0];
	ZeroMemory(&overlapped->wsa_over, sizeof(WSAOVERLAPPED));
	unsigned long flag = 0;
	int retval = WSASend(clients[cl_id].s, &overlapped->wsabuf, 1, NULL, 0,
		&overlapped->wsa_over, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			ErrorDisplay("SendPacket¿¡¼­ ¿¡·¯ ¹ß»ı : ", err_no);
		}
	}
	printf("[SendPacket] ClientID : <%d> Type[%d]\n", cl_id, (int)send_buffer[1]);
	//cout << "[SendPacket] ClientID : <" << cl_id << "> Type [" << (int)send_buffer[1] << "] Size [" << (int)send_buffer[0] << "]\n";

}

void ServerFramework::DisconnectPlayer(int cl_id) {
	// ÇÃ·¹ÀÌ¾î Á¢¼Ó ²÷±â
	closesocket(clients[cl_id].s);
	clients[cl_id].in_use = false;
	printf("[DisconnectPlayer] ClientID : %d\n", cl_id);
	SC_PACKET_REMOVE_PLAYER packet;
	packet.client_id = cl_id;
	packet.size = sizeof(SC_PACKET_REMOVE_PLAYER);
	packet.type = SC_REMOVE_PLAYER;

	// ÇÃ·¹ÀÌ¾î°¡ ³ª°¬´Ù´Â Á¤º¸¸¦ ¸ğµç Å¬¶óÀÌ¾ğÆ®¿¡ »Ñ·ÁÁØ´Ù.
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use == true) {
			SendPacket(i, &packet);
		}
	}

}

bool ServerFramework::IsStartGame() {
	int ready_counter = 0;
	for (auto i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (player_ready[i] == true)
			ready_counter++;
	}
	if (ready_counter == 4)
		return true;
	else
		return false;
}

void ServerFramework::TimerFunc() {
	while (true) {
		time_point<system_clock> cur_time = system_clock::now();
		duration<float> elapsed_time = cur_time - prev_time;
		Update(elapsed_time);
		prev_time = cur_time;
	}
}
void ServerFramework::Update(duration<float>& elapsed_time) {
	//printf("%lf\n", elapsed_time);// ´ÜÀ§ ¼¼ÄÁµåÀÎµí ?
	// ¸Â´Ù
	// ¿©±â¼­ ³Ñ¾î¿À´Â elapsed_timeÀº s´ÜÀ§ÀÌ´Ù. 
	float elapsed_double = elapsed_time.count();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].is_move_foward) {
			if (clients[i].is_running) {
				clients[i].z += (10000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¾ÕÀ¸·Î ¶Ú´Ù %lf\n", i, clients[i].z);
			}
			else {
				clients[i].z += (6000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¾ÕÀ¸·Î °£´Ù! %lf\n", i, clients[i].z);
			}
		}
		if (clients[i].is_move_backward) {
			if (clients[i].is_running) {
				clients[i].z -= (10000.f * elapsed_double / 3600.f);
				printf("%d ¹ø µÚ·Î ¶Ú´Ù %lf\n", i, clients[i].z);
			}
			else {
				clients[i].z -= (6000.f * elapsed_double / 3600.f);
				printf("%d ¹ø µÚ·Î °£´Ù! %lf\n", i, clients[i].z);
			}
		}
		if (clients[i].is_move_left) {
			if (clients[i].is_running) {
				clients[i].x -= (10000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¿ŞÂÊÀ¸·Î ¶Ú´Ù %lf\n", i, clients[i].x);
			}
			else {
				clients[i].x -= (6000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¿ŞÂÊÀ¸·Î °£´Ù! %lf\n", i, clients[i].x);
			}
		}
		if (clients[i].is_move_right) {
			if (clients[i].is_running) {
				clients[i].x += (10000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¿À¸¥ÂÊÀ¸·Î ¶Ú´Ù %lf\n", i, clients[i].x);
			}
			else {
				clients[i].x += (6000.f * elapsed_double / 3600.f);
				printf("%d ¹ø ¿À¸¥ÂÊÀ¸·Î °£´Ù! %lf\n", i, clients[i].x);
			}

		}
	}
}