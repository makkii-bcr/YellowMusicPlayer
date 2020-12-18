// DirectMusic Performance���[�h(Legacy)

// #pragma once
#ifndef DMPERFORMANCEPLAYER_H
#define DMPERFORMANCEPLAYER_H

#include <Windows.h>
#include "Constants.h"
#include "Interface.h"
#include "Smf.h"
#include <dxmus.core/Controller.h>

namespace GuruGuruSmf { namespace Core { namespace DirectMusicPerformance {

	/// <summary>
	/// DirectMusic Performance���[�h�̃v���C���[
	/// </summary>
	class Player : public IPlayer
	{
	private:
		bool isOpen;

	public:
		Player();	// �R���X�g���N�^
		~Player();	// �f�X�g���N�^

		//
		// ISmfPlayer������
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
		void Pause(){ GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformancePause(); }
		
		// �ꎞ��~����
		void Restart(){ GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceRestart(); }
		
		// ���t��Ԃ��擾
		void GetStatus(PlayerStatus* status){ GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceGetStatus(status); }
		
		// �}�X�^�[�e���|
		void SetMasterTempo(float tempo){ GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceSetMasterTempo(tempo); }
		
		// �}�X�^�[�{�����[��
		void SetMasterVolume(int volume){ GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceSetMasterVolume(volume); }
		
		// �}�X�^�[�s�b�`
		void SetMasterPitch(int pitch){}
		
	};




}}}

#endif
