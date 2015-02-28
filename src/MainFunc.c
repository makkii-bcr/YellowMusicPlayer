#include "Global.h"


BOOL mainInit() {
	//int  YMPTempExist = 0;
	
	// �����񏉊���
	memset(readBuf3               , 0, sizeof(readBuf3               ));
	memset(readBuf4               , 0, sizeof(readBuf4               ));
	memset(readBuf5               , 0, sizeof(readBuf5               ));
	memset(readBuf6               , 0, sizeof(readBuf6               ));
	memset(readBuf7               , 0, sizeof(readBuf7               ));
	memset(eff_midiAddr           , 0, sizeof(eff_midiAddr           ));
	memset(eff_midiDir            , 0, sizeof(eff_midiDir            ));
	memset(eff_midiDir_old        , 0, sizeof(eff_midiDir_old        ));
	memset(ogg_sigAddr            , 0, sizeof(ogg_sigAddr            ));
	memset(bgmStr                 , 0, sizeof(bgmStr                 ));
	memset(bgmStr_old             , 0, sizeof(bgmStr_old             ));
	memset(playedBgmStr           , 0, sizeof(playedBgmStr           ));
	memset(oggBgmStr              , 0, sizeof(oggBgmStr              ));
	memset(oggBgmStr_old          , 0, sizeof(oggBgmStr_old          ));
	memset(playedOggBgmStr        , 0, sizeof(playedOggBgmStr        ));
	memset(midiPlayerStatusAddr   , 0, sizeof(midiPlayerStatusAddr   ));
	memset(defaultTonyuprjFileAddr, 0, sizeof(defaultTonyuprjFileAddr));
	memset(usrFolderAddr          , 0, sizeof(usrFolderAddr          ));
	memset(midiFileAddrList       , 0, sizeof(midiFileAddrList       ));
	
	// �v���b�g�t�H�[���擾�i9x�n�� NT�n���H�j
	osPlatformId = getPlatformId();
	
	// �p�X�����߂�
	memset(crDir, 0, sizeof(crDir));
	GetCurrentDirectory(sizeof(crDir), crDir);
	
	// YellowMusicPlayer�̐ݒ��ǂݍ���
	memset(playerExeName, 0, sizeof(playerExeName));
	setMidiPlayerConfig(".\\Usr\\files\\YMPConfig.txt", osPlatformId, playerExeName, sizeof(playerExeName));
	
	// �������ł��邱�Ƃ�Tonyu�ɒm�点��t�@�C�����쐬����(YellowMusicPlayer���[�h�̎��̂�)
	if (playerMode == 0) createPreparationFile();
	
	// Tonyu�v���Z�X�̐����m�F�����邩���f
	processCheckingInit(crDir, osPlatformId, &dwProcessId, &processCheckFlag, playerExeName, sizeof(playerExeName));
	
	// Midi�֌W�̏�����
	if (!playerInit(playerDirectMusicMode)) {
		PostMessage(hWnd, WM_CLOSE, 0, 0); // �������Ɏ��s������I��
		return FALSE;
	}
	midiPlayerOK = 1;
	
	// DirectSound��������
	if (!createDS(&pDS8, hWnd)) {
		PostMessage(hWnd, WM_CLOSE, 0, 0); // �������Ɏ��s������I��
		return FALSE;
	}
	
	// �Ƃ肠���������ݒ�Ƃ��āA�����^�C���ł̏ꍇ�̃A�h���X��ݒ肷��
	if (strlen(crDir) + strlen("\\Usr\\files\\YMPStatus.dat") < sizeof(defaultTonyuprjFileAddr)) {
		memcpy(usrFolderAddr, crDir, strlen(crDir));                           // 
		strcat(usrFolderAddr, "\\Usr");                                        // Player.exe�̃f�B���N�g��/Usr
		
		memcpy(defaultTonyuprjFileAddr, crDir, strlen(crDir));                 // 
		strcat(defaultTonyuprjFileAddr, "\\Usr\\default.tonyuprj");            // Player.exe�̃f�B���N�g��/Usr/default.tonyuprj
		
		memcpy(eff_midiDir, crDir, strlen(crDir));                             // 
		strcat(eff_midiDir, "\\Usr\\files");                                   // Player.exe�̃f�B���N�g��/Usr/files
		memcpy(eff_midiDir_old, eff_midiDir, sizeof(eff_midiDir_old));         // �X�V�O�ƍX�V��̔��f�����邽�߂ɕK�v�Ȃ�����̃o�b�t�@
		
		memcpy(eff_midiAddr, crDir, strlen(crDir));                            // 
		strcat(eff_midiAddr, "\\Usr\\files\\eff_midi.txt");                    // Player.exe�̃f�B���N�g��/Usr/files/eff_midi.txt
		
		memcpy(ogg_sigAddr, crDir, strlen(crDir));                             // 
		if (playerMode != 3) strcat(ogg_sigAddr, "\\Usr\\files\\ogg_sig.dat"); // Player.exe�̃f�B���N�g��/Usr/files/ogg_sig.dat
		else                 strcat(ogg_sigAddr, "\\Usr\\files\\temp.txt");    // Player.exe�̃f�B���N�g��/Usr/files/temp.txt
		
		memcpy(midiPlayerStatusAddr, crDir, strlen(crDir));                    // 
		strcat(midiPlayerStatusAddr, "\\Usr\\files\\YMPStatus.dat");           // Player.exe�̃f�B���N�g��/Usr/files/YMPStatus.dat
		
		memcpy(YMPPreparationAddr, crDir, strlen(crDir));                      // 
		strcat(YMPPreparationAddr, "\\Usr\\files\\YMPPreparation.dat");        // Player.exe�̃f�B���N�g��/Usr/files/YMPPreparation.dat
		
		
		
	}
	if (PathFileExists(eff_midiAddr)) {
		// Midi�t�@�C�����ɓǂݍ���ł���
		beforeReadMidi(usrFolderAddr, defaultTonyuprjFileAddr, midiFileAddrList, &midiFileAddrListLength);
		midiEffectFlag = 1;
	}
	
	// DirectMusic���[�h�̏ꍇ�ADls�t�H���_����Dls�t�@�C�������邩���ׂ�
	// ��������ǂݍ���
	if (playerDirectMusicMode) {
		readMidiDls();
	}
	
	// �������ł��邱�Ƃ�Tonyu�ɒm�点��t�@�C��������
	deletePreparationFile();
	
	return TRUE;
}


