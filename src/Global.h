/********************************/ #ifndef GLOBAL_H /*****************************/
#define GLOBAL_H

#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <mmsystem.h>
#include "GuruGuruSMF4_C_Dynamic.h"
#include "OggFunc.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")



#define DEBUG             0
#define MIDIFILELIST_SIZE 255
#define FILEPATH_SIZE     300
#define BUF_SIZE_S        512
#define BUF_SIZE_M        2048
#define BUF_SIZE_L        32768


// Main.c ÇÃïœêî /////////////////////////////////

#if DEBUG
extern char testStr[BUF_SIZE_M];
#endif

extern HWND  hWnd;
extern DWORD dwTimer;
extern BOOL  windowClose;
extern BOOL  threadClose;

extern int   bgmType; // 0:í‚é~ 1:midi 2:ogg
extern int   midiPause;
extern int   midiGettingPlayerStatus;

extern int    midiVolume;
extern double midiTempo;
extern int    midiPitch;

extern int   midiFadeInTime;
extern int   midiFadeOutTime;
extern int   midiKeyShift;
extern int   midiStartTick;

extern int   midiSkipBeginning;
extern int   midiSkipEnding;
extern int   midiMasterPitch, midiPlayingMasterPitch;
extern int   midiNormalize;
extern int   midiExclusive;

extern int    oggVolume;
extern double oggTempo;
extern double oggPan;

extern int    oggFadeInTime;
extern int    oggFadeOutTime;
extern int    oggStartTime;
extern int    oggLoopTime;
extern int    oggFileLoop;

extern int   playerDirectMusicMode, playerDirectMusicMode_old;
extern int   playerMode;
extern int   playerSpeedMode;
extern int   playerCacheMode;

extern BOOL  exitFlag;
extern BOOL  midiPlayerOK;

extern FARPROC GetModuleFileNameExA;

//////////////////////////////////////

extern int cnt, i, j, k;
extern DWORD    hdError;
extern DWORD    fileReadSize, fileReadSize2;
extern BOOL     eff_midiDirChangeFlag;

extern char     bgmStr[_MAX_PATH], bgmStr_old[FILEPATH_SIZE];
extern char     playedBgmStr[FILEPATH_SIZE];
extern char     oggBgmStr[FILEPATH_SIZE], oggBgmStr_old[FILEPATH_SIZE];
extern char     playedOggBgmStr[FILEPATH_SIZE];
extern char     eff_midiAddr[FILEPATH_SIZE];
extern char     eff_midiDir[FILEPATH_SIZE], eff_midiDir_old[FILEPATH_SIZE];
extern char     ogg_sigAddr[FILEPATH_SIZE];
extern char     midiPlayerStatusAddr[FILEPATH_SIZE];
extern char     defaultTonyuprjFileAddr[FILEPATH_SIZE];
extern char     usrFolderAddr[FILEPATH_SIZE];
extern char     YMPPreparationAddr[FILEPATH_SIZE];
extern char     readBuf[BUF_SIZE_S];
extern char     readBuf2[BUF_SIZE_S];
extern char     readBuf3[BUF_SIZE_S];
extern char     readBuf4[BUF_SIZE_S];
extern char     readBuf5[BUF_SIZE_S];
extern char     readBuf6[BUF_SIZE_S];
extern char     readBuf7[BUF_SIZE_S];

extern BOOL     playFlag;
extern BOOL     playFlagCnt;
extern BOOL     playingFlag;
extern int      playerStatusCnt;
extern BOOL     loopFlag;
extern int      playId;
extern BOOL     fileDeleteFlag;
extern GGSERROR errorFlag;

extern int   loopFlag_r;

extern int   midiPause_r;
extern int   midiGettingPlayerStatus_r;

extern int    midiVolume_r;
extern double midiTempo_r;
extern int    midiPitch_r;

extern int   midiFadeInTime_r;
extern int   midiFadeOutTime_r;
extern int   midiKeyShift_r;
extern int   midiStartTick_r, midiStartTick_temp;

extern int   midiSkipBeginning_r;
extern int   midiSkipEnding_r;
extern int   midiMasterPitch_r;
extern int   midiNormalize_r;
extern int   midiExclusive_r;

extern int    oggVolume_r;
extern double oggTempo_r;
extern double oggPan_r;

extern int    oggFadeInTime_r;
extern int    oggFadeOutTime_r;
extern int    oggStartTime_r;
extern int    oggLoopTime_r;
extern int    oggFileLoop_r;

extern int   readValue; // MidiOggPlayeróp

extern GGS4SMFINFORMATION midiInfo;
extern GGS4PLAYERSTATUS midiStatus;

extern char  crDir[FILEPATH_SIZE];
extern char  playerExeName[FILEPATH_SIZE];
extern BOOL  processCheckFlag;
extern DWORD dwProcessId;
extern DWORD osPlatformId;

extern char *midiFileAddrList[255];
extern int   midiFileAddrListLength;
extern int   tempInt;
extern char  tempStr[BUF_SIZE_M];

extern BOOL  midiEffectFlag;
extern SYSTEMTIME stTime;

extern HANDLE hFile;

//////////////////////////////////////////////////


BOOL  mainInit(void);
void  initMidiParam(void);
void  createPreparationFile(void);
void  deletePreparationFile(void);
DWORD getPlatformId(void);
BOOL  processCheckingInit(char *crDir, DWORD osPlatformId, DWORD *pDwProcessId, BOOL *pProcessCheckFlag, char *playerExeName, size_t playerExeNameSize);
BOOL  playerInit(int DirectMusicOn);
void  beforeReadMidi(char *usrFolderAddr, char *defaultTonyuprjFileAddr, char **midiFileAddrList, int *pMidiFileAddrListLength);
BOOL  setEffectFileAddr(char *bgmStr, 
                        char *eff_midiAddr           , size_t eff_midiAddrSize,
                        char *eff_midiDir            , size_t eff_midiDirSize,
                        char *ogg_sigAddr            , size_t ogg_sigAddrSize,
                        char *defaultTonyuprjFileAddr, size_t defaultTonyuprjFileAddrSize,
                        char *usrFolderAddr          , size_t usrFolderAddrSize,
                        char *midiPlayerStatusAddr   , size_t midiPlayerStatusAddrSize,
                        char *YMPPreparationAddr     , size_t YMPPreparationAddrSize
                        );
void  setMidiPlayerConfig(char *readFileName, DWORD osPlatformId, char *playerExeName, size_t playerExeNameSize);
void  readMidiDls(void);

DWORD  readFile_C(LPCTSTR fileName, char *readBuf, DWORD readBufSize, int share);
DWORD  writeFile_C(LPCTSTR fileName, char *writeBuf, DWORD writeBufSize, int share);
BOOL   fileAddrToDir(char *fileAddr, char *fileDir, size_t fileDirSize);
int    strToInt(char *str, int start, int end, int falseReturnValue);
double strToDouble(char *str, int start, int end, double falseReturnValue);
int    getLinePoint(char *str, int strSize, int *point);
char  *getMemSearch(char *str1, int size1, char *str2);

DWORD  searchProcess(LPTSTR lpszFile, DWORD osPlatformId);
BOOL   processExist(LPTSTR lpszFile, DWORD osPlatformId, DWORD dwProcessId);
void   getOggPlayingTime(void);
//BOOL   playerRestart(void);

/*************************************/ #endif /**********************************/
