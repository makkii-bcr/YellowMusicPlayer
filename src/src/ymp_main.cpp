#include "ymp_main.h"
#include "ymp_midi.h"
#include "ymp_ogg.h"

static void CALLBACK timerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

int YMPlayer::ympInit(const HWND hWnd) {
    // �ݒ�t�@�C���̓ǂݍ���
    // Midi�̏���
    // Ogg�̏���

    int midiOK = m_ympMidi.init();
    int oggOK = m_ympOgg.init();

    m_hWnd = hWnd;
    DWORD mmResult = timeSetEvent(15, 0, timerProc, (DWORD_PTR)this, TIME_PERIODIC);
    return 0;
}

static void CALLBACK timerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
    YMPlayer &ymplayer = *((YMPlayer *)dwUser);
    HWND hWnd = ymplayer.getHWnd();
    PostMessage(hWnd, YMP_MSG_TEST1, 0, 0);

    // �t�@�C���̓ǂݍ���
    // Midi�̏���
    // Ogg�̏���

}
