/*
* ****************************************************************
* This software is a media player SDK implementation
* GPL:
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Library General Public License for more details. You should
* have received a copy of the GNU Library General Public License
* along with this library; if not, write to the Free Foundation,
* Inc., 59 Temple Place, Eite 330, Boston, MA  02111-1307  USA
*
* Project: EC < Enjoyable Coding >
*
* EPlayer.cpp
* This file for EasyPlayer [EPlayer] Application implement.
*
* Eamil:   epengao@126.com
* Author:  Gao Peng
* Version: Intial first version.
* ****************************************************************
*/

#include <time.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <TCHAR.H>
#include <ShlObj.h>
#include <Commdlg.h>
#include <windows.h>
#include <commctrl.h>
#include "Resource.h"

using namespace std;

#include "EPlayer.h"
#include "MediaEngine.h"


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    InitCommonControls();
    g_hInstance = hInstance;
    WNDCLASSEX   wndclassex  = {0};
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.lpfnWndProc   = WndProc;
    wndclassex.lpszClassName = szAppClassName;
    wndclassex.hInstance     = g_hInstance;
    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
    wndclassex.hCursor       = LoadCursor(NULL, IDC_HAND);
    //wndclassex.hIcon         = LoadIcon(NULL, (LPCTSTR)IDI_ICON1);
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.hIconSm       = LoadIcon(wndclassex.hInstance, (LPCTSTR)IDI_ICON1);

    RegisterClassEx(&wndclassex);
    float rat = 0.5;
    DWORD sw = ::GetSystemMetrics(SM_CXSCREEN);
    DWORD sh = ::GetSystemMetrics(SM_CYSCREEN);
    int nWndHeight = sh * rat;
    int nWndWidth = (nWndHeight * 4 / 3);
    g_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES,
                            szAppClassName, szAppName,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            nWndWidth, nWndHeight,
                            NULL, NULL, hInstance, NULL);
    CenterWindow(g_hWnd);
    g_hMenu = GetMenu(g_hWnd);
    ShowWindow(g_hWnd, iCmdShow);
    UpdateWindow(g_hWnd);
    EnableMenu(IDM_FILE_CLOSE, FALSE);
    EnableMenu(IDM_FILE_SHOT, FALSE);
    EnableMenu(IDM_PLAY_PAUSE, FALSE);
    EnableMenu(IDM_STOP, FALSE);
    EnableMenu(IDM_NEXTFRAME, FALSE);
    EnableMenu(IDM_DECREASERATE, FALSE);
    EnableMenu(IDM_INCREASERATE, FALSE);
    EnableMenu(IDM_RESETRATE, FALSE);
    EnableMenu(IDM_SCREEN_DEFAULT, FALSE);
    EnableMenu(IDM_SCREEN_STRETCH, FALSE);
    EnableMenu(IDM_SCREEN_4_3, FALSE);
    EnableMenu(IDM_SCREEN_5_4, FALSE);
    EnableMenu(IDM_SCREEN_16_9, FALSE);
    EnableMenu(IDM_SIZE_50, FALSE);
    EnableMenu(IDM_SIZE_75, FALSE);
    EnableMenu(IDM_SIZE_100, FALSE);
    EnableMenu(IDM_SIZE_150, FALSE);
    EnableMenu(IDM_SIZE_FULL, FALSE);

    g_TopMode = 0;
    g_hCursor = LoadCursor(NULL, IDC_HAND);
    HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_ACCELERATOR1);
    RegisterHotKey(g_hWnd, 1000, MOD_ALT,'Z');

    if(strlen(szCmdLine) > 2)
    {
        char pPath[256] = {0};
        WCHAR wszCmd[300] = { 0 };
        strncpy(pPath, szCmdLine + 1, strlen(szCmdLine) - 2);
        int nLen = MultiByteToWideChar(CP_ACP, 0, pPath, -1, NULL, NULL);
        MultiByteToWideChar(CP_ACP, 0, pPath, -1, wszCmd, nLen);
        PlayFile(wszCmd);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static HDC hdcMem;
    static int bmpWidth, bmpHeight; /* logo picture size */
    PAINTSTRUCT ps;
    HGDIOBJ hOld;
    static int width, height; /* Main window size */
    static int panelWidth, panelHeight; /* control board size */
    static int captionHeight, menuHeight, borderHeight; /* menu hight */
    static int iSel1 = IDM_SCREEN_DEFAULT;
    static int iSel2 = IDM_TOP1;
    TCHAR szFile[MAX_PATH];
    RECT rect;

    switch (message)
    {
        case WM_DROPFILES:
        {
            HDROP hDropInfo;
            hDropInfo = (HDROP)wParam;
            DragQueryFile(hDropInfo, (UINT)0xFFFFFFFF, (LPTSTR)NULL, (UINT)NULL);
            DragQueryFile(hDropInfo, 0, szFile, sizeof(szFile));
            PlayFile(szFile);
            DragFinish(hDropInfo);
        }
        break;
        case WM_CREATE:
        {
            HBITMAP	hBitmap;
            hBitmap = LoadBitmap(g_hInstance, (LPCTSTR)IDB_BITMAP_LOGO);
            BITMAP bitmap;
            GetObject(hBitmap, sizeof(BITMAP), &bitmap);
            bmpWidth = bitmap.bmWidth;
            bmpHeight = bitmap.bmHeight;

            hdc = BeginPaint(hwnd, &ps);
            hdcMem = CreateCompatibleDC(hdc);
            hOld = SelectObject(hdcMem, hBitmap);
            EndPaint(hwnd, &ps);
            DeleteObject(hOld);

            //CreateVideoScreenWindow(hwnd);

            captionHeight = GetSystemMetrics(SM_CYCAPTION);
            menuHeight = GetSystemMetrics(SM_CYMENUSIZE);
            borderHeight = 2 * (GetSystemMetrics(SM_CXEDGE) + GetSystemMetrics(SM_CXSIZEFRAME));

            CreatePanel(hwnd);
            GetWindowRect(g_hWndPanel, &rect);
            panelWidth = rect.right - rect.left;
            panelHeight = rect.bottom - rect.top;
            ctrlPanelWndWidth = panelWidth;
            ctrlPanelWndHeight = panelHeight;
            SetCursor(g_hCursor);
        }
        break;
        case WM_COMMAND:
        {
            WORD wID;
            wID = LOWORD(wParam);
            switch (wID)
            {
                case IDM_FILE_OPEN:
                {
                    OPENFILENAME ofn;
                    TCHAR szFile[MAX_PATH];
                    memset(&szFile, 0, MAX_PATH);
                    memset(&ofn, 0, sizeof(OPENFILENAME));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = TEXT("所有文件(*.*)\0*.*\0媒体文件\0*.3gp;*.mpg;*.mpeg;*.avi;*.rmvb;*.rm;*.ra;*.ram;*.mp3;*.mid;*.wav;*.mov;*.qt;*.wmv;*.wma;*.asf;*.vob;*.dat;*.mkv;*.mp4;*.mka;*.midi;*.ogg;*.omg\0\0");
                    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
                    if (!GetOpenFileName(&ofn))
                        return 0;
                    PlayFile(szFile);
                }break;
                case IDM_FILE_CLOSE:
                {
                    Close();
                }break;
                case IDM_FILE_EXIT:
                {
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                }break;
                case IDM_PLAY_PAUSE:
                {
                    PlayOrPause();
                }break;
                case IDM_STOP:
                {
                    Close();
                }break;
                case IDM_NEXTFRAME:
                {
                    StepFrame();
                }break;
                case IDM_DECREASERATE:
                {
                    DecreaseRate();
                }break;
                case IDM_INCREASERATE:
                {
                    IncreaseRate();
                }break;
                case IDM_RESETRATE:
                {
                    ResetRate();
                }break;
                case IDM_SCREEN_DEFAULT:
                case IDM_SCREEN_STRETCH:
                case IDM_SCREEN_4_3:
                case IDM_SCREEN_5_4:
                case IDM_SCREEN_16_9:
                {
                    CheckMenu(iSel1, false);
                    iSel1 = LOWORD(wParam);
                    CheckMenu(iSel1);
                }break;
                case IDM_SIZE_50:
                {
                    UpdateMainWindow(g_MediaInfo.nVideoWidth, g_MediaInfo.nVideoHeight,0.5);
                }break;
                case IDM_SIZE_75:
                {
                    UpdateMainWindow(g_MediaInfo.nVideoWidth, g_MediaInfo.nVideoHeight, 0.75);
                }break;
                case IDM_SIZE_100:
                {
                    UpdateMainWindow(g_MediaInfo.nVideoWidth, g_MediaInfo.nVideoHeight, 1);
                }break;
                case IDM_SIZE_150:
                {
                    UpdateMainWindow(g_MediaInfo.nVideoWidth, g_MediaInfo.nVideoHeight, 1.5);
                }break;
                case IDM_SIZE_FULL:
                {
                    VideoEntryFullScreen();
                }break;
                case IDM_TOP1:
                case IDM_TOP2:
                case IDM_TOP3:
                {
                    SetTopMode(wID - IDM_TOP1);
                    CheckMenu(iSel2, false);
                    iSel2 = LOWORD(wParam);
                    CheckMenu(iSel2);
                }break;
                case IDM_ABOUT:
                {
                    MessageBox(g_hWnd, TEXT(" 易播(EPlayer): 基于 FFmpeg 实现。\r\n 一款界面整洁，简单易用的播放器。\r\n 你可以基于EPlayerSDK开发自己的播放器。 \r\nE-mail:  epengao@126.com. \r\n QQ:  380154185"), TEXT("关于"), MB_ICONINFORMATION);
                }break;
            }
        } /* case WM_COMMAND: */
        break;
        case WM_PAINT:
        {
            if (g_pMediaEng && (g_pMediaEng->HasPlayMedia()))
                break;
            hdc = BeginPaint(hwnd, &ps);//无图像时，重绘Logo
            BitBlt(hdc, (width - bmpWidth) / 2, (height - bmpHeight - panelHeight) / 2, bmpWidth, bmpHeight, hdcMem, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
        }
        break;
        //case WM_MOUSEMOVE:
        //{
        //    //if (!g_DXGraph.GetFullScreen())
        //    //{
        //    //    SetCursor(g_hCursor);
        //    //}
        //}
        //break;
        //case WM_LBUTTONDOWN:
        //{
        //    SetCursor(g_hCursor);
        //}
        //break;
        //case WM_LBUTTONUP:
        //{
        //    SetCursor(g_hCursor);
        //}
        //break;
        //case WM_LBUTTONDBLCLK:
        //{
        //    SetCursor(g_hCursor);
        //}
        //break;
        case WM_KEYDOWN:
        {
            if (wParam == VK_SPACE)
                PlayOrPause();
            else if (wParam == VK_ESCAPE)
                VideoExitFullScreen();
            else if (wParam == VK_RETURN)
                VideoEntryFullScreen();
            break;
        }
        break;
        case WM_SIZE:
        {
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            mainWndWidth = width;
            mainWndHeight = height;
            videoWndWidth = width;
            videoWndHeight = height - panelHeight;
            ctrlPanelWndWidth = width;
            ctrlPanelWndHeight = panelHeight;
            UpdateVideoScreen(videoWndWidth, videoWndHeight);
            SetWindowPos(g_hWndPanel, HWND_TOP, 0, height - panelHeight, width, panelHeight, SWP_SHOWWINDOW);
            rt_StatusBarText3.left = width - 120;
            rt_StatusBarText3.right = width - 10;
            rt_StatusBarText2.right = width - 120;
            if (true/*g_DXGraph.IsHaveVideo()*/)
            {
                /*g_DXGraph.SetWindowPos(0, 0, width, height - panelHeight);*/
                rect.left = 0;
                rect.top = height - 20;
                rect.right = width;
                rect.bottom = height;
                InvalidateRect(hwnd, &rect, 0);
            }
            else
            {
                rect.left = 0;
                rect.top = 0;//menuHeight;
                rect.right = width;
                rect.bottom = height - panelHeight;
                InvalidateRect(hwnd, &rect, 0);
                rect.top = height - 20;
                rect.bottom = height;
                InvalidateRect(hwnd, &rect, 0);
            }
        }
        break;
        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* xi;
            xi = (MINMAXINFO*)lParam;
            xi->ptMinTrackSize.x = 300;
            xi->ptMinTrackSize.y = 300;
            xi->ptMaxTrackSize.x = 2000;
            xi->ptMaxTrackSize.y = 2000;
        }
        break;
        case WM_DESTROY:
        {
            DeleteDC(hdcMem);
            if ((g_pMediaEng) && (g_pMediaEng->HasPlayMedia()))
            {
                g_pMediaEng->Stop();
                delete g_pMediaEng;
            }
            PostQuitMessage(0);
            UninitSystemVolumeControl();
            return(0);
        }
        break;
        case WM_TIMER:
        {
            UpdatePlayPostion();
        }
        break;
        case WM_HOTKEY:
        {
            if (g_bHide){
                ShowWindow(g_hWnd, SW_SHOW);
                g_bHide = false;
            }
            else
            {
                if (false/*g_DXGraph.IsRunning() || g_DXGraph.IsPaused()*/)
                {
                    ShowWindow(g_hWnd, SW_HIDE);
                    SetToolBarState(TB_PAUSE, TBSTATE_CHECKED | TBSTATE_ENABLED);
                    SetToolBarState(TB_PLAY, TBSTATE_ENABLED);
                    Pause();
                    g_bHide = true;
                }
            }
        }
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

INT_PTR CALLBACK PanelWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static HDC hdcMem;
    PAINTSTRUCT ps;
    HFONT hFont;
    LOGFONT font;

    static int width, height; //Bottom playback control Panel size.
    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hDlg, &ps);
            SetBkMode(hdc, TRANSPARENT);
            RECT rt;
            GetClientRect(hDlg, &rt);
            HBRUSH hBrush, hOldBrush;
            hBrush = CreateSolidBrush(RGB(0, 0, 0));
            hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom) ;
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
            ZeroMemory(&font, sizeof(font));
            font.lfHeight = -12;
            hFont = CreateFontIndirect(&font);
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawText(hdc, g_StatusBarText1, _tcslen(g_StatusBarText1), &rt_StatusBarText1, DT_LEFT);
            DrawText(hdc, g_PlaybakTimeText, _tcslen(g_PlaybakTimeText), &rt_StatusBarText3, DT_RIGHT);
            //DrawText(hdc, g_StatusBarText3, _tcslen(g_StatusBarText3), &rt_StatusBarText3, DT_RIGHT);
            SetTextColor(hdc, RGB(255, 255, 0));
            DrawText(hdc, g_StatusBarText2, _tcslen(g_StatusBarText2), &rt_StatusBarText2, DT_LEFT);
            DrawIconEx(hdc, 2, 50, g_hIcon, 16, 16, 0, NULL, DI_NORMAL);
            EndPaint(hDlg, &ps);
            break;

        case WM_LBUTTONDOWN:
            PostMessage(g_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case TB_PLAY:
                    Play();
                    break;
                case TB_PAUSE:
                    Pause();
                    break;
                case TB_STOP:
                    Close();
                    break;
                case TB_PREV:
                    PlayPrev();
                    break;
                case TB_DECREASERATE:
                    DecreaseRate();
                    break;
                case TB_INCREASERATE:
                    IncreaseRate();
                    break;
                case TB_NEXT:
                    PlayNext();
                    break;
                case TB_NEXTFRAME:
                    StepFrame();
                    break;
                case TB_SOUND:
                    EnableWindow(g_hWndVolume, g_bSilence);
                    SetSystemVolumeMute(!g_bSilence);
                    TBBUTTONINFO btn;
                    if(g_bSilence)
                    {
                        btn.iImage = 8;
                        //g_DXGraph.SetAudioVolume(GetProgress(g_hWndVolume) - 4000);
                    }
                    else
                    {
                        btn.iImage = 9;
                        //g_DXGraph.SetAudioVolume(-10000);
                    }
                    btn.cbSize = sizeof(TBBUTTONINFO);
                    btn.dwMask = TBIF_IMAGE;
                    SendMessage(g_hWndToolBar, TB_SETBUTTONINFO, TB_SOUND, (LPARAM)&btn);
                    g_bSilence = !g_bSilence;
                    break;
            }
            break; /* case WM_COMMAND: */
        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            MoveWindow(g_hWndProgress, 0, 0, width, 21, FALSE);
            break;
        case WM_MOUSEHWHEEL:
            return false;
    }
    return false;
}

