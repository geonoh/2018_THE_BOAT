#include "stdafx.h"
#include "ServerFramework.h"
#include "CHeightMapImage.h"

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

	XMFLOAT3 xmf3Scale(8.0f, 2.f, 8.0f);
	LPCTSTR file_name = _T("terrain17.raw");
	height_map = new CHeightMapImage(file_name, 513, 513, xmf3Scale);

	client_lock.lock();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		clients[i].x = 450.f;
		clients[i].z = 800.f;
		clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
	}
	client_lock.unlock();

	// OOBB ��
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		//clients[i].SetOOBB(XMFLOAT3(0, 0, 0), XMFLOAT3(10.f, 10.f, 10.f), XMFLOAT4(0, 0, 0, 1));
		clients[i].SetOOBB(XMFLOAT3(clients[i].x, clients[i].y, clients[i].z), XMFLOAT3(OBB_SCALE_X, OBB_SCALE_Y, OBB_SCALE_Z), XMFLOAT4(0, 0, 0, 1));
		printf("client %d���� OBB�� ���� ���� %f \n", i, clients[i].bounding_box.Extents.x);
	}

}

void ServerFramework::AcceptPlayer() {
	SOCKADDR_IN c_addr;
	ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(SERVER_PORT);
	c_addr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(SOCKADDR_IN);

	int new_key = -1;
	auto client_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&c_addr), &addr_len, NULL, NULL);

	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

	int client_id = -1;
	client_lock.lock();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use == false) {
			client_id = i;
			break;
		}
	}
	client_lock.unlock();
	if (client_id == -1) {
		printf("�ִ� ���� �ʰ�\n");
	}
	printf("[%d] �÷��̾� ����\n", client_id);
	client_lock.lock();
	clients[client_id].s = client_socket;
	clients[client_id].ar_mag = 0;
	clients[client_id].sub_mag = 0;
	clients[client_id].ar_weapons = ARWeapons::NON_AR;
	clients[client_id].sub_weapons = SubWeapons::NON_SUB;
	client_lock.unlock();
	clients[client_id].is_ready = false;
	clients[client_id].is_running = false;
	ZeroMemory(&clients[client_id].overlapped_ex.wsa_over, sizeof(WSAOVERLAPPED));
	clients[client_id].overlapped_ex.is_recv = true;
	clients[client_id].overlapped_ex.wsabuf.buf = clients[client_id].overlapped_ex.io_buffer;
	clients[client_id].overlapped_ex.wsabuf.len = sizeof(clients[client_id].overlapped_ex.io_buffer);
	clients[client_id].packet_size = 0;
	clients[client_id].prev_packet_size = 0;
	clients[client_id].team = Team::NON_TEAM;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
		iocp_handle, client_id, 0);
	// �÷��̾� ���� ǥ��
	player_entered[client_id] = true;
	clients[client_id].in_use = true;
	unsigned long flag = 0;
	WSARecv(client_socket, &clients[client_id].overlapped_ex.wsabuf, 1, NULL,
		&flag, &clients[client_id].overlapped_ex.wsa_over, NULL);

	// �÷��̾� �����ߴٰ� ��Ŷ ���������.
	// �� �������� �÷��̾��� �ʱ� ��ġ������ ���ԵǾ�� �Ѵ�. 
	SC_PACKET_ENTER_PLAYER packet;
	packet.id = client_id;
	packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
	packet.type = SC_ENTER_PLAYER;
	packet.x = clients[client_id].x;
	packet.y = clients[client_id].y;
	packet.z = clients[client_id].z;
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use) {
			printf("%d �÷��̾� ���� ���� ����\n", i);
			SendPacket(i, &packet);	
		}
	}

	// �ش� Ŭ���̾�Ʈ���Ե� �ٸ� Ŭ���̾�Ʈ�� ��ġ�� ��������Ѵ�!~
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		ZeroMemory(&packet, sizeof(packet));
		if (i != client_id) {
			if (clients[i].in_use == true) {
				packet.id = i;
				packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
				packet.type = SC_ENTER_PLAYER;
				clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
				packet.x = clients[i].x;
				packet.y = clients[i].y;
				packet.z = clients[i].z;
				SendPacket(client_id, &packet);
				printf("%d���� %d�� ������ ������\n", client_id, i);
			}
		}
	}

}

void ServerFramework::ProcessPacket(int cl_id, char* packet) {
	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(packet);

	switch (packet_buffer->type) {
	case CS_KEY_PRESS_UP:
		clients[cl_id].is_move_foward = true;
		printf("%d�÷��̾��� �� ��ġ x : %f, y : %f, z : %f\n", cl_id, clients[cl_id].x, clients[cl_id].y, clients[cl_id].z);
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

	case CS_RIGHT_BUTTON_DOWN:
		clients[cl_id].is_right_click = true;
		break;
	case CS_RIGHT_BUTTON_UP:
		clients[cl_id].is_right_click = false;
		break;

	case CS_LEFT_BUTTON_DOWN:
		clients[cl_id].is_left_click = true;
		break;
	case CS_LEFT_BUTTON_UP:
		clients[cl_id].is_left_click = false;
		break;

	case CS_MOUSE_MOVE: {
		clients[cl_id].look_vec = packet_buffer->look_vec;
		SC_PACKET_LOOCVEC packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_LOOCVEC);
		packets.type = SC_PLAYER_LOOKVEC;
		packets.look_vec = clients[cl_id].look_vec;
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}
		break;
	}
	case CS_PLAYER_READY:
		player_ready[cl_id] = true;
		break;
	case CS_PLAYER_TEAM_SELECT:
		break;
	}

}

