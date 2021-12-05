
#include "stdafx.h"
#include "DmPerformancePlayer.h"

namespace GuruGuruSmf { namespace Core { namespace DirectMusicPerformance {

	// �R���X�g���N�^
	Player::Player()
	{
		isOpen = false;
	}

	// �f�X�g���N�^
	Player::~Player()
	{
		CloseDevice();
	}

	// �f�o�C�X���J��
	GGSERROR Player::OpenDevice(int deviceId, HWND handle)
	{
		GGSERROR err = GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceOpen(handle, 128);
		if(err != 0) return err;
		isOpen = true;
		return GgsError::NoError;
	}

	// �f�o�C�X�����
	void Player::CloseDevice()
	{
		if (isOpen){
			Stop(0);
			GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceClose();
			isOpen = false;
		}
	}

	// ���t�I
	GGSERROR Player::Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int keyShift)
	{
		if (!isOpen) return GgsError::NotReady;
		
		Smf* smf = smfCtrl->GetSmf();

		PerformancePlaySmfData smfData;
		smfData.StoredType = smf->GetStoredType();
		if(smfData.StoredType == StoredType::File){
			smfData.FileName = smf->GetFileName();
		}else{
			smfData.SmfBinary = (void*)smf->GetBaseAddres();
			smfData.SmfBinaryLength = smf->GetLength();
		}
		smfData.TimeBase = smf->GetTimeBase();
		smfData.TotalTick = smf->GetData()->TotalTick;
		smfData.FirstNoteTick = smf->GetData()->FirstNoteTick;
		smfData.LastNoteTick = smf->GetData()->LastNoteTick;
		smfData.LoopTick = smf->GetData()->LoopTick;
		
		int res = GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformancePlay(&smfData, option, seek);
		if (res != 0) return GgsError::CannotStartTimer;	//TODO: �G���[���Ȃ�Ƃ�
		return GgsError::NoError;
	}
	
	// ��~�I
	void Player::Stop(int fadeOutTime)
	{
		if (isOpen)
			GuruGuruSmf::Dxmus::Controller::GetInstance()->PerformanceStop();
	}

}}}


