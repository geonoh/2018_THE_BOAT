#include "stdafx.h"
#include "ServerFramework.h"

ServerFramework server_framework;

void InitServer();


int main() {
	InitServer();

	
	return 0;
}


void InitServer() {
	server_framework.Initialize();
}