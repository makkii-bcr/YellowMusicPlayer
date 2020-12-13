#include "define.h"
#include "ymp_main.h"

static void CALLBACK timerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef struct {
    HWND hWnd;
} TimerMsg;

static TimerMsg tMsg;

int ympMain(HWND hWnd) {
    tMsg.hWnd = hWnd;
    DWORD mmResult = timeSetEvent(15, 0, timerProc, (DWORD_PTR)&tMsg, TIME_PERIODIC);
    return 0;
}

static void CALLBACK timerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
    TimerMsg &tmsg = *(reinterpret_cast<TimerMsg *>(dwUser));
    HWND hWnd = tmsg.hWnd;
    PostMessage(hWnd, YMP_MSG_TEST1, 0, 0);
}



