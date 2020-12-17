// ���Ԋ֌W�̃f�[�^

#pragma once

#include "Constants.h"
#include "Interface.h"
#include "Smf.h"

namespace GuruGuruSmf { namespace Core {
namespace Analyzer {
	
	// ���Ԋ֌W�������N���X(Analyzer)
	class AnalyzerTimes
	{
	private:
		double timeBaseA, tempoA;
		int markerTick;
		double markerTime;
		
	public:
		// �^�C���x�[�X
		void SetTimeBase(int tb){ timeBaseA = tb * 1000; }
		
		// �e���|(�vTimeBase)
		void SetTempo(double tp){ tempoA = tp / timeBaseA; }
		
		// ��������Tick
		int GetMarkerTick(){ return markerTick; }
		
		// �������̉��t����
		double GetMarkerTime(){ return markerTime; }
		
		// �`���Ɋ����߂�
		void Rewind();
		
		// Tick��Time���X�V
		void Update(int tick);
	};
}

namespace WindowsMultimedia
{
	
	// ���Ԋ֌W�������N���X(WindowsMultimedia.Player)
	class PlayerTimes
	{
	private:
		int timeBase;
		double tempo, tempoA, masterTempo, deltaTick;
		int markerTick, queueTick;
		double nowTime, nowTick, markerTime, queueTime;
		int loopTick, firstNoteTick, lastNoteTick, totalTick;
		double loopTime, firstNoteTime, lastNoteTime, totalTime;
		
	public:
		// �R���X�g���N�^
		PlayerTimes();
		
		// �e���|(SMF�ł̕\���l)
		void SetTempo(double tp);
		
		// �}�X�^�[�e���|
		void SetMasterTempo(double mt){ masterTempo = mt; }
		
		// ���݂�Tick
		int GetNowTick(){ return (int)(nowTick + 0.001); }
		
		// ���݂̉��t����
		int GetNowTime(){ return (int)(nowTime + 0.001); }
		
		// BPM�\�L�ł̃e���|�l���擾
		int GetMusicTempo(){ return (int)(60000000 / tempo + 0.001); }
		
		// MarkerTick���Ȍ�Ȃ�true
		bool PassedMarkerTick(){ return nowTick >= markerTick; }
		
		// ���[�v�ʒu�ȍ~�Ȃ�true
		bool PassedLoopTick(){ return markerTick >= loopTick; }
		
		// �ŏ��̉��ȍ~�Ȃ�true
		bool PassedFirstNote(){ return markerTick >= firstNoteTick; }
		
		// �Ō�̉�����Ȃ�true
		bool PassedLastNote(){ return markerTick > lastNoteTick; }
		
		// �w���Tick�ȍ~�Ȃ�true
		bool PassedTick(int tick){ return markerTick >= tick; }
		
		// MarkerTick���X�V
		void UpdateMarkerTick(int delta);
		
		// ���݂�Tick��i�߂�
		void UpdateNowTick();
		
		// ���݂̉��t���Ԃ��C�� �V�[�N���Ɏg�p
		void FixNowTime(int tick);
		
		// �ŏ��̔����̈ʒu�ֈړ�
		void GotoFirstNote();
		
		// ���[�v�ʒu�ֈړ�
		void GotoLoopPoint(bool isSkipEnding);
		
		// SMF�ŏ�����
		void Initialize(Smf* smf);
		
		// �`���Ɋ����߂�
		void Rewind();
		
	};

}

namespace DirectMusicPort {

	// ���Ԋ֌W�������N���X(DirectMusicPort.Player)
	class PlayerTimes : public IDirectMusicTimer
	{
	private:
		bool skipping, realtimeMode;
		int timeBase;
		double tempo, masterTempo;
		__int64 markerDmTime, queueDmTime , nowDmTime, fixedQueueDmTime, pausedDmTime;
		double markerTime, queueTime, nowTime;
		int markerTick, queueTick, nowTick;
		int loopTick, firstNoteTick, lastNoteTick;
		double loopTime, firstNoteTime, lastNoteTime;
		
	public:
		PlayerTimes();	// �R���X�g���N�^
		
		//
		// IDirectMusicTimer������
		//
		
		// ���̃��b�Z�[�W���M�\�莞��
		__int64 GetMarkerDmTime();

		// ���M���Ԃ�1���Z(�f�[�^���M���p)
		void IncrementMarkerDmTime(){ markerDmTime++; fixedQueueDmTime++; }
		
		//
		
		// �e���|(SMF�ł̕\���l)
		void SetTempo(double tp){ tempo = tp; }
		
		// �}�X�^�[�e���|
		void SetMasterTempo(double mt){ masterTempo = mt; }
		
		// ��������Tick
		void SetMarkerTick(int mt){ markerTick = mt; }
		
		// �������̉��t����
		void SetMarkerTime(double mt){ markerTime = mt; }
		
		// ���݂�Tick(�v�Z�ɂ���ĎZ�o)
		int GetNowTick(){ return nowTick; }
		
		// ���݂̉��t����(�v�Z�ɂ���ĎZ�o)
		double GetNowTime(){ return nowTime; }
		
		// BPM�\�L�ł̃e���|�l���擾
		int GetMusicTempo(){ return (int)(60000000 / tempo + 0.001); }
		
		// DirectMusic�������Ԃ�ǂ�ł���
		void ReadMasterClock();
		
		// ��΂����������鎞��true
		void SetSkipping(bool s){ skipping = s; }
		
		// ���O�ɉ������M���Ă�����E�F�C�g�����āA���ĂȂ���΍��̎��Ԃ��Z�b�g
		void SetMarkerDmTimeByQueueDmTime(__int64 wait);
		
		// ���M���ԂɃE�F�C�g��������
		void Wait(long wait){ markerDmTime += wait; }
		
		// �����ςݎ��Ԃ܂Ŋ����߂�
		void RewindQueueTime(){ markerDmTime = queueDmTime; }
		
		// ���[�v�ʒu�ֈړ�
		void GotoLoopPoint();
		
		// �X�V����
		void UpdateMarkerTick(int delta);

		// ���M���Ԃ��X�V
		void UpdateQueueDmTime();
		
		// ���݂̎��Ԃ��Z�o
		void CalcNowTime();
		
		// ���b�Z�[�W�������Ԃ��߂��Ă�����true
		bool PassedPrepareTime(){ return markerDmTime > nowDmTime + Constant::PrepareTime; }
		
		// ���[�v�ʒu�ȍ~�Ȃ�true
		bool PassedLoopTick(){ return markerTick >= loopTick; }
		
		// �ŏ��̉��ȍ~�Ȃ�true
		bool PassedFirstNote(){ return markerTick >= firstNoteTick; }
		
		// �Ō�̉�����Ȃ�true
		bool PassedLastNote(){ return markerTick > lastNoteTick; }
		
		// �w���Tick�ȍ~�Ȃ�true
		bool PassedTick(int tick){ return markerTick >= tick; }
		
		// �ꎞ��~
		void Pause(){ pausedDmTime = nowDmTime; }
		
		// �ꎞ��~����
		void Restart();
		
		// QueueDmTime�̕␳
		void FixQueueDmTime(){ fixedQueueDmTime += Constant::TimeInterval * 10000; }
		
		// ���A���^�C�����[�h�ɓ���
		void EnterRealtimeMode(){ realtimeMode = true; }
		
		// ���A���^�C�����[�h����
		void LeaveRealtimeMode(){ realtimeMode = false; }
		
		// SMF�ŏ�����
		void Initialize(Smf* smf);
		
		// �`���Ɋ����߂�
		void Rewind();
		
		
		
	};


}

}}


