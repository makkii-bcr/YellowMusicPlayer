// ���ʊ֌W

// #pragma once
#ifndef VOLUME_H
#define VOLUME_H

#include <Windows.h>
#include "Constants.h"
#include "Interface.h"

namespace GuruGuruSmf { namespace Core {

	/// <summary>
	/// ���ʃR���g���[��
	/// </summary>
	class Volume
	{
	private:
		IDevice* device;
		VOLUMESTATE state;
		int* volumes;
		int masterVolume, normalizedGain;
		bool updated;
		int fadingCount, fadingTime, fadingGain, oldFadingGain;
		CRITICAL_SECTION lockObject;

	public:
		Volume(IDevice* dev);	// �R���X�g���N�^
		~Volume();	// �f�X�g���N�^

		// ���
		int GetState(){ return state; }
		
		// true���Ɖ��ʂ������I��0�ɂ���Afalse���Ɖ���(����)
		void Mute(bool isMute);
		
		// �`�����l�����Ƃ̃{�����[���ݒ�(����)
		void SetVolume(int channel, int vol);
		
		// �}�X�^�[�{�����[����ݒ�(�񓯊�)
		void SetMasterVolume(int vol);
		
		// �t�F�[�h�C����ݒ�(�񓯊�) InitializeModule�̑O�ɌĂ�
		void SetFadeIn(int time);
		
		// �t�F�[�h�A�E�g��ݒ�(�񓯊�)
		void SetFadeOut(int time);

		// �t�F�[�h���ʂ��v�Z
		void CalcFadingGain();
		
		// �񓯊������p�̃G���g���[�|�C���g ture�̏ꍇ�t�F�[�h�A�E�g�I��
		bool Worker();
		
		// ������
		void InitializeModule(bool normalize, int maxVolume);

	};


}}

#endif
