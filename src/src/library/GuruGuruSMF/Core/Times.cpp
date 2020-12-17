
#include "stdafx.h"
#include "Times.h"
#include <dxmus.core/Controller.h>


namespace GuruGuruSmf { namespace Core {

namespace Analyzer {
	
	//
	// PlayerTimes
	//
	
	// �`���Ɋ����߂�
	void AnalyzerTimes::Rewind()
	{
		SetTempo(Constant::DefaultTempo);
		markerTick = 0;
		markerTime = 0;
	}
	
	// Tick��Time���X�V
	void AnalyzerTimes::Update(int tick)
	{
		markerTime += (tick - markerTick) * tempoA;
		markerTick = tick;
	}

}

namespace WindowsMultimedia
{
	//
	// PlayerTimes
	//
	
	// �R���X�g���N�^
	PlayerTimes::PlayerTimes()
	{
		masterTempo = 1;

		timeBase = 96;
		SetTempo(Constant::DefaultTempo);
		nowTime = 0;
		nowTick = 0;
	}
	
	// �e���|(SMF�ł̕\���l)
	void PlayerTimes::SetTempo(double tp)
	{
		tempo = tp;
		tempoA = tp / (timeBase * 1000);
		deltaTick = 1000 * timeBase * Constant::TimeInterval / tp;
	}
	
	// MarkerTick���X�V
	void PlayerTimes::UpdateMarkerTick(int delta)
	{
		queueTick = markerTick;
		queueTime = markerTime;
		markerTick += delta;
		markerTime += delta * tempoA;
	}
	
	// ���݂�Tick��i�߂�
	void PlayerTimes::UpdateNowTick()
	{
		nowTime += Constant::TimeInterval * masterTempo;
		nowTick += deltaTick * masterTempo;
	}
	
	// ���݂̉��t���Ԃ��C�� �V�[�N���Ɏg�p
	void PlayerTimes::FixNowTime(int tick)
	{
		nowTime = queueTime + (tick - queueTick) * tempoA;
		nowTick = tick;
	}
	
	// �ŏ��̔����̈ʒu�ֈړ�
	void PlayerTimes::GotoFirstNote()
	{
		nowTime = firstNoteTime;
		markerTime = nowTime;
		nowTick = firstNoteTick;
	}
	
	// ���[�v�ʒu�ֈړ�
	void PlayerTimes::GotoLoopPoint(bool isSkipEnding)
	{
		markerTick = loopTick;
		markerTime = loopTime;
		
		double sTime = totalTime;
		double sTick = totalTick;
		
		if (isSkipEnding){
			sTime = lastNoteTime;
			sTick = lastNoteTick;
		}
		nowTime = loopTime + (nowTime - sTime);
		nowTick = loopTick + (nowTick - sTick);
	}
	
	// SMF�ŏ�����
	void PlayerTimes::Initialize(Smf* smf)
	{
		timeBase = smf->GetTimeBase();
		
		loopTick = smf->GetData()->LoopTick;
		loopTime = smf->GetData()->LoopTime;
		firstNoteTick = smf->GetData()->FirstNoteTick;
		firstNoteTime = smf->GetData()->FirstNoteTime;
		lastNoteTick = smf->GetData()->LastNoteTick;
		lastNoteTime = smf->GetData()->LastNoteTime;
		totalTick = smf->GetData()->TotalTick;
		totalTime = smf->GetData()->TotalTime;
		
		queueTick = 0;
		queueTime = 0;
	}
	
	// �`���Ɋ����߂�
	void PlayerTimes::Rewind()
	{
		SetTempo(Constant::DefaultTempo);
		markerTick = 0;
		markerTime = 0;
		
		nowTick = 0;
		nowTime = 0;
	}
	
	
}

namespace DirectMusicPort {

	//
	// PlayerTimes
	//
	
	// �R���X�g���N�^
	PlayerTimes::PlayerTimes()
	{
		queueDmTime = 0;
		fixedQueueDmTime = 0;
		masterTempo = 1;
		realtimeMode = false;

		timeBase = 96;
		SetTempo(Constant::DefaultTempo);
		nowTime = 0;
		nowTick = 0;
	}
	
	// ���̃��b�Z�[�W���M�\�莞��
	__int64 PlayerTimes::GetMarkerDmTime(){
		if (realtimeMode){
			if(fixedQueueDmTime > markerDmTime-10000){
				// �ʏ�̏������Ԃ�ǂ������Ȃ��悤�ɍ׍H
				markerDmTime = fixedQueueDmTime + 10000;
			}
			return fixedQueueDmTime;
		}else{
			return markerDmTime;
		}
	}
	
	// DirectMusic�������Ԃ�ǂ�ł���
	void PlayerTimes::ReadMasterClock()
	{
		GuruGuruSmf::Dxmus::Controller::GetInstance()->PortGetReferenceTime(&nowDmTime);
	}
	
	// ���O�ɉ������M���Ă�����E�F�C�g�����āA���ĂȂ���΍��̎��Ԃ��Z�b�g
	void PlayerTimes::SetMarkerDmTimeByQueueDmTime(__int64 wait)
	{
		__int64 qt = fixedQueueDmTime + wait;
		markerDmTime = qt > nowDmTime ? qt : nowDmTime;
	}
	
	// ���[�v�ʒu�ֈړ�
	void PlayerTimes::GotoLoopPoint()
	{
		markerTick = loopTick;
		markerTime = loopTime;
	}
		
	// �X�V����
	void PlayerTimes::UpdateMarkerTick(int delta)
	{
		queueDmTime = markerDmTime;
		fixedQueueDmTime = queueDmTime;
		queueTime = markerTime;
		queueTick = markerTick;
		
		markerTick += delta;
		double a = delta * tempo / timeBase;
		markerTime += a / 1000;
		
		// �X�L�b�v���̓}�[�J�[�^�C�������Z���Ȃ�
		if (!skipping){
			markerDmTime += (__int64)(a*10 / masterTempo);
		}
	}

	// ���M���Ԃ��X�V
	void PlayerTimes::UpdateQueueDmTime()
	{
		queueDmTime = markerDmTime;
		fixedQueueDmTime = queueDmTime;
	}
	
	// ���݂̎��Ԃ��Z�o
	void PlayerTimes::CalcNowTime()
	{
		double a = (double)((nowDmTime + Constant::PrepareTime) - queueDmTime);
		a *= masterTempo;
		nowTime = queueTime + a / 10000;
		nowTick = queueTick + (int)(a * timeBase / (tempo * 10));
	}
	
	// �ꎞ��~����
	void PlayerTimes::Restart()
	{
		__int64 t = nowDmTime - pausedDmTime;
		markerDmTime += t;
		queueDmTime += t;
		fixedQueueDmTime = markerDmTime;
	}
	
	// SMF�ŏ�����
	void PlayerTimes::Initialize(Smf* smf)
	{
		timeBase = smf->GetTimeBase();
		
		loopTick = smf->GetData()->LoopTick;
		loopTime = smf->GetData()->LoopTime;
		firstNoteTick = smf->GetData()->FirstNoteTick;
		firstNoteTime = smf->GetData()->FirstNoteTime;
		lastNoteTick = smf->GetData()->LastNoteTick;
		lastNoteTime = smf->GetData()->LastNoteTime;
		
		skipping = false;
	}
	
	// �`���Ɋ����߂�
	void PlayerTimes::Rewind()
	{
		SetTempo(Constant::DefaultTempo);
		markerTick = 0;
		markerTime = 0;
		
		queueTime = 0;
		queueTick = 0;
	}
	
	
}

}}