LRESULT CALLBACK VideoScreenWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDBLCLK:
        {
            if (g_bVideoFullScreen)
                VideoExitFullScreen();
            else
                VideoEntryFullScreen();
            break;
        }
        case WM_KEYDOWN:
        {
            if (wParam == VK_SPACE)
                PlayOrPause();
            else if (wParam == VK_ESCAPE)
                VideoExitFullScreen();
            else if (wParam == VK_RETURN)
                VideoEntryFullScreen();
            break;
        }
        case WM_MOUSEMOVE:
        {
            if (g_bVideoFullScreen)
            {
                /* 鼠标移动显示控制Panel, 定时消失 */
                //int curTime = GetTickCount();
                //int offset = curTime - entyFullScreenTime;
                //if (offset < 500) break;

                //POINT point;
                //static int ox = 0;
                //static int oy = 0;
                //point.x = LOWORD(lParam);
                //point.y = HIWORD(lParam);
                //if ((ox != point.x) || (oy != point.y))
                //{
                //    ox = point.x;
                //    oy = point.y;
                //    //ShowFullScreenCtrlPanel();
                //    lastMouseMovTime = curTime;
                //}
                /* 鼠标位置在控制Panel上才显示 */
                POINT point;
                point.x = LOWORD(lParam);
                point.y = HIWORD(lParam);
                if(IsPointInCtrlPanelRect(point))
                    ShowFullScreenCtrlPanel();
                else
                    HideFullScreenCtrlPanel();
            }
            break;
        }
        case WS_EX_TOPMOST:
        {
            HideFullScreenCtrlPanel();
            break;
        }
        case WM_TIMER:
        {
            if (g_bVideoFullScreen)
            {
                POINT   point;
                GetCursorPos(&point);
                if (IsPointInCtrlPanelRect(point))
                    break;

                int curTime = GetTickCount();
                int offset = curTime - lastMouseMovTime;
                if (offset > 1000)
                {
                    //HideFullScreenCtrlPanel();
                }
            }
            break;
        }
    }
    return CallWindowProc(oldVideoScreenProc, hwnd, message, wParam, lParam);
}