// �������Eeff_midi.txt�̃f�B���N�g�����ς�������ɌĂяo�����
void initMidiParam() {
	
	midiGettingPlayerStatus = 0;
	
	midiVolume      = 0;
	midiTempo       = 1.0;
	midiPitch       = 0;
	
	midiFadeInTime  = 0;
	if (playerMode == 2) midiFadeOutTime = 1000; // MidiOggPlayer���[�h��1000
	else                 midiFadeOutTime =    0; // ����ȊO�̃��[�h��0
	midiKeyShift    = 0;
	midiStartTick   = 0;
	
	midiSkipBeginning = 1;
	if (playerMode == 1) midiSkipEnding = 1; // TonyuMidiLitest���[�h�̓X�L�b�v����
	else                 midiSkipEnding = 0; // ����ȊO�̃��[�h�̓X�L�b�v�Ȃ�
	midiMasterPitch   = 0; midiPlayingMasterPitch = 0;
	midiNormalize     = 0;
	midiExclusive     = 0;
	
	GGS4SetMasterTempo(midiTempo);
	GGS4SetMasterVolume(midiVolume);
	GGS4SetMasterPitch(midiPitch);
}


void createPreparationFile() {
	HANDLE   hFile;
	
	// �������ł��邱�Ƃ�Tonyu�ɒm�点��
	hFile = CreateFile(".\\Usr\\files\\YMPPreparation.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
	return ;
}


void deletePreparationFile() {
	int i;
	for (i = 0; i < 10; i++) {
		DeleteFile(".\\Usr\\files\\YMPPreparation.dat");
		if (!PathFileExists(".\\Usr\\files\\YMPPreparation.dat")) break ;
		Sleep(15);
	}
	return ;
}


// OS�̌n�����擾
DWORD getPlatformId() {
	OSVERSIONINFO osInfo;
	
	osInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&osInfo);
	
	return osInfo.dwPlatformId;
}


