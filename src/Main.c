/*
  YellowMusicPlayer 2.37  -  Copyright (C) 2012 - 2018 マッキー
*/

#include "Define.h"
#include "Global.h"

#if DEBUG
HWND   hStatic;
TCHAR  testStr[BUF_SIZE_M];
DWORD  timeStart;
DWORD  timeStartSW = 0;
DWORD  timeMaxDelay;
DWORD  timeInst; // 計測
DWORD  rpsTimeOld = 0;
DWORD  rpsTime;
#endif
TCHAR  testStr[BUF_SIZE_M];

HWND  hWnd;
DWORD dwTimer = 0;
//HANDLE hMusicThread = NULL;
DWORD dwMusicThreadParam;
BOOL  windowClose = 0;
BOOL  threadClose = 0;

int   bgmType = 0; // 0:停止 1:midi 2:ogg
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

// TonyuでOggを鳴らすサンプルモード で使う
int    ogg3VolumeVariation = 0;
double ogg3VolumeCnt       = 0.0;
double ogg3VolumeWk        = 0.0;

BOOL  exitFlag = 0;
BOOL  midiPlayerOK = 0;


//typedef GGSERROR (WINAPI *FunctionOpenDevice)(int deviceId, HWND handle);




// スレッド内で使う変数 ///////////////////////////////////////////////////////////////////////////////////////////////////
int      cnt = 0, i, j, k;
DWORD    hdError;
DWORD    fileReadSize, fileReadSize2;
BOOL     eff_midiDirChangeFlag = 0;

TCHAR    bgmStr[_MAX_PATH], bgmStr_old[_MAX_PATH];
TCHAR    playedBgmStr[FILEPATH_SIZE];
TCHAR    oggBgmName[_MAX_PATH];
TCHAR    oggBgmStr[_MAX_PATH], oggBgmStr_old[_MAX_PATH];
TCHAR    playedOggBgmStr[FILEPATH_SIZE];
TCHAR    eff_midiAddr[FILEPATH_SIZE];
TCHAR    eff_midiDir[FILEPATH_SIZE], eff_midiDir_old[FILEPATH_SIZE];
TCHAR    ogg_sigAddr[FILEPATH_SIZE];
TCHAR    midiPlayerStatusAddr[FILEPATH_SIZE];
TCHAR    defaultTonyuprjFileAddr[FILEPATH_SIZE];
TCHAR    usrFolderAddr[FILEPATH_SIZE];
TCHAR    YMPPreparationAddr[FILEPATH_SIZE];
char     readBuf[BUF_SIZE_S];
char     readBuf2[BUF_SIZE_S];
char     readBuf3[BUF_SIZE_S];
char     readBuf4[BUF_SIZE_S];
char     readBuf5[BUF_SIZE_S];
char     readBuf6[BUF_SIZE_S];
char     readBuf7[BUF_SIZE_S];

BOOL     stopFlag        = 0;
BOOL     playFlag        = 0;
BOOL     playFlagCnt     = 0;
BOOL     playingFlag     = 0;
int      playerStatusCnt = 0;
BOOL     loopFlag        = 1;
int      playId          = 0;
BOOL     fileDeleteFlag  = 0;
GGSERROR errorFlag       = 0;
int      exitCheckFlag   = 0;

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

int    readValue; // MidiOggPlayer用

GGS4SMFINFORMATION midiInfo;
GGS4PLAYERSTATUS midiStatus;

TCHAR crDir[FILEPATH_SIZE];
TCHAR playerExeName[FILEPATH_SIZE];
BOOL  processCheckFlag;
DWORD dwProcessId;
DWORD osPlatformId = 0;
DWORD osMajorVersion = 0;
DWORD osMinorVersion = 0;

TCHAR *midiFileAddrList[MIDIFILELIST_SIZE];
int   midiFileAddrListLength = 0;
int   tempInt;
TCHAR tempStr[BUF_SIZE_M];

BOOL  midiEffectFlag;
SYSTEMTIME stTime;

HANDLE hFile;

BOOL playerRestartSW = 0;

int dispx = 0;
int dispy = 0;
int midiStatus_State_old = 0;

int joystickCheckTimeCnt = 0;
// JOYSTICK_CHECK_TIME
int joystickOnId1 = 0;
int joystickOnId2 = 0;
unsigned char joistickOnIdAll = 0;

//int frameRunCnt = 1; // 実行回数
//double frameSkip_d = 1.0;
int frameSkipCnt = 0;
DWORD nowTime, oldTime;
int timerThreadRestartCnt = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////