void UpdateVideoScreen(unsigned int nWidth, unsigned int nHeight)
{
    if (g_pMediaEng && g_pMediaEng->HasPlayMedia())
    {
        g_pMediaEng->UpdateVideoScreen(g_hWndVideoScreen, nWidth, nHeight);
        SetWindowPos(g_hWndVideoScreen, HWND_TOP, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);
    }
}

void UpdateMainWindow(unsigned int nVideoW, unsigned int nVideoH, float scale)
{
    int offsetW = ((double)nVideoW*scale) - videoWndWidth;
    int offsetH = ((double)nVideoH*scale) - videoWndHeight;
    RECT rect;
    GetWindowRect(g_hWnd, &rect);
    int mainW = rect.right - rect.left;
    int mainH = rect.bottom - rect.top;
    mainW = mainW + offsetW;
    mainH = mainH + offsetH;

    int dwX = ::GetSystemMetrics(SM_CXSCREEN);
    int dwY = ::GetSystemMetrics(SM_CYSCREEN);
    if ((mainW > dwX) ||
        (mainH > dwY))
    {
        ShowWindow(g_hWnd, SW_MAXIMIZE);
        return;
    }

    int x = (dwX - (int)mainW) / 2;
    int y = (dwY - (int)mainH) / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    MoveWindow(g_hWnd, x, y, mainW, mainH, 1);
}