BOOL processCheckingInit(char *crDir, DWORD osPlatformId, DWORD *pDwProcessId, BOOL *pProcessCheckFlag, char *playerExeName, size_t playerExeNameSize) {
	TCHAR crDirExe[FILEPATH_SIZE];
	
	*pProcessCheckFlag = 1;
	
	if (!GetModuleFileNameExA) {
		if (osPlatformId != 1) {
			// NT�n�Ȃ�DLL��ǂݍ���
			GetModuleFileNameExA = GetProcAddress(LoadLibrary("psapi.DLL") , "GetModuleFileNameExA");
			if (GetModuleFileNameExA == NULL) { // DLL���ǂݍ��߂Ȃ�����
				*pProcessCheckFlag = 0;
				return FALSE;
			}
		}
	}
	
	memset(crDirExe, 0, sizeof(crDirExe));
	
	if (playerExeName[0] != '\0') {
		// XXXXXX.exe�v���Z�X�����邩�H
		memcpy(crDirExe, crDir, sizeof(crDirExe)-1);
		if (lstrlen(crDirExe) + strlen("\\") + strlen(playerExeName) < sizeof(crDirExe)) { strcat(crDirExe, "\\"); strcat(crDirExe, playerExeName); }
		*pDwProcessId = searchProcess(crDirExe, osPlatformId);
		// XXXXXX.exe���������΃p�X��ۑ����ďI��
		if (*pDwProcessId != (DWORD)0 && *pDwProcessId != (DWORD)0xFFFFFFFF) {
			if ((unsigned int)lstrlen(crDirExe) < playerExeNameSize) memcpy(playerExeName, crDirExe, lstrlen(crDirExe));
			goto methodLabel;
		}
	}
	
	// Player.exe�v���Z�X�����邩�H
	memcpy(crDirExe, crDir, sizeof(crDirExe)-1);
	if (lstrlen(crDirExe) + strlen("\\Player.exe") < sizeof(crDirExe)) strcat(crDirExe, "\\Player.exe");
	*pDwProcessId = searchProcess(crDirExe, osPlatformId);
	// Player.exe��������I��
	if (*pDwProcessId != (DWORD)0 && *pDwProcessId != (DWORD)0xFFFFFFFF) {
		if ((unsigned int)lstrlen(crDirExe) < playerExeNameSize) memcpy(playerExeName, crDirExe, lstrlen(crDirExe));
		goto methodLabel;
	}
	
	// Tonyu.exe�v���Z�X�����邩�H
	memcpy(crDirExe, crDir, sizeof(crDirExe)-1);
	if (lstrlen(crDirExe) + strlen("\\Tonyu.exe") < sizeof(crDirExe)) strcat(crDirExe, "\\Tonyu.exe");
	*pDwProcessId = searchProcess(crDirExe, osPlatformId);
	if (*pDwProcessId != (DWORD)0 && *pDwProcessId != (DWORD)0xFFFFFFFF) {
		if ((unsigned int)lstrlen(crDirExe) < playerExeNameSize) memcpy(playerExeName, crDirExe, lstrlen(crDirExe));
		goto methodLabel;
	}
	
	// �v���Z�X�����Ȃ�or�G���[
	*pProcessCheckFlag = 0; // �v���Z�X�`�F�b�N��OFF
	
	methodLabel:
		;
	return TRUE;
}



BOOL playerInit(int DirectMusicOn) {
	GGSERROR errorFlag;
	char     tempStr[BUF_SIZE_S];
	static   SYSTEMTIME stTime;
	int      value;
	DWORD    fileReadSize;
	
	writeFile_C(".\\Kernel\\sig", "", 0, 1);
	if (playerMode == 0) writeFile_C(".\\Usr\\files\\ogg_sig.dat", "", 0, 1);
	
	if (playerMode == 2) { // MidiOggPlayer���[�h�̏����ݒ�
		writeFile_C(".\\Usr\\files\\midi_sig.dat", "0", 1, 1);
		writeFile_C(".\\Usr\\files\\ogg_sig.dat", "0", 1, 1);
		
		GetLocalTime(&stTime);
		sprintf(tempStr, "%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d",
		        stTime.wYear, stTime.wMonth, stTime.wDay,
		        stTime.wHour, stTime.wMinute, stTime.wSecond);
		writeFile_C(".\\Usr\\files\\realtime.dat", tempStr, strlen(tempStr), 1);
	}
	if (playerMode == 3) {
		writeFile_C(".\\Usr\\files\\temp.txt", "0", 1, 1);
		
		fileReadSize = readFile_C(".\\Usr\\files\\loop.txt", tempStr, 64, 0);
		if (fileReadSize > 0) {
			value = strToInt(tempStr, 0, fileReadSize, -9999);
			if (value != -9999) {
				ogg3LoopFlag = !!value;
			}
		}
		fileReadSize = readFile_C(".\\Usr\\files\\volume.txt", tempStr, 64, 0);
		if (fileReadSize > 0) {
			value = strToInt(tempStr, 0, fileReadSize, -9999);
			if (value != -9999) {
				if (value <     0) value =     0;
				if (value > 10000) value = 10000;
				ogg3Volume = (int)( (double)(value * 127 / 10000) + 0.5);
				oggVolume  = ogg3Volume;
				if (ogg3Volume <   0) ogg3Volume =   0;
				if (ogg3Volume > 127) ogg3Volume = 127;
			}
		}
		fileReadSize = readFile_C(".\\Usr\\files\\fade_s.txt", tempStr, 64, 0);
		if (fileReadSize > 0) {
			value = strToInt(tempStr, 0, fileReadSize, -9999);
			if (value != -9999) {
				if (value <     0) value =     0;
				if (value > 10000) value = 10000;
				ogg3FadeOutTime = 10000 * 250 / value;
				oggFadeOutTime  = ogg3FadeOutTime;
			}
		}
		
	}
	
	if (DirectMusicOn) errorFlag = GGS4OpenDevice(GGSDEVICE_DIRECTMUSIC, hWnd); // DirectMusic
	else               errorFlag = GGS4OpenDevice(-1, hWnd);                    // MIDI_MAPPER
	if (errorFlag != GGSERROR_NOERROR) {
		return FALSE;
	}
	
	// Midi�p�����[�^������
	initMidiParam();
	
	return TRUE;
}


