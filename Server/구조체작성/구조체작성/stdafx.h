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

// �ش� Ŭ���̾�Ʈ���� ����Ǿ� ���� ������� �ʴ��̻� �״�� ����
// �Ǿ��ִ�. 
struct CtsPacket {
	BYTE keyboard_click;
	BYTE mouse_click;
	MousePoint mouse_direction;

	// Ŭ���̾�Ʈ���� ���� �� Ű������ Ŭ���� ���� ��
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