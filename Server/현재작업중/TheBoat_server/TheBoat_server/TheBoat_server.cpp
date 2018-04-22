// 2018.4.3 FrameWork2 제작시작
//
// 2018.4.5 플레이어 접속하면 다른 클라이언트에서도 
// 확인할 수 있게 해야함.
// 

#include "stdafx.h"
#include "ServerFramework.h"
#define NUM_OF_THREAD	8

ServerFramework server_framework;

// 시간에 관련된 변수
time_point<system_clock> prev_time = system_clock::now();
time_point<system_clock> send_prev_time = system_clock::now();

void InitInstance();
void AcceptPlayer();
void WorkerThread();
void TimerThreadFunc();
void TimerSend();


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
	thread timer_thread{ TimerThreadFunc };
	thread timer_send_thread{ TimerSend };
	// 4명의 플레이어 모두 들어올때까지 대기 
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
	timer_send_thread.join();
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

void TimerThreadFunc() {
	while (true) {
		time_point<system_clock> cur_time = system_clock::now();
		duration<float> elapsed_time = cur_time - prev_time;
		server_framework.Update(elapsed_time);
		prev_time = cur_time;
	}
	//server_framework.TimerFunc();
}

void TimerSend() {
	while (true) {
		time_point<system_clock> cur_time = system_clock::now();
		duration<float> elapsed_time = cur_time - send_prev_time;
		server_framework.TimerSend(elapsed_time);
		send_prev_time = cur_time;
	}
}