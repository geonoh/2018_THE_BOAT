#pragma once
#include "STCPacket.h"
#include "CTSPacket.h"
#include <iostream>
#include <Windows.h>

using namespace std;

struct MousePoint {
	int x, y;
};


struct StcPacket {
	
};

// 해당 클라이언트에서 선언되어 값이 변경되지 않는이상 그대로 저장
// 되어있다. 
struct CtsPacket {
	BYTE keyboard_click;
	BYTE mouse_click;
	MousePoint mouse_direction;

	// 클라이언트에서 보낼 때 키보드의 클릭을 받을 때
	void PushArrowKeyUp() {
	}
	void PushArrowKeyDown() {
	}
	void PushArrowKeyRight() {
	}
	void PushArrowKeyLeft() {
	}
	void ReleaseArrowKeyUp() {
	}
	void ReleaseArrowKeyDown() {
	}
	void ReleaseArrowKeyRight() {
	}
	void ReleaseArrowKeyLeft() {
	}
};