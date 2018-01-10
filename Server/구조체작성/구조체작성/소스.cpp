#include <WinSock2.h>
#include "stdafx.h"

int main() 
{
	/*BYTE test;
	UINT test2;

	test2 = 0xffffffff;
	test = 0xff;
	UINT test3 = test2 >> 28;


	printf("%x\n", test3);*/


	CtsPacket test1;
	test1.keyboard_click = 0x00010101;
	test1.PushArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	test1.ReleaseArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	printf("%d\n", sizeof(test1));
}