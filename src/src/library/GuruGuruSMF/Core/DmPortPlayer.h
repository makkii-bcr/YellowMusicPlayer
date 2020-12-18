// DirectMusic Port���[�h�v���C���[

// #pragma once
#ifndef _DMPORTPLAYER_H_
#define _DMPORTPLAYER_H_

#include <Windows.h>
#include <map>

#include "Interface.h"
#include "Device.h"
#include "Volume.h"
#include "Times.h"
#include "Thread.h"
#include "Player.h"


namespace GuruGuruSmf { namespace Core { namespace DirectMusicPort {


	/// <summary>
	/// DirectMusic Port���[�h�̃v���C���[
	/// </summary>
	class Player : public Core::Player::PlayerBase, public IPlayer, public IPlayerCore
	{
	private:
		Device* device;
		bool sentReset;
		
		Thread* workerThread;
		volatile bool isWorkerOn;
		
		ISmfController* smfController;

		std::map<int, CodeEvent> seekingCodeEventTable;
		bool doMasterPitch;
		
		PlayerTimes* times;
		Volume* volume;
		
		PLAYERSTATE state;
		int loopPosition, loopCount;
		bool isMusicEnd;
		
		bool isPause, doRestart;

		// ���ݎ���+PrepareTime�܂ł̃C�x���g������
		bool TimeEventStep();
		
		// �`���̋󔒂��X�L�b�v
		void SkipBeginning();
		
		// �V�[�N����
		void Seek(int tick);
		
		// �X���b�h�J�n
		void StartWorker();

		// �X���b�h��~
		void StopWorker();
		
		// �ꎞ��~
		void WorkerPause();
		
		// �ꎞ��~����
		void WorkerRestart();
		
		// ���y��~���̍ŏI����
		void FinalizeMusic(bool isWorker, bool doReset);
		
		// �}�X�^�[�s�b�`�𑗐M
		void SendMasterPitch();

	protected:
		// SMF�ŏ�����
		void Initialize(Smf* newSmf);

		// �`���Ɋ����߂�
		void Rewind();

	public:
		Player();	// �R���X�g���N�^
		~Player();	// �f�X�g���N�^

		// �X���b�h���s��
		void OnWorker();
		
		//
		// IPlayer������
		//
		
		// �f�o�C�X���J��
		GGSERROR OpenDevice(int deviceId, HWND handle);
		
		// �f�o�C�X�����
		void CloseDevice();
		
		// ���t�I
		GGSERROR Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int newKeyShift);
		
		// ��~
		void Stop(int fadeOutTime);
		
		// �ꎞ��~
		void Pause();
		
		// �ꎞ��~����
		void Restart();

		// ���t�����擾
		void GetStatus(PlayerStatus* status);
		
		// �}�X�^�[�e���|
		void SetMasterTempo(float masterTempo);
		
		// �}�X�^�[�{�����[��
		void SetMasterVolume(int masterVolume);
		
		// �}�X�^�[�s�b�`
		void SetMasterPitch(int pitch);
		
		
		//
		// IPlayerCore������
		//
		
		// MIDI���b�Z�[�W���M�f�o�C�X���擾
		IDevice* GetDevice(){ return device; }
		
		// �}�[�J�[Tick�ɉ��Z����
		void AddMarkerTick(int deltaTime);
		
		// �I�_����
		void MusicEnd();
		
		// �e���|���Z�b�g
		void SetTempo(int tempo);
		
		// �{�����[�����b�Z�[�W�𑗐M
		void SendVolume(int channel, int vol);
		
		// �s�b�`�x���h���b�Z�[�W�𑗐M
		void SendPitchBend(int channel, int pitch);
		
		// �G�N�X�N���[�V�u�𑗐M
		void SendExclusive(CodeEvent* codeEvent);
		
		// Seek�p�ɃC�x���g��o�^
		void RegisterEvent(CODE_EVENTTYPE type, int channel, int data1, CodeEvent* codeEvent);

		// �V�t�g���ꂽ�m�[�g�ԍ����擾
		int GetShiftedNote(int note, int channel);


	};

}}}

#endif
