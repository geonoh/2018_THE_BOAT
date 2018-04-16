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
	wprintf(L"에러%s\n", lpMsgBuf);
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
		printf("WSAStartup() 에러\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (iocp_handle == NULL)
		printf("최초: CreateIoCompletionPort() 에러\n");

	// 비동기 방식의 Listen 소켓 생성
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_socket == INVALID_SOCKET)
		printf("listen_socket 생성 오류\n");

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);			// 9000번 포트
	retval = ::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
		printf("bind 에러\n");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		printf("listen 에러\n");


	// HeightMap 불러오기
	XMFLOAT3 xmf_3_scale(1.f, 0.2f, 1.f);
	LPCTSTR file_name = _T("MapResource/HeightMap.raw");
	height_map = new HeightMap(file_name, 257, 257, xmf_3_scale);

	printf("%f\n", height_map->GetHeight(20.f, 30.1f));
}

void ServerFramework::AcceptPlayer() {
	SOCKADDR_IN c_addr;
	ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(SERVER_PORT);
	c_addr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(SOCKADDR_IN);

	// 리슨소켓에서 받은 정보 클라이언트 소켓에 연결
	int new_key = -1;
	auto client_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&c_addr), &addr_len, NULL, NULL);

	printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

	int client_id = -1;
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].in_use == false) {
			client_id = i;
			break;
		}
	}
	if (client_id == -1) {
		printf("최대 유저 초과\n");
	}
	// Accept 하고 나서 아래 코드 실행
	printf("[%d] 플레이어 입장\n",client_id);
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
	// 일단 받기로 먼저 설정
	clients[client_id].overlapped_ex.is_recv = true;
	clients[client_id].overlapped_ex.wsabuf.buf = clients[client_id].overlapped_ex.io_buffer;
	clients[client_id].overlapped_ex.wsabuf.len = sizeof(clients[client_id].overlapped_ex.io_buffer);
	clients[client_id].packet_size = 0;
	clients[client_id].prev_packet_size = 0;
	clients[client_id].team = Team::NON_TEAM;

	// client_id == 0 -> 방장

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
		iocp_handle, client_id, 0);
	// 플레이어 입장 표시
	player_entered[client_id] = true;
	//
	clients[client_id].in_use = true;
	unsigned long flag = 0;
	WSARecv(client_socket, &clients[client_id].overlapped_ex.wsabuf, 1, NULL,
		&flag, &clients[client_id].overlapped_ex.wsa_over, NULL);

	// 플레이어 입장했다고 패킷 보내줘야함.
	SC_PACKET_ENTER_PLAYER packet;
	packet.id = client_id;
	packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
	packet.type = SC_ENTER_PLAYER;
	for (int i = 0; i < 4; ++i) {
		packet.player_in[i] = player_entered[i];
		// 이거 뿐 아니라 플레이어의 레디 상태도 당사자에게 보내야함.
		packet.player_ready[i] = player_ready[i]; 
	}

	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {

		if(clients[i].in_use)
			SendPacket(i, &packet);	// 모든 플레이어에게 입장정보 보내야함
	}
	
}

