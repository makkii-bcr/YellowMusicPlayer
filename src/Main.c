/*
  YellowMusicPlayer 2.03  -  Copyright (C) 2012 �}�b�L�[
  
  ���̃\�[�X�R�[�h�͎��R�ɉ��ς��ăR���p�C�����č\���܂���B
  �����������̂�z�z����ꍇ�́A�\�[�X�R�[�h���������č��o�������Ƃ�ReadMe���ɋL�q���Ă��������B
  
  DirectX ��2004���炢�̃o�[�W�������C���X�g�[�����K�v�ł��B
  �i�C���N���[�h�t�@�C�����������߁j
  lib�t�@�C�����ABorland�p�������肷��̂ŁA
  Visual Studio���ŃR���p�C������ɂ́AMicrosoft�p�ɍ��Ȃ�����Ƃ��K�v�ł��B
  
  �\�[�X���̂�C����ō���Ă��܂��B�iC++�ł͂Ȃ��j
  ���Â炢�E�����̈����\�[�X�Ȃ̂ł��������������c
  �܂��A�O���[�o���ϐ���֐����\�[�X�Ԃŋ��L���Ă��܂��Ă���̂ŁA
  �\�[�X�t�@�C���𔲂��o���đ��̃v���W�F�N�g�Ŏg�p���邱�Ƃ͓���Ǝv���܂��B
  
  ���ƁAUnicode�ɑΉ����邱�Ƃ�z�肵�ĂȂ����Ȃ̂Œ��ӂ��Ă��������B
  
*/

#include "Global.h"

#if DEBUG
HWND   hStatic;
char   testStr[BUF_SIZE_M];
DWORD  timeStart;
DWORD  timeStartSW = 0;
DWORD  timeMaxDelay;
DWORD  timeInst; // �v��
#endif

HWND  hWnd;
DWORD dwTimer = 0;
BOOL  windowClose = 0;
BOOL  threadClose = 0;

int   bgmType = 0; // 0:��~ 1:midi 2:ogg
int   midiPause;
int   midiGettingPlayerStatus;

int    midiVolume;
double midiTempo;
int    midiPitch;

int   midiFadeInTime;
int   midiFadeOutTime;
int   midiKeyShift;
int   midiStartTick;

int   midiSkipBeginning;
int   midiSkipEnding;
int   midiMasterPitch, midiPlayingMasterPitch;
int   midiNormalize;
int   midiExclusive;

int   playerDirectMusicMode, playerDirectMusicMode_old;
int   playerMode;
int   playerSpeedMode;
int   playerCacheMode;

int    oggVolume    = 127;
double oggTempo     = 1.0;
double oggPan       = 0.0;

int    oggFadeInTime  = 0;
int    oggFadeOutTime = 0;
int    oggStartTime   = 0;
int    oggLoopTime    = 0;
int    oggLoopEndTime = 0;
int    oggFileLoop    = 0;

// Tonyu��Ogg��炷�T���v�����[�h �Ŏg��
int    ogg3VolumeVariation = 0;
double ogg3VolumeCnt       = 0.0;
double ogg3VolumeWk        = 0.0;

BOOL  exitFlag = 0;
BOOL  midiPlayerOK = 0;



FARPROC GetModuleFileNameExA = 0;



// �X���b�h���Ŏg���ϐ� ///////////////////////////////////////////////////////////////////////////////////////////////////
int      cnt = 0, i, j, k;
DWORD    hdError;
DWORD    fileReadSize, fileReadSize2;
BOOL     eff_midiDirChangeFlag = 0;

char     bgmStr[_MAX_PATH], bgmStr_old[_MAX_PATH];
char     playedBgmStr[FILEPATH_SIZE];
char     oggBgmStr[_MAX_PATH], oggBgmStr_old[_MAX_PATH];
char     playedOggBgmStr[FILEPATH_SIZE];
char     eff_midiAddr[FILEPATH_SIZE];
char     eff_midiDir[FILEPATH_SIZE], eff_midiDir_old[FILEPATH_SIZE];
char     ogg_sigAddr[FILEPATH_SIZE];
char     midiPlayerStatusAddr[FILEPATH_SIZE];
char     defaultTonyuprjFileAddr[FILEPATH_SIZE];
char     usrFolderAddr[FILEPATH_SIZE];
char     YMPPreparationAddr[FILEPATH_SIZE];
char     readBuf[BUF_SIZE_S];
char     readBuf2[BUF_SIZE_S];
char     readBuf3[BUF_SIZE_S];
char     readBuf4[BUF_SIZE_S];
char     readBuf5[BUF_SIZE_S];
char     readBuf6[BUF_SIZE_S];
char     readBuf7[BUF_SIZE_S];

BOOL     playFlag        = 0;
BOOL     playFlagCnt     = 0;
BOOL     playingFlag     = 0;
int      playerStatusCnt = 0;
BOOL     loopFlag        = 1;
int      playId          = 0;
BOOL     fileDeleteFlag  = 0;
GGSERROR errorFlag       = 0;

int    loopFlag_r;

int    midiPause_r;
int    midiGettingPlayerStatus_r;

int    midiVolume_r;
double midiTempo_r;
int    midiPitch_r;

int    midiFadeInTime_r;
int    midiFadeOutTime_r;
int    midiKeyShift_r;
int    midiStartTick_r, midiStartTick_temp; 

int    midiSkipBeginning_r;
int    midiSkipEnding_r;
int    midiMasterPitch_r;
int    midiNormalize_r;
int    midiExclusive_r;

int    oggVolume_r;
double oggTempo_r;
double oggPan_r;

int    oggFadeInTime_r;
int    oggFadeOutTime_r;
int    oggStartTime_r;
int    oggLoopTime_r;
int    oggLoopEndTime_r;
int    oggFileLoop_r;

int    readValue; // MidiOggPlayer�p

GGS4SMFINFORMATION midiInfo;
GGS4PLAYERSTATUS midiStatus;

char  crDir[FILEPATH_SIZE];
char  playerExeName[FILEPATH_SIZE];
BOOL  processCheckFlag;
DWORD dwProcessId;
DWORD osPlatformId = 0;

char *midiFileAddrList[255];
int   midiFileAddrListLength = 0;
int   tempInt;
char  tempStr[BUF_SIZE_M];

BOOL  midiEffectFlag;
SYSTEMTIME stTime;

HANDLE hFile;

BOOL playerRestartSW = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////