void ServerFramework::WorkerThread() {
	unsigned long data_size = 0;
	unsigned long long client_id = 0;

	WSAOVERLAPPED* overlapped;

	while (true) {
		bool retval = GetQueuedCompletionStatus(iocp_handle, &data_size,
			&client_id, &overlapped, INFINITE);
		if (retval == FALSE) {
			printf("[WorkerThread::GQCS] ���� ClientID : %d\n", client_id);
			if (data_size == 0) {
				DisconnectPlayer(client_id);
				continue;
			}
		}
		OverlappedExtensionSet* overlapped_buffer = reinterpret_cast<OverlappedExtensionSet*>(overlapped);
		if (overlapped_buffer->is_recv == true) {
			int recved_size = data_size;
			char* ptr = overlapped_buffer->io_buffer;
			while (recved_size > 0) {
				if (clients[client_id].packet_size == 0) {
					clients[client_id].packet_size = ptr[0];
				}
				int remain = clients[client_id].packet_size - clients[client_id].prev_packet_size;
				if (remain <= recved_size) {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						remain);
					ProcessPacket(static_cast<int>(client_id), clients[client_id].prev_packet);
					recved_size -= remain;
					ptr += remain;
					clients[client_id].packet_size = 0;
					clients[client_id].prev_packet_size = 0;
				}
				else {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						recved_size);
					recved_size -= recved_size;
					ptr += recved_size;
				}
			}

			unsigned long rflag = 0;
			ZeroMemory(&overlapped_buffer->wsa_over, sizeof(WSAOVERLAPPED));
			int retval = WSARecv(clients[client_id].s, &overlapped_buffer->wsabuf, 1, NULL, &rflag, &overlapped_buffer->wsa_over, NULL);
			if (retval != 0) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING) {
					ErrorDisplay("Error in WorkerThread(Recv) : ", err_no);
				}
			}

		}
		// TimerThread���� ȣ��
		// 1/20 ���� ��� �÷��̾�� ���� ����
		else if (overlapped_buffer->command == SC_PLAYER_MOVE) {
			if (clients[client_id].in_use) {
				SC_PACKET_POS packets;
				packets.id = client_id;
				packets.size = sizeof(SC_PACKET_POS);
				packets.type = SC_POS;
				clients[client_id].y = height_map->GetHeight(clients[client_id].x, clients[client_id].z);
				packets.x = clients[client_id].x;
				packets.y = clients[client_id].y;
				packets.z = clients[client_id].z;
				for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
					if (clients[i].in_use == true) {
						SendPacket(i, &packets);
					}
				}
				ZeroMemory(overlapped_buffer, sizeof(OverlappedExtensionSet));
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

void ServerFramework::Update(duration<float>& elapsed_time) {
	Sleep(1);
	float elapsed_double = elapsed_time.count();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		client_lock.lock();
		if (clients[i].is_move_foward) {
			if (clients[i].is_running) {
				clients[i].z += PIXER_PER_METER * clients[i].look_vec.z * (10000.f * elapsed_double / 3600.f);
				clients[i].x += PIXER_PER_METER * clients[i].look_vec.x * (10000.f * elapsed_double / 3600.f);
			}
			else {
				clients[i].z += PIXER_PER_METER * clients[i].look_vec.z * (6000 * elapsed_double / 3600.f);
				clients[i].x += PIXER_PER_METER * clients[i].look_vec.x * (6000 * elapsed_double / 3600.f);
			}
		}
		if (clients[i].is_move_backward) {
			if (clients[i].is_running) {
				clients[i].z += (-1) * PIXER_PER_METER * clients[i].look_vec.z * (10000.f * elapsed_double / 3600.f);
				clients[i].x += (-1) * PIXER_PER_METER * clients[i].look_vec.x * (10000.f * elapsed_double / 3600.f);
			}
			else {
				clients[i].z += (-1) * PIXER_PER_METER * clients[i].look_vec.z * (6000 * elapsed_double / 3600.f);
				clients[i].x += (-1) * PIXER_PER_METER * clients[i].look_vec.x * (6000 * elapsed_double / 3600.f);
			}
		}
		if (clients[i].is_move_left) {
			if (clients[i].is_running) {
				clients[i].z += PIXER_PER_METER * clients[i].look_vec.x * (10000.f * elapsed_double / 3600.f);
				clients[i].x += (-1) * PIXER_PER_METER * clients[i].look_vec.z * (10000.f * elapsed_double / 3600.f);
			}
			else {
				clients[i].z += PIXER_PER_METER * clients[i].look_vec.x * (6000 * elapsed_double / 3600.f);
				clients[i].x += (-1) * PIXER_PER_METER * clients[i].look_vec.z * (6000 * elapsed_double / 3600.f);
			}
		}
		if (clients[i].is_move_right) {
			if (clients[i].is_running) {
				clients[i].z += (-1) * PIXER_PER_METER * clients[i].look_vec.x * (10000.f * elapsed_double / 3600.f);
				clients[i].x += PIXER_PER_METER * clients[i].look_vec.z * (10000.f * elapsed_double / 3600.f);
			}
			else {
				clients[i].z += (-1) * PIXER_PER_METER * clients[i].look_vec.x * (6000 * elapsed_double / 3600.f);
				clients[i].x += PIXER_PER_METER * clients[i].look_vec.z * (6000 * elapsed_double / 3600.f);
			}

		}

		// ���⼭ OBB�� ������Ʈ ������
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			//printf("%d �� ��¥ �������׽�Ʈ %f\n",i, clients[i].bounding_box.Extents.x);
			//clients[i].x;
			XMFLOAT4X4 danwi(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, clients[i].x, height_map->GetHeight(clients[i].x, clients[i].z), clients[i].z, 1);
			clients[i].bounding_box.Transform(clients[i].bounding_box,
				DirectX::XMLoadFloat4x4(&danwi)); // �� �ù�? 
			XMStoreFloat4(&clients[i].bounding_box.Orientation, XMQuaternionNormalize(XMLoadFloat4(&clients[i].bounding_box.Orientation)));
			clients[i].bounding_box.Extents.x = OBB_SCALE_X;
			clients[i].bounding_box.Extents.y = OBB_SCALE_Y;
			clients[i].bounding_box.Extents.z = OBB_SCALE_Z;
		}
		client_lock.unlock();

		//

		//if (clients[0].bounding_box.Intersects(clients[1].bounding_box)) {
		//	printf("0��(x : %f, y : %f, z : %f), 1��(x : %f, y : %f, z : %f    ",
		//		clients[0].bounding_box.Center.x,
		//		clients[0].bounding_box.Center.y,
		//		clients[0].bounding_box.Center.z,
		//		clients[1].bounding_box.Center.x,
		//		clients[1].bounding_box.Center.y,
		//		clients[1].bounding_box.Center.z);
		//	clients[0].bounding_box.Extents.x;
		//	printf("xũ��� %d   ", clients[0].bounding_box.Extents.x);
		//	printf("�浹 �غ���\n");
		//}
		ContainmentType containType = clients[0].bounding_box.Contains(clients[1].bounding_box);
		switch (containType)
		{
		case DISJOINT:
		{
			printf("0��(x : %f, y : %f, z : %f), 1��(x : %f, y : %f, z : %f    ",
				clients[0].bounding_box.Center.x,
				clients[0].bounding_box.Center.y,
				clients[0].bounding_box.Center.z,
				clients[1].bounding_box.Center.x,
				clients[1].bounding_box.Center.y,
				clients[1].bounding_box.Center.z);
			printf("   �ڽ� ũ�� : %f    ", clients[0].bounding_box.Extents.x);
			printf("�浹 ���Ԥ�\n");
			break;
		}
		case INTERSECTS:
		{
			printf("0��(x : %f, y : %f, z : %f), 1��(x : %f, y : %f, z : %f    ",
				clients[0].bounding_box.Center.x,
				clients[0].bounding_box.Center.y,
				clients[0].bounding_box.Center.z,
				clients[1].bounding_box.Center.x,
				clients[1].bounding_box.Center.y,
				clients[1].bounding_box.Center.z);
			printf("�浹 ����\n");
			break;
		}
		case CONTAINS:
			printf("0��(x : %f, y : %f, z : %f), 1��(x : %f, y : %f, z : %f   ",
				clients[0].bounding_box.Center.x,
				clients[0].bounding_box.Center.y,
				clients[0].bounding_box.Center.z,
				clients[1].bounding_box.Center.x,
				clients[1].bounding_box.Center.y,
				clients[1].bounding_box.Center.z);
			printf("   �ڽ� ũ�� : %f    ", clients[0].bounding_box.Extents.x);

			printf("�浹!!!!\n");
			break;
		}
	}
}

void ServerFramework::TimerSend(duration<float>& elapsed_time) {
	sender_time += elapsed_time.count();
	if (sender_time >= UPDATE_TIME) {	// 1/60 �ʸ��� ������ �۽�
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].is_move_backward || clients[i].is_move_foward || clients[i].is_move_left || clients[i].is_move_right) {
				// PQCS�� Ȯ������
				ol_ex[i].command = SC_PLAYER_MOVE;
				PostQueuedCompletionStatus(iocp_handle, 0, i, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[i]));
				printf("%d�� �ٿ���ڽ� x : %f  y : %f  z : %f \n", i, clients[i].bounding_box.Center.x, clients[i].bounding_box.Center.y,
					clients[i].bounding_box.Center.z);
			}
		}
		sender_time = 0;
		// �ӽ� :: �÷��̾� OBB ���� 
	}
}