// SMF����

// #pragma once
#ifndef SMFINTERPRETER_H
#define SMFINTERPRETER_H

#include "Constants.h"
#include "Smf.h"

namespace GuruGuruSmf { namespace Core {
	
	/// <summary>
	/// SMF������N���X
	/// </summary>
	class SmfInterpreter
	{
	public:
		/// <summary>
		/// 1�g���b�N���̃f�[�^
		/// </summary>
		struct TrackData
		{
			SmfTrack* SmfTrack;		// �g���b�N�I�u�W�F�N�g�̎Q��
			TRACKSTATE State;				// �g���b�N�̏��
			int Tick;				// Tick
		};
		
	private:
		Smf* smf;
		int trackCount;
		TrackData* trackData;
		
		// �S�g���b�N�ōŏ���Tick���擾�B�I�����Ă�����-1��Ԃ�
		int GetMinTick();
		
	protected:
		SmfInterpreter();	// �R���X�g���N�^
		virtual ~SmfInterpreter();	// �f�X�g���N�^
		
		// SMF�I�u�W�F�N�g�u����
		Smf* GetSmf(){ return smf; }
		
		// �g���b�N��
		int GetTrackCount(){ return trackCount; }
		
		// �w���SMF�ŏ�����
		virtual void Initialize(Smf* newSmf);
		
		// �ŏ��܂Ŋ����߂�
		virtual void Rewind();
		
		// Tick��i�߂�
		virtual void UpdateTick(int newTick) = 0;
		
		// �C�x���g����
		virtual void EventExecute(SmfEvent* smfEvent) = 0;
		
		// �C�x���g�����߂��ď���
		GGSERROR InterpretSmf();
		
		
	};
	
	
}}



#endif
