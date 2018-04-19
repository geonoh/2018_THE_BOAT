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
	wprintf(L"����%s\n", lpMsgBuf);
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
		printf("WSAStartup() ����\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (iocp_handle == NULL)
		printf("����: CreateIoCompletionPort() ����\n");

	// �񵿱� ����� Listen ���� ����
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_socket == INVALID_SOCKET)
		printf("listen_socket ���� ����\n");

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);			// 9000�� ��Ʈ
	retval = ::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
		printf("bind ����\n");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		printf("listen ����\n");

	// �÷��̾��� ��ġ �ʱ�ȭ ���ֱ�;
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		clients[i].x = 200.f;
		clients[i].y = 200.f;
		clients[i].z = 1500.f;
	}

	// HeightMap �ҷ�����
	XMFLOAT3 xmf_3_scale(1.f, 0.2f, 1.f);
	LPCTSTR file_name = _T("MapResource/HeightMap.raw");
	height_map = new HeightMap(file_name, 257, 257, xmf_3_scale);

	//printf("%f\n", height_map->GetHeight(20.f, 30.1f));
}

void ServerFramework::AcceptPlayer() {
	SOCKADDR_IN c_addr;
	ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(SERVER_PORT);
	c_addr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(SOCKADDR_IN);

	// �������Ͽ��� ���� ���� Ŭ���̾�Ʈ ���Ͽ� ����
	int new_key = -1;
	auto client_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&c_addr), &addr_len, NULL, NULL);

	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

	int client_id = -1;
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use == false) {
			client_id = i;
			break;
		}
	}
	if (client_id == -1) {
		printf("�ִ� ���� �ʰ�\n");
	}
	// Accept �ϰ� ���� �Ʒ� �ڵ� ����
	printf("[%d] �÷��̾� ����\n",client_id);
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
	// �ϴ� �ޱ�� ���� ����
	clients[client_id].overlapped_ex.is_recv = true;
	clients[client_id].overlapped_ex.wsabuf.buf = clients[client_id].overlapped_ex.io_buffer;
	clients[client_id].overlapped_ex.wsabuf.len = sizeof(clients[client_id].overlapped_ex.io_buffer);
	clients[client_id].packet_size = 0;
	clients[client_id].prev_packet_size = 0;
	clients[client_id].team = Team::NON_TEAM;

	// client_id == 0 -> ����

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
		iocp_handle, client_id, 0);
	// �÷��̾� ���� ǥ��
	player_entered[client_id] = true;
	//
	clients[client_id].in_use = true;
	unsigned long flag = 0;
	WSARecv(client_socket, &clients[client_id].overlapped_ex.wsabuf, 1, NULL,
		&flag, &clients[client_id].overlapped_ex.wsa_over, NULL);

	// �÷��̾� �����ߴٰ� ��Ŷ ���������.
	SC_PACKET_ENTER_PLAYER packet;
	packet.id = client_id;
	packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
	packet.type = SC_ENTER_PLAYER;
	for (int i = 0; i < 4; ++i) {
		packet.player_in[i] = player_entered[i];
		// �̰� �� �ƴ϶� �÷��̾��� ���� ���µ� ����ڿ��� ��������.
		packet.player_ready[i] = player_ready[i]; 
	}

	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {

		if(clients[i].in_use)
			SendPacket(i, &packet);	// ��� �÷��̾�� �������� ��������
	}
	
}