void beforeReadMidi(char *usrFolderAddr, char *defaultTonyuprjFileAddr, char **midiFileAddrList, int *pMidiFileAddrListLength) {
	char readBuf[BUF_SIZE_L];
	char midiAddr[FILEPATH_SIZE];    // Usr���猩��midi�t�@�C���̃A�h���X
	char midiAbsAddr[FILEPATH_SIZE]; // ��΃A�h���X
	char *pBgmTagBegin;
	char *pBgmTagEnd;
	char *pTemp, *pTempR;
	int  i;
	BOOL readError = 0;
	GGSERROR errorFlag;
	DWORD fileReadSize;
	
	for (i = 0; i < 30; i++) {
		fileReadSize = readFile_C(defaultTonyuprjFileAddr, readBuf, sizeof(readBuf) - 1, 1);
		if (fileReadSize > 0) break;
		Sleep(15);
	}
	if (fileReadSize == 0) return ;
	
	// midiFileAddrList��������
	for (i = 0; i < 255; i++) {
		if (midiFileAddrList[i]) { // �|�C���^���L���Ȃ�
			free(midiFileAddrList[i]); // ������������
		}
		midiFileAddrList[i] = 0; // �|�C���^��0�ɂ���
	}
	*pMidiFileAddrListLength = 0;
	
	GGS4ClearList(); // GGS4���̃��X�g�̓��e��S����
	
	pTemp = readBuf;
	while (TRUE) {
		pTempR = strstr(pTemp, "<BGM name=\"");
		if (pTempR == NULL) break;
		pTemp = pTempR + 11;
		
		pTempR = strstr(pTemp, "\" ");
		if (pTempR == NULL) break;
		pTemp = pTempR + 2;
		
		pTempR = strstr(pTemp, "src=\"");
		if (pTempR == NULL) break;
		pTemp = pTempR + 5;
		pBgmTagBegin = pTemp;
		
		pTempR = strstr(pBgmTagBegin, "\"/>");
		if (pTempR == NULL) break;
		pBgmTagEnd = pTempR;
		pTemp = pTempR + 3;
		
		// midi�t�@�C���̃A�h���X�擾
		memset(midiAbsAddr, 0, sizeof(midiAbsAddr));
		memset(midiAddr, 0, sizeof(midiAddr));
		if (pBgmTagEnd - pBgmTagBegin < sizeof(midiAddr)) memcpy(midiAddr, pBgmTagBegin, pBgmTagEnd - pBgmTagBegin); else readError = 1;
		
		// ���΃p�X�����΃p�X�ɕϊ� (��: C:\Users\makkii\Desktop\Game\Usr\ + bgm1.mid)
		if (strlen(usrFolderAddr)                   < sizeof(midiAbsAddr)) memcpy(midiAbsAddr, usrFolderAddr, strlen(usrFolderAddr)); else readError = 1;
		if (strlen(midiAbsAddr) + strlen("\\")      < sizeof(midiAbsAddr)) strcat(midiAbsAddr, "\\");                                 else readError = 1;
		if (strlen(midiAbsAddr) + strlen(midiAddr)  < sizeof(midiAbsAddr)) strcat(midiAbsAddr, midiAddr);                             else readError = 1;
		
		if (readError == 0) {
			
			// midi�t�@�C����ǂݍ���ł���(�Ď��s3��)
			if (playerCacheMode != 0) { // �L���b�V�����L���̂Ƃ�
				for (i = 3; i > 0; i--) {
					errorFlag = GGS4AddListFromFileA(midiAbsAddr, GGSLOAD_BUFFERED*1, *pMidiFileAddrListLength);
					if (errorFlag == 0) break ;
					Sleep(15);
				}
			}
			
			if (i > 0) { // midi�t�@�C���̓ǂݍ��݂ɐ��������ꍇ
				// midi�t�@�C���A�h���X���X�g�ɁAmidi�̐�΃A�h���X��o�^
				midiFileAddrList[*pMidiFileAddrListLength] = (char *)malloc(sizeof(char) * strlen(midiAbsAddr) + 1); // �������m��
				if (midiFileAddrList[*pMidiFileAddrListLength] == NULL) exit(-1); // �������m�ێ��s �� �����I��
				memset(midiFileAddrList[*pMidiFileAddrListLength], 0, strlen(midiAbsAddr) + 1);
				memcpy(midiFileAddrList[*pMidiFileAddrListLength], midiAbsAddr, strlen(midiAbsAddr));
				
				(*pMidiFileAddrListLength) ++;
				if (*pMidiFileAddrListLength >= 255) break;
			}
		}
		readError = 0;
	}
	
	return ;
}