int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
static void CALLBACK TimerThreadC(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2);
//static DWORD WINAPI musicThread(LPVOID hWnd);
#if DEBUG
static void RPSOutput(void);
#endif


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	WNDCLASS winc;
	TIMECAPS timeCaps;
	MMRESULT timerCapsResult;
	//TCHAR teesssttooo[999];
	
	winc.style         = (CS_HREDRAW | CS_VREDRAW)*0;
	winc.lpfnWndProc   = WndProc;
	winc.cbClsExtra    = 0;
	winc.cbWndExtra	   = 0;
	winc.hInstance     = hInstance;
	winc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_1));
	winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName  = NULL;
	winc.lpszClassName = _T("WNDCLASS");
	
	if (!RegisterClass(&winc)) return 0;
	
	//wsprintf(teesssttooo, _T("%d"), FILEPATH_SIZE);
	//MessageBox(0,teesssttooo,0,0);
	
	#if DEBUG
	hWnd = CreateWindow(_T("WNDCLASS"), _T("YellowMusicPlayer"), WS_OVERLAPPEDWINDOW,
	                    0, 0, 1024-8, 150+20*12, NULL, NULL, hInstance, NULL);
	#else
	hWnd = CreateWindow(_T("WNDCLASS"), _T("YellowMusicPlayer"), WS_OVERLAPPEDWINDOW,
	                    0, 0, 350, 150, NULL, NULL, hInstance, NULL);
	#endif
	
	if (hWnd == NULL) return 0;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	#if DEBUG
	ShowWindow(hWnd, SW_SHOWNA);
	UpdateWindow(hWnd);
	#endif
	
	// GuruGuruSMF4を初期化
	if (GGS4Initialize() != GGSERROR_NOERROR) {
		MessageBox(0, 
			_T(
				"YellowMusicPlayerに必要なdllがない、または読み込めないため\n"
				"BGMを演奏することができません\n"
				"\n"
				"「MidiPlayer2.exe」があるフォルダに\n"
				"「GuruGuruSMF4.dll」を置いてください"
			)
			, _T("YellowMusicPlayer エラー"), MB_ICONERROR);
		return 0;
	}
	
	// タイマー分解能の精度を高める
	timerCapsResult = timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
	if (timerCapsResult == TIMERR_NOERROR) {
		timeBeginPeriod(timeCaps.wPeriodMin);
	}
	
	// スレッド関係の初期化
	if (mainInit()) {
		// 初期化できたらスレッドを実行
		oldTime = timeGetTime();
		dwTimer = timeSetEvent(15, 0, TimerThreadC, CREATE_SUSPENDED, TIME_PERIODIC);
		//hMusicThread = CreateThread(NULL, 0, musicThread, hWnd, 0, &dwMusicThreadParam);
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// タイマー分解能の精度を元に戻す
	if (timerCapsResult == TIMERR_NOERROR) {
		timeEndPeriod(timeCaps.wPeriodMin);
	}
	
	return msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp) {
	int i;
	DWORD dwParam;
	
	switch (msg) {
	case WM_CLOSE:
		windowClose = 1;
		oggStop(0);
		timeKillEvent(dwTimer);
		//Sleep(50);
		/*
		for (i = 0; i < 40; i++) {
			GetExitCodeThread(hMusicThread , &dwParam);
			if (dwParam != STILL_ACTIVE) break ;
			Sleep(50);
		}
		if (dwParam == STILL_ACTIVE) TerminateThread(hMusicThread , FALSE);
		CloseHandle(hMusicThread);
		*/
		if (midiPlayerOK) GGS4Free(); // ダイナミックリンク
		//if (pDSNotify) IDirectSoundNotify_Release(pDSNotify);
    	if (pDSBuffer) IDirectSoundBuffer_Release(pDSBuffer);
    	if (pDS) IDirectSoundBuffer_Release(pDS);
    	oggFileClear();
		
		/*
		if (g_hNotifyThread) {
			
			GetExitCodeThread(g_hNotifyThread , &dwParam);
			if (dwParam != STILL_ACTIVE) {
				ResumeThread(g_hNotifyThread);
				Sleep(50);
			}
			for (i = 0; i < 10; i++) {
				GetExitCodeThread(g_hNotifyThread , &dwParam);
				if (dwParam != STILL_ACTIVE) break;
				Sleep(20);
			}
			CloseHandle(g_hNotifyThread);
		}
		GetExitCodeThread(g_hNotifyThread , &dwParam);
		if (dwParam != STILL_ACTIVE) {
			ResumeThread(g_hNotifyThread);
			Sleep(50);
		}
		*/
		break;
	case WM_DESTROY:
		
		PostQuitMessage(0);
		return 0;
		
	#if DEBUG
	case WM_CREATE:
		hStatic = CreateWindow(_T("STATIC") , _T("") ,WS_CHILD | WS_VISIBLE ,
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

//static DWORD WINAPI musicThread(LPVOID hWnd) {
static void CALLBACK TimerThreadC(UINT uiID, UINT uiNo, DWORD dwCookie, DWORD dwNo1, DWORD dwNo2) {
	//StopProc();
	if (threadClose) return ;
	
	nowTime = timeGetTime();
	if (oldTime / 15 == nowTime / 15) frameSkipCnt ++; else frameSkipCnt = 0;
	
	// PCのスリープ対策
//	if (nowTime - oldTime >= 60000) { // 1分
//		oldTime = nowTime;
//		timeKillEvent(dwTimer);
//		
//		// Midiデバイスを閉じる
//		GGS4CloseDevice();
//		// Midiデバイスを開き直す
//		// Midiデバイスを開く //
//		if (playerDirectMusicMode) {
//			errorFlag = GGS4OpenDevice(GGSDEVICE_DIRECTMUSIC, hWnd); // DirectMusic
//			if (errorFlag != GGSERROR_NOERROR) playerDirectMusicMode = 0; // エラーならDirectMusicをOFFにして再試行
//		}
//		if (playerDirectMusicMode == 0) errorFlag = GGS4OpenDevice(-1, hWnd); // MIDI_MAPPER
//		// 今まで再生していたものを再生する
//		playFlag = 1;
//		if (playerMode == 0 || playerMode == 3) loopFlag = 0;
//		fileDeleteFlag = 0;
//		
//		dwTimer = timeSetEvent(15, 15, TimerThreadC, CREATE_SUSPENDED, TIME_PERIODIC);
//		timerThreadRestartCnt ++;
//		return ;
//	}
	
	// 処理されすぎないように制御
	if (frameSkipCnt >= 1) {
		Sleep(1);
		#if DEBUG
		RPSOutput();
		#endif
		return ;
	}
	
	//while (windowClose == 0) {
	
	cnt ++;
	
	
	playFlagCnt ++; // 時間カウント
	
	
	// sigファイル読み込み
	if (playFlag == 0) {
		
		// 読み込み頻度調節
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel;
		
		// ファイル読み込み(sig)
		#if !CREATE_TONYU_OLD
			fileReadSize = readFile_C(_T(".\\Kernel\\sig"), readBuf, sizeof(readBuf) - 1, 0);
		#else
			fileReadSize = readFile_C(_T(".\\files\\midi_sig.dat"), readBuf, sizeof(readBuf) - 1, 0);
		#endif
		
		if (fileReadSize < 3) goto readFileEndLabel;
		
		// １行目
		if (playerMode != 2) { // YellowMusicPlayer, TonyuMidiLitest, TonyuでOggを鳴らすサンプル //
			if (readBuf[0] == 'P') { // 再生（ループなし）
				playFlag = 1;
				if (playerMode == 0 || playerMode == 3) loopFlag = 0;
				fileDeleteFlag = 0;
				
			} else if (readBuf[0] == 'R') { // 再生（ループあり）
				playFlag = 1;
				loopFlag = 1;
				fileDeleteFlag = 0;
				
			} else if (readBuf[0] == 'S') { // 停止
				playFlag = 0;
				
				if (playingFlag != 0) { // 再生中or一時停止中
					stopFlag = 1;
					playerStatusCnt = 0;
					//GGS4Stop(midiFadeOutTime); // 先にフェードアウト時間を読ませる
					fileDeleteFlag = 5;
					playingFlag = 0;
					//bgmType = 0;
				}
				
			}else if (readBuf[0] == 'X') { // 閉じる
				if (processCheckFlag) exitCheckFlag = 60; // プロセスチェックしていたらプロセスチェックで終了を判断
				else exitFlag = 1; // プロセスチェックしていなかったら即座に終了
				fileDeleteFlag = 1;
			}
		} else { // MidiOggPlayer の場合はファイル内容が変わったら実行 //
			
			if (memicmp(readBuf, readBuf7, sizeof(readBuf)) != 0) { // 変わった時だけ処理
				memcpy(readBuf7, readBuf, sizeof(readBuf7));
				
				if (readBuf[0] == 'S') { // 停止
					
					playFlag = 0;
					if (playingFlag != 0) { // 再生中or一時停止中
						memset(readBuf5, 0, sizeof(readBuf5));
						playerStatusCnt = 0;
						GGS4Stop(midiFadeOutTime);
						playingFlag = 0;
						//bgmType = 0;
					}
					
				} else if (readBuf[0] == 'X') { // 閉じる
					if (processCheckFlag) exitCheckFlag = 60; // プロセスチェックしていたらプロセスチェックで終了を判断
					else exitFlag = 1; // プロセスチェックしていなかったら即座に終了
					fileDeleteFlag = 1;
				} else {
					
					// ２行目が変更されているか確かめる
					i = 0;
					j = 0;
					getLinePoint(readBuf, fileReadSize, &i); i+=2; j=i; // １行目
					if (i >= (int)fileReadSize) goto readFileEndLabel;  // １行目で終わりなら終了
					
					if (memicmp(readBuf+j, readBuf5, sizeof(readBuf)-j) != 0) memcpy(readBuf5, readBuf+j, sizeof(readBuf)-j);
					else                                                     goto readFileEndLabel;
					
					playFlag = 1;
					fileDeleteFlag = 0;
				}
			}
			
		}
		
		
		
		
		
		
		
		// 演奏するならmidiファイル名を取得
		if (playFlag == 1) {
			i = 0;
			j = 0;
			getLinePoint(readBuf, fileReadSize, &i); i+=2; j=i; // １行目
			getLinePoint(readBuf, fileReadSize, &i);
			if (i >= (int)fileReadSize) {playFlag = 0; goto readFileEndLabel2;}
			
			// ファイル名
			if (i-j < sizeof(bgmStr) - 1) {
				memcpy(bgmStr, readBuf+j, i-j); // midiファイル名抽出
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
		
		// MidiOggPlayerモードは「midi_sig.dat」を読み込む
		if (playerMode == 2) {
			if (playFlag == 0) { // sigで再生されていないとき
				
				// 読み込み頻度調節
				if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel2;
				
				#if !CREATE_TONYU_OLD
					fileReadSize = readFile_C(_T(".\\Usr\\files\\midi_sig.dat"), readBuf, sizeof(readBuf) - 1, 0);
				#else
					fileReadSize = readFile_C(_T(".\\files\\midi_sig.dat"), readBuf, sizeof(readBuf) - 1, 0);
				#endif
				if (fileReadSize <= 0) goto readFileEndLabel2;
				
				if (memicmp(readBuf, readBuf3, sizeof(readBuf)) != 0) { // 変更あり
					memcpy(readBuf3, readBuf, sizeof(readBuf));
					if      (readBuf[0] == '0') GGS4Stop(midiFadeOutTime);
					else if (readBuf[0] == '1') GGS4Pause();
					else if (readBuf[0] == '2') GGS4Restart();
					else {
						// 相対パスのままにする
						playFlag = 1;
						memcpy(bgmStr, readBuf, fileReadSize); // midiファイル名抽出
						bgmStr[fileReadSize] = '\0';
						
						// 相対パスを絶対パスに直す
						//memset(tempStr, 0, sizeof(tempStr));
						//if (strlen(crDir) < sizeof(tempStr)) memcpy(tempStr, crDir, strlen(crDir));  else goto bgmErrorLabel2;
						//if (strlen(tempStr) + strlen("\\") < sizeof(tempStr)) strcat(tempStr, "\\"); else goto bgmErrorLabel2;
						//if (readBuf[0] == '.' && (readBuf[1] == '/' || readBuf[1] == '\\')) { // "./Usr/BGM/bgm01.mid"
						//	if (strlen(tempStr) + strlen(readBuf+2) < sizeof(tempStr)) strcat(tempStr, readBuf+2); else goto bgmErrorLabel2;
						//} else { // "Usr/BGM/bgm01.mid"
						//	if (strlen(tempStr) + strlen(readBuf) < sizeof(tempStr)) strcat(tempStr, readBuf); else goto bgmErrorLabel2;
						//}
						//memcpy(bgmStr, tempStr, sizeof(bgmStr)-1); // midiファイルの絶対パスをコピー
						//bgmStr[sizeof(bgmStr)-1] = '\0'; // バッファの末尾に必ずnull文字が付くようにする（要らないかも）
						
					}
					
				}
			}
			
			bgmErrorLabel2:
				;
			readFileEndLabel2:
				;
		}
		
	}
	
	
	
	
	// eff_midi.txt があればアドレスをセットして、エフェクトファイルを監視するようにする
	// 再生する時にチェック
	if (((playFlag == 1 || playFlag == 2) && playerMode != 2) || (midiEffectFlag == 0 && PathFileExists(eff_midiAddr))) {
		
		if (midiEffectFlag == 0 && PathFileExists(eff_midiAddr)) { // Midiが鳴らされずeff_midi.txtだけ生成されたら
			memset(bgmStr, 0, sizeof(bgmStr));
			memcpy(bgmStr, eff_midiDir, sizeof(bgmStr) - 1);
		}
		
		// eff_midi.txt の場所を調べる
		// eff_midiAddr, eff_midiDir が更新される
		midiEffectFlag = setEffectFileAddr(bgmStr, 
		                                   eff_midiAddr           , sizeof(eff_midiAddr),
		                                   eff_midiDir            , sizeof(eff_midiDir),
		                                   ogg_sigAddr            , sizeof(ogg_sigAddr),
		                                   defaultTonyuprjFileAddr, sizeof(defaultTonyuprjFileAddr),
		                                   usrFolderAddr          , sizeof(usrFolderAddr),
		                                   midiPlayerStatusAddr   , sizeof(midiPlayerStatusAddr),
		                                   YMPPreparationAddr     , sizeof(YMPPreparationAddr)
		                                   );
		
		// eff_midi.txt のディレクトリの位置が変わったらエフェクトを初期化
		if (memicmp(eff_midiDir_old, eff_midiDir, sizeof(eff_midiDir_old)) != 0) {
			if (midiEffectFlag) eff_midiDirChangeFlag = 1;
			if (oggPlayerStatus != STATUS_STOP) oggStop(0);
			
			initMidiParam(); // エフェクトリセット
			
			// 今のアドレスを記録
			memset(eff_midiDir_old,           0, sizeof(eff_midiDir_old));
			memcpy(eff_midiDir_old, eff_midiDir, sizeof(eff_midiDir_old));
		}
		
		// eff_midiのディレクトリがチェンジしたとき
		if (eff_midiDirChangeFlag) {
			
			// default.tonyuprj を読み込みMidiのパス名を知る or Midiを読み込む
			beforeReadMidi(usrFolderAddr, defaultTonyuprjFileAddr, midiFileAddrList, &midiFileAddrListLength);
			
			eff_midiDirChangeFlag = 0;
		}
	}
	
	
	// プロジェクトが切り替わった時、default.tonyuprjファイルを読み込んでからでないと
	// midi番号が正常に検索できなくなるのでここへ処理を持ってきた
	if (playFlag == 1) {
		// 登録されているmidiファイルか調べる
		for (playId = 0; playId < midiFileAddrListLength; playId++) {
			if (lstrcmpi(midiFileAddrList[playId], bgmStr) == 0) break ;
		}
		
		// キャッシュモードのときは読み込みはせずそのまま再生
		if (playerCacheMode != 0) {
			if (playId < midiFileAddrListLength) { // 登録されているmidiだった場合
				playFlag = 2; // 再生フェーズへ
			} else {
				playFlag = 1; // 読み込みフェーズへ（バグとかあった場合にも読めるように）
			}
		} else { // キャッシュなしのときは同じ
			if (memicmp(bgmStr, playedBgmStr, sizeof(bgmStr)) != 0) {
				memcpy(playedBgmStr, bgmStr, sizeof(playedBgmStr));
				playFlag = 1; // 読み込みフェーズへ
			} else {
				playFlag = 2; // 再生フェーズへ
			}
		}
		
		//GGS4GetPlayerStatus(&midiStatus, sizeof(midiStatus));
		GGS4GetPlayerStatus(&midiStatus);
		midiStatus_State_old = midiStatus.State; // 状態を記録しておく
		if (midiStatus.State == GGSSTATE_STOP) { // 既に停止している
			playFlagCnt = 2; // ２フレーム分待たない
		} else { // 再生中or一時停止中
			GGS4Stop(0);
			playFlagCnt = 0; // ２フレーム分待つ
		}
	}
	
	
	// ファイル読み込み(eff_midi.txt)
	if (midiEffectFlag && playerMode != 2) {
		
		// 読み込み頻度調節
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto eff_midiEndLabel;
		
		fileReadSize2 = readFile_C(eff_midiAddr, readBuf2, sizeof(readBuf2) - 1, 0);
		if (fileReadSize2 >= 2) {
			// midi音量
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
			
			// midiテンポ
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
			
			// midiフェードアウト
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiFadeOutTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiFadeOutTime_r != -9999) {
				if (midiFadeOutTime_r < 0) midiFadeOutTime_r = 0;
				midiFadeOutTime = midiFadeOutTime_r;
			}
			
			// midi一時停止・再開
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiPause_r = strToInt(readBuf2, j, i, -9999);  i+=2; j=i;
			if (midiPause_r != -9999) {
				if (midiPause_r != midiPause) {
					midiPause = midiPause_r;
					if (midiPause == 1) GGS4Pause();
					else                GGS4Restart();
					playerStatusCnt = 2; // 0にすると書きこぼしが発生しやすい
				}
			}
			
			
			// TonyuMidiLitestモードの場合、ここで処理を打ち切る
			if (playerMode == 1) goto eff_midiEndLabel;
			
			
			// midiピッチ
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
			
			// midiフェードイン
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiFadeInTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiFadeInTime_r != -9999) {
				if (midiFadeInTime_r < 0) midiFadeInTime_r = 0;
				midiFadeInTime = midiFadeInTime_r;
			}
			
			// midiキーシフト
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiKeyShift_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiKeyShift_r != -9999) {
				midiKeyShift = midiKeyShift_r;
			}
			
			// スタートティック
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiStartTick_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiStartTick_r != -9999) {
				if (midiStartTick_r < 0) midiStartTick_r = 0;
				midiStartTick = midiStartTick_r;
			}
			
			// midi冒頭スキップ
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiSkipBeginning_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiSkipBeginning_r != -9999) {
				midiSkipBeginning = !!midiSkipBeginning_r;
			}
			
			// midi末尾スキップ
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiSkipEnding_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiSkipEnding_r != -9999) {
				midiSkipEnding = !!midiSkipEnding_r;
			}
			
			// midiマスターピッチ
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiMasterPitch_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiMasterPitch_r != -9999) {
				midiMasterPitch = !!midiMasterPitch_r;
			}
			
			// midiノーマライズ
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			midiNormalize_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (midiNormalize_r != -9999) {
				midiNormalize = !!midiNormalize_r;
			}
			
			// midiエクスクルーシブ
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
			
			
			// Tonyuでoggを鳴らすサンプルモード の場合はここで処理を打ち切る
			if (playerMode == 3)  goto eff_midiEndLabel;
			
			// ogg音量
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
			
			// oggテンポ
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
			
			// oggパン
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
			
			// oggフェードイン
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggFadeInTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggFadeInTime_r != -9999) {
				if (oggFadeInTime_r < 0) oggFadeInTime_r = 0;
				oggFadeInTime = oggFadeInTime_r;
			}
			
			// oggフェードアウト
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggFadeOutTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggFadeOutTime_r != -9999) {
				if (oggFadeOutTime_r < 0) oggFadeOutTime_r = 0;
				oggFadeOutTime = oggFadeOutTime_r;
			}
			
			// スタートティック
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggStartTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggStartTime_r != -9999) {
				if (oggStartTime_r < 0) oggStartTime_r = 0;
				oggStartTime = oggStartTime_r;
			}
			
			// oggループして戻ってくる位置
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggLoopTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggLoopTime_r != -9999) {
				if (oggLoopTime_r < 0) oggLoopTime_r = 0;
				oggLoopTime = oggLoopTime_r;
			}
			
			// oggループする位置
			if (i >= (int)fileReadSize2) goto eff_midiEndLabel;
			getLinePoint(readBuf2, fileReadSize2, &i);
			oggLoopEndTime_r = strToInt(readBuf2, j, i, -9999); i+=2; j=i;
			if (oggLoopEndTime_r != -9999) {
				if (oggLoopEndTime_r < 0) oggLoopEndTime_r = 0;
				oggLoopEndTime = oggLoopEndTime_r;
			}
			
			// oggファイルのループを使う
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
	
	// MidiOggPlayerモードは「bgm_config.dat」を読み込む
	// MidiOggPlayerはTonyu開発環境版では動かないので、対応させなくてもいいや！！
	if (playerMode == 2) {
		
		// 読み込み頻度調節
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto eff_midiEndLabel2;
		
		#if !CREATE_TONYU_OLD
			fileReadSize2 = readFile_C(_T(".\\Usr\\files\\bgm_config.dat"), readBuf2, sizeof(readBuf2) - 1, 0);
		#else
			fileReadSize2 = readFile_C(_T(".\\files\\bgm_config.dat"), readBuf2, sizeof(readBuf2) - 1, 0);
		#endif
		if (fileReadSize2 <= 0) goto eff_midiEndLabel2;
		
		if (memicmp(readBuf2, readBuf6, sizeof(readBuf2)) != 0) memcpy(readBuf6, readBuf2, sizeof(readBuf6));
		else                                                   goto eff_midiEndLabel2;
		
		if (fileReadSize2 >= 2) {
			readValue = 0;
			i = 0;
			j = 0;
			// 音量(midi)
			getLinePoint(readBuf2, fileReadSize2, &i);
			readValue  = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			midiVolume = readValue;
			if      (midiVolume > 127) midiVolume = 127;
			else if (midiVolume <   0) midiVolume =   0;
			midiVolume -= 127;
			GGS4SetMasterVolume(midiVolume);
			
			// ループ(midi)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			loopFlag = !!readValue;
			
			// フェードアウト(midi)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			midiFadeOutTime = readValue;
			if (midiFadeOutTime < 0) midiFadeOutTime = 0;
			
			// 音量(ogg)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			oggVolume = readValue / 2;
			if      (oggVolume > 127) oggVolume = 127;
			else if (oggVolume <   0) oggVolume =   0;
			oggSetVolume(oggVolume);
			
			// ループ・ループ位置(ogg)
			if (i < (int)fileReadSize2) {
				getLinePoint(readBuf2, fileReadSize2, &i);
				readValue = strToInt(readBuf2, j, i, 0); i+=2; j=i;
			}
			oggLoopTime = readValue - 1;
			if (oggLoopTime < 0) oggLoopTime = 0;
			oggLoopFlag = 1;
			oggLoopSeek = (double)oggLoopTime / 1000.0;
			if (oggLoopSeek >= oggTotalTime - 0.04) oggLoopSeek = oggTotalTime - 0.04;
			
			// フェードアウト(ogg)
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
	
	
	
	
	// midi関係 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 読み込み・再生 //
	if (playFlag == 1) { // midiファイルを読み込んでから再生
		if (playFlagCnt >= 30) {playFlagCnt = 0; playFlag = 0;} // 時間切れ
		
		// midiファイル読み込み //
		if (playerCacheMode != 0) i = playId;
		else                      i = 0;
		#ifdef UNICODE
			errorFlag = GGS4AddListFromFileW(bgmStr, GGSLOAD_BUFFERED*1, i);
		#else
			errorFlag = GGS4AddListFromFileA(bgmStr, GGSLOAD_BUFFERED*1, i);
		#endif
		if (errorFlag != 0) goto playEndLabel;
		
		if (midiFileAddrListLength < MIDIFILELIST_SIZE && playId == midiFileAddrListLength) { // playIdが新しい番号ならmidiのパス名を追加
			// midiファイルアドレスリストに、midiの絶対アドレスを登録
			midiFileAddrList[midiFileAddrListLength] = (TCHAR *)malloc(sizeof(TCHAR) * lstrlen(bgmStr) + 1); // free()は書くのめんどいので省略…
			if (midiFileAddrList[midiFileAddrListLength] == NULL) exit(-1);
			memset(midiFileAddrList[midiFileAddrListLength], 0, lstrlen(bgmStr) + 1);
			memcpy(midiFileAddrList[midiFileAddrListLength], bgmStr, lstrlen(bgmStr));
			
			midiFileAddrListLength ++;
		}
		
		//playFlagCnt = 0;
		playFlag = 2;
	}
	if (playFlag == 2 && playFlagCnt >= 2) { // 15*2ミリ秒経過した後に再生or即再生
		if (playFlagCnt >= 30) {playFlagCnt = 0; playFlag = 0;} // 時間切れ
		
		// 再生 //
		if (playerCacheMode != 0) i = playId;
		else                      i = 0;
		
		// StartTick が TotalTic を超えると強制終了になるため TotalTic を調べる
		//GGS4GetSmfInformation(&midiInfo, sizeof(midiInfo), playId);
		GGS4GetSmfInformation(&midiInfo, i);
		if (midiStartTick >= midiInfo.TotalTick) midiStartTick_temp = midiInfo.TotalTick - 1;
		else                                     midiStartTick_temp = midiStartTick;
		
		// マスターピッチの調整
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
		
		if (errorFlag != 0) {
			sprintf(testStr, "GGS4Play errorFlag=%d", errorFlag);
			MessageBox(0,testStr,0,0);
		}
		
		if (errorFlag != 0) goto playEndLabel;
		
		playerStatusCnt = 0;
		playFlag = 0;
		playingFlag = 1;
		//bgmType = 1;
		//fileDeleteFlag = 1;
	}
	
	if (stopFlag == 1) {
		stopFlag = 0;
		GGS4Stop(midiFadeOutTime);
	}
	
	playEndLabel:
		;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// ogg関係 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ファイル読み込み(ogg_sig.dat) //////////////////////////////////////////////////////////////////
	
	if (playerMode != 1) { // TonyuMidiLitestモード以外はoggの処理を実行する
		// 読み込み頻度調節
		if (playerSpeedMode >= 1 && cnt % (playerSpeedMode+1) != 0) goto readFileEndLabel3;
		
		fileReadSize = readFile_C(ogg_sigAddr, readBuf, sizeof(readBuf) - 1, 0);
		if (fileReadSize <= 0) goto readFileEndLabel3;
		
		if (playerMode == 2 || playerMode == 3) {
			if (memicmp(readBuf, readBuf4, sizeof(readBuf)) != 0) { // MidiOggPlayer 又は TonyuでOggを鳴らすサンプル の場合はファイル内容が変わったら実行
				memcpy(readBuf4, readBuf, sizeof(readBuf));
			} else {
				goto readFileEndLabel3;
			}
		}
		
		// ここでoggの再生・停止処理を行ってしまう //////////////////////
		
		if (playerMode == 3) {
			if (ogg3VolumeCnt > 0.0 && readBuf[0] != '3') ogg3VolumeCnt = 0.0;
		}
		i = 0;
		j = 0;
		getLinePoint(readBuf, fileReadSize, &i);
		if (readBuf[0] == '0' && i == 1) { // 停止
			oggStop(oggFadeOutTime);
			
		} else if(readBuf[0] == '1' && i == 1) { // 一時停止
			oggPause();
			
		} else if(readBuf[0] == '2' && i == 1) { // 再開
		  	oggRestart();
			
		} else if(readBuf[0] == '3' && i == 1 && playerMode == 3) { // 音量調節 (TonyuでOggを鳴らすサンプル用)
			
			if (ogg3VolumeCnt == 0.0) {
				// 変化量
				i+=2; j=i;
				getLinePoint(readBuf, fileReadSize, &i);
				k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
				if (k != -99999999) ogg3VolumeVariation = k;
				
				// 反復量
				if (i < (int)fileReadSize) {
					getLinePoint(readBuf, fileReadSize, &i);
					k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
					if (k != -99999999) {
						if (k < 0) k = 0;
						ogg3VolumeCnt = (double)k;
					}
				}
			}
		} else if(readBuf[0] == '9' && i == 1 && playerMode == 3) {
			; // 何もしない
			
		} else { // ファイルパス
			//timeStartSW = 1;
			
			memset(oggBgmStr, 0, sizeof(oggBgmStr));
			if (i >= sizeof(oggBgmStr) - 1) goto bgmErrorLabel3;
			
			memset(oggBgmName, 0, sizeof(oggBgmName));
			memcpy(oggBgmName, readBuf, i);
			
			memcpy(oggBgmStr, readBuf, i); i+=2; j=i;
			
			
			// oggファイルの絶対パスを求める（ループ符号を取り除く）
			if (playerMode == 0) {
				memset(tempStr, 0, sizeof(tempStr));
				//for (k = strlen(usrFolderAddr) - 1; k >= 0; k--) { // Usrフォルダの親フォルダに移る
				//	if (usrFolderAddr[k] == '\\') break ;
				//}
				//if (playerMode == 3) { // さらに上に移る
				//	for (k = k - 1; k >= 0; k--) { // Usrフォルダの親フォルダの親フォルダに移る
				//		if (usrFolderAddr[k] == '\\') break ;
				//	}
				//}
				//if (k < 0) goto bgmErrorLabel3; // パスが不正
				
				if (lstrlen(usrFolderAddr) < sizeof(tempStr)) memcpy(tempStr, usrFolderAddr, lstrlen(usrFolderAddr));  else goto bgmErrorLabel3;
				if (lstrlen(tempStr) + lstrlen(_T("\\")) < sizeof(tempStr)) lstrcat(tempStr, _T("\\")); else goto bgmErrorLabel3;
				if (oggBgmStr[0] == _T('.') && (oggBgmStr[1] == _T('/') || oggBgmStr[1] == _T('\\'))) { // "./Usr/BGM/bgm01.ogg"
					if (lstrlen(tempStr) + lstrlen(oggBgmStr+2) < sizeof(tempStr)) lstrcat(tempStr, oggBgmStr+2); else goto bgmErrorLabel3;
				} else { // "Usr/BGM/bgm01.ogg"
					if (lstrlen(tempStr) + lstrlen(oggBgmStr) < sizeof(tempStr)) lstrcat(tempStr, oggBgmStr); else goto bgmErrorLabel3;
				}
				memcpy(oggBgmStr, tempStr, sizeof(oggBgmStr)-2); // oggファイルの絶対パスをコピー
				oggBgmStr[sizeof(oggBgmStr)-1] = _T('\0'); // バッファの末尾に必ずnull文字が付くようにする（要らないかも）
			}
			
			if (playerMode == 0) { // MidiOggPlayer, TonyuでOggを鳴らすサンプル はそれぞれ bgm_config.dat , loop.txt でループを指定するのでここは不要
				if (i < (int)fileReadSize && readBuf[i] == 0) oggLoopFlag = 0;
				else                                          oggLoopFlag = 1;
			} else if (playerMode == 3) { // TonyuでOggを鳴らすサンプル は音量・ループ指定を個別に設定できる
				
				// Programフォルダを無視してUsrフォルダから見る
				i-=2; if (i<0) i=0;
				if (oggBgmStr[0] == '.' && (oggBgmStr[1] == '/' || oggBgmStr[1] == '\\')) k = 2; // "./Usr/BGM/bgm01.ogg"
				else                                                                      k = 0;
				memset(tempStr, 0, sizeof(tempStr));
				memcpy(tempStr, oggBgmStr, sizeof(oggBgmStr) - 1);
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
				if (i < (int)fileReadSize) { // 音量
					getLinePoint(readBuf, fileReadSize, &i);
					k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
					if (k == -99999999) {
						if (k <     0) k =     0;
						if (k > 10000) k = 10000;
						oggVolume = (int)( (double)(k * 127 / 10000) + 0.5);
					}
					if (i < (int)fileReadSize) { // ループ
						getLinePoint(readBuf, fileReadSize, &i);
						k = strToInt(readBuf, j, i, -99999999); i+=2; j=i;
						if (k == -99999999) {
							oggLoopFlag = !!k;
						}
					} else { // ３行目が無かったら音量リセット
						oggVolume = ogg3Volume;
					}
				}
				ogg3VolumeWk = (double)oggVolume;
			}
			
			//timeStart = timeGetTime();
			if (memicmp(oggBgmStr, oggBgmStr_old, sizeof(oggBgmStr)) != 0) { // 変更あり
				memcpy(oggBgmStr_old, oggBgmStr, sizeof(oggBgmStr));
				
				if (playerMode != 3) { // Yellowでは開始位置指定・ループ後途中から再生・フェードイン の機能が使える
					if (!oggPlay(oggBgmStr, oggLoopFlag, oggFileLoop, (double)oggStartTime/1000.0, (double)oggLoopTime/1000.0, (double)oggLoopEndTime/1000.0, oggFadeInTime)) goto bgmErrorLabel3; // 新しく読み込み直して再生
					
				} else { // TonyuでOggを鳴らすサンプル では開始位置指定・ループ後途中から再生・フェードイン の機能が使えない
					if (!oggPlay(oggBgmStr, oggLoopFlag, 0, 0.0, 0.0, 0.0, 0)) goto bgmErrorLabel3; // 新しく読み込み直して再生
					
				}
			} else {
				if (playerMode != 3) {
					if (!oggPlay(NULL, oggLoopFlag, oggFileLoop, (double)oggStartTime/1000.0, (double)oggLoopTime/1000.0, (double)oggLoopEndTime/1000.0, oggFadeInTime)) goto bgmErrorLabel3; // 前にならした曲をそのまま再生
					
				} else {
					if (!oggPlay(NULL, oggLoopFlag, 0, 0.0, 0.0, 0.0, 0)) goto bgmErrorLabel3; // 前にならした曲をそのまま再生
					
				}
			}
			
		}
		
		
		bgmErrorLabel3:
			if (playerMode == 0) writeFile_C(ogg_sigAddr, "", 0, 0); // ファイルを空にする
		
		readFileEndLabel3:
			;
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//StopProc(); // デバッグ用（ここで処理を停滞させる…処理落ちの再現ぽいかな）
		
		//if (oggPauseFlag) oggPause(); // 時間を遅らせて一時停止（バグ防止）
		
		if (oggPlayerStatus == STATUS_PLAY) {
			
			// バッファ書き込み速度を制御
			if (oggBufWriteSpeed) k = (int)oggSect / 2;
			else {
				if (oggTempo < 1.0) k = 25;                   // テンポが1未満ならスピード = 1
				else                k = (int)(25 * oggTempo); // テンポが1以上ならスピード = (int)oggTempo
			}
			
			
			
			for (i = 0; i < k; i++) {
				oggBufLoopCnt = oggBufLoopCnt_thread; // スレッド切替で値が計算途中で変えられないようにする
				oggAllWriteFlag = oggAllWriteFlag_thread; // スレッド切替で値が計算途中で変えられないようにする
	    		IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint, 0);
				
				// 再生時間計算 //
				getOggPlayingTime();
				
				if ((oggFlag != oggPoint / oggSectSize || oggAllWriteFlag >= 1) && oggAllWriteFlag >= 0) {
					
					if (oggFlag != oggSect - 2 || (oggFlag == oggSect - 2 && oggAllWriteFlag >= 1)) { // バッファの末尾の時は、再生場所がバッファをループしてから実行
						// 書き込み
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
											if (oggPlayingTimeInt >= (DWORD)tempInt) oggLoopCnt ++; // カウントが足りなかったらさらにカウント
											getOggPlayingTime();
										}
									}
									oggPlayingTimeInt2 = (int)(oggLoopSeek * 1000.0);
									if (!oggLoopFlag) { oggStop(0); break; }
								}
							}
							
							if (oggFlag == oggSect - 2) { // バッファの末尾
								oggAllWriteFlag_thread --; // 追いついたので減らす
							}
							oggFlag = (oggFlag + 1) % oggSect; // 次のバッファに位置を合わせる
						}
					}
				} else {
					break ;
				}
			}
			
			if (playerMode != 3) { // フェードイン・フェードアウトの処理
				// フェードインの音量調節
				if (oggFadeInTimeRun > 0) {
					oggFadeCnt += (1000.0 / (double)oggFadeInTimeRun) * (15.0 / 1000.0);
					if (oggFadeCnt >= 1.0) {
						oggFadeCnt       = 1.0;
						oggFadeInTimeRun = 0;
					}
					oggSetVolume(oggVolume * oggFadeCnt);
				}
				// フェードアウトの音量調節
				if (oggFadeOutTimeRun > 0) {
					oggFadeCnt -= (1000.0 / (double)oggFadeOutTimeRun) * (15.0 / 1000.0);
					if (oggFadeCnt <= 0.0) {
						oggFadeCnt        = 0.0;
						oggFadeOutTimeRun = 0;
						oggStop(0);
					}
					oggSetVolume(oggVolume * oggFadeCnt);
				}
			} else { // 音量調節・フェードアウトの処理 (TonyuでOggを鳴らすサンプルモード)
				// 音量調節
				if (ogg3VolumeCnt > 0.0) {
					ogg3VolumeWk  += (double)ogg3VolumeVariation * 127.0 * (15.0 / 250.0) / 10000.0;
					ogg3VolumeCnt -= (15.0 / 250.0);
					
					oggVolume = (int)ogg3VolumeWk;
					if (ogg3VolumeWk < 0.0) { // 音量が0未満になった
						oggVolume = 0;
					} else if (ogg3VolumeWk > 127.0) { // 音量が127超えになった
						oggVolume = 127;
					}
					if (ogg3VolumeCnt <= 0.0) { // カウントが終了した
						ogg3VolumeCnt = 0.0;
						// 読み込めたら9にして書き込む
						readBuf[0] = '9';
						writeFile_C(ogg_sigAddr, readBuf, fileReadSize, 0);
					}
					
					oggSetVolume2(oggVolume);
				}
				
				// フェードアウトの音量調節
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
			
			
			// 読み込み始めは遅く、再生位置まで追いつくor再生位置より１週遅れ になると速度を速める
			if (oggBufWriteSpeed == 0 && ((oggAllWriteFlag == 0 && oggFlag == oggPoint / oggSectSize) || (oggFlag - (oggPoint / oggSectSize)) + (1 - oggAllWriteFlag) * oggSect >= 0x80000000)) oggBufWriteSpeed = 1;
			
			
			#if DEBUG
			/*
			if (timeStartSW && oggPoint != 0) {
				// 処理にかかった時間を計測
				timeInst = timeGetTime() - timeStart;
				if (timeMaxDelay <= timeInst) timeMaxDelay = timeInst;
				sprintf(testStr, "%d %d", timeMaxDelay, oggPoint);
				SetWindowText(hStatic, testStr);
				timeMaxDelay = 0;
				timeStartSW  = 0;
			}
			*/
			/*
			sprintf(testStr, 
			        "oggPoint:[%d] %d oggSect:%d oggSectSize:%d\r\n"
			        "書き込み[%d] oggFlag:%d %u %u %f\r\n"
			        "oggAllWriteFlag:%d\r\n"
			        "oggLoopCnt:%d oggBufLoopCnt:%d\r\n"
			        "oggFadeCnt:%f oggFadeInTime:%d oggFadeOutTime:%d oggVolume:%d \r\n"
			        "%d BufSize:%d BufWriteSpeed:%d ov_time_tell:%f\r\n"
			        "oggLoopSeek:%f oggLoopEndSeek:%f oggFileLoop:%d\r\n"
			        "%f %f %d %f %f %f\r\n%d %f (%d)"
			        ,
			        oggPoint, oggPoint / oggSectSize, oggSect, oggSectSize,
			        oggSectSize * oggFlag, oggFlag, AB1, AB2, (double)DSBufferDesc.dwBufferBytes / oggSect,
			        oggAllWriteFlag,
			        oggLoopCnt, oggBufLoopCnt,
			        oggFadeCnt, oggFadeInTime, oggFadeOutTime, oggVolume,
			        (oggFlag - (oggPoint / oggSectSize)) + (1 - oggAllWriteFlag) * oggSect, DSBufferDesc.dwBufferBytes, oggBufWriteSpeed, ov_time_tell(&ovf),
			        oggLoopSeek, oggLoopEndSeek, oggFileLoop,
			        (double) oggPoint / (waveFormat.nSamplesPerSec * waveFormat.nBlockAlign),
			        oggStartSeek,                                    // 
			        oggBufLoopCnt * oggPlayTime,                     // バッファの時間
			        oggLoopSeek   * oggLoopCnt,                      // ループ始端
			        - (oggLoopEndSeek == 0.0) * (oggTotalTime * oggLoopCnt),      // ループ終端がない
			        - (oggLoopEndSeek >  0.0) * ((oggLoopEndSeek) * oggLoopCnt), // ループ終端がある
			        oggPlayingTimeInt2,
			        (ov_time_tell(&ovf) - oggPlayingTime - oggPlayTime), (oggFlag - (oggPoint / oggSectSize)) + (1 - oggAllWriteFlag) * oggSect >= 0x80000000
			        );
			SetWindowText(hStatic, testStr);
			*/
			/*
			sprintf(testStr, "%d %d %d", oggPoint2, oggPoint2_old, oggBufLoopCnt);
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
			
			// ジョイスティックが刺さっているか判定
			if (joystickCheckTimeCnt <= 0) {
				joystickOnId1 = Joystick_getOn(JOYSTICKID1);
				joystickOnId2 = Joystick_getOn(JOYSTICKID2);
				joistickOnIdAll = ((joystickOnId1 & 1) << 0) | ((joystickOnId2 & 1) << 1);
				joystickCheckTimeCnt = JOYSTICK_CHECK_TIME;
			}
			
			GetLocalTime(&stTime);
			memset(tempStr, 0, 512);
			i = 0;
			// midi再生時間取得 //
			GGS4GetPlayerStatus(&midiStatus);
			
			if (playFlag == 0) {
				memcpy(tempStr+i, &midiStatus.State      , sizeof(BYTE)); i += sizeof(BYTE);
			} else {
				memcpy(tempStr+i, &midiStatus_State_old  , sizeof(BYTE)); i += sizeof(BYTE);
			}
			memcpy(tempStr+i, &playId                , sizeof(WORD)); i += sizeof(WORD);
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
			
			// ogg再生時間計算 //
    		if (pDSBuffer) {
    			oggBufLoopCnt = oggBufLoopCnt_thread; // スレッド切替で再生時間計算中に値が変更されないようにする
    			IDirectSoundBuffer_GetCurrentPosition(pDSBuffer, &oggPoint, 0);
				if (oggPlayerStatus != STATUS_STOP) getOggPlayingTime();
			}
			memcpy(tempStr+i, &oggPlayerStatus          , sizeof(BYTE));  i += sizeof(BYTE); // 
			memcpy(tempStr+i, oggBgmName                , lstrlen(oggBgmName)); i += lstrlen(oggBgmName); // Oggファイル名
			memcpy(tempStr+i, _T("\r\n")                , 2);             i += lstrlen(_T("\r\n")); // 改行コード
			memcpy(tempStr+i, &oggPlayingTimeInt        , sizeof(int));   i += sizeof(int);  // 
			memcpy(tempStr+i, &oggLoopCnt               , sizeof(int));   i += sizeof(int);  // 
			memcpy(tempStr+i, &waveFormat.nChannels     , sizeof(BYTE));  i += sizeof(BYTE); // チャンネル数(2バイト)
			memcpy(tempStr+i, &waveFormat.nSamplesPerSec, sizeof(int));   i += sizeof(int);  // サンプリングレート
			memcpy(tempStr+i, &oggTotalTimeInt          , sizeof(int));   i += sizeof(int);  // 合計時間
			memcpy(tempStr+i, &oggTotalPcm              , sizeof(int));   i += sizeof(int);  // 合計サンプル数
			memcpy(tempStr+i, &oggLoopSeekInt           , sizeof(int));   i += sizeof(int);  // Oggループが始まる位置
			memcpy(tempStr+i, &oggLoopEndSeekInt        , sizeof(int));   i += sizeof(int);  // Oggループが終わる位置
			
			// 日付・時間
			memcpy(tempStr+i, &stTime.wHour        , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wMinute      , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wSecond      , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wMilliseconds, sizeof(WORD)); i += sizeof(WORD);
			memcpy(tempStr+i, &stTime.wYear        , sizeof(WORD)); i += sizeof(WORD);
			memcpy(tempStr+i, &stTime.wMonth       , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wDay         , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &stTime.wDayOfWeek   , sizeof(BYTE)); i += sizeof(BYTE);
			
			// ディスプレイ解像度
			dispx = GetSystemMetrics(SM_CXSCREEN);
			dispy = GetSystemMetrics(SM_CYSCREEN);
			memcpy(tempStr+i, &dispx , sizeof(WORD)); i += sizeof(WORD);
			memcpy(tempStr+i, &dispy , sizeof(WORD)); i += sizeof(WORD);
			
			// OSのバージョン
			memcpy(tempStr+i, &osPlatformId , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &osMajorVersion , sizeof(BYTE)); i += sizeof(BYTE);
			memcpy(tempStr+i, &osMinorVersion , sizeof(BYTE)); i += sizeof(BYTE);
			
			// Joystickが刺さっているか(ID1, ID2)
			memcpy(tempStr+i, &joistickOnIdAll , sizeof(BYTE)); i += sizeof(BYTE);
			
			
			//sprintf(testStr, "%d", i);
			//MessageBox(0,testStr,0,0);
			
			hdError = writeFile_C(midiPlayerStatusAddr, tempStr, i, 0);
			if (hdError != 0) playerStatusCnt = midiGettingPlayerStatus;
		}
		if (midiGettingPlayerStatus < 1000 || (midiGettingPlayerStatus >= 1000 && playerStatusCnt <= 5))
			playerStatusCnt --; // midiGettingPlayerStatus が 1000 のときは定期的に書き込まない
	} else playerStatusCnt = 0;
	
	// ジョイスティック判定までの残り時間を減らす
	if (joystickCheckTimeCnt > 0) joystickCheckTimeCnt --;
	
	
	
	// ファイル削除
	if (fileDeleteFlag > 0 && cnt % (playerSpeedMode+1) == 0) {	// 削除頻度調節
		// MidiOggPlayerモード以外はファイルをnullにする
		fileDeleteFlag --;
		if (fileDeleteFlag <= 0) {
			if (playerMode != 2) {
				#if !CREATE_TONYU_OLD
					hFile = CreateFile(_T(".\\Kernel\\sig"), 0, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				#else
					hFile = CreateFile(_T(".\\files\\midi_sig.dat"), 0, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				#endif
				if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile); // クローズ
			}
			fileDeleteFlag = 0;
		}
	}
	
	
	#if DEBUG
	/*
	if (cnt % 60 == 0) {
		
		rpsTime = timeGetTime();
		
		sprintf(testStr, "bgmStr                 :「%s」\r\n"
		                 "oggBgmStr              :「%s」\r\n"
		                 "usrFolderAddr          :「%s」\r\n"
		                 "ogg_sigAddr            :「%s」\r\n"
		                 "defaultTonyuprjFileAddr:「%s」\r\n"
		                 "eff_midiDir            :「%s」\r\n"
		                 "eff_midiAddr           :「%s」\r\n"
		                 "midiPlayerStatusAddr   :「%s」\r\n"
		                 "YMPPreparationAddr:「%s」 "
		                 "processCheckFlag:「%d」 "
		                 "dwProcessId:「%d」 "
		                 "playerExeName:「%s」 "
		                 "cnt:「%d」 "
		                 "processExist:「%d」\r\n"
		                 "midiEffectFlag:「%d」"
		                 "rpsTime:「%u」"
		                 ,
		                 bgmStr,
		                 oggBgmStr,
		                 usrFolderAddr,
		                 ogg_sigAddr,
		                 defaultTonyuprjFileAddr,
		                 eff_midiDir,
		                 eff_midiAddr,
		                 midiPlayerStatusAddr,
		                 YMPPreparationAddr,
		                 processCheckFlag,
		                 dwProcessId,
		                 playerExeName,
		                 cnt,
		                 processExist(dwProcessId),
		                 midiEffectFlag,
		                 rpsTime - rpsTimeOld
		                 );
		
		rpsTimeOld = rpsTime;
		
		//sprintf(testStr, "bgmType:%d\r\nplayingFlag:%d\r\oggAllWriteFlag:%d\r\noggPlayingTime:%f", bgmType, playingFlag, oggAllWriteFlag, oggPlayingTime);
		//sprintf(testStr, "", bgmType, playingFlag, oggAllWriteFlag, oggPlayingTime);
		SetWindowText(hStatic, testStr);
		
		
		
	}
	*/
	#endif
	
	
	// DirectMusicのON/OFF切替・ファイルの読み込み頻度の変更(YellowMusicPlayerモードの時のみ) (１秒に２回読み込む)
	if (playerMode == 0) {
		if (cnt % 33 == 0) {
			fileReadSize = readFile_C(YMPPreparationAddr, readBuf,  sizeof(readBuf) - 1, 0);
			if (fileReadSize >= 1) {
				
				playerDirectMusicMode_old = playerDirectMusicMode;
				
				// デフォルト値
				// OSによってMidiデバイスを自動選択
				if (osPlatformId == 1 || (osPlatformId == 2 && osMajorVersion <= 5)) playerDirectMusicMode = 0; // XP以前ではDirectMusicを使わない
				else                                                                 playerDirectMusicMode = 1; // Vista以降ではDirectMusicを使う
				
				i = 0; j = 0;
				getLinePoint(readBuf, fileReadSize, &i);
				tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
				if      (tempInt == 1) playerDirectMusicMode = 1; // 全てのOSでDirectMusicを使う
				else if (tempInt == 2) playerDirectMusicMode = 0; // 全てのOSでDirectMusicを使わない
				
				if (i < (int)fileReadSize) {
					getLinePoint(readBuf, fileReadSize, &i);
					tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
					if (tempInt <   0) tempInt =   0;
					if (tempInt > 100) tempInt = 100;
					playerSpeedMode = tempInt; // playerSpeedMode*15ミリ秒ごとに動作
				}
				
				// DirectMusicのON/OFFが変わったらMidiデバイスを開き直す
				if (playerDirectMusicMode != playerDirectMusicMode_old) {
					// Midiデバイスを閉じる
					GGS4CloseDevice();
					
					// Midiデバイスを開き直す
					
					// Midiデバイスを開く //
					if (playerDirectMusicMode) {
						errorFlag = GGS4OpenDevice(GGSDEVICE_DIRECTMUSIC, hWnd); // DirectMusic
						if (errorFlag != GGSERROR_NOERROR) playerDirectMusicMode = 0; // エラーならDirectMusicをOFFにして再試行
					}
					if (playerDirectMusicMode == 0) errorFlag = GGS4OpenDevice(-1, hWnd); // MIDI_MAPPER
					
					// DirectMusicモードの場合、Dlsフォルダ内にDlsファイルがあるか調べる
					// あったら読み込む
					if (playerDirectMusicMode) {
						readMidiDls();
					}
					
					// Midiの音量・テンポが初期化されるので初期化
					midiVolume = 127;
					midiTempo  = 1.0;
				}
				
				DeleteFile(YMPPreparationAddr);
				
			}
		}
	}
	
	
	
	
	// プロセスチェックがONなら、15*100ミリ秒(1.5秒)毎にプロセスチェック
	// もしくは、sigファイルに終了フラグが立っていたら、プロセスチェックして自分の担当しているTonyuが終了していたら自分も終了
	// （Tonyu複数起動時に１つTonyuを終了するとつられて複数のMusicPlayerが消えるのを防ぐ）
	if (processCheckFlag && (cnt % 100 == 0 || exitCheckFlag >= 1)) {
		// プロセスが居るか？
		if (!processExist(dwProcessId)) exitFlag = 1;
	}
	if (!processCheckFlag && exitCheckFlag >= 1) exitFlag = 1;
	
	if (exitCheckFlag > 0) exitCheckFlag --; // プロセスチェック中の残り時間を減らす
	
	if (exitFlag) {
		GGS4Stop(0);
		oggStop(0);
		
		#if !CREATE_TONYU_OLD
			//DeleteFile(_T(".\\Kernel\\midi_sig.dat"));
			DeleteFile(_T(".\\Usr\\files\\ogg_sig.dat"));
			DeleteFile(_T(".\\Usr\\files\\YMPStatus.dat"));
		#else
			DeleteFile(_T(".\\files\\midi_sig.dat"));
			DeleteFile(_T(".\\files\\ogg_sig.dat"));
			DeleteFile(_T(".\\files\\YMPStatus.dat"));
		#endif
		
		#if DEBUG
			SetWindowText(hStatic, _T("終了"));
		#endif
		threadClose = 1;
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return ;
	}
	
	
	// スリープするか //
	/*
	frameSkip_d -= 1.0;
	while (TRUE) {
		#if DEBUG
			wsprintf(testStr, _T("処理回数 : %f\n時間 : %d"), frameSkip_d, timeGetTime() - oldTime);
			SetWindowText(hStatic, testStr);
		#endif
		if (frameSkip_d >= 1.0) { // まだ処理が必要ならスリープさせない
			//Sleep(0);
			break ;
		}
		nowTime = timeGetTime();
		if (nowTime - oldTime >= 15) { // スリープが必要なくなったら処理回数を計算して処理させる
			frameSkip_d += (int)(nowTime - oldTime) / 15.0;
			
			
			// フレームスキップ数の範囲を制限
			//if (frameSkip_d > 10.0) frameSkip_d = 10.0;
			if (frameSkip_d < 0.0 ) frameSkip_d = 0.0;
			
			oldTime = nowTime;
			break ;
		}
		
			//if (frameSkip_d > 10.0) frameSkip_d = 10.0;Sleep(1);
	}*/
	
	
	#if DEBUG
		
		RPSOutput();
		
	#endif
	
	
	oldTime = nowTime;
	
	//}
	
	
	return ;
}


#if DEBUG
static void RPSOutput(void) {
	wsprintf(testStr, _T("処理時間 : %d\n１周期の時間 : %d\nコールバック呼び出し間隔 : %d\nたまっている処理数 : %d\nexitCheckFlag : %d\nexitFlag : %d\ncnt : %d\ntimerThreadRestartCnt : %d"),
	                timeGetTime() - nowTime, 
	                timeGetTime() - oldTime, 
	                nowTime - oldTime, 
	                frameSkipCnt, 
	                exitCheckFlag,
	                exitFlag,
	                cnt,
	                timerThreadRestartCnt);
	SetWindowText(hStatic, testStr);
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
