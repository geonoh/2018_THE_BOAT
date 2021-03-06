// TheBoat_client(serv_test).cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TheBoat_client(serv_test).h"
#include "ServerMgr.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

ServerMgr server_mgr;

bool is_pushed[11] = { 0 };	// 키 중복입력 방지하기 위한 bool 형

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_THEBOATCLIENTSERVTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THEBOATCLIENTSERVTEST));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THEBOATCLIENTSERVTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_THEBOATCLIENTSERVTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   server_mgr.Initialize(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int key_buffer = wParam;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

		// 비동기로 관리된 소켓 정보가 넘어오게 되면 msg에 소켓데이터가 넘어온다.
	case WM_RBUTTONDOWN:
		if (is_pushed[CS_RIGHT_BUTTON_DOWN] == false) {
			printf("[WM_RBUTTONDOWN] : MouseRight 키 입력\n");
			server_mgr.SendPacket(CS_RIGHT_BUTTON_DOWN);
			is_pushed[CS_RIGHT_BUTTON_DOWN] = true;
		}
		break;
	case WM_LBUTTONDOWN:
		if (is_pushed[CS_LEFT_BUTTON_DOWN] == false) {
			printf("[WM_RBUTTONDOWN] : MouseLeft 키 입력\n");
			server_mgr.SendPacket(CS_LEFT_BUTTON_DOWN);
			is_pushed[CS_LEFT_BUTTON_DOWN] = true;
		}
		break;
	case WM_MOUSEMOVE:
		printf("[WM_MOUSEMOVE] : Mouse 움직임\n");
		server_mgr.SendPacket(CS_MOUSE_MOVE);
		break;

	case WM_RBUTTONUP:
		printf("[WM_RBUTTONDOWN] : MouseRight 키 놓음\n");
		server_mgr.SendPacket(CS_RIGHT_BUTTON_UP);
		is_pushed[CS_RIGHT_BUTTON_DOWN] = false;
		break;
	case WM_LBUTTONUP:
		printf("[WM_RBUTTONDOWN] : MouseLeft 키 놓음\n");
		server_mgr.SendPacket(CS_LEFT_BUTTON_UP);
		is_pushed[CS_LEFT_BUTTON_DOWN] = false;
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			exit(-1);
		}
		else if (wParam == VK_SHIFT) {
			if (is_pushed[CS_KEY_PRESS_SHIFT] == false) {
				printf("[WM_KEYUP] : Shift 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_SHIFT);
				is_pushed[CS_KEY_PRESS_SHIFT] = true;
			}
		}
		else if (wParam == VK_SPACE) {
			if (is_pushed[CS_KEY_PRESS_SPACE] == false) {
				printf("[WM_KEYUP] : Space 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_SPACE);
				is_pushed[CS_KEY_PRESS_SPACE] = true;
			}
		}
		// 키 입력
		switch (key_buffer) {
			// 
		case 'w':
		case 'W':
			if (is_pushed[CS_KEY_PRESS_UP] == false) {
				printf("[WM_KEYUP] : w,W 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_UP);
				is_pushed[CS_KEY_PRESS_UP] = true;
			}
			break;
		case 'a':
		case 'A':
			if (is_pushed[CS_KEY_PRESS_LEFT] == false) {
				printf("[WM_KEYUP] : a,A 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_LEFT);
				is_pushed[CS_KEY_PRESS_LEFT] = true;
			}
			break;
		case 's':
		case 'S':
			if (is_pushed[CS_KEY_PRESS_DOWN] == false) {
				printf("[WM_KEYUP] : s,S 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_DOWN);
				is_pushed[CS_KEY_PRESS_DOWN] = true;
			}
			break;
		case 'd':
		case 'D':
			if (is_pushed[CS_KEY_PRESS_RIGHT] == false) {
				printf("[WM_KEYUP] : d,D 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_RIGHT);
				is_pushed[CS_KEY_PRESS_RIGHT] = true;
			}
			break;
		case '1':
			if (is_pushed[CS_KEY_PRESS_1] == false) {
				printf("[WM_KEYUP] : 1 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_1);
				is_pushed[CS_KEY_PRESS_1] = true;
			}
			break;
		case '2':
			if (is_pushed[CS_KEY_PRESS_2] == false) {
				printf("[WM_KEYUP] : 2 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_2);
				is_pushed[CS_KEY_PRESS_2] = true;
			}
			break;
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE) {
			exit(-1);
		}
		else if (wParam == VK_SHIFT) {
			printf("[WM_KEYUP] : Shift 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_SHIFT);
			is_pushed[CS_KEY_PRESS_SHIFT] = false;
		}
		else if (wParam == VK_SPACE) {
			printf("[WM_KEYUP] : Space 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_SPACE);
			is_pushed[CS_KEY_PRESS_SPACE] = false;
		}
		// 키 놓음
		switch (key_buffer) {
		case 'w':
		case 'W':
			printf("[WM_KEYUP] : w,W 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_UP);
			is_pushed[CS_KEY_PRESS_UP] = false;
			break;
		case 'a':
		case 'A':
			printf("[WM_KEYUP] : a,A 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_LEFT);
			is_pushed[CS_KEY_PRESS_LEFT] = false;
			break;
		case 's':
		case 'S':
			printf("[WM_KEYUP] : s,S 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_DOWN);
			is_pushed[CS_KEY_PRESS_DOWN] = false;
			break;
		case 'd':
		case 'D':
			printf("[WM_KEYUP] : d,D 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_RIGHT);
			is_pushed[CS_KEY_PRESS_RIGHT] = false;
			break;

			// 무기 변경키는 키를 놓는다고 해서 따로 패킷을 보내지 않아도 된다ㅓ. 
		case '1':
			printf("[WM_KEYUP] : 1 키 놓음\n");
			//server_mgr.SendPacket(CS_KEY_RELEASE_1);
			is_pushed[CS_KEY_PRESS_1] = false;
			break;
		case '2':
			printf("[WM_KEYUP] : 2 키 놓음\n");
			//server_mgr.SendPacket(CS_KEY_RELEASE_2);
			is_pushed[CS_KEY_PRESS_2] = false;
			break;
		}
		break;
	case WM_SOCKET: {
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			server_mgr.ClientError();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			server_mgr.ReadPacket();
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			server_mgr.ClientError();
			break;
		}
	}
	default:break;
		//return 0;
    }
        return DefWindowProc(hWnd, message, wParam, lParam);
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
