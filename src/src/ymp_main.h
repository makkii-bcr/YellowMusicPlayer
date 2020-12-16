#ifndef YMP_MAIN_H
#define YMP_MAIN_H

#include <windows.h>
#include "define.h"
#include "ymp_midi.h"
#include "ymp_ogg.h"

class YMPlayer {
public:
    int ympInit(const HWND hWnd);

    HWND getHWnd() { return m_hWnd; }
    YmpMidi getYmpMidi() { return m_ympMidi; }
    YmpOgg getYmpOgg() { return m_ympOgg; }
private:
    HWND m_hWnd;
    YmpMidi m_ympMidi;
    YmpOgg m_ympOgg;
    TCHAR m_eff_midiAddr[FILEPATH_SIZE];
};

#endif