// -----------------------------------------------------
// Project. The boat 
// Server Framework 
// Start From 2018. 2. 18 
// Programed by Gunny
// -----------------------------------------------------


#include "stdafx.h"
#include "ServerFramework.h"

ServerFramework server_framework;
UINT player_counter = 0;
void InitServer();
void PlayingSession();

int main() {
	InitServer();



	while (true) {
		player_counter++;
		if (player_counter <= 4) {
			if (server_framework.AcceptClient(player_counter) == RUOK) {
				cout << player_counter << " player enterd" << endl;
			}
		}
		else if (player_counter == 4) {
			PlayingSession();
		}
	}


	return 0;
}


void InitServer() {
	server_framework.Initialize();
}

void PlayingSession() {
	// update from 
	server_framework.Update();
}