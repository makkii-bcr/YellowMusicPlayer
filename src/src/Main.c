/*
  YellowMusicPlayer 2.40  -  Copyright (C) 2012 - 2020 makkii_bcr
*/

#include <tchar.h>
#include <windows.h>

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    MSG msg;
    WNDCLASS winc;
    
    winc.style         = (CS_HREDRAW | CS_VREDRAW)*0;
    winc.lpfnWndProc   = WndProc;
    winc.cbClsExtra    = 0;
    winc.cbWndExtra    = 0;
    winc.hInstance     = hInstance;
    winc.hIcon         = NULL;
    winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszMenuName  = NULL;
    winc.lpszClassName = _T("WNDCLASS");
    
    if (!RegisterClass(&winc)) return 0;
    
    hWnd = CreateWindow(_T("WNDCLASS"), _T("YellowMusicPlayer"), WS_OVERLAPPEDWINDOW,
                        0, 0, 350, 150, NULL, NULL, hInstance, NULL);
    
    if (hWnd == NULL) return 0;
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    while (GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
    }
    
    return DefWindowProc(hWnd, msg, wp, lp);
}