LRESULT CALLBACK ProgressWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            g_bIsSeeking = true;
            g_bTrackMouseDown = TRUE;
            DWORD CurrentPos, ClickPos;
            CurrentPos = GetProgress(g_hWndProgress);
            RECT rc;
            GetClientRect(g_hWndProgress, &rc);
            ClickPos = ((GET_X_LPARAM(lParam) - 9) * MAX_PROGRESS / (rc.right - 9 * 2));
            SendMessage(g_hWndProgress, TBM_SETPAGESIZE, 0, abs(int(ClickPos - CurrentPos)));
            break;
        }
        case WM_LBUTTONUP:
        {
            int nPos;
            nPos = GetProgress(g_hWndProgress);
            double ratio = (double)nPos / MAX_PROGRESS;
            unsigned int seekPos = mediaContentDuration*ratio;
            Seek(seekPos);
            g_bTrackMouseDown = FALSE;
            break;
        }
        case WM_PAINT:
        {
            SetFocus(g_hWnd);
            break;
        }
        case WM_TIMER:
        {
            //UpdatePlayPostion();
            break;
        }
        case WM_SETFOCUS:
        {
            return 1;
        }
        case WM_KILLFOCUS:
        {
            return 1;
        }
    }
    return CallWindowProc(oldProgressProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK VolumeWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDOWN:
            if (g_bSilence)return 0;
            DWORD CurrentPos, ClickPos;
            CurrentPos = GetProgress(g_hWndVolume);
            RECT rc;
            GetClientRect(g_hWndVolume, &rc);
            ClickPos = ((GET_X_LPARAM(lParam)-9) * MAX_PROGRESS / (rc.right-9*2));
            SendMessage(g_hWndVolume, TBM_SETPAGESIZE, 0, abs(int(ClickPos - CurrentPos)));
            break;
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
            if (g_bSilence)return 0;
            int nPos;
            nPos = GetProgress(g_hWndVolume);
            SetSystemVolumeValue(nPos);
            break;
        case WM_PAINT:
            SetFocus(g_hWnd);
            break;
        case WM_SETFOCUS:
            return 0;
    }
    return CallWindowProc(oldVolumeProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ToolbarWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        PostMessage(g_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;
    }
    return CallWindowProc(oldToolbarProc, hwnd, message, wParam, lParam);
}

void SetProgrss(HWND hWnd, int nValue)
{
    SendMessage(hWnd, TBM_SETPOS, 1, nValue);
}

int GetProgress(HWND hWnd)
{
    return SendMessage(hWnd, TBM_GETPOS, 0, 0);
}

BOOL SetToolBarState(int iButton, int iState)
{
    return SendMessage(g_hWndToolBar, TB_SETSTATE, iButton, MAKELONG(iState, 0));
}

void EnableMenu(int nID, bool flag)
{
    if(flag)
        EnableMenuItem(g_hMenu, nID, MF_ENABLED);
    else
        EnableMenuItem(g_hMenu, nID, MF_DISABLED | MF_GRAYED);
}

void CheckMenu(int nID, bool flag)
{
    if(flag)
        CheckMenuItem(g_hMenu, nID, MF_CHECKED);
    else
        CheckMenuItem(g_hMenu, nID, MF_UNCHECKED);
}

void EnableWindow(HWND hWnd, bool flag)
{
    SendMessage(hWnd, WM_ENABLE, flag, 0);
    SetFocus(g_hWnd);
}

void Play()
{
    if(g_TopMode == 2)
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    _tcscpy(g_StatusBarText1, TEXT("正在播放"));
    InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
    EnableWindow(g_hWndProgress);
    g_pMediaEng->Play();
}

void Pause()
{
    if(g_TopMode == 2)
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    _tcscpy(g_StatusBarText1, TEXT("已暂停"));
    InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
    g_pMediaEng->Pause();
}

void Stop()
{
    if(g_TopMode == 2)
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    _tcscpy(g_StatusBarText1, TEXT("已停止"));
    _tcscpy(g_StatusBarText2, TEXT(""));
    _tcscpy(g_PlaybakTimeText, TEXT(""));
    InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
    EnableWindow(g_hWndProgress, false);
    SetProgrss(g_hWndProgress, 0);
    if (g_pMediaEng) g_pMediaEng->Stop();
    KillTimer(g_hWnd, TIMER_GETPRGRESS);
}

void PlayOrPause()
{
    if (g_pMediaEng && (g_pMediaEng->HasPlayMedia()))
    {
        if (g_pMediaEng->IsPlaying())
        {
            _tcscpy(g_StatusBarText1, TEXT("已暂停"));
            InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
            SetToolBarState(TB_PAUSE, TBSTATE_CHECKED | TBSTATE_ENABLED);
            SetToolBarState(TB_PLAY, TBSTATE_ENABLED);
            Pause();
        }
        else
        {
            _tcscpy(g_StatusBarText1, TEXT("正在播放"));
            InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
            SetToolBarState(TB_PLAY, TBSTATE_CHECKED | TBSTATE_ENABLED);
            SetToolBarState(TB_PAUSE, TBSTATE_ENABLED);
            Play();
        }
    }
}

void Close()
{
    _tcscpy(g_StatusBarText1, TEXT("已停止"));
    SetWindowText(g_hWnd, TEXT("EPlayer"));
    g_hIcon = NULL;
    InvalidateRect(g_hWndPanel, NULL, false);
    KillTimer(g_hWnd, TIMER_GETPRGRESS);
    //g_pMediaEng->Reset();
    EnableMenu(IDM_FILE_CLOSE, FALSE);
    EnableMenu(IDM_FILE_SHOT, FALSE);
    EnableMenu(IDM_PLAY_PAUSE, FALSE);
    EnableMenu(IDM_STOP, FALSE);
    EnableMenu(IDM_NEXTFRAME, FALSE);
    EnableMenu(IDM_DECREASERATE, FALSE);
    EnableMenu(IDM_INCREASERATE, FALSE);
    EnableMenu(IDM_RESETRATE, FALSE);
    EnableMenu(IDM_SCREEN_DEFAULT, FALSE);
    EnableMenu(IDM_SCREEN_STRETCH, FALSE);
    EnableMenu(IDM_SCREEN_4_3, FALSE);
    EnableMenu(IDM_SCREEN_5_4, FALSE);
    EnableMenu(IDM_SCREEN_16_9, FALSE);
    EnableMenu(IDM_SIZE_50, FALSE);
    EnableMenu(IDM_SIZE_75, FALSE);
    EnableMenu(IDM_SIZE_100, FALSE);
    EnableMenu(IDM_SIZE_150, FALSE);
    EnableMenu(IDM_SIZE_FULL, FALSE);

    SetToolBarState(TB_PLAY, 0);
    SetToolBarState(TB_PAUSE, 0);
    SetToolBarState(TB_STOP, 0);
    SetToolBarState(TB_PREV, 0);
    SetToolBarState(TB_DECREASERATE, 0);
    SetToolBarState(TB_INCREASERATE, 0);
    SetToolBarState(TB_NEXT, 0);
    SetToolBarState(TB_NEXTFRAME, 0);
    Stop();

    if (g_bVideoFullScreen)
        VideoExitFullScreen();

    RECT rect;
    GetWindowRect(g_hWnd, &rect);
    int x = rect.left;
    int y = rect.top;
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    SetWindowPos(g_hWnd, NULL, x+1, y+1, w, h, SWP_SHOWWINDOW);
    ShowWindow(g_hWndVideoScreen, SW_HIDE);
    SetFocus(g_hWnd);
    //UpdateWindow(g_hWndVideoScreen);
    //float rat = 0.618;
    //DWORD sw = ::GetSystemMetrics(SM_CXSCREEN);
    //DWORD sh = ::GetSystemMetrics(SM_CYSCREEN);
    //int nWndHeight = sh * rat;
    //int nWndWidth = (nWndHeight * 3 / 2);
    //int x = (sw - nWndWidth) / 2;
    //int y = (sh - nWndHeight) / 2;

    //RECT rect;
    //GetWindowRect(g_hWndVideoScreen, &rect);
    //InvalidateRect(g_hWndVideoScreen, &rect, true);
    //MoveWindow(g_hWnd, x, y, nWndWidth, nWndHeight, TRUE);
    //SetFocus(g_hWnd);
}

