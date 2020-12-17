
#include "Define.h"
#include "OggFunc.h"
#include "Global.h"

char *cmdLineStr;

IDirectSound        *pDS;
IDirectSoundBuffer  *ptmpBuf   = 0;
IDirectSoundBuffer  *pDSBuffer = 0;
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
BOOL   oggAllWriteFlag_thread = 0;
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
DWORD  oggPoint2;
DWORD  oggPoint2_old;
BOOL   oggLoopFlag     = 1;
int    oggPlayerStatus = STATUS_STOP;
BOOL   oggFileRead     = 0;
double oggTotalTime    = 0.0; // ���v����
int    oggTotalPcm     = 0.0; // ���v�T���v����
double oggPlayingTime  = 0.0; // �Đ�����
int    oggBufLoopCnt = 0;     // �o�b�t�@�Ń��[�v������
int    oggBufLoopCnt_thread = 0; // �o�b�t�@�Ń��[�v������
int    oggLoopCnt      = 0;   // ���[�v������

int    oggFadeOutTimeRun = 0; 
int    oggFadeInTimeRun  = 0; 
double oggFadeCnt     = 0.0; 
DWORD  oggTotalTimeInt;
DWORD  oggPlayingTimeInt;
DWORD  oggPlayingTimeInt2;
BOOL   oggPauseFlag = 0; // �ꎞ��~���J��Ԃ��ƍĐ��ʒu�����������Ȃ�ꍇ������̂ŁA�o�O�鎞�ԂɈꎞ��~�������͎��Ԃ��o�߂�����ꎞ��~�����s����

int    oggBufWriteSpeed = 0;

int    ogg3LoopFlag    = 1;
int    ogg3Volume      = 127;
int    ogg3FadeOutTime = 0;

// ����
int    oggPlayTime = 2;
DWORD  oggSect     = 50;

// �C�x���g�֘A
HANDLE g_hNotificationEvent = NULL;
DWORD  g_dwNotifyThreadID = 0;
HANDLE g_hNotifyThread = NULL;
//LPDIRECTSOUNDNOTIFY pDSNotify = NULL;
//DSBPOSITIONNOTIFY   aPosNotify[1];
HRESULT             hr;


static void CALLBACK TimerThreadOgg(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2);

BOOL createDS(IDirectSound **ppDS, HWND hWnd);
BOOL openOggFile(TCHAR *fileName, OggVorbis_File *pOvf, vorbis_info *pOggInfo, WAVEFORMATEX *pWaveFormat);
BOOL createDSBuffer(DSBUFFERDESC *pDSBufferDesc, WAVEFORMATEX *pWaveFormat,
                    IDirectSound *pDS, IDirectSoundBuffer **pptmpBuf, IDirectSoundBuffer **ppDSBuffer, int oggPlayTime);
BOOL InitLock(IDirectSoundBuffer *pDSBuffer);

BOOL oggPlay(TCHAR *fileName, BOOL loop, BOOL fileLoop, double startSeek, double loopSeek, double loopEndSeek, int fadeInTime);
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





BOOL createDS(IDirectSound **ppDS, HWND hWnd) {
    // DirectSound�̍쐬
    if (DirectSoundCreate(NULL, ppDS, NULL) != S_OK) return FALSE;
    if (IDirectSound_SetCooperativeLevel(*ppDS, hWnd, DSSCL_PRIORITY) != S_OK) return FALSE;
	
    return TRUE;
}

