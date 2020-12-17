
#include "stdafx.h"

#include "Volume.h"

namespace GuruGuruSmf { namespace Core {

	// �R���X�g���N�^
	Volume::Volume(IDevice* dev)
	{
		device = dev;

		InitializeCriticalSection(&lockObject);
		volumes = new int[16];
		masterVolume = 0;
	}

	// �f�X�g���N�^
	Volume::~Volume()
	{
		delete[] volumes;

		DeleteCriticalSection(&lockObject);
	}

	// ������
	void Volume::InitializeModule(bool normalize, int maxVolume)
	{
		if (normalize){
			normalizedGain = 127 - maxVolume;
		}else{
			normalizedGain = 0;
		}
		
		for (int i=0; i<16; i++){
			// �{�����[�����b�Z�[�W�𑗂��Ă��܂��܂�
			SetVolume(i, 100);
		}
		updated = false;
	}

	// true���Ɖ��ʂ������I��0�ɂ���Afalse���Ɖ���(����)
	void Volume::Mute(bool isMute)
	{
		if (isMute){
			for (int i=0; i<16; i++){
				device->SendShortMessage(0xb0+i, 7, 0);
			}
		}else{
			for (int i=0; i<16; i++){
				SetVolume(i, volumes[i]);
			}
		}
	}

	// �`�����l�����Ƃ̃{�����[���ݒ�(����)
	void Volume::SetVolume(int channel, int vol)
	{
		volumes[channel] = vol;
		vol += normalizedGain + masterVolume + fadingGain;
		
		if (vol > 127){
			vol = 127;
		}else if (vol < 0){
			vol = 0;
		}
		
		device->SendShortMessage(0xb0+channel, 7, vol);
	}

	// �}�X�^�[�{�����[����ݒ�(�񓯊�)
	void Volume::SetMasterVolume(int vol)
	{
		masterVolume = vol;
		updated = true;
	}

	// �t�F�[�h�C����ݒ�(�񓯊�) InitializeModule�̑O�ɌĂ�
	void Volume::SetFadeIn(int time)
	{
		if (time > 0){
			fadingTime = time;
			fadingCount = 0;
			fadingGain = -127;
			state = VolumeState::FadeIn;
		}else{
			fadingGain = 0;
			state = VolumeState::Normal;
		}
	}

	// �t�F�[�h�A�E�g��ݒ�(�񓯊�)
	void Volume::SetFadeOut(int time)
	{
		EnterCriticalSection(&lockObject);

		if (state == VolumeState::Normal){
			fadingCount = time;
		}else{
			fadingCount = fadingCount * time / fadingTime;
		}
		fadingTime = time;
		state = VolumeState::FadeOut;
		
		LeaveCriticalSection(&lockObject);
	}

	// �t�F�[�h���ʂ��v�Z
	void Volume::CalcFadingGain()
	{
		fadingGain = fadingCount * 127 / fadingTime - 127;
		if (fadingGain > 0){
			fadingGain = 0;
		}
		if (fadingGain != oldFadingGain){
			updated = true;
			oldFadingGain = fadingGain;
		}
	}

	// �񓯊������p�̃G���g���[�|�C���g ture�̏ꍇ�t�F�[�h�A�E�g�I��
	bool Volume::Worker()
	{
		bool terminated = false;
		
		EnterCriticalSection(&lockObject);

		if (state == VolumeState::FadeIn){
			fadingCount += Constant::TimeInterval;
			CalcFadingGain();
			if (fadingCount >= fadingTime){
				state = VolumeState::Normal;
			}
		}else if (state == VolumeState::FadeOut){
			fadingCount -= Constant::TimeInterval;
			CalcFadingGain();
			if (fadingCount < 0){
				state = VolumeState::Normal;
				terminated = true;
			}
		}
		
		LeaveCriticalSection(&lockObject);
		
		
		if (updated){
			for (int i=0; i<16; i++){
				SetVolume(i, volumes[i]);
			}
			updated = false;
		}
		return terminated;
	}



}}