void ServerFramework::ProcessPacket(int cl_id, char* packet) {
	// 내생각으로는 이 모든 패킷을 아우를수 있는걸로
	// 일단 reinterpret 해주고, type만 확인한다음에 다시 
	// reinterpret해주면 되지않을까? 이런식으로 패킷 구분하면 좋을듯?
	// 형변환 오버헤드 큼?
	// NO! 작은거에서 큰걸로 reinterpret해줘도 가능하다

	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(packet);
	switch (packet_buffer->type) {

		// <단순>플레이어 키 입력 부분
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
	case CS_KEY_PRESS_1:
		break;
	case CS_KEY_PRESS_2:
		break;
	case CS_KEY_PRESS_SHIFT:
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
		break;
	case CS_KEY_RELEASE_SPACE:
		break;




	// 레디, 팀선택
	case CS_PLAYER_READY:
		player_ready[cl_id] = true;
		break;
	case CS_PLAYER_TEAM_SELECT:
		break;
	}




	// 이 아래 모든 패킷 다 보내줘야한다.
	// 모든 플레이어에게 해당 플레이어가 이동한것만 보내주면 된다.

	// 이동 관련된 패킷 처리 (점프 포함)
	if (CS_KEY_PRESS_UP <= packet_buffer->type && packet_buffer->type <= CS_KEY_PRESS_SHIFT) {
		SC_PACKET_POS packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_POS);
		packets.type = SC_POS;
		packets.x = clients[cl_id].x;
		packets.y = clients[cl_id].y;
		packets.z = clients[cl_id].z;
		// 포지션 패킷 
		for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
			if (clients[i].in_use == true) {
				SendPacket(i, &packets);
			}
		}

	}
	// 플레이어 상태 변화 _ Ready, Team 변경, Mode 변경등
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

		// 송수신 확인 용 
		OverlappedExtensionSet* is_recv_or_send = reinterpret_cast<OverlappedExtensionSet*>(overlapped);
		if(is_recv_or_send->is_recv==true)
			printf("[WorkerThread::GQCS] 수신 ClientID : %d, Size : %d\n", client_id, data_size);
		else
			printf("[WorkerThread::GQCS] 송신 ClientID : %d, Size : %d\n", client_id, data_size);

		if (retval == FALSE) {
			printf("[WorkerThread::GQCS] 접속 해제 ClientID : %d\n", client_id);
			DisconnectPlayer(client_id);
			continue;
		}
		// 플레이어가 접속을 해지했을 때 
		if (data_size == 0) {
			DisconnectPlayer(client_id);
			continue;
		}
		OverlappedExtensionSet* overlapped_buffer = reinterpret_cast<OverlappedExtensionSet*>(overlapped);

		// Data 수신 시작
		if (overlapped_buffer->is_recv == true) {
			int recved_size = data_size;
			char* ptr = overlapped_buffer->io_buffer;
			while (recved_size > 0) {
				if (clients[client_id].packet_size == 0) {
					clients[client_id].packet_size = ptr[0];
				}
				int remain = clients[client_id].packet_size - clients[client_id].prev_packet_size;
				// (받아야 하는 데이터 中) 받은데이터가 남은데이터보다 더 많을 때
				if (remain <= recved_size) {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						remain);
					// 패킷 재조립 후 모든 클라이언트에 패킷 내용 처리 후 전송해주기
					ProcessPacket(static_cast<int>(client_id), clients[client_id].prev_packet);
					recved_size -= remain;
					ptr += remain;
					clients[client_id].packet_size = 0;
					clients[client_id].prev_packet_size = 0;
				}
				// (받아야 하는 데이터 中) 남은 데이터가 더 클 경우
				else {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						recved_size);
					recved_size -= recved_size;
					ptr += recved_size;	// 이것도 의미가 없는 코드
				}
			}

			unsigned long rflag = 0;
			ZeroMemory(&overlapped_buffer->wsa_over, sizeof(WSAOVERLAPPED));
			int retval = WSARecv(clients[client_id].s, &overlapped_buffer->wsabuf, 1, NULL, &rflag, &overlapped_buffer->wsa_over, NULL);
			if (retval != 0) {
				int err_no = WSAGetLastError();
				// 이 함수에 대한 설명은 Send 부분 참조
				if (err_no != WSA_IO_PENDING) {
					ErrorDisplay("Error in WorkerThread(Recv) : ", err_no);
				}
			}

		}
		// Send로 인해 할당된 영역 반납
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
			ErrorDisplay("SendPacket에서 에러 발생 : ", err_no);
		}
	}
	printf("[SendPacket] ClientID : <%d> Type[%d]\n", cl_id, (int)send_buffer[1]);
	//cout << "[SendPacket] ClientID : <" << cl_id << "> Type [" << (int)send_buffer[1] << "] Size [" << (int)send_buffer[0] << "]\n";

}

void ServerFramework::DisconnectPlayer(int cl_id) {
	// 플레이어 접속 끊기
	closesocket(clients[cl_id].s);
	clients[cl_id].in_use = false;
	printf("[DisconnectPlayer] ClientID : %d\n", cl_id);
	SC_PACKET_REMOVE_PLAYER packet;
	packet.client_id = cl_id;
	packet.size = sizeof(SC_PACKET_REMOVE_PLAYER);
	packet.type = SC_REMOVE_PLAYER;

	// 플레이어가 나갔다는 정보를 모든 클라이언트에 뿌려준다.
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
		duration<double> elapsed_time = cur_time - prev_time;
		Update(elapsed_time);
		prev_time = cur_time;
	}
}
void ServerFramework::Update(duration<double>& elapsed_time) {
	//printf("%lf\n", elapsed_time);// 단위 세컨드인듯 ?
	// 맞다
	// 여기서 넘어오는 elapsed_time은 s단위이다. 
	double elapsed_double = elapsed_time.count();
	for (int i = 0; i < MAXIMUM_PLAYER; ++i) {
		if (clients[i].is_move_foward) {
			clients[i].z += (6000.f * elapsed_double / 3600.f);
			printf("%d 번 앞으로 간다! %lf\n",i, clients[i].z);
		}
		if (clients[i].is_move_backward) {
			clients[i].z -= (6000.f * elapsed_double / 3600.f);
			printf("%d 번 뒤로 간다! %lf\n", i, clients[i].z);
		}
		if (clients[i].is_move_left) {
			clients[i].x -= (6000.f * elapsed_double / 3600.f);
			printf("%d 번 왼쪽으로 간다! %lf\n", i, clients[i].x);
		}
		if (clients[i].is_move_right) {
			clients[i].x += (6000.f * elapsed_double / 3600.f);
			printf("%d 번 오른쪽으로 간다! %lf\n", i, clients[i].x);
		}
	}
}