BOOL setEffectFileAddr(char *bgmStr, 
                       char *eff_midiAddr           , size_t eff_midiAddrSize,
                       char *eff_midiDir            , size_t eff_midiDirSize,
                       char *ogg_sigAddr            , size_t ogg_sigAddrSize,
                       char *defaultTonyuprjFileAddr, size_t defaultTonyuprjFileAddrSize,
                       char *usrFolderAddr          , size_t usrFolderAddrSize,
                       char *midiPlayerStatusAddr   , size_t midiPlayerStatusAddrSize,
                       char *YMPPreparationAddr     , size_t YMPPreparationAddrSize
                       ) {
	int i, len;
	int sc = 1;
	
	if (bgmStr[0] == '\0') return FALSE;
	
	len = strlen(bgmStr);
	for (i = len - 1; i >= 0; i--) {
		if (bgmStr[i] == '\\') {
			// default.tonyuprj �͂ǂ̃v���W�F�N�g�ɂ����݂���̂ŁA
			// ���̃t�@�C����������΁Aeff_midiAddr �̗L���������ɂ킩��
			
			// defaultTonyuprjFileAddr
			memset(defaultTonyuprjFileAddr, 0, defaultTonyuprjFileAddrSize);
			if (i + strlen("\\default.tonyuprj") >= defaultTonyuprjFileAddrSize) break ;
			memcpy(defaultTonyuprjFileAddr, bgmStr, i);
			strcat(defaultTonyuprjFileAddr, "\\default.tonyuprj");
			
			if (PathFileExists(defaultTonyuprjFileAddr)) { // default.tonyuprj ������ꍇ
				
				// usrFolderAddr�̓��e���ς���ĂȂ���������s���Ȃ�
				if (memcmp(bgmStr, usrFolderAddr, i) == 0) return TRUE;
				
				// usrFolderAddr
				memset(usrFolderAddr, 0, usrFolderAddrSize);
				memcpy(usrFolderAddr, bgmStr, i);
				
				// midiPlayerStatusAddr
				memset(midiPlayerStatusAddr, 0, midiPlayerStatusAddrSize);
				if (strlen(usrFolderAddr) + strlen("\\files\\YMPStatus.dat")>= midiPlayerStatusAddrSize) break ;
				memcpy(midiPlayerStatusAddr, usrFolderAddr, strlen(usrFolderAddr));
				strcat(midiPlayerStatusAddr, "\\files\\YMPStatus.dat");
				
				// eff_midiAddr
				memset(eff_midiAddr, 0, eff_midiAddrSize);
				if (i + strlen("\\files\\eff_midi.txt") >= eff_midiAddrSize) break ;
				memcpy(eff_midiAddr, bgmStr, i);
				strcat(eff_midiAddr, "\\files\\eff_midi.txt");
				
				// ogg_sigAddr
				memset(ogg_sigAddr, 0, ogg_sigAddrSize);
				if (playerMode != 3) {
					if (i + strlen("\\files\\ogg_sig.dat") >= ogg_sigAddrSize) break ;
					memcpy(ogg_sigAddr, bgmStr, i);
					strcat(ogg_sigAddr, "\\files\\ogg_sig.dat");
				} else {
					if (i + strlen("\\files\\temp.txt") >= ogg_sigAddrSize) break ;
					memcpy(ogg_sigAddr, bgmStr, i);
					strcat(ogg_sigAddr, "\\files\\temp.txt");
				}
				
				// YMPPreparationAddr
				memset(YMPPreparationAddr, 0, YMPPreparationAddrSize);
				if (i + strlen("\\files\\YMPPreparation.dat") >= YMPPreparationAddrSize) break ;
				memcpy(YMPPreparationAddr, bgmStr, i);
				strcat(YMPPreparationAddr, "\\files\\YMPPreparation.dat");
				
				
				if (PathFileExists(eff_midiAddr)) { // eff_midi.txt ������ꍇ
					// eff_midiDir
					memset(eff_midiDir, 0, eff_midiDirSize);
					if (i + strlen("\\files") >= eff_midiDirSize) break ;
					memcpy(eff_midiDir, bgmStr, i);
					strcat(eff_midiDir, "\\files");
					
					return TRUE;
				}
				break ;
			}
			
			sc ++;
			if (sc > 4) break ;
		}
	}
	memset(eff_midiDir , 0, eff_midiDirSize );
	memset(eff_midiAddr, 0, eff_midiAddrSize);
	//memset(defaultTonyuprjFileAddr, 0, defaultTonyuprjFileAddrSize);
	
	return FALSE;
}


