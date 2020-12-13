/*
  YellowMusicPlayer 2.40  -  Copyright (C) 2012 - 2020 makkii_bcr
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include "define.h"
#include "main.h"
#include "resource.h"
#include "ymp_main.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc;
    wc.style         = (CS_HREDRAW | CS_VREDRAW)*0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_1));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("WNDCLASS");

    if (!RegisterClass(&wc)) return 0;

    HWND hWnd = CreateWindow(_T("WNDCLASS"), _T("YellowMusicPlayer"), WS_OVERLAPPEDWINDOW,
                        0, 0, 350, 150, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // タイマー分解能の精度を高める
    TIMECAPS timeCaps;
    MMRESULT timerCapsResult = timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
    if (timerCapsResult == TIMERR_NOERROR) {
        timeBeginPeriod(timeCaps.wPeriodMin);
    }

    if (ympMain(hWnd)) {
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // タイマー分解能の精度を元に戻す
    if (timerCapsResult == TIMERR_NOERROR) {
        timeEndPeriod(timeCaps.wPeriodMin);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CLOSE:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case YMP_MSG_TEST1: {
        TCHAR str[BUF_SIZE_PRINT];
        DWORD cnt = timeGetTime();
        _sntprintf(str, sizeof(str), "Test: %d", cnt);
        SetWindowText(hWnd, str);
        break;
    }
    }
    
    return DefWindowProc(hWnd, msg, wp, lp);
}