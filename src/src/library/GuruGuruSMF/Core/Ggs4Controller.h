// GuruGuruSMF4�̑���

// #pragma once
#ifndef GGS4CONTROLLER_H
#define GGS4CONTROLLER_H

#include <stdio.h>
#include <Windows.h>
#include <map>

#include "Constants.h"
#include "Smf.h"
#include "Analyzer.h"
#include "Compiler.h"
#include "WinmmPlayer.h"
#include "DmPortPlayer.h"
#include "DmPerformancePlayer.h"

namespace GuruGuruSmf { namespace Core {

	// SMF��̓f�[�^�\����(ver.0.3.3)
	struct SmfInformation3{
		int Size;			//���̍\���̂̃T�C�Y
		int TimeBase;		//�x�[�X�J�E���g
		int TotalTick;		//��Tick
		int TotalTime;		//�����t����
		int FirstNoteTick;	//�ŏ��̔���tick
		int LastNoteTick;	//�Ō�̔���tick
		int LoopTick;		//CC#111�̈ʒu
	};

	// ���t�f�[�^�\����(ver.0.3.3)
	struct PlayerStatus3{
		int Size;		//���̍\���̂̃T�C�Y
		int State;		//���t�����ǂ���
		int Tempo;		//���݂̃e���|
		int Tick;		//���݂̃e�B�b�N
		int Time;		//���݂̉��t����
		int LoopCount;	//���݂̃��[�v��
	};

	// �R���g���[�����N���X
	class GgsController
	{
	private:
		std::map<int, SmfController*> smfList;
		SmfListKeys smfListKeys;
		int* listKeys;
		std::string listKeysString;
		Analyzer::Analyzer* analyzer;
		Compiler::Compiler* compiler;
		IPlayer* smfPlayer;
		int playingId;
		int nextId;

	protected:
		GgsController();		// �R���X�g���N�^
		virtual ~GgsController();		// �f�X�g���N�^

		// SMF�R���g���[���[���쐬(Factory)
		virtual SmfController* MakeSmfController(bool buffered) = 0;

		// �v���C���[�I�u�W�F�N�g���쐬(Factory)
		virtual IPlayer* MakePlayer(int deviceId) = 0;

	public:
		// �f�o�C�X���J��
		GGSERROR OpenDevice(int deviceId, HWND handle);
		
		// �f�o�C�X�����
		void CloseDevice();
		
		// DLS��ǉ�
		GGSERROR AddDlsA(LPCSTR fileName);
		GGSERROR AddDlsW(LPCWSTR fileName);
		
		// �t�@�C�����烊�X�g�֒ǉ�
		GGSERROR AddListFromFileA(LPCSTR fileName, int option, int musicId);
		GGSERROR AddListFromFileW(LPCWSTR fileName, int option, int musicId);
		GGSERROR AddListFromFile3(LPCSTR fileName, int option, int musicId);
		
		// ���������烊�X�g�֒ǉ�
		GGSERROR AddListFromMemory(const byte* smfBinary, int length, int option, int musicId);
		GGSERROR AddListFromMemory3(const byte* smfBinary, int length, int option, int musicId);

		// ���X�g���̃A�C�e����1�폜
		void DeleteListItem(int musicId);
		
		// ���X�g���N���A
		void ClearList();
		
		// ���X�g����ID���
		const SmfListKeys* EnumList();

		// ���X�g����ID��񋓂��ĕ�����(Legacy)
		const char* EnumListToString();

		// ���t�I
		GGSERROR Play(int option, int musicId, int seek, int fadeInTime, int keyShift);
		
		// ��~�I
		void Stop(int fadeOutTime);
		
		// �ꎞ��~
		void Pause();
		
		// �ꎞ��~����
		void Restart();
		
		// �ꎞ��~/����(Legacy)
		void PauseRestart();

		// SMF�̏����擾
		GGSERROR GetSmfInformation(SmfInformation* info, int size, int musicId);

		// SMF�̏����擾(Legacy)
		GGSERROR GetSmfInformation3(SmfInformation3* info, int musicId);
		
		// ���t��Ԃ��擾
		void GetPlayerStatus(PlayerStatus* status, int size);

		// ���t��Ԃ��擾(Legacy)
		void GetPlayerStatus3(PlayerStatus3* status);
		
		// �}�X�^�[�e���|
		void SetMasterTempo(float tempo);
		
		// �}�X�^�[�{�����[��
		virtual void SetMasterVolume(int volume);
		
		// �}�X�^�[�s�b�`
		void SetMasterPitch(int pitch);

	};



	// �R���g���[��(GuruGuruSMF4)
	class Ggs4Controller : public GgsController
	{
	private:
		static Ggs4Controller* instance;	// �C���X�^���X�u����
		Ggs4Controller(){}		// �R���X�g���N�^
		~Ggs4Controller(){}		// �f�X�g���N�^

	protected:
		// SMF�R���g���[���[���쐬(Factory)
		SmfController* MakeSmfController(bool buffered);

		// �v���C���[�I�u�W�F�N�g���쐬(Factory)
		IPlayer* MakePlayer(int deviceId);

	public:
		// �C���X�^���X���擾(singleton)
		static Ggs4Controller* GetInstance();

		// �C���X�^���X��j��
		static void FreeInstance();

		// �}�X�^�[�{�����[��
		void SetMasterVolume(int volume);
	};
	
	// �R���g���[��(GuruGuruSMF3)
	class Ggs3Controller : public GgsController
	{
	private:
		static Ggs3Controller* instance;	// �C���X�^���X�u����
		bool isPerformanceMode;

		Ggs3Controller(){}		// �R���X�g���N�^
		~Ggs3Controller(){}		// �f�X�g���N�^

	protected:
		// SMF�R���g���[���[���쐬(Factory)
		SmfController* MakeSmfController(bool buffered);

		// �v���C���[�I�u�W�F�N�g���쐬(Factory)
		IPlayer* MakePlayer(int deviceId);

	public:
		// �C���X�^���X���擾(singleton)
		static Ggs3Controller* GetInstance();

		// �C���X�^���X��j��
		static void FreeInstance();

		// �}�X�^�[�{�����[��
		void SetMasterVolume(int volume);
	};
		
}}

#endif
