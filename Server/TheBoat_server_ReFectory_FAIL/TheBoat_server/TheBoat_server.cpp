// 2018.4.3 FrameWork2 제작시작
//
// 2018.4.5 플레이어 접속하면 다른 클라이언트에서도 
// 확인할 수 있게 해야함.
// 

#include "stdafx.h"
#include "ServerFramework.h"
#define NUM_OF_THREAD	4

ServerFramework server_framework;

void InitInstance();
void AcceptPlayer();
void WorkerThread();

void TimerSend();


void TimerThread();

int main()
{
	vector<thread*> worker_vector;
	InitInstance();
	for (int i = 0; i < NUM_OF_THREAD; ++i)
		worker_vector.push_back(new thread{ WorkerThread });

	vector<thread*> accept_thread;
	accept_thread.push_back(new thread{ AcceptPlayer });
	thread timer_thread{ TimerThread };

	while (true) {
		if (server_framework.IsStartGame())
			break;
	}
	printf("게임 시작\n");
	while (true) {
		printf("게임 중\n");
	}

	for (auto th : worker_vector) {
		th->join();
		delete th;
	}
	for (auto th : accept_thread) {
		th->join();
		delete th;
	}
	timer_thread.join();
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

void TimerThread() {
	server_framework.TimerThread();
}

void TimerSend() {
	while (true) {
		//int current_time = GetTickCount();
		//int elapsed_time = current_time - send_prev_time;
		//server_framework.TimerSend(elapsed_time);
		//send_prev_time = current_time;
	}
}