int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
static void CALLBACK TimerThreadC(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	WNDCLASS winc;
	
	winc.style         = (CS_HREDRAW | CS_VREDRAW)*0;
	winc.lpfnWndProc   = WndProc;
	winc.cbClsExtra    = 0;
	winc.cbWndExtra	   = 0;
	winc.hInstance     = hInstance;
	winc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_1));
	winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName  = NULL;
	winc.lpszClassName = "WNDCLASS";
	
	if (!RegisterClass(&winc)) return 0;
	
	#if DEBUG
	hWnd = CreateWindow("WNDCLASS", "YellowMusicPlayer", WS_OVERLAPPEDWINDOW,
	                    0, 0, 350*3, 150+20*9, NULL, NULL, hInstance, NULL);
	#else
	hWnd = CreateWindow("WNDCLASS", "YellowMusicPlayer", WS_OVERLAPPEDWINDOW,
	                    0, 0, 350, 150, NULL, NULL, hInstance, NULL);
	#endif
	
	if (hWnd == NULL) return 0;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	#if DEBUG
	ShowWindow(hWnd, SW_SHOWNA);
	UpdateWindow(hWnd);
	#endif
	
	// GuruGuruSMF4��������
	if (GGS4Initialize() != GGSERROR_NOERROR) {
		MessageBox(0, "YellowMusicPlayer�ɕK�v��dll���Ȃ��A�܂��͓ǂݍ��߂Ȃ�����\nBGM�����t���邱�Ƃ��ł��܂���\n\n�uMidiPlayer2.exe�v������t�H���_��\n�uGuruGuruSMF4.dll�v��u���Ă�������", "YellowMusicPlayer �G���[", MB_ICONERROR);
		return 0;
	}
	
	// �X���b�h�֌W�̏�����
	if (mainInit()) {
		// �������ł�����^�C�}�X���b�h�����s
		dwTimer = timeSetEvent(15, 15, TimerThreadC, CREATE_SUSPENDED , TIME_PERIODIC);
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	
	return msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp) {
	switch (msg) {
	case WM_CLOSE:
		windowClose = 1;
		Sleep(50);
		
		break;
	case WM_DESTROY:
		if (midiPlayerOK) GGS4Free(); // �_�C�i�~�b�N�����N
		if (pDSNotify) IDirectSoundNotify_Release(pDSNotify);
    	if (pDSBuffer) IDirectSoundBuffer_Release(pDSBuffer);
    	if (pDS8) IDirectSoundBuffer_Release(pDS8);
    	oggFileClear();
		timeKillEvent(dwTimer);
		
		PostQuitMessage(0);
		return 0;
		
	#if DEBUG
	case WM_CREATE:
		hStatic = CreateWindow("STATIC" , "" ,WS_CHILD | WS_VISIBLE ,
		                       0 , 0 , 200 , 45 , hWnd , (HMENU)1 ,((LPCREATESTRUCT)(lp))->hInstance , NULL);
		return 0;
	case WM_SIZE:
		MoveWindow(hStatic , 0 , 0 , LOWORD(lp) , HIWORD(lp) , TRUE);
		return 0;
	#endif
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK TimerThreadC(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2) {
	
	cnt ++;
	
	
	// �v���Z�X�`�F�b�N��ON�Ȃ�A15*100�~���b(1.5�b)���Ƀv���Z�X�`�F�b�N
	if (processCheckFlag && cnt % 100 == 0) {
		// �v���Z�X�����邩�H
		if (!processExist(playerExeName, osPlatformId, dwProcessId)) exitFlag = 1;
	}
	
	
	playFlagCnt ++; // ���ԃJ�E���g
	
	
	// sig�t�@�C���ǂݍ���
	if (playFlag == 0) {
		
		// �ǂݍ��ݕp�x����
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel;
		
		// �t�@�C���ǂݍ���(sig)
		fileReadSize = readFile_C(".\\Kernel\\sig", readBuf, sizeof(readBuf) - 1, 0);
		if (fileReadSize < 3) goto readFileEndLabel;
		
		// �P�s��
		if (playerMode != 2) { // YellowMusicPlayer, TonyuMidiLitest, Tonyu��Ogg��炷�T���v�� //
			if (readBuf[0] == 'P') { // �Đ��i���[�v�Ȃ��j
				playFlag = 1;
				if (playerMode == 0 || playerMode == 3) loopFlag = 0;
				fileDeleteFlag = 0;
				
			}else if (readBuf[0] == 'R') { // �Đ��i���[�v����j
				playFlag = 1;
				loopFlag = 1;
				fileDeleteFlag = 0;
				
			}else if (readBuf[0] == 'S') { // ��~
				playFlag = 0;
				
				if (playingFlag != 0) { // �Đ���or�ꎞ��~��
					playerStatusCnt = 0;
					GGS4Stop(midiFadeOutTime);
					fileDeleteFlag = 5;
					playingFlag = 0;
					//bgmType = 0;
				}
				
			}else if (readBuf[0] == 'X') { // ����
				exitFlag = 1;
				fileDeleteFlag = 0;
			}
		} else { // MidiOggPlayer �̏ꍇ�̓t�@�C�����e���ς��������s //
			
			if (memcmp(readBuf, readBuf7, sizeof(readBuf)) != 0) { // �ς��������������
				memcpy(readBuf7, readBuf, sizeof(readBuf7));
				
				if (readBuf[0] == 'S') { // ��~
					
					playFlag = 0;
					if (playingFlag != 0) { // �Đ���or�ꎞ��~��
						memset(readBuf5, 0, sizeof(readBuf5));
						playerStatusCnt = 0;
						GGS4Stop(midiFadeOutTime);
						playingFlag = 0;
						//bgmType = 0;
					}
					
				} else if (readBuf[0] == 'X') { // ����
					exitFlag = 1;
					fileDeleteFlag = 0;
				} else {
					
					// �Q�s�ڂ��ύX����Ă��邩�m���߂�
					i = 0;
					j = 0;
					getLinePoint(readBuf, fileReadSize, &i); i+=2; j=i; // �P�s��
					if (i >= (int)fileReadSize) goto readFileEndLabel;  // �P�s�ڂŏI���Ȃ�I��
					
					if (memcmp(readBuf+j, readBuf5, sizeof(readBuf)-j) != 0) memcpy(readBuf5, readBuf+j, sizeof(readBuf)-j);
					else                                                     goto readFileEndLabel;
					
					playFlag = 1;
					fileDeleteFlag = 0;
				}
			}
			
		}
		
		
		
		
		
		
		
		// ���t����Ȃ�midi�t�@�C�������擾
		if (playFlag == 1) {
			i = 0;
			j = 0;
			getLinePoint(readBuf, fileReadSize, &i); i+=2; j=i; // �P�s��
			getLinePoint(readBuf, fileReadSize, &i);
			if (i >= (int)fileReadSize) {playFlag = 0; goto readFileEndLabel2;}
			
			// �t�@�C����
			if (i-j < sizeof(bgmStr) - 1) {
				memcpy(bgmStr, readBuf+j, i-j); // midi�t�@�C�������o
				bgmStr[i-j] = '\0';
			} else {
				bgmStr[j] = '\0';
				playFlag  = 0;
				if (playerMode != 2) GGS4Stop(0);
				goto bgmErrorLabel;
			}
			
			bgmErrorLabel:
				;if (playerMode != 2) fileDeleteFlag = 1;
			
			readFileEndLabel:
				;
		}
		
		// MidiOggPlayer���[�h�́umidi_sig.dat�v��ǂݍ���
		if (playerMode == 2) {
			if (playFlag == 0) { // sig�ōĐ�����Ă��Ȃ��Ƃ�
				
				// �ǂݍ��ݕp�x����
				if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel2;
				
				fileReadSize = readFile_C(".\\Usr\\files\\midi_sig.dat", readBuf, sizeof(readBuf) - 1, 0);
				if (fileReadSize <= 0) goto readFileEndLabel2;
				
				if (memcmp(readBuf, readBuf3, sizeof(readBuf)) != 0) { // �ύX����
					memcpy(readBuf3, readBuf, sizeof(readBuf));
					if      (readBuf[0] == '0') GGS4Stop(midiFadeOutTime);
					else if (readBuf[0] == '1') GGS4Pause();
					else if (readBuf[0] == '2') GGS4Restart();
					else {
						// ���΃p�X�̂܂܂ɂ���
						playFlag = 1;
						memcpy(bgmStr, readBuf, fileReadSize); // midi�t�@�C�������o
						bgmStr[fileReadSize] = '\0';
						
						// ���΃p�X���΃p�X�ɒ���
						//memset(tempStr, 0, sizeof(tempStr));
						//if (strlen(crDir) < sizeof(tempStr)) memcpy(tempStr, crDir, strlen(crDir));  else goto bgmErrorLabel2;
						//if (strlen(tempStr) + strlen("\\") < sizeof(tempStr)) strcat(tempStr, "\\"); else goto bgmErrorLabel2;
						//if (readBuf[0] == '.' && (readBuf[1] == '/' || readBuf[1] == '\\')) { // "./Usr/BGM/bgm01.mid"
						//	if (strlen(tempStr) + strlen(readBuf+2) < sizeof(tempStr)) strcat(tempStr, readBuf+2); else goto bgmErrorLabel2;
						//} else { // "Usr/BGM/bgm01.mid"
						//	if (strlen(tempStr) + strlen(readBuf) < sizeof(tempStr)) strcat(tempStr, readBuf); else goto bgmErrorLabel2;
						//}
						//memcpy(bgmStr, tempStr, sizeof(bgmStr)-1); // midi�t�@�C���̐�΃p�X���R�s�[
						//bgmStr[sizeof(bgmStr)-1] = '\0'; // �o�b�t�@�̖����ɕK��null�������t���悤�ɂ���i�v��Ȃ������j
						
					}
					
				}
			}
			
			bgmErrorLabel2:
				;
			readFileEndLabel2:
				;
		}
		
	}
	
	
	
	
	// eff_midi.txt ������΃A�h���X���Z�b�g���āA�G�t�F�N�g�t�@�C�����Ď�����悤�ɂ���
	// �Đ����鎞�Ƀ`�F�b�N
	if (((playFlag == 1 || playFlag == 2) && playerMode != 2) || (midiEffectFlag == 0 && PathFileExists(eff_midiAddr))) {
		
		if (midiEffectFlag == 0 && PathFileExists(eff_midiAddr)) { // Midi���炳�ꂸeff_midi.txt�����������ꂽ��
			memset(bgmStr, 0, sizeof(bgmStr));
			memcpy(bgmStr, eff_midiDir, sizeof(bgmStr) - 1);
		}
		
		// eff_midi.txt �̏ꏊ�𒲂ׂ�
		// eff_midiAddr, eff_midiDir ���X�V�����
		midiEffectFlag = setEffectFileAddr(bgmStr, 
                                           eff_midiAddr           , sizeof(eff_midiAddr),
                                           eff_midiDir            , sizeof(eff_midiDir),
                                           ogg_sigAddr            , sizeof(ogg_sigAddr),
                                           defaultTonyuprjFileAddr, sizeof(defaultTonyuprjFileAddr),
                                           usrFolderAddr          , sizeof(usrFolderAddr),
                                           midiPlayerStatusAddr   , sizeof(midiPlayerStatusAddr),
		                                   YMPPreparationAddr     , sizeof(YMPPreparationAddr)
                                           );
		
		// eff_midi.txt �̃f�B���N�g���̈ʒu���ς������G�t�F�N�g��������
		if (memcmp(eff_midiDir_old, eff_midiDir, sizeof(eff_midiDir_old)) != 0) {
			if (midiEffectFlag) eff_midiDirChangeFlag = 1;
			
			initMidiParam(); // �G�t�F�N�g���Z�b�g
			
			// ���̃A�h���X���L�^
			memset(eff_midiDir_old,           0, sizeof(eff_midiDir_old));
			memcpy(eff_midiDir_old, eff_midiDir, sizeof(eff_midiDir_old));
		}
		
		// eff_midi�̃f�B���N�g�����`�F���W�����Ƃ�
		if (eff_midiDirChangeFlag) {
            
			// default.tonyuprj ��ǂݍ���Midi�̃p�X����m�� or Midi��ǂݍ���
			beforeReadMidi(usrFolderAddr, defaultTonyuprjFileAddr, midiFileAddrList, &midiFileAddrListLength);
			
			eff_midiDirChangeFlag = 0;
		}
	}
	
	
	// �v���W�F�N�g���؂�ւ�������Adefault.tonyuprj�t�@�C����ǂݍ���ł���łȂ���
	// midi�ԍ�������Ɍ����ł��Ȃ��Ȃ�̂ł����֏����������Ă���
	if (playFlag == 1) {
		// �o�^����Ă���midi�t�@�C�������ׂ�
		for (playId = 0; playId < midiFileAddrListLength; playId++) {
			if (strcmpi(midiFileAddrList[playId], bgmStr) == 0) break ;
		}
		
		// �L���b�V�����[�h�̂Ƃ��͓ǂݍ��݂͂������̂܂܍Đ�
		if (playerCacheMode != 0) {
			if (playId < midiFileAddrListLength) { // �o�^����Ă���midi�������ꍇ
				playFlag = 2; // �Đ��t�F�[�Y��
			} else {
				playFlag = 1; // �ǂݍ��݃t�F�[�Y�ցi�o�O�Ƃ��������ꍇ�ɂ��ǂ߂�悤�Ɂj
			}
		} else { // �L���b�V���Ȃ��̂Ƃ��͓���
			if (memcmp(bgmStr, playedBgmStr, sizeof(bgmStr)) != 0) {
				memcpy(playedBgmStr, bgmStr, sizeof(playedBgmStr));
				playFlag = 1; // �ǂݍ��݃t�F�[�Y��
			} else {
				playFlag = 2; // �Đ��t�F�[�Y��
			}
		}
		
		//GGS4GetPlayerStatus(&midiStatus, sizeof(midiStatus));
		GGS4GetPlayerStatus(&midiStatus);
		if (midiStatus.State == GGSSTATE_STOP) { // ���ɒ�~���Ă���
			playFlagCnt = 2; // �Q�t���[�����҂��Ȃ�
		} else { // �Đ���or�ꎞ��~��
			GGS4Stop(0);
			playFlagCnt = 0; // �Q�t���[�����҂�
		}
	}
	
	
	// �t�@�C���ǂݍ���(eff_midi.txt)
	if (midiEffectFlag && playerMode != 2) {
		
		// �ǂݍ��ݕp�x����
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto eff_midiEndLabel;
		
		fileReadSize2 = readFile_C(eff_midiAddr, readBuf2, sizeof(readBuf2) - 1, 0);
		if (fileReadSize2 >= 2) {
			// midi����
			i = 0;
			j = 0;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiVolume_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiVolume_r != -9999) {
				if      (midiVolume_r > 127) midiVolume_r = 127;
				else if (midiVolume_r <   0) midiVolume_r =   0;
				midiVolume_r -= 127;
				if (midiVolume_r != midiVolume) {
					midiVolume = midiVolume_r;
					GGS4SetMasterVolume(midiVolume);
				}
			}
			
			// midi�e���|
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiTempo_r = strToDouble(readBuf2, j, i, -9999.0); i+=2; j=i;
			if (midiTempo_r != -9999.0) {
				if      (midiTempo_r > 8.0) midiTempo_r = 8.0;
				else if (midiTempo_r < 0.1) midiTempo_r = 0.1;
				if (midiTempo_r != midiTempo) {
					midiTempo = midiTempo_r;
					GGS4SetMasterTempo(midiTempo);
				}
			}
			
			// midi�t�F�[�h�A�E�g
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiFadeOutTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiFadeOutTime_r != -9999) {
				if (midiFadeOutTime_r < 0) midiFadeOutTime_r = 0;
				midiFadeOutTime = midiFadeOutTime_r;
			}
			
			// midi�ꎞ��~�E�ĊJ
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiPause_r = strToInt(readBuf2, j, i, -9999);  i+=2; j=i;
			if (midiPause_r != -9999) {
				if (midiPause_r != midiPause) {
					midiPause = midiPause_r;
					if (midiPause == 1) GGS4Pause();
					else                GGS4Restart();
					playerStatusCnt = 2; // 0�ɂ���Ə������ڂ����������₷��
				}
			}
			
			
			// TonyuMidiLitest���[�h�̏ꍇ�A�����ŏ�����ł��؂�
			if (playerMode == 1) goto eff_midiEndLabel;
			
			
			// midi�s�b�`
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiPitch_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiPlayingMasterPitch && midiPitch_r != -9999) {
				if      (midiPitch_r >  8191) midiPitch_r =  8191;
				else if (midiPitch_r < -8192) midiPitch_r = -8192;
				if (midiPitch_r != midiPitch) {
					midiPitch = midiPitch_r;
					GGS4SetMasterPitch(midiPitch);
				}
			}
			
			// midi�t�F�[�h�C��
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiFadeInTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiFadeInTime_r != -9999) {
				if (midiFadeInTime_r < 0) midiFadeInTime_r = 0;
				midiFadeInTime = midiFadeInTime_r;
			}
			
			// midi�L�[�V�t�g
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiKeyShift_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiKeyShift_r != -9999) {
				midiKeyShift = midiKeyShift_r;
			}
			
			// �X�^�[�g�e�B�b�N
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiStartTick_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiStartTick_r != -9999) {
				if (midiStartTick_r < 0) midiStartTick_r = 0;
				midiStartTick = midiStartTick_r;
			}
			
			// midi�`���X�L�b�v
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiSkipBeginning_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiSkipBeginning_r != -9999) {
				midiSkipBeginning = !!midiSkipBeginning_r;
			}
			
			// midi�����X�L�b�v
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiSkipEnding_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiSkipEnding_r != -9999) {
				midiSkipEnding = !!midiSkipEnding_r;
			}
			
			// midi�}�X�^�[�s�b�`
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiMasterPitch_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiMasterPitch_r != -9999) {
				midiMasterPitch = !!midiMasterPitch_r;
			}
			
			// midi�m�[�}���C�Y
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiNormalize_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiNormalize_r != -9999) {
				midiNormalize = !!midiNormalize_r;
			}
			
			// midi�G�N�X�N���[�V�u
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiExclusive_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiExclusive_r != -9999) {
				midiExclusive = !!midiExclusive_r;
			}
			
			// gettingPlayerStatus
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiGettingPlayerStatus_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiGettingPlayerStatus_r != -9999) {
				if      (midiGettingPlayerStatus_r <    0) midiGettingPlayerStatus_r =    0;
				else if (midiGettingPlayerStatus_r > 1000) midiGettingPlayerStatus_r = 1000;
				if (midiGettingPlayerStatus_r != midiGettingPlayerStatus) {
					midiGettingPlayerStatus = midiGettingPlayerStatus_r;
					if (playerStatusCnt >= midiGettingPlayerStatus) playerStatusCnt = midiGettingPlayerStatus;
				}
			}
			
			
			// Tonyu��ogg��炷�T���v�����[�h �̏ꍇ�͂����ŏ�����ł��؂�
			if (playerMode == 3)  goto eff_midiEndLabel;
			
			// ogg����
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggVolume_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggVolume_r != -9999) {
				if      (oggVolume_r > 127) oggVolume_r = 127;
				else if (oggVolume_r <   0) oggVolume_r =   0;
				if (oggVolume_r != oggVolume) {
					oggVolume = oggVolume_r;
					oggSetVolume(oggVolume);
				}
			}
			
			// ogg�e���|
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggTempo_r = strToDouble(readBuf2, j, i, -9999.0); i+=2; j=i;
			if (oggTempo_r != -9999.0) {
				if      (oggTempo_r > 8.0  ) oggTempo_r = 8.0  ;
				else if (oggTempo_r < 0.001) oggTempo_r = 0.001;
				if (oggTempo_r != oggTempo) {
					oggTempo = oggTempo_r;
					oggSetFrequency(oggTempo);
				}
			}
			
			// ogg�p��
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggPan_r = strToDouble(readBuf2, j, i, -9999.0); i+=2; j=i;
			if (oggPan_r != -9999.0) {
				if      (oggPan_r >  1.0) oggPan_r =  1.0;
				else if (oggPan_r < -1.0) oggPan_r = -1.0;
				if (oggPan_r != oggPan) {
					oggPan = oggPan_r;
					oggSetPan(oggPan);
				}
			}
			
			// ogg�t�F�[�h�C��
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggFadeInTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggFadeInTime_r != -9999) {
				if (oggFadeInTime_r < 0) oggFadeInTime_r = 0;
				oggFadeInTime = oggFadeInTime_r;
			}
			
			// ogg�t�F�[�h�A�E�g
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggFadeOutTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggFadeOutTime_r != -9999) {
				if (oggFadeOutTime_r < 0) oggFadeOutTime_r = 0;
				oggFadeOutTime = oggFadeOutTime_r;
			}
			
			// �X�^�[�g�e�B�b�N
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggStartTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggStartTime_r != -9999) {
				if (oggStartTime_r < 0) oggStartTime_r = 0;
				oggStartTime = oggStartTime_r;
			}
			
			// ogg���[�v���Ė߂��Ă���ʒu
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggLoopTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggLoopTime_r != -9999) {
				if (oggLoopTime_r < 0) oggLoopTime_r = 0;
				oggLoopTime = oggLoopTime_r;
			}
			
			// ogg���[�v����ʒu
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggLoopEndTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggLoopEndTime_r != -9999) {
				if (oggLoopEndTime_r < 0) oggLoopEndTime_r = 0;
				oggLoopEndTime = oggLoopEndTime_r;
			}
			
			// ogg�t�@�C���̃��[�v���g��
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggFileLoop_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggFileLoop_r != -9999) {
				oggFileLoop = !!oggFileLoop_r;
			}
			
			
		}
		eff_midiEndLabel:
			;
	}
	
	// MidiOggPlayer���[�h�́ubgm_config.dat�v��ǂݍ���
	// MidiOggPlayer��Tonyu�J�����łł͓����Ȃ��̂ŁA�Ή������Ȃ��Ă�������I�I
	if (playerMode == 2) {
		
		// �ǂݍ��ݕp�x����
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto eff_midiEndLabel2;
		
		fileReadSize2 = readFile_C(".\\Usr\\files\\bgm_config.dat", readBuf2, sizeof(readBuf2) - 1, 0);
		if (fileReadSize2 <= 0) goto eff_midiEndLabel2;
		
		if (memcmp(readBuf2, readBuf6, sizeof(readBuf2)) != 0) memcpy(readBuf6, readBuf2, sizeof(readBuf6));
		else                                                   goto eff_midiEndLabel2;
		
		if (fileReadSize2 >= 2) {
			readValue = 0;
			i = 0;
			j = 0;
			// ����(midi)
			getLinePoint(readBuf2, fileReadSize2, &i);
			readValue  = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			midiVolume = readValue;
			if      (midiVolume > 127) midiVolume = 127;
			else if (midiVolume <   0) midiVolume =   0;
			midiVolume -= 127;
			GGS4SetMasterVolume(midiVolume);
			
			// ���[�v(midi)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			loopFlag = !!readValue;
			
			// �t�F�[�h�A�E�g(midi)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			midiFadeOutTime = readValue;
			if (midiFadeOutTime < 0) midiFadeOutTime = 0;
			
			// ����(ogg)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			oggVolume = readValue / 2;
			if      (oggVolume > 127) oggVolume = 127;
			else if (oggVolume <   0) oggVolume =   0;
			oggSetVolume(oggVolume);
			
			// ���[�v�E���[�v�ʒu(ogg)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			oggLoopTime = readValue - 1;
			if (oggLoopTime < 0) oggLoopTime = 0;
			oggLoopFlag = 1;
			oggLoopSeek = (double)oggLoopTime / 1000.0;
			if (oggLoopSeek >= oggTotalTime - 0.04) oggLoopSeek = oggTotalTime - 0.04;
			
			// �t�F�[�h�A�E�g(ogg)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			oggFadeOutTime = readValue * 1000 / 60;
			if (oggFadeOutTime < 0) oggFadeOutTime = 0;
			
		}
		eff_midiEndLabel2:
			;
	}
	
	
	
	
	// midi�֌W ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ǂݍ��݁E�Đ� //
	if (playFlag == 1) { // midi�t�@�C����ǂݍ���ł���Đ�
		if (playFlagCnt >= 30) {playFlagCnt = 0; playFlag = 0;} // ���Ԑ؂�
		
		// midi�t�@�C���ǂݍ��� //
		if (playerCacheMode != 0) i = playId;
		else                      i = 0;
		errorFlag = GGS4AddListFromFileA(bgmStr, GGSLOAD_BUFFERED*1, i);
		if (errorFlag != 0) goto playEndLabel;
		
		if (midiFileAddrListLength < 255 && playId == midiFileAddrListLength) { // playId���V�����ԍ��Ȃ�midi�̃p�X����ǉ�
			// midi�t�@�C���A�h���X���X�g�ɁAmidi�̐�΃A�h���X��o�^
			midiFileAddrList[midiFileAddrListLength] = (char *)malloc(sizeof(char) * strlen(bgmStr) + 1); // free()�͏����̂߂�ǂ��̂ŏȗ��c
			if (midiFileAddrList[midiFileAddrListLength] == NULL) exit(-1);
			memset(midiFileAddrList[midiFileAddrListLength], 0, strlen(bgmStr) + 1);
			memcpy(midiFileAddrList[midiFileAddrListLength], bgmStr, strlen(bgmStr));
			
			midiFileAddrListLength ++;
		}
		
		//playFlagCnt = 0;
		playFlag = 2;
	}
	if (playFlag == 2 && playFlagCnt >= 2) { // 15*2�~���b�o�߂�����ɍĐ�or���Đ�
		if (playFlagCnt >= 30) {playFlagCnt = 0; playFlag = 0;} // ���Ԑ؂�
		
		// �Đ� //
		if (playerCacheMode != 0) i = playId;
		else                      i = 0;
		
		// StartTick �� TotalTic �𒴂���Ƌ����I���ɂȂ邽�� TotalTic �𒲂ׂ�
		//GGS4GetSmfInformation(&midiInfo, sizeof(midiInfo), playId);
		GGS4GetSmfInformation(&midiInfo, i);
		if (midiStartTick >= midiInfo.TotalTick) midiStartTick_temp = midiInfo.TotalTick - 1;
		else                                     midiStartTick_temp = midiStartTick;
		
		// �}�X�^�[�s�b�`�̒���
		midiPlayingMasterPitch = midiMasterPitch;
		if (midiPlayingMasterPitch) GGS4SetMasterPitch(midiPitch);
		else                        GGS4SetMasterPitch(0);
		
		midiPause = 0;
		
		errorFlag = GGS4Play(GGSPLAY_LOOP          * loopFlag          |
		                     GGSPLAY_SKIPBEGINNING * midiSkipBeginning |
		                     GGSPLAY_SKIPENDING    * midiSkipEnding    |
		                     GGSPLAY_MASTERPITCH   * midiMasterPitch   |
		                     GGSPLAY_NORMALIZE     * midiNormalize     |
		                     GGSPLAY_EXCLUSIVE     * midiExclusive
		                     ,i, midiStartTick_temp, midiFadeInTime, midiKeyShift);
		
		if (errorFlag != 0) goto playEndLabel;
		
		playerStatusCnt = 0;
		playFlag = 0;
		playingFlag = 1;
		//bgmType = 1;
		//fileDeleteFlag = 1;
	}
	
	playEndLabel:
		;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// ogg�֌W ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �t�@�C���ǂݍ���(ogg_sig.dat) //////////////////////////////////////////////////////////////////
	
	if (playerMode != 1) { // TonyuMidiLitest���[�h�ȊO��ogg�̏��������s����
		// �ǂݍ��ݕp�x����
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel3;
		
		fileReadSize = readFile_C(ogg_sigAddr, readBuf, sizeof(readBuf) - 1, 0);
		if (fileReadSize <= 0) goto readFileEndLabel3;
		
		if (playerMode == 2 || playerMode == 3) {
			if (memcmp(readBuf, readBuf4, sizeof(readBuf)) != 0) { // MidiOggPlayer ���� Tonyu��Ogg��炷�T���v�� �̏ꍇ�̓t�@�C�����e���ς��������s
				memcpy(readBuf4, readBuf, sizeof(readBuf));
			} else {
				goto readFileEndLabel3;
			}
		}
		
		// ������ogg�̍Đ��E��~�������s���Ă��܂� //////////////////////
		
		if (playerMode == 3) {
			if (ogg3VolumeCnt > 0.0 && readBuf[0] != '3') ogg3VolumeCnt = 0.0;
		}
		i = 0;
		j = 0;
		getLinePoint(readBuf, fileReadSize, &i);
		if (readBuf[0] == '0' && i == 1) { // ��~
			oggStop(oggFadeOutTime);
			
		} else if(readBuf[0] == '1' && i == 1) { // �ꎞ��~
			oggPause();
			
		} else if(readBuf[0] == '2' && i == 1) { // �ĊJ
		  	oggRestart();
			
		} else if(readBuf[0] == '3' && i == 1 && playerMode == 3) { // ���ʒ��� (Tonyu��Ogg��炷�T���v���p)
			
			if (ogg3VolumeCnt == 0.0) {
				// �ω���
				i+=2; j=i;
				getLinePoint(readBuf, fileReadSize, &i);
				k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
				if (k != -99999999) ogg3VolumeVariation = k;
				
				// ������
				if (i < (int)fileReadSize) {
					getLinePoint(readBuf, fileReadSize, &i);
					k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
					if (k != -99999999) {
						if (k < 0) k = 0;
						ogg3VolumeCnt = (double)k;
					}
				}
			}
		} else if(readBuf[0] == '9' && i == 1 && playerMode == 3){
			; // �������Ȃ�
			
		} else { // �t�@�C���p�X
			//timeStartSW = 1;
			
			memset(oggBgmStr, 0, sizeof(oggBgmStr));
			memcpy(oggBgmStr, readBuf, i); i+=2; j=i;
			
			// ogg�t�@�C���̐�΃p�X�����߂�i���[�v��������菜���j
			if (playerMode == 0) {
				memset(tempStr, 0, sizeof(tempStr));
				//for (k = strlen(usrFolderAddr) - 1; k >= 0; k--) { // Usr�t�H���_�̐e�t�H���_�Ɉڂ�
				//	if (usrFolderAddr[k] == '\\') break ;
				//}
				//if (playerMode == 3) { // ����ɏ�Ɉڂ�
				//	for (k = k - 1; k >= 0; k--) { // Usr�t�H���_�̐e�t�H���_�̐e�t�H���_�Ɉڂ�
				//		if (usrFolderAddr[k] == '\\') break ;
				//	}
				//}
				if (k < 0) goto bgmErrorLabel3; // �p�X���s��
				
				if (strlen(usrFolderAddr) < sizeof(tempStr)) memcpy(tempStr, usrFolderAddr, strlen(usrFolderAddr));  else goto bgmErrorLabel3;
				if (strlen(tempStr) + strlen("\\") < sizeof(tempStr)) strcat(tempStr, "\\"); else goto bgmErrorLabel3;
				if (oggBgmStr[0] == '.' && (oggBgmStr[1] == '/' || oggBgmStr[1] == '\\')) { // "./Usr/BGM/bgm01.ogg"
					if (strlen(tempStr) + strlen(oggBgmStr+2) < sizeof(tempStr)) strcat(tempStr, oggBgmStr+2); else goto bgmErrorLabel3;
				} else { // "Usr/BGM/bgm01.ogg"
					if (strlen(tempStr) + strlen(oggBgmStr) < sizeof(tempStr)) strcat(tempStr, oggBgmStr); else goto bgmErrorLabel3;
				}
				memcpy(oggBgmStr, tempStr, sizeof(oggBgmStr)-1); // ogg�t�@�C���̐�΃p�X���R�s�[
				oggBgmStr[sizeof(oggBgmStr)-1] = '\0'; // �o�b�t�@�̖����ɕK��null�������t���悤�ɂ���i�v��Ȃ������j
			}
			
			if (playerMode == 0) { // MidiOggPlayer, Tonyu��Ogg��炷�T���v�� �͂��ꂼ�� bgm_config.dat , loop.txt �Ń��[�v���w�肷��̂ł����͕s�v
				if (i < (int)fileReadSize && readBuf[i] == 0) oggLoopFlag = 0;
				else                                          oggLoopFlag = 1;
			} else if (playerMode == 3) { // Tonyu��Ogg��炷�T���v�� �͉��ʁE���[�v�w����ʂɐݒ�ł���
				
				// Program�t�H���_�𖳎�����Usr�t�H���_���猩��
				i-=2; if (i<0) i=0;
				if (oggBgmStr[0] == '.' && (oggBgmStr[1] == '/' || oggBgmStr[1] == '\\')) k = 2; // "./Usr/BGM/bgm01.ogg"
				else                                                                      k = 0;
				memset(tempStr, 0, sizeof(tempStr));
				memcpy(tempStr, oggBgmStr, sizeof(tempStr) - 1);
				for (; k <= sizeof(tempStr) - 1; k++) {
					if (tempStr[k] == '\\' || tempStr[k] == '/' || tempStr[k] == '\0' || tempStr[k] == '\r') break ;
				}
				if (tempStr[k] == '\\' || tempStr[k] == '/') k ++;
				if (i <= (int)fileReadSize && k < (int)fileReadSize) {
					memset(oggBgmStr, 0, sizeof(oggBgmStr));
					memcpy(oggBgmStr, tempStr + k, i);
				}
				i+=2;
				
				oggVolume   = ogg3Volume;
				oggLoopFlag = ogg3LoopFlag;
				if (i < (int)fileReadSize) { // ����
					getLinePoint(readBuf, fileReadSize, &i);
					k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
					if (k == -99999999) {
						if (k <     0) k =     0;
						if (k > 10000) k = 10000;
						oggVolume = (int)( (double)(k * 127 / 10000) + 0.5);
					}
					if (i < (int)fileReadSize) { // ���[�v
						getLinePoint(readBuf, fileReadSize, &i);
						k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
						if (k == -99999999) {
							oggLoopFlag = !!k;
						}
					} else { // �R�s�ڂ����������特�ʃ��Z�b�g
						oggVolume = ogg3Volume;
					}
				}
				ogg3VolumeWk = (double)oggVolume;
			}
			
			//timeStart = timeGetTime();
			if (memcmp(oggBgmStr, oggBgmStr_old, sizeof(oggBgmStr)) != 0) { // �ύX����
				memcpy(oggBgmStr_old, oggBgmStr, sizeof(oggBgmStr));
				if (playerMode != 3) { // Yellow�ł͊J�n�ʒu�w��E���[�v��r������Đ��E�t�F�[�h�C�� �̋@�\���g����
					if (!oggPlay(oggBgmStr, oggLoopFlag, oggFileLoop, (double)oggStartTime/1000.0, (double)oggLoopTime/1000.0, (double)oggLoopEndTime/1000.0, oggFadeInTime)) goto bgmErrorLabel3; // �V�����ǂݍ��ݒ����čĐ�
					
				} else { // Tonyu��Ogg��炷�T���v�� �ł͊J�n�ʒu�w��E���[�v��r������Đ��E�t�F�[�h�C�� �̋@�\���g���Ȃ�
					if (!oggPlay(oggBgmStr, oggLoopFlag, 0, 0.0, 0.0, 0.0, 0)) goto bgmErrorLabel3; // �V�����ǂݍ��ݒ����čĐ�
					
				}
			} else {
				if (playerMode != 3) {
					if (!oggPlay(NULL, oggLoopFlag, oggFileLoop, (double)oggStartTime/1000.0, (double)oggLoopTime/1000.0, (double)oggLoopEndTime/1000.0, oggFadeInTime)) goto bgmErrorLabel3; // �O�ɂȂ炵���Ȃ����̂܂܍Đ�
					
				} else {
					if (!oggPlay(NULL, oggLoopFlag, 0, 0.0, 0.0, 0.0, 0)) goto bgmErrorLabel3; // �O�ɂȂ炵���Ȃ����̂܂܍Đ�
					
				}
			}
			
		}
		
		
		bgmErrorLabel3:
			if (playerMode == 0) writeFile_C(ogg_sigAddr, "", 0, 0); // �t�@�C������ɂ���
		
		readFileEndLabel3:
			;
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//StopProc(); // �f�o�b�O�p�i�����ŏ������؂�����c���������̍Č��ۂ����ȁj
		
		if (oggPauseFlag) oggPause(); // ���Ԃ�x�点�Ĉꎞ��~�i�o�O�h�~�j
		
		if (oggPlayerStatus == STATUS_PLAY) {
			
			// �o�b�t�@�������ݑ��x�𐧌�
			if (oggBufWriteSpeed) k = (int)oggSect / 4;
			else {
				if (midiTempo < 1.0) k = 3;             // �e���|��1�����Ȃ�X�s�[�h = 1
				else                 k = 3 * midiTempo; // �e���|��1�ȏ�Ȃ�X�s�[�h = (int)midiTempo
			}
			
			
			
			for (i = 0; i < k; i++) {
	    		IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint, 0);
				
				// �Đ����Ԍv�Z //
				getOggPlayingTime();
				
				if ((oggFlag != oggPoint / oggSectSize || oggAllWriteFlag >= 1) && oggAllWriteFlag >= 0) {
					
					if (oggFlag != oggSect - 2 || (oggFlag == oggSect - 2 && oggAllWriteFlag >= 1)) { // �o�b�t�@�̖����̎��́A�Đ��ꏊ���o�b�t�@�����[�v���Ă�����s
						// ��������
						if (IDirectSoundBuffer_Lock(pDSBuffer, oggSectSize * oggFlag, oggSectSize, &AP1, &AB1, &AP2, &AB2, 0) == DS_OK) {
							getPCMBuffer(&ovf, (char*)AP1, AB1, oggLoopFlag, oggLoopSeek, oggLoopEndSeek, &oggEndFlag);
							IDirectSoundBuffer_Unlock(pDSBuffer, AP1, AB1, AP2, AB2);
							if (oggEndFlag) {
								if ((oggLoopEndSeek == 0.0 && oggPlayingTimeInt >= oggTotalTimeInt) ||
								    (oggLoopEndSeek >  0.0 && oggPlayingTimeInt >= (DWORD)(oggLoopEndSeek * 1000))) {
									oggEndCnt  = 0;
									oggEndFlag --;
									
									oggLoopCnt ++;
									if (oggLoopFlag) {
										tempInt = oggPlayingTimeInt2;
										for (j = 0; j < 100; j++) {
											oggPlayingTimeInt2 = (int)(oggLoopSeek * 1000.0);
											if (oggPlayingTimeInt <= (DWORD)(oggLoopSeek * 1000.0)) break ;
											getOggPlayingTime();
											if (oggPlayingTimeInt >= (DWORD)tempInt) oggLoopCnt ++; // �J�E���g������Ȃ������炳��ɃJ�E���g
											getOggPlayingTime();
										}
									}
									oggPlayingTimeInt2 = (int)(oggLoopSeek * 1000.0);
									if (!oggLoopFlag) { oggStop(0); break; }
								}
							}
							
							if (oggFlag == oggSect - 2) { // �o�b�t�@�̖���
								oggAllWriteFlag --; // �ǂ������̂Ō��炷
							}
							oggFlag = (oggFlag + 1) % oggSect; // ���̃o�b�t�@�Ɉʒu�����킹��
						}
					}
				} else {
					break ;
				}
			}
			
			if (playerMode != 3) { // �t�F�[�h�C���E�t�F�[�h�A�E�g�̏���
				// �t�F�[�h�C���̉��ʒ���
				if (oggFadeInTimeRun > 0) {
					oggFadeCnt += (1000.0 / (double)oggFadeInTimeRun) * (15.0 / 1000.0);
					if (oggFadeCnt >= 1.0) {
						oggFadeCnt       = 1.0;
						oggFadeInTimeRun = 0;
					}
					oggSetVolume(oggVolume * oggFadeCnt);
				}
				// �t�F�[�h�A�E�g�̉��ʒ���
				if (oggFadeOutTimeRun > 0) {
					oggFadeCnt -= (1000.0 / (double)oggFadeOutTimeRun) * (15.0 / 1000.0);
					if (oggFadeCnt <= 0.0) {
						oggFadeCnt        = 0.0;
						oggFadeOutTimeRun = 0;
						oggStop(0);
					}
					oggSetVolume(oggVolume * oggFadeCnt);
				}
			} else { // ���ʒ��߁E�t�F�[�h�A�E�g�̏��� (Tonyu��Ogg��炷�T���v�����[�h)
				// ���ʒ���
				if (ogg3VolumeCnt > 0.0) {
					ogg3VolumeWk  += (double)ogg3VolumeVariation * 127.0 * (15.0 / 250.0) / 10000.0;
					ogg3VolumeCnt -= (15.0 / 250.0);
					
					oggVolume = (int)ogg3VolumeWk;
					if (ogg3VolumeWk < 0.0) { // ���ʂ�0�����ɂȂ���
						oggVolume = 0;
					} else if (ogg3VolumeWk > 127.0) { // ���ʂ�127�����ɂȂ���
						oggVolume = 127;
					}
					if (ogg3VolumeCnt <= 0.0) { // �J�E���g���I������
						ogg3VolumeCnt = 0.0;
						// �ǂݍ��߂���9�ɂ��ď�������
						readBuf[0] = '9';
						writeFile_C(ogg_sigAddr, readBuf, fileReadSize, 0);
					}
					
					oggSetVolume2(oggVolume);
				}
				
				// �t�F�[�h�A�E�g�̉��ʒ���
				if (oggFadeOutTimeRun > 0) {
					oggFadeCnt -= (1000.0 / (double)oggFadeOutTimeRun) * (15.0 / 1000.0);
					if (oggFadeCnt <= 0.0) {
						oggFadeCnt        = 0.0;
						oggFadeOutTimeRun = 0;
						oggStop(0);
					}
					oggSetVolume2(oggVolume * oggFadeCnt);
				}
			}
			
			
			// �ǂݍ��ݎn�߂͒x���A�Đ��ʒu�܂Œǂ���or�Đ��ʒu���P�T�x�� �ɂȂ�Ƒ��x�𑬂߂�
			if (oggBufWriteSpeed == 0 && ((oggAllWriteFlag == 0 && oggFlag == oggPoint / oggSectSize) || (oggFlag - (oggPoint / oggSectSize)) + (1 - oggAllWriteFlag) * oggSect <= 0)) oggBufWriteSpeed = 1;
			
			
			#if DEBUG
			/*
			if (timeStartSW && oggPoint != 0) {
				// �����ɂ����������Ԃ��v��
				timeInst = timeGetTime() - timeStart;
				if (timeMaxDelay <= timeInst) timeMaxDelay = timeInst;
				sprintf(testStr, "%d %d", timeMaxDelay, oggPoint);
				SetWindowText(hStatic, testStr);
				timeMaxDelay = 0;
				timeStartSW  = 0;
			}
			*/
			/*
			sprintf(testStr, "oggPoint:[%d] %d\r\n��������[%d] oggFlag:%d %u %u %f\r\noggAllWriteFlag:%d\r\noggLoopCnt:%d oggBufLoopCnt:%d\r\noggFadeCnt:%f oggFadeInTime:%d oggFadeOutTime:%d oggVolume:%d \r\n%d BufSize:%d BufWriteSpeed:%d ov_time_tell:%f\r\noggLoopSeek:%f oggLoopEndSeek:%f oggFileLoop:%d\r\n%f %f %d %f %f %f   %d %f",
			        oggPoint, oggPoint / oggSectSize, oggSectSize * oggFlag, oggFlag, AB1, AB2, (double)DSBufferDesc.dwBufferBytes / oggSect, oggAllWriteFlag,
			        oggLoopCnt, oggBufLoopCnt,
			        oggFadeCnt, oggFadeInTime, oggFadeOutTime, oggVolume,
			        (oggFlag - (oggPoint / oggSectSize)) + (1 - oggAllWriteFlag) * oggSect, DSBufferDesc.dwBufferBytes, oggBufWriteSpeed, ov_time_tell(&ovf),
			        oggLoopSeek, oggLoopEndSeek, oggFileLoop,
			        (double) oggPoint / (waveFormat.nSamplesPerSec * waveFormat.nBlockAlign),
	                oggStartSeek,                                    // 
	                oggBufLoopCnt * oggPlayTime,                     // �o�b�t�@�̎���
	                oggLoopSeek   * oggLoopCnt,                      // ���[�v�n�[
	                - (oggLoopEndSeek == 0.0) * (oggTotalTime * oggLoopCnt),      // ���[�v�I�[���Ȃ�
	                - (oggLoopEndSeek >  0.0) * ((oggLoopEndSeek) * oggLoopCnt), // ���[�v�I�[������
	                oggPlayingTimeInt2, (ov_time_tell(&ovf) - oggPlayingTime - oggPlayTime)
			        );
			SetWindowText(hStatic, testStr);
			*/
			/*
			sprintf(testStr, "ogg3VolumeCnt:%f ogg3VolumeVariation:%d ogg3VolumeWk:%f oggVolume:%d",
			        ogg3VolumeCnt, ogg3VolumeVariation, ogg3VolumeWk, oggVolume);
			*/
			//SetWindowText(hStatic, testStr);
			
			
			#endif
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if (midiGettingPlayerStatus > 0) {
		if (playerStatusCnt <= 0 && midiPlayerStatusAddr[0] != '\0') {
			GetLocalTime(&stTime);
			memset(tempStr, 0, 256);
			i = 0;
			// midi�Đ����Ԏ擾 //
			GGS4GetPlayerStatus(&midiStatus);
			
			memcpy(tempStr+i, &midiStatus.State      , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &playId                , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &midiStatus.Tempo      , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiStatus.Tick       , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiStatus.Time       , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiStatus.LoopCount  , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.TimeBase     , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.TotalTick    , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.TotalTime    , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.FirstNoteTick, sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.FirstNoteTime, sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.LastNoteTick , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.LastNoteTime , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.LoopTick     , sizeof(int));  i += sizeof(int);
			memcpy(tempStr+i, &midiInfo.LoopTime     , sizeof(int));  i += sizeof(int);
			
			// ogg�Đ����Ԍv�Z //
    		if (pDSBuffer) {
    			IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint, 0);
				if (oggPlayerStatus != STATUS_STOP) getOggPlayingTime();
			}
			memcpy(tempStr+i, &oggPlayerStatus          , sizeof(BYTE));  i += sizeof(BYTE); // 
			memcpy(tempStr+i, &oggPlayingTimeInt        , sizeof(int));   i += sizeof(int);  // 
			memcpy(tempStr+i, &oggLoopCnt               , sizeof(int));   i += sizeof(int);  // 
			memcpy(tempStr+i, &waveFormat.nChannels     , sizeof(BYTE));  i += sizeof(BYTE); // �`�����l����(2�o�C�g)
			memcpy(tempStr+i, &waveFormat.nSamplesPerSec, sizeof(int));   i += sizeof(int);  // �T���v�����O���[�g
			memcpy(tempStr+i, &oggTotalTimeInt          , sizeof(int));   i += sizeof(int);  // ���v����
			memcpy(tempStr+i, &oggTotalPcm              , sizeof(int));   i += sizeof(int);  // ���v�T���v����
			memcpy(tempStr+i, &oggLoopSeekInt           , sizeof(int));   i += sizeof(int);  // Ogg���[�v���n�܂�ʒu
			memcpy(tempStr+i, &oggLoopEndSeekInt        , sizeof(int));   i += sizeof(int);  // Ogg���[�v���I���ʒu
			
			// ���t�E����
			memcpy(tempStr+i, &stTime.wHour        , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wMinute      , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wSecond      , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wMilliseconds, sizeof(WORD)); i += sizeof(WORD);
			memcpy(tempStr+i, &stTime.wYear        , sizeof(WORD)); i += sizeof(WORD);
			memcpy(tempStr+i, &stTime.wMonth       , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wDay         , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wDayOfWeek   , sizeof(BYTE)); i += sizeof(BYTE);
			
			//sprintf(testStr, "%d", i);
			//MessageBox(0,testStr,0,0);
			
			hdError = writeFile_C(midiPlayerStatusAddr, tempStr, i, 0);
			if (hdError != 0) playerStatusCnt = midiGettingPlayerStatus;
		}
		if (midiGettingPlayerStatus < 1000 || (midiGettingPlayerStatus >= 1000 && playerStatusCnt <= 5))
			playerStatusCnt --; // midiGettingPlayerStatus �� 1000 �̂Ƃ��͒���I�ɏ������܂Ȃ�
	} else playerStatusCnt = 0;
	
	
	// �t�@�C���폜
	if (fileDeleteFlag > 0 && cnt % (playerSpeedMode+1) == 0) {	// �폜�p�x����
		// MidiOggPlayer���[�h�ȊO�̓t�@�C����null�ɂ���
		fileDeleteFlag --;
		if (fileDeleteFlag <= 0) {
			if (playerMode != 2) {
				hFile = CreateFile(".\\Kernel\\sig", 0, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile); // �N���[�Y
			}
			fileDeleteFlag = 0;
		}
	}
	
	
	#if DEBUG
	if (cnt % 60 == 0) {
		
		sprintf(testStr, "%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v\r\n%s:�u%s�v %s:�u%d�v %s:�u%d�v %s:�u%s�v %s:�u%d�v %s:�u%d�v\r\n%s:�u%d�v",
		                 "bgmStr                 ",bgmStr,
		                 "oggBgmStr              ",oggBgmStr,
		                 "usrFolderAddr          ",usrFolderAddr,
		                 "ogg_sigAddr            ",ogg_sigAddr,
		                 "defaultTonyuprjFileAddr",defaultTonyuprjFileAddr,
		                 "eff_midiDir            ",eff_midiDir,
		                 "eff_midiAddr           ",eff_midiAddr,
		                 "midiPlayerStatusAddr   ",midiPlayerStatusAddr,
		                 "YMPPreparationAddr"     ,YMPPreparationAddr,
		                 "processCheckFlag"       ,processCheckFlag,
		                 "dwProcessId"            ,dwProcessId,
		                 "playerExeName"          ,playerExeName,
		                 "cnt"                    ,cnt,
		                 "processExist"           ,processExist(playerExeName, osPlatformId, dwProcessId),
		                 "midiEffectFlag"         ,midiEffectFlag
		                 );
		
		//sprintf(testStr, "bgmType:%d\r\nplayingFlag:%d\r\oggAllWriteFlag:%d\r\noggPlayingTime:%f", bgmType, playingFlag, oggAllWriteFlag, oggPlayingTime);
		//sprintf(testStr, "", bgmType, playingFlag, oggAllWriteFlag, oggPlayingTime);
		SetWindowText(hStatic, testStr);
		
		
		
	}
	#endif
	
	
	// DirectMusic��ON/OFF�ؑցE�t�@�C���̓ǂݍ��ݕp�x�̕ύX(YellowMusicPlayer���[�h�̎��̂�) (�P�b�ɂQ��ǂݍ���)
	if (playerMode == 0) {
		if (cnt % 33 == 0) {
			fileReadSize = readFile_C(YMPPreparationAddr, readBuf,  sizeof(readBuf) - 1, 0);
			if (fileReadSize >= 1) {
				
				playerDirectMusicMode_old = playerDirectMusicMode;
				
				// �f�t�H���g�l
				if (osPlatformId == 1) playerDirectMusicMode = 0; // 9x�n�ł�DirectMusic���g��Ȃ�
				else                   playerDirectMusicMode = 1; // NT�n�ł�DirectMusic���g��
				
				i = 0; j = 0;
				getLinePoint(readBuf, fileReadSize, &i);
				tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
				if      (tempInt == 1) playerDirectMusicMode = 1; // �S�Ă�OS��DirectMusic���g��
				else if (tempInt == 2) playerDirectMusicMode = 0; // �S�Ă�OS��DirectMusic���g��Ȃ�
				
				if (i < (int)fileReadSize) {
					getLinePoint(readBuf, fileReadSize, &i);
					tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
					if (tempInt <   0) tempInt =   0;
					if (tempInt > 100) tempInt = 100;
					playerSpeedMode = tempInt; // playerSpeedMode*15�~���b���Ƃɓ���
				}
				
				// DirectMusic��ON/OFF���ς������Midi�f�o�C�X���J������
				if (playerDirectMusicMode != playerDirectMusicMode_old) {
					// Midi�f�o�C�X�����
					GGS4CloseDevice();
					
					// Midi�f�o�C�X���J������
					if (playerDirectMusicMode) errorFlag = GGS4OpenDevice(GGSDEVICE_DIRECTMUSIC, hWnd); // DirectMusic
					else                       errorFlag = GGS4OpenDevice(-1, hWnd);                    // MIDI_MAPPER
				}
				
				DeleteFile(YMPPreparationAddr);
				
			}
		}
	}
	
	if (exitFlag) {
		GGS4Stop(0);
		oggStop(0);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		threadClose = 1;
		return ;
	}
	
	return ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