BOOL openOggFile(TCHAR *fileName, OggVorbis_File *pOvf, vorbis_info *pOggInfo, WAVEFORMATEX *pWaveFormat) {
	static char readBuf[BUF_SIZE_M];
	DWORD   fileReadSize;
	char   *tempP;
	
	if (fileName != NULL) { // �t�@�C����������
		if (!PathFileExists(fileName)) return FALSE; // �t�@�C��������������G���[
	} else { // �t�@�C����������
		return FALSE; // �ǂ̃t�@�C����ǂ߂΂����̂��킩��Ȃ��̂ŃG���[
	}
	
	// ogg�t�@�C���̃��[�v���g���Ƃ��́Aogg�t�@�C���𕪐�
	oggFileLoopTime    = 0;
	oggFileLoopEndTime = 0;
	fileReadSize = readFile_C(fileName, readBuf, sizeof(readBuf), 0);
	readBuf[sizeof(readBuf) - 1] = '\0';
	if (fileReadSize >= 48) { // �^�O�𒲂ׁA���l��ǂݍ���
		tempP = getMemSearch(readBuf, fileReadSize, "LOOPSTART=");
		if (tempP != NULL) oggFileLoopTime = strToInt(tempP, 10, 26, 0);
		tempP = getMemSearch(readBuf, fileReadSize, "LOOPLENGTH=");
		if (tempP != NULL) oggFileLoopEndTime = strToInt(tempP, 11, 27, 0);
	}
	
	// Ogg�t�@�C���I�[�v��
	if (ov_fopen(fileName, pOvf) != 0) return FALSE;
	
	// Ogg�t�@�C���̉����t�H�[�}�b�g���
	pOggInfo = ov_info(pOvf, -1);
	
	// WAVE���
	pWaveFormat->wFormatTag      = WAVE_FORMAT_PCM;
	pWaveFormat->nChannels       = pOggInfo->channels;
	pWaveFormat->nSamplesPerSec  = pOggInfo->rate;
	pWaveFormat->wBitsPerSample  = 16;
	pWaveFormat->nBlockAlign     = pOggInfo->channels * 16 / 8;
	pWaveFormat->nAvgBytesPerSec = pWaveFormat->nSamplesPerSec * pWaveFormat->nBlockAlign;
	pWaveFormat->cbSize          = 0;
	
	// Ogg���
	oggTotalTime = ov_time_total(pOvf, -1);
	oggTotalPcm  = ov_pcm_total(pOvf, -1);
	
	oggFileRead = 1;
	return TRUE;
}

BOOL createDSBuffer(DSBUFFERDESC *pDSBufferDesc, WAVEFORMATEX *pWaveFormat,
                    IDirectSound *pDS, IDirectSoundBuffer **pptmpBuf, IDirectSoundBuffer **ppDSBuffer, int oggPlayTime) {
	
    // DirectSoundBuffer���
    pDSBufferDesc->dwSize          = sizeof(DSBUFFERDESC);
    pDSBufferDesc->dwFlags         = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
    pDSBufferDesc->dwBufferBytes   = pWaveFormat->nSamplesPerSec * pWaveFormat->nBlockAlign * oggPlayTime;
    //pDSBufferDesc->dwBufferBytes   = 44100 * 4 * oggPlayTime;
    pDSBufferDesc->dwReserved      = 0;
    pDSBufferDesc->lpwfxFormat     = pWaveFormat;
    pDSBufferDesc->guid3DAlgorithm = GUID_NULL;
	
    // �Z�J���_���o�b�t�@�쐬
    if (IDirectSound_CreateSoundBuffer(pDS, pDSBufferDesc, pptmpBuf, NULL) == DS_OK) {
        IDirectSoundBuffer_QueryInterface(ptmpBuf, &IID_IDirectSoundBuffer, (void**)ppDSBuffer);
    } else {
        return FALSE;
    }
    IDirectSoundBuffer_Release(ptmpBuf);
    
	// �C�x���g�ƃX���b�h�̍쐬�i���ǔŁj
	if(g_hNotifyThread){
		ResumeThread(g_hNotifyThread);
	} else {
		g_hNotifyThread = CreateThread(NULL, 0, NotificationProc, hWnd, CREATE_SUSPENDED, &g_dwNotifyThreadID);
	}
	
    return TRUE;
}

BOOL InitLock(IDirectSoundBuffer *pDSBuffer) {
	void* AP1 = 0, *AP2 = 0;
	DWORD AB1 = 0, AB2  = 0;
    
    // �o�b�t�@�����b�N���ď����f�[�^��������
	// �ǂݎn�߂� oggSectSize * 25 ��������������Œu��
    if (IDirectSoundBuffer_Lock(pDSBuffer, 0, 0, &AP1, &AB1, &AP2, &AB2, DSBLOCK_ENTIREBUFFER) == DS_OK) {
	//if (IDirectSoundBuffer_Lock(pDSBuffer, oggSectSize * oggFlag, oggSectSize * 5, &AP1, &AB1, &AP2, &AB2, 0) == DS_OK) {
        memset(AP1, 0, AB1);
		getPCMBuffer(&ovf, (char*)AP1, oggSectSize * 22, oggLoopFlag, oggLoopSeek, oggLoopEndSeek, &oggEndFlag);
		IDirectSoundBuffer_Unlock(pDSBuffer, AP1, AB1, AP2, AB2);
		oggFlag = (oggFlag + 22) % oggSect;
		oggAllWriteFlag = 1;  // �o�b�t�@�̍ŏ���ʂ�����
		oggAllWriteFlag_thread = 1;  // �o�b�t�@�̍ŏ���ʂ�����
		oggBufWriteSpeed = 0; // �ŏ��͒x���o�b�t�@�ɏ�������
		
		//sprintf(tempStr, "��������[%d] oggFlag:%d %u %u", oggPoint, oggFlag, AB1, AB2);
		//SetWindowText(hStatic, tempStr);
	}
    else {
        return FALSE;
    }
    return TRUE;
}