void setMidiPlayerConfig(char *readFileName, DWORD osPlatformId, char *playerExeName, size_t playerExeNameSize) {
	int   i = 0, j;
	DWORD fileReadSize;
	int   tempInt;
	char  readBuf[BUF_SIZE_S];
	
	// �f�t�H���g�ݒ� //////////////////////////////////////////////
	
	// OS�ɂ����Midi�f�o�C�X�������I��
	if (osPlatformId == 1) playerDirectMusicMode = 0; // 9x�n�ł�DirectMusic���g��Ȃ�
	else                   playerDirectMusicMode = 1; // NT�n�ł�DirectMusic���g��
	playerMode      = 0; // YellowMusicPlayer���[�h
	playerSpeedMode = 0; // 15�~���b���Ƃɓ���
	playerCacheMode = 0; // �L���b�V���Ȃ����[�h
	
	////////////////////////////////////////////////////////////////
	// �t�@�C����ǂݍ��ݕύX�_��ݒ� //
	
	fileReadSize = readFile_C(readFileName, readBuf, sizeof(readBuf) - 1, 1);
	if (fileReadSize <= 0) return ;
	
	// Player.exe �̖��O���ς����Ă��鎞�A���̃Q�[���������I���������ł�YellowMusicPlayer�̎����I�����ۏ؂����
	// Player.exe , Tonyu.exe ����̋N���ł���΁A���̍s�͎w��Ȃ��Ō��\
	getLinePoint(readBuf, fileReadSize, &i);
	if (i < (int)playerExeNameSize) memcpy(playerExeName, readBuf, i);
	i+=2; j=i;
	
	if (i >= (int)fileReadSize) return ;
	getLinePoint(readBuf, fileReadSize, &i);
	tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
	if      (tempInt == 1) playerDirectMusicMode = 1; // �S�Ă�OS��DirectMusic���g��
	else if (tempInt == 2) playerDirectMusicMode = 0; // �S�Ă�OS��DirectMusic���g��Ȃ�
	
	if (i >= (int)fileReadSize) return ;
	getLinePoint(readBuf, fileReadSize, &i);
	tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
	if      (tempInt == 1) playerMode = 1; // TonyuMidiLitest���[�h
	else if (tempInt == 2) playerMode = 2; // MidiOggPlayer���[�h
	else if (tempInt == 3) playerMode = 3; // Tonyu��Ogg Vorbis��炷�T���v�� ���[�h
	
	if (i >= (int)fileReadSize) return ;
	getLinePoint(readBuf, fileReadSize, &i);
	tempInt = strToInt(readBuf, j, i, -1); i+=2; j=i;
	if (tempInt <   0) tempInt =   0;
	if (tempInt > 100) tempInt = 100;
	playerSpeedMode = tempInt; // playerSpeedMode*15�~���b���Ƃɓ���
	
	if (i >= (int)fileReadSize) return ;
	getLinePoint(readBuf, fileReadSize, &i);
	tempInt = strToInt(readBuf, j, i, -1); //i+=2; j=i;
	if (tempInt == 1) playerCacheMode = 1; // �L���b�V�����胂�[�h
	
}


