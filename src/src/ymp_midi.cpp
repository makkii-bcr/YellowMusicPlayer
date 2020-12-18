#include "ymp_midi.h"
#include "GuruGuruSMF4_Cpp.h"

int YmpMidi::init(const HWND hWnd) {
    GuruGuruSmf::Ggs4 &ggs = *(GuruGuruSmf::Ggs4::GetInstance());
    // GuruGuruSmf::GGSERROR ggsError = ggs.OpenDevice(-1, hWnd);
    GuruGuruSmf::GGSERROR ggsError = ggs.OpenDevice(GuruGuruSmf::Device::DirectMusic, hWnd);

    // if (*DirectMusicOn) {
    //     errorFlag = GGS4OpenDevice(GGSDEVICE_DIRECTMUSIC, hWnd); // DirectMusic
    //     if (errorFlag != GGSERROR_NOERROR) *DirectMusicOn = 0; // ÉGÉâÅ[Ç»ÇÁDirectMusicÇOFFÇ…ÇµÇƒçƒééçs
    // }
    // if (*DirectMusicOn == 0) errorFlag = GGS4OpenDevice(-1, hWnd); // MIDI_MAPPER
    
    // if (errorFlag != GGSERROR_NOERROR) {
    //     //return FALSE;
    // }
    return 0;
}