void CenterWindow(HWND hwnd)
{
    DWORD dwX = GetSystemMetrics(SM_CXSCREEN);
    DWORD dwY = GetSystemMetrics(SM_CYSCREEN);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd,
              (dwX-(rect.right-rect.left))/2,
              (dwY-(rect.bottom-rect.top))/2,
              (rect.right-rect.left), rect.bottom-rect.top, FALSE);
}

bool PlayFile(const TCHAR* pszFileName)
{
    int iLength = WideCharToMultiByte(CP_ACP, 0, pszFileName, -1, NULL, 0, NULL, NULL);
    char *pMediaPath = NULL;
    pMediaPath = new char[iLength * 4];
    pMediaPath[iLength * 4 - 1] = 0;
    WideCharToMultiByte(CP_ACP, 0, pszFileName, -1, pMediaPath, iLength, NULL, NULL);

    if (g_hWndVideoScreen == NULL)
        CreateVideoScreenWindow(g_hWnd);
    else
        ShowWindow(g_hWndVideoScreen, SW_SHOW);

    if (g_pMediaEng == NULL)
    {
        g_pMediaEng = new MediaEngine(g_hWndVideoScreen, videoWndWidth, videoWndHeight);
        MediaNotifier  notify;
        notify.pUserData = NULL;
        notify.Callback = MediaoNotifyHandler;
        g_pMediaEng->SetMediaNotify(&notify);
    }

    if (g_pMediaEng)
    {
        if (g_pMediaEng->HasPlayMedia())
        {
            g_pMediaEng->Stop();
            Sleep(100);
        }
    }

    int nRet = g_pMediaEng->OpenMedia(pMediaPath);

    _tcscpy(g_StatusBarText2, TEXT(""));
    InvalidateRect(g_hWndPanel, &rt_StatusBarText2, false);
    if (nRet != 0)
    {
        _tcscpy(g_StatusBarText2, TEXT("媒体文件播放错误"));
        InvalidateRect(g_hWndPanel, &rt_StatusBarText2, false);
        Close();
        return FALSE;
    }
    g_pMediaEng->Play();

    _tcscpy(g_StatusBarText1, TEXT("正在播放"));
    InvalidateRect(g_hWndPanel, &rt_StatusBarText1, false);
    SetToolBarState(TB_PLAY, TBSTATE_CHECKED | TBSTATE_ENABLED);
    SetToolBarState(TB_PAUSE, TBSTATE_ENABLED);
    SetToolBarState(TB_STOP, TBSTATE_ENABLED);
    EnableWindow(g_hWndProgress, true);

    mediaContentDuration = g_pMediaEng->GetMediaDuration();
    SetMediaDuration(mediaContentDuration);
    SetTimer(g_hWnd, TIMER_GETPRGRESS, 50, NULL);

    if (g_pMediaEng->HasVideo())
        oldVideoScreenProc = (WNDPROC)SetWindowLong(g_hWndVideoScreen, GWL_WNDPROC, (LONG)VideoScreenWndProc);

    EnableMenu(IDM_STOP, TRUE);
    EnableMenu(IDM_SIZE_50, TRUE);
    EnableMenu(IDM_SIZE_75, TRUE);
    EnableMenu(IDM_SIZE_100, TRUE);
    EnableMenu(IDM_SIZE_150, TRUE);
    EnableMenu(IDM_SIZE_FULL, TRUE);
    EnableMenu(IDM_FILE_CLOSE, TRUE);
    EnableMenu(IDM_PLAY_PAUSE, TRUE);

    if(g_TopMode == 2)
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    else
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

    ShowWindow(g_hWndVideoScreen, SW_HIDE);
    g_pMediaEng->GetMediaInfo(&g_MediaInfo);
	if (g_MediaInfo.hasVideo)
	{
		UpdateMainWindow(g_MediaInfo.nVideoWidth, g_MediaInfo.nVideoHeight, 1.0);
		ShowWindow(g_hWndVideoScreen, SW_SHOW);
	}

    TCHAR szTitle[100] = {0};
    SplitFileName(pszFileName, 1, szTitle);
    swprintf(szTitle, TEXT("%s"), szTitle);
    SetWindowText(g_hWnd, szTitle);

    return TRUE;
}

void DecreaseRate()
{
    //double rate = g_DXGraph.GetRate();
    //if(rate)
    //    g_DXGraph.SetRate(rate - 0.25);
}

void IncreaseRate()
{
    //double rate = g_DXGraph.GetRate();
    //if(rate)
    //    g_DXGraph.SetRate(rate + 0.25);
}

void ResetRate()
{
    //g_DXGraph.SetRate(1);
}

void StepFrame()
{
    //SetToolBarState(TB_PAUSE, TBSTATE_CHECKED | TBSTATE_ENABLED);
    //SetToolBarState(TB_PLAY, TBSTATE_ENABLED);
    //Pause();
    //g_DXGraph.StepFrame(1);
}

void PlayPrev()
{
}

void PlayNext(bool random)
{
}

void SetTopMode(int nMode)
{
    g_TopMode = nMode;
    if(g_TopMode == 0)
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    else if(g_TopMode == 1)
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}

