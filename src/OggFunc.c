#include "OggFunc.h"
#include "Global.h"

char *cmdLineStr;

IDirectSound8       *pDS8;
IDirectSoundBuffer  *ptmpBuf   = 0;
IDirectSoundBuffer8 *pDSBuffer = 0;
DSBUFFERDESC         DSBufferDesc;
DSCAPS               DSCaps;
OggVorbis_File       ovf;
vorbis_info         *oggInfo;
WAVEFORMATEX         waveFormat;
void*  AP1 = 0, *AP2 = 0;
DWORD  AB1 = 0, AB2  = 0;
DWORD  oggSectSize;
DWORD  oggFlag;
BOOL   oggAllWriteFlag = 0;
BOOL   oggEndFlag  = 0;
DWORD  oggEndCnt   = 0;
double oggLoopSeek     = 0.0;
double oggLoopEndSeek  = 0.0;
int    oggLoopSeekInt    = 0;
int    oggLoopEndSeekInt = 0;
int    oggFileLoopTime    = 0;
int    oggFileLoopEndTime = 0;
double oggStartSeek = 0.0;
DWORD  oggPoint;
BOOL   oggLoopFlag     = 1;
int    oggPlayerStatus = STATUS_STOP;
BOOL   oggFileRead     = 0;
double oggTotalTime    = 0.0; // 合計時間
int    oggTotalPcm     = 0.0; // 合計サンプル数
double oggPlayingTime  = 0.0; // 再生時間
int    oggBufLoopCnt   = 0;   // バッファでループした回数
int    oggLoopCnt      = 0;   // ループした回数

int    oggFadeOutTimeRun = 0; 
int    oggFadeInTimeRun  = 0; 
double oggFadeCnt     = 0.0; 
DWORD  oggTotalTimeInt;
DWORD  oggPlayingTimeInt;
DWORD  oggPlayingTimeInt2;
BOOL   oggPauseFlag = 0; // 一時停止を繰り返すと再生位置がおかしくなる場合があるので、バグる時間に一時停止した時は時間が経過した後一時停止を実行する

int    oggBufWriteSpeed = 0;

int    ogg3LoopFlag    = 1;
int    ogg3Volume      = 127;
int    ogg3FadeOutTime = 0;

// 調節
int    oggPlayTime = 8;
DWORD  oggSect     = 200;

// イベント関連
HANDLE g_hNotificationEvent = NULL;
DWORD  g_dwNotifyThreadID = 0;
HANDLE g_hNotifyThread = NULL;
LPDIRECTSOUNDNOTIFY pDSNotify = NULL;
DSBPOSITIONNOTIFY   aPosNotify[1];
HRESULT             hr;


static void CALLBACK TimerThreadOgg(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2);

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
void oggSetPan(double pan);
void oggSetFrequency(double frequency);
static LONG getDSBVolume(double volume);
static LONG getDSBPan(double volume);

BOOL oggFileClear(void);


//void StopProc(void);
unsigned int getPCMBuffer(OggVorbis_File *ovf, char* buffer, int bufferSize, BOOL isLoop, double loopSeek, double loopEndSeek, BOOL* isEnd);
DWORD WINAPI NotificationProc(LPVOID lpParameter);





BOOL createDS(IDirectSound8 **ppDS8, HWND hWnd) {
    // DirectSoundの作成
    if (DirectSoundCreate8(NULL, ppDS8, NULL) != S_OK) return FALSE;
    if (IDirectSound_SetCooperativeLevel(*ppDS8, hWnd, DSSCL_PRIORITY) != S_OK) return FALSE;
	
	// 周波数のサポート範囲を取得
	DSCaps.dwSize  = sizeof(DSCAPS);
	DSCaps.dwFlags = DSCAPS_CONTINUOUSRATE;
	IDirectSound_GetCaps(pDS8, &DSCaps);
	
    return TRUE;
}

