/********************************/ #ifndef GLOBAL_H /*****************************/
#define GLOBAL_H

// Unicodeアプリケーションにするか(ONにしても文字コード変換してないのでエラーになる)
//#define UNICODE
#include <errno.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <mmsystem.h>
#include "GuruGuruSMF4_C_Dynamic.h"
#include "OggFunc.h"
#include "Joystick.h"
#include "resource.h"
#include "Define.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")



// Main.c の変数 /////////////////////////////////

#if DEBUG
extern char testStr[BUF_SIZE_M];
#endif

extern HWND  hWnd;
extern DWORD dwTimer;
extern BOOL  windowClose;
extern BOOL  threadClose;

extern int   bgmType; // 0:停止 1:midi 2:ogg
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


typedef DWORD (WINAPI *FunctionGetModuleFileNameExA)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);

//////////////////////////////////////

extern int cnt, i, j, k;
extern DWORD    hdError;
extern DWORD    fileReadSize, fileReadSize2;
extern BOOL     eff_midiDirChangeFlag;

extern TCHAR    bgmStr[_MAX_PATH], bgmStr_old[_MAX_PATH];
extern TCHAR    playedBgmStr[FILEPATH_SIZE];
extern TCHAR    oggBgmStr[_MAX_PATH], oggBgmStr_old[_MAX_PATH];
extern TCHAR    playedOggBgmStr[FILEPATH_SIZE];
extern TCHAR     eff_midiAddr[FILEPATH_SIZE];
extern TCHAR    eff_midiDir[FILEPATH_SIZE], eff_midiDir_old[FILEPATH_SIZE];
extern TCHAR    ogg_sigAddr[FILEPATH_SIZE];
extern TCHAR    midiPlayerStatusAddr[FILEPATH_SIZE];
extern TCHAR    defaultTonyuprjFileAddr[FILEPATH_SIZE];
extern TCHAR    usrFolderAddr[FILEPATH_SIZE];
extern TCHAR    YMPPreparationAddr[FILEPATH_SIZE];
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

extern int   readValue; // MidiOggPlayer用

extern GGS4SMFINFORMATION midiInfo;
extern GGS4PLAYERSTATUS midiStatus;

extern TCHAR crDir[FILEPATH_SIZE];
extern TCHAR playerExeName[FILEPATH_SIZE];
extern BOOL  processCheckFlag;
extern DWORD dwProcessId;
extern DWORD osPlatformId;
extern DWORD osMajorVersion;
extern DWORD osMinorVersion;

extern TCHAR *midiFileAddrList[MIDIFILELIST_SIZE];
extern int   midiFileAddrListLength;
extern int   tempInt;
extern TCHAR tempStr[BUF_SIZE_M];

extern BOOL  midiEffectFlag;
extern SYSTEMTIME stTime;

extern HANDLE hFile;

//////////////////////////////////////////////////


BOOL  mainInit(void);
void  initMidiParam(void);
void  createPreparationFile(void);
void  deletePreparationFile(void);
BOOL getPlatformId(DWORD *osPlatformId, DWORD *osMajorVersion, DWORD *osMinorVersion);
BOOL  processCheckingInit(TCHAR *crDir, DWORD osPlatformId, DWORD *pDwProcessId, BOOL *pProcessCheckFlag, TCHAR *playerExeName, size_t playerExeNameSize);
BOOL  playerInit(int *DirectMusicOn);
void  beforeReadMidi(TCHAR *usrFolderAddr, TCHAR *defaultTonyuprjFileAddr, TCHAR **midiFileAddrList, int *pMidiFileAddrListLength);
BOOL  setEffectFileAddr(TCHAR *bgmStr, 
                        TCHAR *eff_midiAddr           , size_t eff_midiAddrSize,
                        TCHAR *eff_midiDir            , size_t eff_midiDirSize,
                        TCHAR *ogg_sigAddr            , size_t ogg_sigAddrSize,
                        TCHAR *defaultTonyuprjFileAddr, size_t defaultTonyuprjFileAddrSize,
                        TCHAR *usrFolderAddr          , size_t usrFolderAddrSize,
                        TCHAR *midiPlayerStatusAddr   , size_t midiPlayerStatusAddrSize,
                        TCHAR *YMPPreparationAddr     , size_t YMPPreparationAddrSize
                        );
void  setMidiPlayerConfig(TCHAR *readFileName, DWORD osPlatformId, DWORD osMajorVersion, TCHAR *playerExeName, size_t playerExeNameSize);
void  readMidiDls(void);

DWORD  readFile_C(LPCTSTR fileName, char *readBuf, DWORD readBufSize, int share);
DWORD  writeFile_C(LPCTSTR fileName, char *writeBuf, DWORD writeBufSize, int share);
DWORD  getFileSize_C(LPCTSTR fileName);
BOOL   fileAddrToDir(char *fileAddr, char *fileDir, size_t fileDirSize);
int    strToInt(char *str, int start, int end, int falseReturnValue);
double strToDouble(char *str, int start, int end, double falseReturnValue);
int    getLinePoint(char *str, int strSize, int *point);
char  *getMemSearch(char *str1, int size1, char *str2);

DWORD  searchProcess(LPTSTR lpszFile, DWORD osPlatformId);
BOOL   processExist(DWORD dwProcessId);
void   getOggPlayingTime(void);
//BOOL   playerRestart(void);

/*************************************/ #endif /**********************************/