BOOL oggPlay(TCHAR *fileName, BOOL loop, BOOL fileLoop, double startSeek, double loopSeek, double loopEndSeek, int fadeInTime) {
	BOOL v1, v2;
	
	oggPlayerStatus = STATUS_STOP;
	if (pDSBuffer)   IDirectSoundBuffer_Stop(pDSBuffer);
	if (oggFileRead) ov_time_seek(&ovf, 0.0);
	if (pDSBuffer)   IDirectSoundBuffer_SetCurrentPosition(pDSBuffer, 0);
	playerStatusCnt = 0;
	oggFlag = 0;
	oggPoint = 0;
	oggPoint2 = 0;
	oggPoint2_old = 0;
	oggEndCnt = 0;
	oggEndFlag = 0;
	oggLoopCnt = 0;
	oggBufLoopCnt = 0;
	oggBufLoopCnt_thread = 0;
	oggPlayingTimeInt2 = 0;
	oggPauseFlag = 0;
	
	if (fileName != NULL) {
		oggFileClear(); // �t�@�C���f�[�^�N���A
		if (!openOggFile(fileName, &ovf, oggInfo, &waveFormat)) return FALSE;    // �t�@�C���I�[�v��
		if (pDSBuffer) { IDirectSoundBuffer_Release(pDSBuffer); pDSBuffer = 0; } // �Z�J���_���o�b�t�@�����
		if (!createDSBuffer(&DSBufferDesc, &waveFormat, pDS, &ptmpBuf, &pDSBuffer, oggPlayTime)) return FALSE; // �Z�J���_���o�b�t�@�쐬
		oggSectSize = DSBufferDesc.dwBufferBytes / oggSect;
	} else { // �O�̃f�[�^���c���Ă��邩
		if (!oggFileRead) return FALSE;
	}
	
	if (pDSBuffer) {
		oggLoopFlag = loop;
		
		oggInfo = ov_info(&ovf, -1);
		
		// ���[�v�J�n�ʒu
		if (fileLoop) oggLoopSeek = (double)oggFileLoopTime / oggInfo->rate; // Ogg���[�v�g��
		else          oggLoopSeek = loopSeek;                                // Ogg���[�v�g��Ȃ�
		if (oggLoopSeek < 0.0)                  oggLoopSeek = 0.0;
		if (oggLoopSeek >= oggTotalTime - 0.04) oggLoopSeek = oggTotalTime - 0.04;
		oggLoopSeekInt = (double)oggFileLoopTime / oggInfo->rate * 1000;
		
		// ���[�v�I���ʒu
		if (fileLoop) oggLoopEndSeek = (double)(oggFileLoopEndTime + oggFileLoopTime) / oggInfo->rate; // Ogg���[�v�g��
		else          oggLoopEndSeek = loopEndSeek;                                                    // Ogg���[�v�g��Ȃ�
		if (oggLoopEndSeek > 0.0) { // ���[�v����ʒu�w�肳��Ă���
			if (oggLoopEndSeek <  oggLoopSeek + 0.04) oggLoopEndSeek = oggLoopSeek + 0.04;
			if (oggLoopEndSeek < 0.0)                 oggLoopEndSeek = 0.0;
			if (oggLoopEndSeek >= oggTotalTime)       oggLoopEndSeek = oggTotalTime;
		} else { // ���[�v����ʒu�w�肳��Ă��Ȃ�
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
		ResumeThread(g_hNotifyThread);
	}
	
	return TRUE;
}

BOOL oggStop(int fadeOutTime) {
	if (oggPlayerStatus == STATUS_STOP) return FALSE;
	
	oggFadeInTimeRun = 0;
	
	if (fadeOutTime > 0) { // �t�F�[�h�A�E�g����
		oggFadeOutTimeRun = fadeOutTime;
		//oggFadeCnt     = 1.0;
	} else { // �t�F�[�h�A�E�g�Ȃ�
		if (pDSBuffer)   IDirectSoundBuffer_Stop(pDSBuffer);
		if (oggFileRead) ov_time_seek(&ovf, 0.0);
		if (pDSBuffer)   IDirectSoundBuffer_SetCurrentPosition(pDSBuffer, 0);
		//bgmType = 0;
		oggFlag = 0;
		oggPoint = 0;
		oggPoint2 = 0;
		oggPoint2_old = 0;
		oggEndCnt = 0;
		oggEndFlag = 0;
		oggBufLoopCnt = 0;
		oggBufLoopCnt_thread = 0;
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
		//if (oggPoint / oggSectSize == oggSect - 1) {
		//	oggPauseFlag = 1;
		//	return TRUE;
		//}
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
	DWORD value, value2;
	if (pDSBuffer) {
		if (frequency >= 0.0 && oggFileRead) {
			
			value = (DWORD)(waveFormat.nSamplesPerSec * frequency);
			if      (value < DSBFREQUENCY_MIN) value = DSBFREQUENCY_MIN;
			else if (value > DSBFREQUENCY_MAX) value = DSBFREQUENCY_MAX;
			IDirectSoundBuffer_SetFrequency(pDSBuffer, value);
			
			if (value > 100000) {
				IDirectSoundBuffer_GetFrequency(pDSBuffer, &value2);
				if (value2 != value) {
					value = 100000;
					IDirectSoundBuffer_SetFrequency(pDSBuffer, value);
				}
			}
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





// �w��T�C�Y��PCM�����o�b�t�@�𖄂߂�֐�
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
			// �t�@�C���G���h�ɒB���� or ���[�v���ׂ��ʒu�ɒB����
			if (isLoop == TRUE) {
				// ���[�v����ꍇ�ǂݍ��݈ʒu���ŏ��ɖ߂�
				ov_time_seek(ovf, loopSeek);
				if (isEnd) (*isEnd)++;
			} else {
				// ���[�v���Ȃ��ꍇ�t�@�C���G���h�ɒB������I��
				memset((char*)(buffer + comSize), 0, requestSize);
				if (isEnd) (*isEnd)++;
				return comSize;
			}
		}
		
		
		comSize += readSize;
		
		if (comSize >= bufferSize) {
			// �o�b�t�@�𖄂ߐs�������̂ŏI��
			return comSize;
		}
		
		if (bufferSize - comSize < 4096) {
			requestSize = bufferSize - comSize;
		}
	}
}


// �ꎞ��~�֐�
void StopProc() {
	// A�L�[�ňꎞ��~
	while ( GetAsyncKeyState( 65 ) ) Sleep( 1 );
}


// �X���b�h����
DWORD WINAPI NotificationProc(LPVOID lpParameter){
	MSG msg;
	//HWND hWnd = (HWND)lpParameter;
	BOOL bDone = FALSE;
	DWORD dwResult;
	
	while(!bDone){
		/*
		dwResult = MsgWaitForMultipleObjects(1, &g_hNotificationEvent, FALSE, INFINITE, QS_ALLEVENTS);
		
		switch(dwResult) {
			case WAIT_OBJECT_0 + 0:
				// �o�b�t�@�����[�v������J�E���g�����Z
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
		*/
		
		IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint2, 0);
		if (oggPoint2 < oggPoint2_old) {
			if (oggAllWriteFlag_thread < 10) { // 80�b���������x��Ă��܂�������߂�
				oggAllWriteFlag_thread ++;
				oggBufLoopCnt_thread   ++;
			}
		}
		oggPoint2_old = oggPoint2;
		
		//PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//if(msg.message == WM_QUIT){
		//	break ;
		//}
		
		Sleep(1);
		
		if (windowClose) break ;
		if (oggPlayerStatus == STATUS_STOP) SuspendThread(g_hNotifyThread);
	}
	
	return 0;
}
