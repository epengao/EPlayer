
#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include "EPlayerAPI.h"

#define W_WIDTH  1280
#define W_HEIGHT  720

char* filePath = "C:/Users/Public/Videos/Sample\ Videos/Ò°Éú¶¯Îï.wmv";

static EPlayerAPI player;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lparam);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInatance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	WNDCLASS wndclass;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpszClassName = _T("demoPlayerWnd");
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = 0;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = 0;

	if (RegisterClass(&wndclass) == 0)
	{
		return 0;
	}

	HWND hWnd = CreateWindow(_T("demoPlayerWnd"),
							 _T("demoPlayer"),
							 WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
							 100, 100, W_WIDTH, W_HEIGHT, 0, 0, hInstance, 0);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

    InitEPlayerAPI(&player);

    player.Init();
	int ret = player.OpenMedia(filePath, (void*)hWnd, W_WIDTH, W_HEIGHT);
    if (ret == 0)
    {
        player.Play();
    }

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    player.CloseMedia();
    player.Uninit();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM IParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_MAXIMIZE)
			return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, IParam);
	}
	return 0;
}
