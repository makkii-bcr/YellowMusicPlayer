/********************************/ #ifndef OGGFUNC_H /*****************************/
#define OGGFUNC_H

#include <dsound.h>
#include <math.h>
#include <windows.h>
#include "vorbis/vorbisfile.h"
//#include "Global.h"

#pragma comment(lib, "ogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")

extern BOOL  windowClose;
extern BOOL  threadClose;
extern char *cmdLineStr;

extern IDirectSound8       *pDS8;
extern IDirectSoundBuffer  *ptmpBuf;
extern IDirectSoundBuffer8 *pDSBuffer;
extern DSBUFFERDESC         DSBufferDesc;
extern OggVorbis_File       ovf;
extern vorbis_info         *oggInfo;
extern WAVEFORMATEX         waveFormat;
extern void*  AP1, *AP2;
extern DWORD  AB1,  AB2;
extern DWORD  oggSect;
extern DWORD  oggSectSize;
extern DWORD  oggFlag;
extern BOOL   oggAllWriteFlag;
extern BOOL   oggEndFlag;
extern DWORD  oggEndCnt;
extern double oggLoopSeek;
extern double oggLoopEndSeek;
extern int    oggLoopSeekInt;
extern int    oggLoopEndSeekInt;
extern int    oggFileLoopTime;
extern int    oggFileLoopEndTime;
extern double oggStartSeek;
extern DWORD  oggPoint;
extern BOOL   oggLoopFlag;
extern int    oggPlayerStatus;
extern int    oggPlayTime;
extern BOOL   oggFileRead;
extern double oggTotalTime;
extern int    oggTotalPcm;
extern double oggPlayingTime;
extern int    oggBufLoopCnt;
extern int    oggLoopCnt;
extern int    oggLoopCntPointAry[256];
extern int    oggLoopCntPointAryIdx;
extern int    oggLoopCntPointAryIdx2;
extern int    oggFadeOutTimeRun; 
extern int    oggFadeInTimeRun; 
extern double oggFadeCnt; 
extern DWORD  oggTotalTimeInt;
extern DWORD  oggPlayingTimeInt;
extern DWORD  oggPlayingTimeInt2;
extern BOOL   oggPauseFlag;
extern int    oggBufWriteSpeed;


extern int    ogg3LoopFlag;
extern int    ogg3Volume;
extern int    ogg3FadeOutTime;

// イベント関連
extern HANDLE g_hNotificationEvent;
extern DWORD  g_dwNotifyThreadID;
extern HANDLE g_hNotifyThread;
extern LPDIRECTSOUNDNOTIFY pDSNotify;
extern DSBPOSITIONNOTIFY   aPosNotify[1];
extern HRESULT             hr;
extern BOOL oggDSLoopCnt, oggDSLoopCnt_old;

typedef enum {
	STATUS_STOP,
	STATUS_PLAY,
	STATUS_PAUSE,
} PLAYERSTATUS;



BOOL createDS(IDirectSound8 **ppDS8, HWND hWnd);
BOOL openOggFile(char *fileName, OggVorbis_File *pOvf, vorbis_info *pOggInfo, WAVEFORMATEX *pWaveFormat);
BOOL createDSBuffer(DSBUFFERDESC *pDSBufferDesc, WAVEFORMATEX *pWaveFormat,
                    IDirectSound8 *pDS8, IDirectSoundBuffer **pptmpBuf, IDirectSoundBuffer8 **ppDSBuffer, int oggPlayTime);
BOOL InitLock(IDirectSoundBuffer8 *pDSBuffer);
BOOL oggPlay(char *fileName, BOOL loop, BOOL fileLoop, double startSeek, double loopSeek, double loopEndSeek, int fadeInTime);
BOOL oggStop(int fadeOutTime);
BOOL oggPause(void);
BOOL oggRestart(void);
void oggSetVolume(int volume);
void oggSetVolume2(int volume);
void oggSetPan(double pan);
void oggSetFrequency(double frequency);
BOOL oggFileClear(void);
unsigned int getPCMBuffer(OggVorbis_File *ovf, char* buffer, int bufferSize, BOOL isLoop, double loopSeek, double loopEndSeek, BOOL* isEnd);
//void StopProc(void);
/*************************************/ #endif /**********************************/
