// Windows Multimedia API�̃v���C���[

// #pragma once
#ifndef _WINMMPLAYER_H_
#define _WINMMPLAYER_H_

#include <map>

#include "Interface.h"
#include "Device.h"
#include "Times.h"
#include "Volume.h"
#include "Player.h"

namespace GuruGuruSmf { namespace Core { namespace WindowsMultimedia {

	/// <summary>
	/// WMA���[�h�̃v���C���[
	/// </summary>
	class Player : public Core::Player::PlayerBase, public IPlayer, public IPlayerCore
	{
	private:
		Device* device;
		int timerId;

		ISmfController* smfController;

		std::map<int, CodeEvent> seekingCodeEventTable;
		bool doMasterPitch;
		
		PlayerTimes* times;
		Volume* volume;
		
		PLAYERSTATE state;
		int loopPosition, loopCount;
		bool isMusicEnd, isPause, doRestart;
		
		bool sentReset;
		__int64 resetedTime;

		// �^�C�}�[�J�n
		GGSERROR StartTimer();
		
		// �^�C�}�[��~
		void StopTimer();
		
		// �ꎞ��~
		void WorkerPause();
		
		// �ꎞ��~����
		void WorkerRestart();
		
		// �`���̋󔒂��X�L�b�v
		void SkipBeginning();
		
		// �V�[�N����
		void Seek(int tick);
		
		// ���Z�b�g���߂𑗂�
		void SendReset();
		
		// ���Z�b�g��̃E�F�C�g������
		void WaitAfterReset();

	protected:
		// SMF�ŏ�����
		void Initialize(Smf* newSmf);
		
		// �`���Ɋ����߂�
		void Rewind();

	public:
		Player();	// �R���X�g���N�^
		~Player();	// �f�X�g���N�^

		// �^�C�}�[���s��
		void OnTimer();

		//
		// IPlayer������
		//
		
		// �f�o�C�X���J��
		GGSERROR OpenDevice(int deviceId, HWND handle);
		
		// �f�o�C�X�����
		void CloseDevice();
		
		// ���t�I
		GGSERROR Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int keyShift);
		
		// ��~�I
		void Stop(int fadeOutTime);
		
		// �ꎞ��~
		void Pause();
		
		// �ꎞ��~����
		void Restart();
		
		// ���t��Ԃ��擾
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
		void SetTempo(int newTempo);
		
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
