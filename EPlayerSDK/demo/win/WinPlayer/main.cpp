
#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include "EPlayerAPI.h"

#define W_WIDTH  500
#define W_HEIGHT 360

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
	wndclass.lpszClassName = _T("EPlayerWnd");
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = 0;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = 0;

	if (RegisterClass(&wndclass) == 0)
	{
		return 0;
	}

	HWND hWnd = CreateWindow(_T("EPlayerWnd"),
							 _T("WinPlayer"),
                             WS_OVERLAPPEDWINDOW,
							 100, 100, W_WIDTH, W_HEIGHT, 0, 0, hInstance, 0);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

    InitEPlayerAPI(&player);

    player.Init();
    int ret = player.OpenMedia("d:/video/testVideo.mp4", (void*)hWnd, W_WIDTH, W_HEIGHT);
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
    RECT rcClient = { 0 };
    unsigned int nWidth = LOWORD(IParam); // width of client area
    unsigned int nHeight = HIWORD(IParam); // height of client area

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_MAXIMIZE)
			return 0;
    case WM_SIZE:   //要让窗体能够随着缩放改变，要响应WM-SIZE消息  
        if (player.UpdateVideoScreen)
        {
            player.UpdateVideoScreen(hwnd, nWidth, nHeight);
            rcClient.left = 0;
            rcClient.top = 0;
            rcClient.right = nWidth;
            rcClient.bottom = nHeight;
            InvalidateRect(hwnd, &rcClient, 0);
            DefWindowProc(hwnd, uMsg, wParam, IParam);
        }
	default:
		return DefWindowProc(hwnd, uMsg, wParam, IParam);
	}
	return 0;
}