BOOL openOggFile(char *fileName, OggVorbis_File *pOvf, vorbis_info *pOggInfo, WAVEFORMATEX *pWaveFormat) {
	static char readBuf[BUF_SIZE_M];
	DWORD   fileReadSize;
	char   *tempP;
	
	if (fileName != NULL) { // ファイル名がある
		if (!PathFileExists(fileName)) return FALSE; // ファイルが無かったらエラー
	} else { // ファイル名が無い
		return FALSE; // どのファイルを読めばいいのかわからないのでエラー
	}
	
	// oggファイルのループを使うときは、oggファイルを分析
	oggFileLoopTime    = 0;
	oggFileLoopEndTime = 0;
	fileReadSize = readFile_C(fileName, readBuf, sizeof(readBuf), 0);
	readBuf[sizeof(readBuf) - 1] = '\0';
	if (fileReadSize >= 48) { // タグを調べ、数値を読み込む
		tempP = getMemSearch(readBuf, fileReadSize, "LOOPSTART=");
		if (tempP != NULL) oggFileLoopTime = strToInt(tempP, 10, 26, 0);
		tempP = getMemSearch(readBuf, fileReadSize, "LOOPLENGTH=");
		if (tempP != NULL) oggFileLoopEndTime = strToInt(tempP, 11, 27, 0);
	}
	
	// Oggファイルオープン
	if (ov_fopen(fileName, pOvf) != 0) return FALSE;
	
	// Oggファイルの音声フォーマット情報
	pOggInfo = ov_info(pOvf, -1);
	
	// WAVE情報
	pWaveFormat->wFormatTag      = WAVE_FORMAT_PCM;
	pWaveFormat->nChannels       = pOggInfo->channels;
	pWaveFormat->nSamplesPerSec  = pOggInfo->rate;
	pWaveFormat->wBitsPerSample  = 16;
	pWaveFormat->nBlockAlign     = pOggInfo->channels * 16 / 8;
	pWaveFormat->nAvgBytesPerSec = pWaveFormat->nSamplesPerSec * pWaveFormat->nBlockAlign;
	pWaveFormat->cbSize          = 0;
	
	// Ogg情報
	oggTotalTime = ov_time_total(pOvf, -1);
	oggTotalPcm  = ov_pcm_total(pOvf, -1);
	
	oggFileRead = 1;
	return TRUE;
}

BOOL createDSBuffer(DSBUFFERDESC *pDSBufferDesc, WAVEFORMATEX *pWaveFormat,
                    IDirectSound8 *pDS8, IDirectSoundBuffer **pptmpBuf, IDirectSoundBuffer8 **ppDSBuffer, int oggPlayTime) {
	
    // DirectSoundBuffer情報
    pDSBufferDesc->dwSize          = sizeof(DSBUFFERDESC);
    pDSBufferDesc->dwFlags         = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY;
    pDSBufferDesc->dwBufferBytes   = pWaveFormat->nSamplesPerSec * pWaveFormat->nBlockAlign * oggPlayTime;
    pDSBufferDesc->dwReserved      = 0;
    pDSBufferDesc->lpwfxFormat     = pWaveFormat;
    pDSBufferDesc->guid3DAlgorithm = GUID_NULL;
	
    // セカンダリバッファ作成
    if (IDirectSound_CreateSoundBuffer(pDS8, pDSBufferDesc, pptmpBuf, NULL) == DS_OK) {
        IDirectSoundBuffer_QueryInterface(ptmpBuf, &IID_IDirectSoundBuffer, (void**)ppDSBuffer);
    } else {
        return FALSE;
    }
    
	// イベントとスレッドの作成
	if(pDSNotify){
		IDirectSoundNotify_Release(pDSNotify);
		pDSNotify = NULL;
	} else {
		g_hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		g_hNotifyThread = CreateThread(NULL, 0, NotificationProc, hWnd, 0, &g_dwNotifyThreadID);
	}
	
	// バッファ内のイベント発生ポイントの設定
	aPosNotify[0].dwOffset = pDSBufferDesc->dwBufferBytes - 1;
	//aPosNotify[0].dwOffset = 0;
	aPosNotify[0].hEventNotify = g_hNotificationEvent;
	
	hr = IDirectSoundBuffer_QueryInterface(ptmpBuf, &IID_IDirectSoundNotify, (VOID**)&pDSNotify);
	IDirectSoundNotify_SetNotificationPositions(pDSNotify, 1, aPosNotify); // １つ
	
    IDirectSoundBuffer_Release(ptmpBuf);
	
    return TRUE;
}

