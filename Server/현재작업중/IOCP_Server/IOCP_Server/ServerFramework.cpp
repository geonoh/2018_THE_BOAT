#include "ServerFramework.h"


DWORD WINAPI TestThread(LPVOID arg) {

}
DWORD WINAPI ServerFramework::WorkerThread(LPVOID arg) {
	//LPTHREAD_START_ROUTINE;

	return 0;
}

ServerFramework::ServerFramework()
{
	cts_packet = new CtsPacket;
	stc_packet = new StcPacket;

}


ServerFramework::~ServerFramework()
{
	delete cts_packet;
	delete stc_packet;
}

int ServerFramework::CreateWSA() {


}


int ServerFramework::Initialize() {
	// initialize WSA
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 0;
	}
	
	printf("CreateIoCompletionPort...\n");
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) {
		printf("CreateIoCompletionPort 초기화 에러\n");
		return 0;
	}
	printf("Done\n");

	SYSTEM_INFO sys_info;
	printf("Num of Processor : %d\n", (int)sys_info.dwNumberOfProcessors);


	printf("Create Thread...\n");
	HANDLE handle_thread;
	for (int i = 0; i < (int)sys_info.dwNumberOfProcessors * 2; ++i) {
		handle_thread = CreateThread(NULL, 0, &ServerFramework::WorkerThread, hcp, 0, NULL);
		CloseHandle(handle_thread);
	}
	printf("Thread creation Done\n");
}

