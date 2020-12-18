
#include "stdafx.h"
#include "SmfInterpreter.h"

namespace GuruGuruSmf { namespace Core {
	
	// �R���X�g���N�^
	SmfInterpreter::SmfInterpreter()
	{
		trackData = 0;
	}
	
	// �f�X�g���N�^
	SmfInterpreter::~SmfInterpreter()
	{
		delete[] trackData;
	}
	
	// �S�g���b�N�ōŏ���Tick���擾�B�I�����Ă�����-1��Ԃ�
	int SmfInterpreter::GetMinTick()
	{
		int tick = Constant::MaxTick;
		
		for (int i=0; i<trackCount; i++){
			TrackData& trk = trackData[i];
			if (trk.State == TrackState::End) continue;
			
			if (tick > trk.Tick) tick = trk.Tick;
		}
		
		if (tick < Constant::MaxTick){
			return tick;
		}else{
			return -1;	// MaxTick�ȏ�̏ꍇ�ł��I��
		}
	}
	
	// �w���SMF�ŏ�����
	void SmfInterpreter::Initialize(Smf* newSmf)
	{
		smf = newSmf;
		
		trackCount = smf->GetTrackCount();
		trackData = new TrackData[trackCount];
		for (int i=0; i<trackCount; i++){
			TrackData& trk = trackData[i];
			trk.SmfTrack = smf->GetTrack(i);
		}
		
		Rewind();
	}
	
	// �ŏ��܂Ŋ����߂�
	void SmfInterpreter::Rewind()
	{
		smf->Rewind();
		
		for (int i=0; i<trackCount; i++){
			TrackData& trk = trackData[i];
			
			// �ŏ��̃f���^�^�C�����擾
			GGSERROR err;
			trk.Tick = trk.SmfTrack->ReadDeltaTime(err);
			
			if (err == GgsError::NoError){
				trk.State = TrackState::Play;
			}else{
				trk.State = TrackState::End;
			}
		}
		
	}
	
	// �C�x���g�����߂��ď���
	GGSERROR SmfInterpreter::InterpretSmf()
	{
		while(1){
			int tick = GetMinTick();
			if (tick < 0) break;
			UpdateTick(tick);
			
			for (int i=0; i<trackCount; i++){
				TrackData& trk = trackData[i];
				if (trk.State == TrackState::End) continue;
				
				while (tick >= trk.Tick){
					GGSERROR err;
					SmfEvent* smfEvent = trk.SmfTrack->ReadSmfEvent(err);
					if (err == GgsError::TrackLimit || smfEvent->Type == SmfEventType::TrackEnd){
						trk.State = TrackState::End;
						break;
					}
					if (err != GgsError::NoError) return err;

					EventExecute(smfEvent);
					trk.Tick += smfEvent->DeltaTime;
				}
			}
		}
		return GgsError::NoError;
	}
	
	
	
}}