BOOL InitLock(IDirectSoundBuffer8 *pDSBuffer) {
	void* AP1 = 0, *AP2 = 0;
	DWORD AB1 = 0, AB2  = 0;
    
    // バッファをロックして初期データ書き込み
	// 読み始めは oggSectSize * 5 分だけ書き込んで置く
    if (IDirectSoundBuffer_Lock(pDSBuffer, 0, 0, &AP1, &AB1, &AP2, &AB2, DSBLOCK_ENTIREBUFFER) == DS_OK) {
	//if (IDirectSoundBuffer_Lock(pDSBuffer, oggSectSize * oggFlag, oggSectSize * 5, &AP1, &AB1, &AP2, &AB2, 0) == DS_OK) {
        memset(AP1, 0, AB1);
		getPCMBuffer(&ovf, (char*)AP1, oggSectSize * 1, oggLoopFlag, oggLoopSeek, oggLoopEndSeek, &oggEndFlag);
		IDirectSoundBuffer_Unlock(pDSBuffer, AP1, AB1, AP2, AB2);
		oggFlag = (oggFlag + 1) % oggSect;
		oggAllWriteFlag = 1;  // バッファの最初を通った回数
		oggBufWriteSpeed = 0; // 最初は遅くバッファに書き込む
		
		//sprintf(tempStr, "書き込み[%d] oggFlag:%d %u %u", oggPoint, oggFlag, AB1, AB2);
		//SetWindowText(hStatic, tempStr);
	}
    else {
        return FALSE;
    }
    return TRUE;
}



BOOL oggPlay(char *fileName, BOOL loop, BOOL fileLoop, double startSeek, double loopSeek, double loopEndSeek, int fadeInTime) {
	BOOL v1, v2;
	
	oggPlayerStatus = STATUS_STOP;
	if (pDSBuffer)   IDirectSoundBuffer_Stop(pDSBuffer);
	if (oggFileRead) ov_time_seek(&ovf, 0.0);
	if (pDSBuffer)   IDirectSoundBuffer_SetCurrentPosition(pDSBuffer, 0);
	playerStatusCnt = 0;
	oggFlag = 0;
	oggPoint = 0;
	oggEndCnt = 0;
	oggEndFlag = 0;
	oggLoopCnt = 0;
	oggBufLoopCnt = 0;
	oggPlayingTimeInt2 = 0;
	oggPauseFlag = 0;
	
	if (fileName != NULL) {
		oggFileClear(); // ファイルデータクリア
		if (!openOggFile(fileName, &ovf, oggInfo, &waveFormat)) return FALSE;    // ファイルオープン
		if (pDSBuffer) { IDirectSoundBuffer_Release(pDSBuffer); pDSBuffer = 0; } // セカンダリバッファを解放
		if (!createDSBuffer(&DSBufferDesc, &waveFormat, pDS8, &ptmpBuf, &pDSBuffer, oggPlayTime)) return FALSE; // セカンダリバッファ作成
		oggSectSize = DSBufferDesc.dwBufferBytes / oggSect;
	} else { // 前のデータが残っているか
		if (!oggFileRead) return FALSE;
	}
	
	if (pDSBuffer) {
		oggLoopFlag = loop;
		
		oggInfo = ov_info(&ovf, -1);
		
		// ループ開始位置
		if (fileLoop) oggLoopSeek = (double)oggFileLoopTime / oggInfo->rate; // Oggループ使う
		else          oggLoopSeek = loopSeek;                                // Oggループ使わない
		if (oggLoopSeek < 0.0)                  oggLoopSeek = 0.0;
		if (oggLoopSeek >= oggTotalTime - 0.04) oggLoopSeek = oggTotalTime - 0.04;
		oggLoopSeekInt = (double)oggFileLoopTime / oggInfo->rate * 1000;
		
		// ループ終了位置
		if (fileLoop) oggLoopEndSeek = (double)(oggFileLoopEndTime + oggFileLoopTime) / oggInfo->rate; // Oggループ使う
		else          oggLoopEndSeek = loopEndSeek;                                                    // Oggループ使わない
		if (oggLoopEndSeek > 0.0) { // ループする位置指定されている
			if (oggLoopEndSeek <  oggLoopSeek + 0.04) oggLoopEndSeek = oggLoopSeek + 0.04;
			if (oggLoopEndSeek < 0.0)                 oggLoopEndSeek = 0.0;
			if (oggLoopEndSeek >= oggTotalTime)       oggLoopEndSeek = oggTotalTime;
		} else { // ループする位置指定されていない
			oggLoopEndSeek = 0.0;
		}
		oggLoopEndSeekInt = (double)(oggFileLoopEndTime + oggFileLoopTime) / oggInfo->rate * 1000;
		
		oggStartSeek = startSeek;
		v1 = (oggLoopEndSeek >  0.0 && oggStartSeek >= oggLoopEndSeek);
		v2 = (oggLoopEndSeek == 0.0 && oggStartSeek >= oggTotalTime);
		if (v1 || v2) {
			if      (v1) oggStartSeek = oggLoopEndSeek;
			else if (v2) oggStartSeek = oggTotalTime;
			startSeek = oggLoopSeek;
			oggLoopCnt = 1;
			if (!oggLoopFlag) {
				oggStop(0);
				return TRUE;
			}
		}
		
		ov_time_seek(&ovf, startSeek);
		InitLock(pDSBuffer);
		
		oggFadeOutTimeRun = 0;
		if (fadeInTime <= 0) {
			if (playerMode != 3) oggSetVolume(oggVolume);
			else                 oggSetVolume2(oggVolume);
			oggFadeInTimeRun = 0;
			oggFadeCnt       = 1.0;
		} else {
			oggSetVolume(0);
			oggFadeInTimeRun = fadeInTime;
			oggFadeCnt       = 0.0;
		}
		if (playerMode == 0) {
			oggSetFrequency(oggTempo);
			oggSetPan(oggPan);
		}
		IDirectSoundBuffer_Play(pDSBuffer, 0, 0, DSBPLAY_LOOPING);
		oggPlayerStatus = STATUS_PLAY;
	}
	return TRUE;
}

