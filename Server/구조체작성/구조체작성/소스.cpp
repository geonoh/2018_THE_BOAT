#include <WinSock2.h>
#include "stdafx.h"

int main() 
{
	BYTE test;

	test = 0xff;
	printf("%x \n", test);

	printf("%d\n", test);
}