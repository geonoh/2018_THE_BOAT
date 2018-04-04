// 2018.4.3 FrameWork2 제작시작
//

#include "stdafx.h"
#include "ServerFramework.h"
#define NUM_OF_THREAD	8

ServerFramework server_framework;

void InitInstance();
void AcceptPlayer();
void WorkerThread();
int ready_player = 0;

int main()
{
	vector<thread*> worker_vector;
	InitInstance();
	
	// 8개 쓰레드 생성
	for (int i = 0; i < NUM_OF_THREAD; ++i) {
		worker_vector.push_back(new thread{ WorkerThread });
	}

	// 1번 플레이어는 모드를 설정할 수 있다.
	vector<thread*> accept_thread;
	accept_thread.push_back(new thread{ AcceptPlayer });

	// Accpet를 동시에 실행하기로 하자.
	// 모두 레디
	while (true) {
		if(ready_player==4)
			break;
	}
	cout << "게임 시작" << endl;

	// Game 시작
	if (ready_player == 4) {
		while (true) {

		}
	}

	for (auto th : worker_vector) {
		th->join();
		delete th;
	}
	for (auto th : accept_thread) {
		th->join();
		delete th;
	}
    return 0;
}

void InitInstance() {
	// WSA소켓들 리셋해주기
	server_framework.InitServer();
}

void AcceptPlayer() {
	while (true) {
		server_framework.AcceptPlayer();
	}
}

void WorkerThread() {
	server_framework.WorkerThread();
}