// DirectMusic���[�h�Ŏg����DLS�t�@�C����ǂݍ���
void readMidiDls() {
	HANDLE hSearch;
	WIN32_FIND_DATA fd;
	char fileName[FILEPATH_SIZE];
	
	// �t�@�C�����
	hSearch = FindFirstFile(".\\Usr\\files\\dls\\*.dls", &fd);
	if(hSearch == INVALID_HANDLE_VALUE) {
		return ;
	}
	
	while(TRUE) {
		if (strlen(".\\Usr\\files\\dls\\") + strlen(fd.cFileName) >= sizeof(fileName)) { // �o�b�t�@�ɓ���Ȃ�
			goto FindNextFileLabel;
		} else { // ����
			memset(fileName, 0, sizeof(fileName));
			memcpy(fileName, ".\\Usr\\files\\dls\\", strlen(".\\Usr\\files\\dls\\"));
			strcat(fileName, fd.cFileName);
		}
		
		// DLS�t�@�C����ǂݍ���
		GGS4AddDlsA(fileName);
		
		FindNextFileLabel:
		if(!FindNextFile(hSearch, &fd)) break ;
	}
	FindClose(hSearch);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


DWORD readFile_C(LPCTSTR fileName, char *readBuf, DWORD readBufSize, int share) {
	static HANDLE hFile;
	static BOOL   hdError;
	static DWORD  dwFileReadSize;
	
	// �t�@�C���I�[�v��(sig)
	hFile = CreateFile(fileName, GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE)*share, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return 0; // �n���h���G���[
	
	dwFileReadSize = GetFileSize(hFile, NULL);
	if (dwFileReadSize == 0 || dwFileReadSize == (DWORD)0xFFFFFFFF) {
		CloseHandle(hFile);
		return 0; // �t�@�C���T�C�Y��0�܂��̓G���[
	}
	
	// �o�b�t�@������
	memset(readBuf, 0, readBufSize);
	
	// �t�@�C���ǂݍ���
	hdError = ReadFile(hFile, readBuf, readBufSize, &dwFileReadSize, NULL);
	CloseHandle(hFile);
	if (hdError == 0) return 0; // �ǂݍ��݃G���[
	
	return dwFileReadSize;
}


DWORD writeFile_C(LPCTSTR fileName, char *writeBuf, DWORD writeBufSize, int share) {
	static HANDLE hFile;
	static BOOL   hdError;
	static DWORD  dwFileWriteSize;
	
	// �t�@�C���I�[�v��(sig)
	hFile = CreateFile(fileName, GENERIC_WRITE, (FILE_SHARE_READ)*share, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return 0; // �n���h���G���[
	
	// �t�@�C����������
	hdError = WriteFile(hFile, writeBuf, writeBufSize, &dwFileWriteSize, NULL);
	CloseHandle(hFile);
	if (hdError == 0) return 0;
	
	return dwFileWriteSize;
}


BOOL fileAddrToDir(char *fileAddr, char *fileDir, size_t fileDirSize) {
	int i , len;
	
	if (fileAddr[0] == '\0') return FALSE;
	
	len = strlen(fileAddr);
	for (i = len - 1; i >= 0; i--) {
		if (fileAddr[i] == '\\') break ;
	}
	if (i == -1) return FALSE;
	
	memset(fileDir, 0, fileDirSize);
	memcpy(fileDir, fileAddr, i);
	
	return TRUE;
}


int strToInt(char *str, int start, int end, int falseReturnValue) {
	char wkStr[16];
	int  i, zero = 1;
	int  ans;
	
	if (end - start >= sizeof(wkStr) - 1) end = start + sizeof(wkStr) - 2;
	memset(wkStr, 0, sizeof(wkStr));
	memcpy(wkStr, str + start, end - start);
	
	for (i = 0; i < end - start; i++) { // 0�ƁAstoi()���ϊ��ł��Ȃ����ɕԂ�0 ����������
		if (wkStr[i] != '0') { zero = 0; break ; }
	}
	if (start == end) zero = 0; // �����񂪖������͗�O
	
	if (zero) {
		ans = 0; // ������0
	} else {
		wkStr[sizeof(wkStr) - 1] = '\0';
		ans = atoi(wkStr);
		if (ans == 0) ans = falseReturnValue; // ������0�͗�O�̈Ӗ��Ȃ̂ŁA�G���[�l(faluseReturnValue)��Ԃ�
	}
	
	return ans;
}


double strToDouble(char *str, int start, int end, double falseReturnValue) {
	char wkStr[32];
	int  i, zero = 1;
	double ans;
	 
	if (end - start >= sizeof(wkStr) - 1) end = start + sizeof(wkStr) - 2;
	memset(wkStr, 0, sizeof(wkStr));
	memcpy(wkStr, str + start, end - start);
	
	for (i = 0; i < end - start; i++) { // 0.0�ƁAstoi()���ϊ��ł��Ȃ����ɕԂ�0.0 ����������
		if (wkStr[i] != '0') { zero = 0; break ; }
	}
	if (start == end) zero = 0; // �����񂪖������͗�O
	
	if (zero) {
		ans = 0.0; // ������0.0
	} else {
		wkStr[sizeof(wkStr) - 1] = '\0';
		ans = atof(wkStr);
		if (ans == 0.0) ans = falseReturnValue; // ������0�͗�O�̈Ӗ��Ȃ̂ŁA�G���[�l(faluseReturnValue)��Ԃ�
	}
	
	return ans;
}


int getLinePoint(char *str, int strSize, int *point) {
	int i;
	
	for (i = *point; i < strSize; i++) { // ���s�̈ʒu�𒲂ׂ�
		if ((str[i] == '\r' && str[i+1] == '\n') || (str[i] == '\0')) break ;
	}
	
	*point = i;
	
	return i;
}

char *getMemSearch(char *str1, int size1, char *str2) {
	char *p = NULL;
	int i, j, size2;
	
	size2 = strlen(str2);
	for (i = 0; i < size1; i++) {
		for (j = size2 - 1; j >= 0; j--) {
			if (j == 0) {
				p = str1 + i;
				goto loopEnd;
			}
			if (str1[i + j] != str2[j]) break ;
		}
	}
	
	loopEnd:
	return p;
}



/*
�߂�l -1: �G���[
0 : �v���Z�X�����݂��Ȃ�
0,-1�ȊO: �v���Z�XID
*/
DWORD searchProcess(LPTSTR lpszFile, DWORD osPlatformId){
	HANDLE hSnapshot;
	HANDLE hProcess;
	DWORD dwProcessId;
	PROCESSENTRY32 pe;
	TCHAR fileName[FILEPATH_SIZE];
	
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return (-1);
	pe.dwSize = sizeof(PROCESSENTRY32);
	
	if (Process32First(hSnapshot, &pe)) {
		do {
			if (osPlatformId != 1) {
				dwProcessId = pe.th32ProcessID;
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
				if (hProcess != NULL) {
					GetModuleFileNameExA(hProcess, NULL, (LPTSTR)fileName, FILEPATH_SIZE);
					//MessageBox(0,fileName,"test!",0);
					CloseHandle(hProcess);
					if (!lstrcmpi((LPTSTR)fileName, lpszFile)) {
						CloseHandle(hSnapshot);
						return pe.th32ProcessID;
					}
				}
			} else {
				//MessageBox(0,pe.szExeFile,"test!",0);
				if (!lstrcmpi(pe.szExeFile, lpszFile)) {
					CloseHandle(hSnapshot);
					return pe.th32ProcessID;
				}
			}
			
		} while (Process32Next(hSnapshot, &pe));
	}
	
	CloseHandle(hSnapshot);
	return (GetLastError() == ERROR_NO_MORE_FILES) ? 0 : 0xFFFFFFFF;
}


BOOL processExist(LPTSTR lpszFile, DWORD osPlatformId, DWORD dwProcessId) {
	HANDLE hProcess;
	TCHAR fileName[FILEPATH_SIZE];
	
	// Tonyu�Ńy�[�W�ؑւ���ƁATonyu�����I�����Ɏ����I���ł��Ȃ��Ȃ��d�l���������̂ŕʂ̕��@�ɂ���
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	if (hProcess != NULL) {
		if (osPlatformId != 1) { // NT�n
			GetModuleFileNameExA(hProcess, NULL, (LPTSTR)fileName, FILEPATH_SIZE);
			CloseHandle(hProcess);
			if (!lstrcmpi((LPTSTR)fileName, lpszFile)) return TRUE;  // �����t�@�C����������
			else                                       return FALSE; // �����t�@�C�������Ȃ�
		} else { // 9x�n
			CloseHandle(hProcess);
			return TRUE;
		}
	} else {
		return FALSE;
	}
}




void getOggPlayingTime() {
	
	oggTotalTimeInt   = (int)(oggTotalTime * 1000); // ���v���Ԃ�int�ɕϊ�
	
	// ���݂̍Đ����Ԃ��v�Z
	oggPlayingTime    = (double) oggPoint / (waveFormat.nSamplesPerSec * waveFormat.nBlockAlign)
	                  + oggStartSeek                                     // 
	                  + oggBufLoopCnt * oggPlayTime                      // �o�b�t�@�̎���
	                  + oggLoopSeek   * oggLoopCnt                       // ���[�v�n�[
	                  - (oggLoopEndSeek == 0.0) * (oggTotalTime * oggLoopCnt)    // ���[�v�I�[���Ȃ�
	                  - (oggLoopEndSeek >  0.0) * (oggLoopEndSeek * oggLoopCnt); // ���[�v�I�[������
	
	if (oggPlayingTime < 0.0) oggPlayingTime = 0.0;
	
	oggPlayingTimeInt = (int)(oggPlayingTime * 1000.0); // ���݂̍Đ����Ԃ�int�ɕϊ�
	
	// �Đ����Ԃ��傫���Ȃ�߂��Ȃ��悤��
	if (oggLoopEndSeek == 0.0) {
		if (oggPlayingTimeInt >= oggTotalTimeInt) oggPlayingTimeInt = oggTotalTimeInt; // ���[�v�I�[���Ȃ�
	} else {
		if (oggPlayingTimeInt >= (DWORD)(oggLoopEndSeek * 1000)) oggPlayingTimeInt = (DWORD)(oggLoopEndSeek * 1000); // ���[�v�I�[������
	}
	
	if (oggPlayingTimeInt < oggPlayingTimeInt2) oggPlayingTimeInt  = oggPlayingTimeInt2;
	else                                        oggPlayingTimeInt2 = oggPlayingTimeInt;
}





/*
BOOL playerRestart() {
	PROCESS_INFORMATION ps;
	STARTUPINFO si;
	char myExeName[FILEPATH_SIZE];
	
	memset(myExeName, 0, sizeof(myExeName));
	//memcpy(myExeName, "Kernel\\Midiplayer2.exe", strlen("Kernel\\Midiplayer2.exe"));
	GetModuleFileName(NULL, myExeName, sizeof(myExeName));
	//MessageBox(0,myExeName,0,0);
	return !!CreateProcess(myExeName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &ps);
}
*/
