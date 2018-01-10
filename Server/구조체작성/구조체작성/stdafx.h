#pragma once
#include "STCPacket.h"
#include "CTSPacket.h"
#include <iostream>
#include <Windows.h>

#define HALF_BYTE		4

using namespace std;


enum PushByte {
	Two = 0, One, SpaceBar, Shift, Left, Right, Down, Up
};


struct MousePoint {
	int x, y;
};


struct StcPacket {
	
};

// �ش� Ŭ���̾�Ʈ���� ����Ǿ� ���� ������� �ʴ��̻� �״�� ����
// �Ǿ��ִ�. 
struct CtsPacket {
	UINT keyboard_click = 0x00000000;
	UINT mouse_click = 0x00000000;
	MousePoint mouse_direction = { 0 };

	// Ŭ���̾�Ʈ���� ���� �� Ű������ Ŭ���� ���� ��
	// OR ���� -> �� �ϳ��� 1�� ������ TRUE�� ��ȯ�Ѵٴ� ��.
	// �ߺ� Ű �Է��� ���� ��� 1�� �̸� ���� �� �ִµ� �� ��쵵 Ŀ�� ����
	void PushArrowKeyUp() {
		UINT input_buffer = 0x10000000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyDown() {
		UINT input_buffer = 0x01000000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyRight() {
		UINT input_buffer = 0x00100000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyLeft() {
		UINT input_buffer = 0x00010000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyShift() {
		UINT input_buffer = 0x00001000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeySpaceBar() {
		UINT input_buffer = 0x00000100;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyOne() {
		UINT input_buffer = 0x00000010;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyTwo() {
		UINT input_buffer = 0x00000001;
		keyboard_click = keyboard_click | input_buffer;
	}


	// Release�� 1�� 0���� ����� �ִ� �۾��� �ؾ��Ѵ�. 
	// &������ ���ؼ� Ȯ��
	void ReleaseArrowKeyUp() {
		UINT input_buffer = 0x01111111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyDown() {
		UINT input_buffer = 0x10111111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyRight() {
		UINT input_buffer = 0x11011111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyLeft() {
		UINT input_buffer = 0x11101111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyShift() {
		UINT input_buffer = 0x11110111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeySpaceBar() {
		UINT input_buffer = 0x11111011;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyOne() {
		UINT input_buffer = 0x11111101;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyTwo() {
		UINT input_buffer = 0x11111110;
		keyboard_click = keyboard_click & input_buffer;
	}
};