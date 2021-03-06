// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

// Console창 호출
#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Windows 헤더 파일:
#include <windows.h>
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>


// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

// 임시적으로 추가한것. (XMVECTOR 사용을 위한..)
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h> 

#include "..\..\TheBoat_server\TheBoat_server\protocol.h"

using namespace std;
//using namespace DirectX;
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


// 여기에서 모든 키의 is_pressed가 필요함