void CreatePanel(HWND hWndParent)
{
    //创建窗口下方的控制栏
    g_hWndPanel = CreateDialog(g_hInstance, LPCTSTR(IDD_DLG_PANEL), hWndParent, (DLGPROC)PanelWndProc);//创建控制栏

    g_hWndProgress = GetDlgItem(g_hWndPanel, IDC_SLIDER1);
    SendMessage(g_hWndProgress, TBM_SETRANGE, true, (LPARAM)MAKELONG(0, MAX_PROGRESS));
    EnableWindow(g_hWndProgress, false);
    oldProgressProc = (WNDPROC)SetWindowLong(g_hWndProgress, GWL_WNDPROC, (LONG)ProgressWndProc);
    g_bIsSeeking = false;

    TBBUTTON tbb[12];
    tbb[0].iBitmap = TB_PLAY;
    tbb[0].idCommand = TB_PLAY;
    tbb[0].fsState = 0;
    tbb[0].fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECKGROUP;
    tbb[0].dwData = 0;
    tbb[0].iString = (INT_PTR)L"播放";

    tbb[1].iBitmap = TB_PAUSE;
    tbb[1].idCommand = TB_PAUSE;
    tbb[1].fsState = 0;
    tbb[1].fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECKGROUP;
    tbb[1].dwData = 0;
    tbb[1].iString = (INT_PTR)L"暂停";

    tbb[2].iBitmap = TB_STOP;
    tbb[2].idCommand = TB_STOP;
    tbb[2].fsState = 0;
    tbb[2].fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECKGROUP;
    tbb[2].dwData = 0;
    tbb[2].iString = (INT_PTR)L"停止";

    tbb[3].iBitmap = TB_SEPARATE;
    tbb[3].idCommand = TB_SEPARATE;
    tbb[3].fsState = TBSTATE_ENABLED;
    tbb[3].fsStyle = TBSTYLE_SEP;
    tbb[3].dwData = 0;
    tbb[3].iString = 0;

    tbb[4].iBitmap = TB_PREV;
    tbb[4].idCommand = TB_PREV;
    tbb[4].fsState = 0;
    tbb[4].fsStyle = TBSTYLE_BUTTON;
    tbb[4].dwData = 0;
    tbb[4].iString = (INT_PTR)L"上一个";

    tbb[5].iBitmap = TB_DECREASERATE;
    tbb[5].idCommand = TB_DECREASERATE;
    tbb[5].fsState = 0;
    tbb[5].fsStyle = TBSTYLE_BUTTON;
    tbb[5].dwData = 0;
    tbb[5].iString = (INT_PTR)L"减速播放";

    tbb[6].iBitmap = TB_INCREASERATE;
    tbb[6].idCommand = TB_INCREASERATE;
    tbb[6].fsState = 0;
    tbb[6].fsStyle = TBSTYLE_BUTTON;
    tbb[6].dwData = 0;
    tbb[6].iString = (INT_PTR)L"加速播放";

    tbb[7].iBitmap = TB_NEXT;
    tbb[7].idCommand = TB_NEXT;
    tbb[7].fsState = 0;
    tbb[7].fsStyle = TBSTYLE_BUTTON;
    tbb[7].dwData = 0;
    tbb[7].iString = (INT_PTR)L"下一个";

    tbb[8].iBitmap = TB_SEPARATE;
    tbb[8].idCommand = TB_SEPARATE;
    tbb[8].fsState = TBSTATE_ENABLED;
    tbb[8].fsStyle = TBSTYLE_SEP;
    tbb[8].dwData = 0;
    tbb[8].iString = 0;

    tbb[9].iBitmap = TB_NEXTFRAME;
    tbb[9].idCommand = TB_NEXTFRAME;
    tbb[9].fsState = 0;
    tbb[9].fsStyle = TBSTYLE_BUTTON;
    tbb[9].dwData = 0;
    tbb[9].iString = (INT_PTR)L"步进";

    tbb[10].iBitmap = TB_SEPARATE;
    tbb[10].idCommand = TB_SEPARATE;
    tbb[10].fsState = TBSTATE_ENABLED;
    tbb[10].fsStyle = TBSTYLE_SEP;
    tbb[10].dwData = 0;
    tbb[10].iString = 0;

    tbb[11].iBitmap = TB_SOUND;
    tbb[11].idCommand = TB_SOUND;
    tbb[11].fsState = TBSTATE_ENABLED;
    tbb[11].fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECK;
    tbb[11].dwData = 0;
    tbb[11].iString = (INT_PTR)L"静音";

    g_hWndToolBar = CreateToolbarEx(g_hWndPanel,
                                    WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | CCS_NODIVIDER | TBSTYLE_TOOLTIPS | CCS_NOMOVEY,
                                    0,
                                    sizeof(tbb)/sizeof(TBBUTTON),
                                    g_hInstance,
                                    IDB_BITMAP_TOOLBAR,
                                    tbb, sizeof(tbb)/sizeof(TBBUTTON),
                                    16, 16, 0, 0,
                                    sizeof(TBBUTTON));
    SendMessage(g_hWndToolBar, TB_SETMAXTEXTROWS, 0, 0);
    oldToolbarProc = (WNDPROC)SetWindowLong(g_hWndToolBar, GWL_WNDPROC, (LONG)ToolbarWndProc);
    MoveWindow(g_hWndToolBar, 0, 21, 0, 0, true);

    g_hWndVolume = CreateWindowEx(0,
                                  TRACKBAR_CLASS,
                                  TEXT("VolumeBar"),
                                  WS_CHILD | WS_VISIBLE | 0x10,
                                  232, 3, 80, 20, g_hWndToolBar,
                                  0, g_hInstance, NULL);
    InitSystemVolumeControl();
    SendMessage(g_hWndVolume, TBM_SETRANGE, true, (LPARAM)MAKELONG(0, MAX_PROGRESS));
    SetProgrss(g_hWndVolume, MAX_PROGRESS/2);
    SetSystemVolumeMute(false);
    SetSystemVolumeValue(MAX_PROGRESS/2);
    oldVolumeProc = (WNDPROC)SetWindowLong(g_hWndVolume, GWL_WNDPROC, (LONG)VolumeWndProc);
}

void CreateVideoScreenWindow(HWND hWndParent)
{
    WNDCLASSEX   wndclassex = { 0 };
    wndclassex.cbClsExtra = 0;
    wndclassex.cbWndExtra = 0;
    wndclassex.lpfnWndProc = DefWindowProc;//VideoScreenWndProc;
    wndclassex.lpszClassName = TEXT("VideoScreenWndClass");
    wndclassex.hInstance = g_hInstance;
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    //wndclassex.hCursor = LoadCursor(NULL, IDC_HAND);
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassEx(&wndclassex);

    // Here we set the video screen window width & height to 0x0.
    // if we start playback new file, set new size again.
    g_bVideoFullScreen = false;
    DWORD dwX = GetSystemMetrics(SM_CXSCREEN);
    DWORD dwY = GetSystemMetrics(SM_CYSCREEN);
    g_hWndVideoScreen = CreateWindowEx(0, TEXT("VideoScreenWndClass"),
                                       NULL, WS_CHILD | WS_VISIBLE | 0x10,
                                       0, 0, dwX, dwY, hWndParent, NULL, g_hInstance, NULL);
    SetWindowPos(g_hWndVideoScreen, HWND_TOP, 0, 0, dwX, dwY, SWP_SHOWWINDOW);
}