void ServerFramework::ProcessPacket(int cl_id, char* packet) {
	// ���������δ� �� ��� ��Ŷ�� �ƿ츦�� �ִ°ɷ�
	// �ϴ� reinterpret ���ְ�, type�� Ȯ���Ѵ����� �ٽ� 
	// reinterpret���ָ� ����������? �̷������� ��Ŷ �����ϸ� ������?
	// ����ȯ ������� ŭ?
	// NO! �����ſ��� ū�ɷ� reinterpret���൵ �����ϴ�

	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(packet);
	switch (packet_buffer->type) {

		// <�ܼ�>�÷��̾� Ű �Է� �κ�
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

		// ���� �����ϱ�
	case CS_KEY_PRESS_1:
		printf("[ProcessPacket] :: AR ���� ����\n");
		break;
	case CS_KEY_PRESS_2:
		printf("[ProcessPacket] :: ���� ���� ����\n");

		break;

	case CS_KEY_PRESS_SHIFT:
		clients[cl_id].is_running = true;
		break;
	case CS_KEY_PRESS_SPACE:
		break;


		// ���� ���� ��� Ŭ���̾�Ʈ�� �÷��̾� �̵� ������ 
		// ��������Ѵ�. 
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


		// ���콺 �Է� ó�� ���ֱ�
	case CS_RIGHT_BUTTON_DOWN:
		printf("���� Ŭ������\n");
		clients[cl_id].is_right_click = true;
		break;
	case CS_RIGHT_BUTTON_UP:
		printf("���� Ŭ�� ��\n");
		clients[cl_id].is_right_click = false;
		break;


	case CS_LEFT_BUTTON_DOWN:
		printf("���� Ŭ������\n");
		clients[cl_id].is_left_click = true;
		break;
	case CS_LEFT_BUTTON_UP:
		printf("���� Ŭ�� ��\n");
		clients[cl_id].is_left_click = false;
		break;

		// ���콺 �����ӿ��� Player�� Look ���͸� ��������Ѵ�. 
	case CS_MOUSE_MOVE:
		//printf("���콺 ������ �������� ����\n");
		break;


		// ����, ������
	case CS_PLAYER_READY:
		player_ready[cl_id] = true;
		break;
	case CS_PLAYER_TEAM_SELECT:
		break;
	}
	// �� �Ʒ� ��� ��Ŷ �� ��������Ѵ�.
	// ��� �÷��̾�� �ش� �÷��̾ �̵��Ѱ͸� �����ָ� �ȴ�.


	// �̵� ���õ� ��Ŷ ó�� (���� ����)
	if (CS_KEY_PRESS_UP <= packet_buffer->type && packet_buffer->type <= CS_KEY_PRESS_SPACE) {
		//SC_PACKET_POS packets;
		//packets.id = cl_id;
		//packets.size = sizeof(SC_PACKET_POS);
		//packets.type = SC_POS;
		//packets.x = clients[cl_id].x;
		//packets.y = clients[cl_id].y;
		//packets.z = clients[cl_id].z;
		//// ������ ��Ŷ 
		//for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		//	if (clients[i].in_use == true) {
		//		SendPacket(i, &packets);
		//	}
		//}
		if (clients[cl_id].is_running) {
			//printf("�޸��� ��Ŷ ��������\n");

		}
		else {
			//printf("�ȱ� ��Ŷ ��������\n");

		}

		// Ű �Է������� Moving ��Ŷ�� �� Ŭ���̾�Ʈ���� �����ش�. 

	}
	// ���� ���� �׳� ���������� ��������ҰŰ��� �ФФ�
	if (CS_KEY_PRESS_UP <= packet_buffer->type&&packet_buffer->type <= CS_KEY_PRESS_RIGHT) {
		SC_PACKET_POS packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_POS);
		packets.type = SC_POS;		// Ű�� ���� ������ ���� ��Ŷ�� ��������.
		packets.x = clients[cl_id].x;
		packets.y = clients[cl_id].y;
		packets.z = clients[cl_id].z;
		printf("x = %f, y = %f, z = %f \n", packets.x, packets.y, packets.z);
		// ������ ��Ŷ 
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}
	}


	// �޸��� ��ư�� �������� 
	if (CS_KEY_PRESS_SHIFT == packet_buffer->type) {
		// �����̰� �ִ� ���¸� 
		if (clients[cl_id].is_move_foward || clients[cl_id].is_move_backward || clients[cl_id].is_move_right || clients[cl_id].is_move_left ) {
			printf("���� ��Ŷ ��������\n");
		}
	}

	// �̵��� �ϴٰ� key�� ���� �̵��� �����ϴ°͵� �÷��̾�� �˷�����Ѵ�. 
	else if (CS_KEY_RELEASE_UP <= packet_buffer->type && packet_buffer->type <= CS_KEY_RELEASE_SPACE) {
		SC_PACKET_POS packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_POS);
		packets.type = SC_POS;		// Ű�� ���� ������ ���� ��Ŷ�� ��������.
		packets.x = clients[cl_id].x;
		packets.y = clients[cl_id].y;
		packets.z = clients[cl_id].z;
		//printf("x = %f, y = %f, z = %f \n", packets.x, packets.y, packets.z);
		// ������ ��Ŷ 
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}
	}

	// ���� Ŭ���̾�Ʈ�� Look ���� ���� �ؼ� ��������.
	else if (CS_MOUSE_MOVE == packet_buffer->type) {
		printf("���⼭ ���콺 ������ ó���� ��Ŷ ��������h��\n"); 


	}

	// �÷��̾� �Ѿ� �߻� ���� ���� ó�� �ؾ���


	// �÷��̾� ���� ��ȭ _ Ready, Team ����, Mode �����
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

		// �ۼ��� Ȯ�� �� 
		OverlappedExtensionSet* is_recv_or_send = reinterpret_cast<OverlappedExtensionSet*>(overlapped);


		//if(is_recv_or_send->is_recv==true)
		//	printf("[WorkerThread::GQCS] ���� ClientID : %d, Size : %d\n", client_id, data_size);
		//else
		//	printf("[WorkerThread::GQCS] �۽� ClientID : %d, Size : %d\n", client_id, data_size);

		if (retval == FALSE) {
			printf("[WorkerThread::GQCS] ���� ���� ClientID : %d\n", client_id);
			DisconnectPlayer(client_id);
			continue;
		}
		// �÷��̾ ������ �������� �� 
		if (data_size == 0) {
			DisconnectPlayer(client_id);
			continue;
		}
		OverlappedExtensionSet* overlapped_buffer = reinterpret_cast<OverlappedExtensionSet*>(overlapped);

		// Data ���� ����
		if (overlapped_buffer->is_recv == true) {
			int recved_size = data_size;
			char* ptr = overlapped_buffer->io_buffer;
			while (recved_size > 0) {
				if (clients[client_id].packet_size == 0) {
					clients[client_id].packet_size = ptr[0];
				}
				int remain = clients[client_id].packet_size - clients[client_id].prev_packet_size;
				// (�޾ƾ� �ϴ� ������ ��) ���������Ͱ� ���������ͺ��� �� ���� ��
				if (remain <= recved_size) {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						remain);
					// ��Ŷ ������ �� ��� Ŭ���̾�Ʈ�� ��Ŷ ���� ó�� �� �������ֱ�
					ProcessPacket(static_cast<int>(client_id), clients[client_id].prev_packet);
					recved_size -= remain;
					ptr += remain;
					clients[client_id].packet_size = 0;
					clients[client_id].prev_packet_size = 0;
				}
				// (�޾ƾ� �ϴ� ������ ��) ���� �����Ͱ� �� Ŭ ���
				else {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						recved_size);
					recved_size -= recved_size;
					ptr += recved_size;	// �̰͵� �ǹ̰� ���� �ڵ�
				}
			}

			unsigned long rflag = 0;
			ZeroMemory(&overlapped_buffer->wsa_over, sizeof(WSAOVERLAPPED));
			int retval = WSARecv(clients[client_id].s, &overlapped_buffer->wsabuf, 1, NULL, &rflag, &overlapped_buffer->wsa_over, NULL);
			if (retval != 0) {
				int err_no = WSAGetLastError();
				// �� �Լ��� ���� ������ Send �κ� ����
				if (err_no != WSA_IO_PENDING) {
					ErrorDisplay("Error in WorkerThread(Recv) : ", err_no);
				}
			}

		}
		// Send�� ���� �Ҵ�� ���� �ݳ�
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
			ErrorDisplay("SendPacket���� ���� �߻� : ", err_no);
		}
	}
	//printf("[SendPacket] ClientID : <%d> Type[%d]\n", cl_id, (int)send_buffer[1]);
	//cout << "[SendPacket] ClientID : <" << cl_id << "> Type [" << (int)send_buffer[1] << "] Size [" << (int)send_buffer[0] << "]\n";

}