BOOL oggStop(int fadeOutTime) {
	if (oggPlayerStatus == STATUS_STOP) return FALSE;
	
	oggFadeInTimeRun = 0;
	
	if (fadeOutTime > 0) { // フェードアウトあり
		oggFadeOutTimeRun = fadeOutTime;
		//oggFadeCnt     = 1.0;
	} else { // フェードアウトなし
		if (pDSBuffer)   IDirectSoundBuffer_Stop(pDSBuffer);
		if (oggFileRead) ov_time_seek(&ovf, 0.0);
		if (pDSBuffer)   IDirectSoundBuffer_SetCurrentPosition(pDSBuffer, 0);
		//bgmType = 0;
		oggFlag = 0;
		oggPoint = 0;
		oggEndCnt = 0;
		oggEndFlag = 0;
		oggBufLoopCnt = 0;
		oggPlayingTimeInt2 = 0;
		oggFadeOutTimeRun = 0;
		oggFadeCnt        = 0.0;
		oggPauseFlag = 0;
		oggPlayerStatus = STATUS_STOP;
		playerStatusCnt = 0;
	}
	return TRUE;
}

BOOL oggPause() {
	if (oggPlayerStatus != STATUS_PLAY) return FALSE;
	
	if (pDSBuffer) {
		IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint, 0);
		if (oggPoint / oggSectSize == oggSect - 1) {
			oggPauseFlag = 1;
			return TRUE;
		}
		IDirectSoundBuffer_Stop(pDSBuffer);
		oggPauseFlag = 0;
		oggPlayerStatus = STATUS_PAUSE;
		playerStatusCnt = 0;
	}
	return TRUE;
}

BOOL oggRestart() {
	if (oggPlayerStatus != STATUS_PAUSE) return FALSE;
	
	if (pDSBuffer) {
		IDirectSoundBuffer_Play(pDSBuffer, 0, 0, DSBPLAY_LOOPING);
		oggPlayerStatus = STATUS_PLAY;
		playerStatusCnt = 0;
	}
	return TRUE;
}

void oggSetVolume(int volume) {
	if (pDSBuffer) IDirectSoundBuffer_SetVolume(pDSBuffer, getDSBVolume((double)volume/127));
}
void oggSetVolume2(int volume) {
	if (pDSBuffer) IDirectSoundBuffer_SetVolume(pDSBuffer, (volume * 10000 / 127) - 10000);
}
void oggSetPan(double pan) {
	if (pDSBuffer) IDirectSoundBuffer_SetPan(pDSBuffer, getDSBPan(pan));
}