void SplitFileName(const TCHAR* pFileName, int nFlag, TCHAR* pReturn)
{
    int nSize = _tcsclen(pFileName);
    switch(nFlag)
    {
        case 0://取路径
            _tcscpy(pReturn, pFileName);
            while(--nSize)
            {
                if(pReturn[nSize] == '\\')
                    break;
                pReturn[nSize] = 0;
            }
            break;
        case 1://取文件名
            int i;
            i = 0;
            while(--nSize)
            {
                if(pFileName[nSize] == '\\')
                    break;
                i++;
            }
            _tcscpy(pReturn, pFileName + _tcsclen(pFileName) - i);
            break;
        case 2://取扩展名
            break;
    }
}

void UpdatePlayPostion()
{
    if (g_pMediaEng && g_pMediaEng->IsPlaying())
    {
        unsigned int pos = g_pMediaEng->GetPlayPosition();
        unsigned int t = pos / 1000;
        unsigned int hh = t / 3600;
        unsigned int mm = (t % 3600) / 60;
        unsigned int ss = t % 60;
        swprintf(g_PlaybakPositionText, TEXT("%02d:%02d:%02d"), hh, mm, ss);
        swprintf(g_PlaybakTimeText, TEXT("%s / %s"), g_PlaybakPositionText, g_MediaDurationText);
        InvalidateRect(g_hWndPanel, &rt_StatusBarText3, false);
        if (!g_bIsSeeking)
        {
            double rat = (double)pos / (double)mediaContentDuration;
            unsigned int progress = MAX_PROGRESS * rat;
            UpdatePlayProgressBar(progress);
        }
    }
}

void SetMediaDuration(unsigned int duration)
{
    unsigned int t = duration / 1000;
    unsigned int hh = t / 3600;
    unsigned int mm = (t % 3600) / 60;
    unsigned int ss = t % 60;
    swprintf(g_MediaDurationText, TEXT("%02d:%02d:%02d"), hh, mm, ss);
}

void Seek(unsigned int seekPos)
{
    if (g_pMediaEng && g_pMediaEng->HasPlayMedia())
    {
        g_pMediaEng->Seek(seekPos);
		g_bIsSeeking = false;
    }
}

void UpdatePlayProgressBar(unsigned int pos)
{
    SetProgrss(g_hWndProgress, pos);
}

void MediaoNotifyHandler(void* pUserData, unsigned int nMsg)
{
    switch (nMsg)
    {
        case EPlayer_Msg_PlayStart:
        {
            DebugPrintf("======Play start===========\n");
            break;
        }
        case EPlayer_Msg_PlayStop:
        {
            Close();
            break;
        }
        default: break;
    }
}