void ServerFramework::DisconnectPlayer(int cl_id) {
	// �÷��̾� ���� ����
	closesocket(clients[cl_id].s);
	clients[cl_id].in_use = false;
	printf("[DisconnectPlayer] ClientID : %d\n", cl_id);
	SC_PACKET_REMOVE_PLAYER packet;
	packet.client_id = cl_id;
	packet.size = sizeof(SC_PACKET_REMOVE_PLAYER);
	packet.type = SC_REMOVE_PLAYER;

	// �÷��̾ �����ٴ� ������ ��� Ŭ���̾�Ʈ�� �ѷ��ش�.
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
	//printf("%lf\n", elapsed_time);// ���� �������ε� ?
	// �´�
	// ���⼭ �Ѿ���� elapsed_time�� s�����̴�. 
	float elapsed_double = elapsed_time.count();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		// �Ѿ� �߻� �κ� ������Ʈ




		// �̵��κ� ������Ʈ 
		if (clients[i].is_move_foward) {
			if (clients[i].is_running) {
				clients[i].z += (10000.f * elapsed_double / 3600.f);
				//printf("%d �� ������ �ڴ� %lf\n", i, clients[i].z);
			}
			else {
				clients[i].z += (6000.f * elapsed_double / 3600.f);
				//printf("%d �� ������ ����! %lf\n", i, clients[i].z);
			}
		}
		if (clients[i].is_move_backward) {
			if (clients[i].is_running) {
				clients[i].z -= (10000.f * elapsed_double / 3600.f);
				//printf("%d �� �ڷ� �ڴ� %lf\n", i, clients[i].z);
			}
			else {
				clients[i].z -= (6000.f * elapsed_double / 3600.f);
				//printf("%d �� �ڷ� ����! %lf\n", i, clients[i].z);
			}
		}
		if (clients[i].is_move_left) {
			if (clients[i].is_running) {
				clients[i].x -= (10000.f * elapsed_double / 3600.f);
				//printf("%d �� �������� �ڴ� %lf\n", i, clients[i].x);
			}
			else {
				clients[i].x -= (6000.f * elapsed_double / 3600.f);
				//printf("%d �� �������� ����! %lf\n", i, clients[i].x);
			}
		}
		if (clients[i].is_move_right) {
			if (clients[i].is_running) {
				clients[i].x += (10000.f * elapsed_double / 3600.f);
				//printf("%d �� ���������� �ڴ� %lf\n", i, clients[i].x);
			}
			else {
				clients[i].x += (6000.f * elapsed_double / 3600.f);
				//printf("%d �� ���������� ����! %lf\n", i, clients[i].x);
			}

		}
	}
}