void oggSetFrequency(double frequency) {
	DWORD value;
	if (pDSBuffer) {
		if (frequency >= 0.0 && oggFileRead) {
			value = (DWORD)(waveFormat.nSamplesPerSec * frequency);
			if      (value < DSCaps.dwMinSecondarySampleRate) value = DSCaps.dwMinSecondarySampleRate;
			else if (value > DSCaps.dwMaxSecondarySampleRate) value = DSCaps.dwMaxSecondarySampleRate;
			IDirectSoundBuffer_SetFrequency(pDSBuffer, value);
		}
	}
}

static LONG getDSBVolume(double volume) {
    if(volume >= 1.0){
        return DSBVOLUME_MAX;
    }else if(volume <= 0.0){
        return DSBVOLUME_MIN;
    }else{
        return (LONG)(33.2 * log10(volume) * 100);
    }
}

static LONG getDSBPan(double volume) {
    if(volume <= -1.0){
        return DSBPAN_LEFT;
    }else if(volume >= 1.0){
        return DSBPAN_RIGHT;
    }else {
    	if (volume < 0.0) return ( getDSBVolume(1.0+volume)); // -
    	else              return (-getDSBVolume(1.0-volume)); // +
    	//if (volume < 0.0) return (-10000 - getDSBVolume(-volume)); // -
    	//else              return ( 10000 + getDSBVolume( volume)); // +
    	//return (volume * 10000);
    }
}




BOOL oggFileClear() {
	if (oggFileRead) {
		ov_clear(&ovf);
		oggFileRead = NULL;
	}
	return TRUE;
}





// 指定サイズでPCM音声バッファを埋める関数
unsigned int getPCMBuffer(OggVorbis_File *ovf, char* buffer, int bufferSize, BOOL isLoop, double loopSeek, double loopEndSeek, BOOL* isEnd) {
	int  requestSize = 4096;
	int  bitstream   = 0;
	int  readSize;
	int  comSize     = 0;
	
	if (buffer == 0) {
		if (isEnd) *isEnd = TRUE;
		return 0;
	}
	//if (isEnd) *isEnd = FALSE;
	memset(buffer, 0, bufferSize);
	if (bufferSize < requestSize) {
		requestSize = bufferSize;
	}
	
	while(TRUE) {
		readSize = ov_read(ovf, (char*)(buffer + comSize), requestSize, 0, 2, 1, &bitstream);
		if (readSize == 0 || (loopEndSeek != 0.0 && ov_time_tell(ovf) >= loopEndSeek)) {
			// ファイルエンドに達した or ループすべき位置に達した
			if (isLoop == TRUE) {
				// ループする場合読み込み位置を最初に戻す
				ov_time_seek(ovf, loopSeek);
				if (isEnd) (*isEnd)++;
			} else {
				// ループしない場合ファイルエンドに達したら終了
				memset((char*)(buffer + comSize), 0, requestSize);
				if (isEnd) (*isEnd)++;
				return comSize;
			}
		}
		
		
		comSize += readSize;
		
		if (comSize >= bufferSize) {
			// バッファを埋め尽くしたので終了
			return comSize;
		}
		
		if (bufferSize - comSize < 4096) {
			requestSize = bufferSize - comSize;
		}
	}
}


// 一時停止関数
//void StopProc() {
//	// Escapeキーを押したら抜ける
//	while ( GetAsyncKeyState( 65 ) ) Sleep( 1 );
//}


// スレッド処理

DWORD WINAPI NotificationProc(LPVOID lpParameter){
	MSG msg;
	//HWND hWnd = (HWND)lpParameter;
	BOOL bDone = FALSE;
	DWORD dwResult;
	
	while(!bDone){
		dwResult = MsgWaitForMultipleObjects(1, &g_hNotificationEvent, FALSE, INFINITE, QS_ALLEVENTS);
		
		switch(dwResult) {
			case WAIT_OBJECT_0 + 0:
				// バッファをループしたらカウントを加算
				oggAllWriteFlag ++;
				oggBufLoopCnt   ++;
				break;
			case WAIT_OBJECT_0 + 1:
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){ 
					if(msg.message == WM_QUIT){
						bDone = TRUE;
					}
				}
			break;
		}
	}
	return 0;
}