void VideoEntryFullScreen()
{
    if (g_bVideoFullScreen) return;
    if (g_pMediaEng &&
        g_pMediaEng->HasPlayMedia())
    {
        SetParent(g_hWndVideoScreen, GetDesktopWindow());
        SetWindowLong(g_hWndVideoScreen, GWL_STYLE,
                      GetWindowLong(g_hWndVideoScreen, GWL_STYLE) | WS_POPUP);
        SetWindowPos(g_hWndVideoScreen, HWND_TOPMOST,
                     0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
        int fw = ::GetSystemMetrics(SM_CXSCREEN);
        int fh = ::GetSystemMetrics(SM_CYSCREEN);
        UpdateVideoScreen(fw, fh);
        ShowWindow(g_hWnd, SW_HIDE);
        ControlPanelEntryFullScreen();
        entyFullScreenTime = GetTickCount();
        SetTimer(g_hWndVideoScreen, TIMER_MOUSEMOV, 500, NULL);
        g_bVideoFullScreen = true;
    }
}

void VideoExitFullScreen()
{
    if (g_bVideoFullScreen)
    {
        SetWindowPos(g_hWndVideoScreen, HWND_NOTOPMOST,
                     0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
        ShowWindow(g_hWnd, SW_SHOW);
        SetParent(g_hWndVideoScreen, g_hWnd);
        UpdateVideoScreen(videoWndWidth, videoWndHeight);
        ControlPanelExitFullScreen();
        KillTimer(g_hWndVideoScreen, TIMER_MOUSEMOV);
        g_bVideoFullScreen = false;
    }
}

void ControlPanelEntryFullScreen()
{
    RECT panelRect;
    int fw = ::GetSystemMetrics(SM_CXSCREEN);
    int fh = ::GetSystemMetrics(SM_CYSCREEN);
    int x = 0;
    int y = fh - ctrlPanelWndHeight;
    int w = fw;
    int h = ctrlPanelWndWidth;
    rt_StatusBarText3.left = fw - 120;
    rt_StatusBarText3.right = fw - 10;

    GetWindowPlacement(g_hWndPanel, &ctrlPanelState);
    SetParent(g_hWndPanel, NULL);
    LONG style = GetWindowLong(g_hWndPanel, GWL_STYLE);
    SetWindowLong(g_hWndPanel, GWL_STYLE, style | WS_POPUP);
    SetWindowLong(g_hWndPanel, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
    SetWindowPos(g_hWndPanel, HWND_TOPMOST,
                 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    SetWindowPos(g_hWndPanel, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
    ShowWindow(g_hWndPanel, SW_HIDE);
    SetFocus(g_hWndVideoScreen);
}

void ControlPanelExitFullScreen()
{
    SetParent(g_hWndPanel, g_hWnd);
    SetWindowPlacement(g_hWndPanel, &ctrlPanelState);
}

bool IsPointInCtrlPanelRect(POINT point)
{
    RECT rect;
    GetWindowRect(g_hWndPanel, &rect);
    if ((point.x >= rect.left) &&
        (point.x < rect.right) &&
        (point.y >= rect.top) &&
        (point.y < rect.bottom))
        return true;
    return false;
}

void ShowFullScreenCtrlPanel()
{
    if (g_bVideoFullScreen)
    {
        ShowWindow(g_hWndPanel, SW_SHOW);
    }
}
void HideFullScreenCtrlPanel()
{
    if (g_bVideoFullScreen)
    {
        ShowWindow(g_hWndPanel, SW_HIDE);
    }
}

/* volume */
BOOL amdInitialize()
{
    //获取当前混合设备数量
    g_nNumMixers = ::mixerGetNumDevs();
    g_hMixer = NULL;
    ::ZeroMemory(&g_mxcaps, sizeof(MIXERCAPS));

    if (g_nNumMixers != 0)
    {
        //打开混合设备
        if (::mixerOpen(&g_hMixer,
            0,
            (DWORD)g_hWnd,
            NULL,
            MIXER_OBJECTF_MIXER | CALLBACK_WINDOW)
            != MMSYSERR_NOERROR)
            return FALSE;
        // 获取混合器性能
        if (::mixerGetDevCaps((UINT)g_hMixer, &g_mxcaps, sizeof(MIXERCAPS))
            != MMSYSERR_NOERROR)
            return FALSE;
    }
    return TRUE;
}

BOOL amdUninitialize()
{
    BOOL bSucc = TRUE;
    if (g_hMixer != NULL)
    {
        //关闭混合器
        bSucc = ::mixerClose(g_hMixer) == MMSYSERR_NOERROR;
        g_hMixer = NULL;
    }
    return bSucc;
}

BOOL amdGetMasterVolumeControl()
{
    if (g_hMixer == NULL) return FALSE;
    //获得混合器性能
    MIXERLINE mxl;
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    if (::mixerGetLineInfo((HMIXEROBJ)g_hMixer,
        &mxl,
        MIXER_OBJECTF_HMIXER |
        MIXER_GETLINEINFOF_COMPONENTTYPE)
        != MMSYSERR_NOERROR)
        return FALSE;

    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = mxl.dwLineID;
    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;
    //获得混合器线控件
    if (::mixerGetLineControls((HMIXEROBJ)g_hMixer,
        &mxlc,
        MIXER_OBJECTF_HMIXER |
        MIXER_GETLINECONTROLSF_ONEBYTYPE)
        != MMSYSERR_NOERROR)
        return FALSE;
    //记录控件的信息
    g_dwMinimum = mxc.Bounds.dwMinimum;
    g_dwMaximum = mxc.Bounds.dwMaximum;
    g_dwVolumeControlID = mxc.dwControlID;
    return TRUE;
}

BOOL amdGetMasterVolumeValue(DWORD &dwVal)
{
    if (g_hMixer == NULL )return FALSE;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = g_dwVolumeControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    //获取指定混合器控件
    if (::mixerGetControlDetails((HMIXEROBJ)g_hMixer,
        &mxcd,
        MIXER_OBJECTF_HMIXER |
        MIXER_GETCONTROLDETAILSF_VALUE)
        != MMSYSERR_NOERROR)
        return FALSE;
    dwVal = mxcdVolume.dwValue;
    return TRUE;
}

BOOL amdSetMasterVolumeValue(DWORD dwVal)
{
    if (g_hMixer == NULL )return FALSE;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = g_dwVolumeControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    //放置混合器控件
    if (::mixerSetControlDetails((HMIXEROBJ)g_hMixer,
        &mxcd,
        MIXER_OBJECTF_HMIXER |
        MIXER_SETCONTROLDETAILSF_VALUE)
        != MMSYSERR_NOERROR)
        return FALSE;

    return TRUE;
}

BOOL amdIsMasterVolumeMute()
{
    MIXERLINE mxl;
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    if (::mixerGetLineInfo((HMIXEROBJ)g_hMixer,
        &mxl,
        MIXER_OBJECTF_HMIXER |
        MIXER_GETLINEINFOF_COMPONENTTYPE)
        != MMSYSERR_NOERROR)
        return FALSE;

    BOOL bRet;
    MIXERCONTROL mc;
    MIXERLINECONTROLS mlcs;
    MIXERCONTROLDETAILS mcd;
    MIXERCONTROLDETAILS_BOOLEAN mcdMute;
    RtlZeroMemory(&mlcs, sizeof(MIXERLINECONTROLS));
    mlcs.cbStruct = sizeof(MIXERLINECONTROLS);
    mlcs.dwLineID = mxl.dwLineID;
    mlcs.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
    mlcs.cControls = 1;
    mlcs.cbmxctrl = sizeof(MIXERCONTROL);
    mlcs.pamxctrl = &mc;
    if (MMSYSERR_NOERROR == mixerGetLineControls((HMIXEROBJ)g_hMixer, &mlcs, MIXER_GETLINECONTROLSF_ONEBYTYPE))
    {
        RtlZeroMemory(&mcd, sizeof(MIXERCONTROLDETAILS));
        mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
        mcd.dwControlID = mc.dwControlID;
        mcd.cChannels = 1;
        mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mcd.paDetails = &mcdMute;

        if (MMSYSERR_NOERROR == mixerGetControlDetails((HMIXEROBJ)g_hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE))
        {
            bRet = mcdMute.fValue;
        }
    }
    return bRet;
}

BOOL amdSetMasterVolumeMute(BOOL mute)
{
    if (g_hMixer == NULL)return FALSE;

    MIXERCONTROL mc;
    MIXERLINE mixerline;
    MIXERLINECONTROLS mlcs;
    MIXERCONTROLDETAILS mcd;
    MIXERCONTROLDETAILS_BOOLEAN mcdMute;
    RtlZeroMemory(&mixerline, sizeof(MIXERLINE));
    mixerline.cbStruct = sizeof(MIXERLINE);
    mixerline.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    if (MMSYSERR_NOERROR == mixerGetLineInfo((HMIXEROBJ)g_hMixer, &mixerline, MIXER_GETLINEINFOF_COMPONENTTYPE))
    {
        RtlZeroMemory(&mlcs, sizeof(MIXERLINECONTROLS));
        mlcs.cbStruct = sizeof(MIXERLINECONTROLS);
        mlcs.dwLineID = mixerline.dwLineID;
        mlcs.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
        mlcs.cControls = 1;
        mlcs.cbmxctrl = sizeof(MIXERCONTROL);
        mlcs.pamxctrl = &mc;
        if (MMSYSERR_NOERROR == mixerGetLineControls((HMIXEROBJ)g_hMixer, &mlcs, MIXER_GETLINECONTROLSF_ONEBYTYPE))
        {
            RtlZeroMemory(&mcd, sizeof(MIXERCONTROLDETAILS));
            mcdMute.fValue = mute;
            mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mcd.dwControlID = mc.dwControlID;
            mcd.cChannels = 1;
            mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
            mcd.paDetails = &mcdMute;
            if (MMSYSERR_NOERROR == mixerSetControlDetails((HMIXEROBJ)g_hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void InitSystemVolumeControl()
{
    g_dwMinimum = 0;
    g_dwMaximum = 10000;
    if(amdInitialize())
    {
        amdGetMasterVolumeControl();
    }
}

void UninitSystemVolumeControl()
{
    amdUninitialize();
}

void SetSystemVolumeMute(BOOL mute)
{
    amdSetMasterVolumeMute(mute);
}

void SetSystemVolumeValue(DWORD value)
{
    double rat = (double)value/MAX_PROGRESS;
    amdGetMasterVolumeControl();
    amdSetMasterVolumeValue(rat * g_dwMaximum);
}

void DebugPrintf(char* lpszFormat, ...)
{
    va_list   args;
    int       nBuf;
    char     szBuffer[512];

    va_start(args, lpszFormat);

    nBuf = _vsnprintf(szBuffer, sizeof(szBuffer)*sizeof(TCHAR), lpszFormat, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
}
